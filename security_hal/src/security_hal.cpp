/*
 * @copyright Copyright (c) 2018-2020 TOYOTA MOTOR CORPORATION.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "security_hal.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "security_hal_securityhallog.h"

#define MD5_DIGEST_LENGTH 16      // md5 digest length
#define SHA1_DIGEST_LENGTH 20     // sha1 digest length
#define SHA224_DIGEST_LENGTH 28   // sha224 digest length
#define SHA256_DIGEST_LENGTH 32   // sha256 digest length
#define SHA384_DIGEST_LENGTH 48   // sha384 digest length
#define SHA512_DIGEST_LENGTH 64   // sha512 digest length
#define BITS_PER_BYTE 8           // the number of bits per byte
#define KEY_SOURCE_SIZE_128  16   // the size of key source is 128 bits
#define KEY_SOURCE_SIZE_192  24   // the size of key source is 192 bits
#define KEY_SOURCE_SIZE_256  32   // the size of key source is 256 bits

/**
 * the max length of input buffer for RSA asymmetric encrypt or
 * the minimum length of output buffer for RSA asymmetric decrypt
 */
#define RSA_PRIVATE_MAX_SIZE_BYTE \
        (RSA_PRIVATE_EXPONENT_MAX_SIZE/BITS_PER_BYTE - RSA_PADDING_MINIMUM_SIZE)

/**
 * cipher context information
 */
struct CipherContext {
  enum CipherType cipher_type;
  union CipherParameter cipher_parameter;
  union KeyParam key_param;
};

/**
 * hash context information
 */
struct HashContext {
  enum HashType hash_type;
};

/**
 * random number context information
 */
struct RandomContext {
  uint8_t* seed_buffer;
  uint32_t buffer_len;
};

bool CheckParameterVaildity(enum CipherType cipher_type,
                            union CipherParameter* param, union KeyParam* key) {
  if (SYMMETRIC_CIPHER_AES == cipher_type) {
    // check cipher mode for symmetric encrypt/decrypt
    if (SYMMETRIC_CIPHER_MODE_BLOCK_ECB != param->symmetric.mode &&
        SYMMETRIC_CIPHER_MODE_BLOCK_CBC != param->symmetric.mode &&
        SYMMETRIC_CIPHER_MODE_BLOCK_CFB != param->symmetric.mode &&
        SYMMETRIC_CIPHER_MODE_BLOCK_OFB != param->symmetric.mode) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        "don't support the cipher mode for AES symmetric encrypt/decrypt");
      return false;
    }
    // check cipher block size for AES symmetric encrypt/decrypt
    if (SYMMETRIC_CIPHER_BLOCK_SIZE_16 != param->symmetric.block_size) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        "don't support the block size for AES symmetric encrypt/decrypt");
      return false;
    }
    // check cipher key type for AES symmetric encrypt/decrypt
    if (SYMMETRIC_CIPHER_KEY_TYPE_MANAGED != key->symmetric.key_type &&
        SYMMETRIC_CIPHER_KEY_TYPE_USER != key->symmetric.key_type) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        "don't support the cipher key type for AES symmetric encrypt/decrypt");
      return false;
    }
    if (SYMMETRIC_CIPHER_KEY_TYPE_MANAGED == key->symmetric.key_type) {
      // check cipher rounds for AES symmetric encrypt/decrypt
      if (SYMMETRIC_CIPHER_ROUND_10 != param->symmetric.round &&
          SYMMETRIC_CIPHER_ROUND_12 != param->symmetric.round &&
          SYMMETRIC_CIPHER_ROUND_14 != param->symmetric.round) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          "don't support the cipher round for AES symmetric encrypt/decrypt");
        return false;
     }
    } else {
      // check parameter of key provided by user for AES symmetric encrypt/decrypt
      if (NULL == key->symmetric.key_param.user_key.key) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "user_key.key is NULL");
        return false;
      }
      if (KEY_SOURCE_SIZE_128 == key->symmetric.key_param.user_key.key_len) {
        if (SYMMETRIC_CIPHER_ROUND_10 != param->symmetric.round) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          "cipher round should be 10 if key len is 128 bits");
          return false;
        }
      } else if (KEY_SOURCE_SIZE_192 == key->symmetric.key_param.user_key.key_len) {
        if (SYMMETRIC_CIPHER_ROUND_12 != param->symmetric.round) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          "cipher round should be 12 if key len is 192 bits");
          return false;
        }
      } else if (KEY_SOURCE_SIZE_256 == key->symmetric.key_param.user_key.key_len) {
        if (SYMMETRIC_CIPHER_ROUND_14 != param->symmetric.round) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          "cipher round should be 14 if key len is 256 bits");
          return false;
        }
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          "key len should be 128, 192 or 256 bits for AES symmetric encrypt/decrypt");
        return false;
      }
      if (SYMMETRIC_CIPHER_MODE_BLOCK_ECB != param->symmetric.mode) {
        if (NULL == key->symmetric.key_param.user_key.iv) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "user_key.iv is NULL");
          return false;
        }
        if (key->symmetric.key_param.user_key.iv_len != param->symmetric.block_size) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          "user_key.iv_len is not equal to block size for AES symmetric encrypt/decrypt");
          return false;
        }
      }
    }
  } else if (ASYMMETRIC_CIPHER_RSA == cipher_type) {
    // check the padding mode for RSA asymmetric encrypt/decrypt
    if (ASYMMETRIC_PADDING_MODE_RSA_PKCS1 != param->asymmetric.mode &&
        ASYMMETRIC_PADDING_MODE_RSA_SSLV23 != param->asymmetric.mode &&
        ASYMMETRIC_PADDING_MODE_RSA_NOPADDING != param->asymmetric.mode &&
        ASYMMETRIC_PADDING_MODE_RSA_OAEP != param->asymmetric.mode &&
        ASYMMETRIC_PADDING_MODE_RSA_PSS != param->asymmetric.mode) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        "don't support the cipher mode for asymmetric encrypt/decrypt");
      return false;
    }
    // check cipher key type for RSA asymmetric encrypt/decrypt
    if (ASYMMETRIC_CIPHER_KEY_TYPE_MANAGED != key->asymmetric.key_type &&
        ASYMMETRIC_CIPHER_KEY_TYPE_USER_PUBLIC != key->asymmetric.key_type &&
        ASYMMETRIC_CIPHER_KEY_TYPE_USER_PRIVATE != key->asymmetric.key_type) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        "don't support the cipher key type for asymmetric encrypt/decrypt");
      return false;
    }
    if (ASYMMETRIC_CIPHER_KEY_TYPE_USER_PUBLIC == key->asymmetric.key_type) {
      // check parameter of public key provided by user for RSA asymmetric encrypt/decrypt
      if (NULL == key->asymmetric.key_param.user_key.public_key) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "public_key is NULL");
        return false;
      }
      if (RSA_PUBLIC_EXPONENT_MAX_SIZE <
          key->asymmetric.key_param.user_key.public_key->rsa.e_length ||
          RSA_MODULUS_MAX_SIZE <
          key->asymmetric.key_param.user_key.public_key->rsa.n_length) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          "e_length or n_length is too large, e_length:%d, n_length:%d",
          key->asymmetric.key_param.user_key.public_key->rsa.e_length,
          key->asymmetric.key_param.user_key.public_key->rsa.n_length);
        return false;
      }
    } else if (ASYMMETRIC_CIPHER_KEY_TYPE_USER_PRIVATE == key->asymmetric.key_type) {
      // check parameter of key provided by user for RSA asymmetric encrypt/decrypt
      if (NULL == key->asymmetric.key_param.user_key.private_key) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "private_key is NULL");
        return false;
      }
      if (RSA_PRIVATE_EXPONENT_MAX_SIZE <
          key->asymmetric.key_param.user_key.private_key->rsa.d_length ||
          RSA_MODULUS_MAX_SIZE <
          key->asymmetric.key_param.user_key.private_key->rsa.n_length) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          "d_length or n_length is too large, d_length:%d, n_length:%d",
          key->asymmetric.key_param.user_key.private_key->rsa.d_length,
          key->asymmetric.key_param.user_key.private_key->rsa.n_length);
        return false;
      }
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "the cipher_type isn't support");
    return false;
  }
  return true;
}

// Initialize the encrypt context information
EFrameworkunifiedStatus EncryptStart(enum CipherType cipher_type, union CipherParameter* param,
                        union KeyParam* key, void** ctx) {
  if (SYMMETRIC_CIPHER_AES != cipher_type && ASYMMETRIC_CIPHER_RSA != cipher_type) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "the cipher_type isn't support");
    return eFrameworkunifiedStatusInvldParam;
  }
  if (NULL == param || NULL == key || NULL == ctx) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
      "param, key or ctx is NULL, param:%p key:%p ctx:%p", param, key, ctx);
    return eFrameworkunifiedStatusInvldParam;
  }
  bool ret = CheckParameterVaildity(cipher_type, param, key);
  if (true != ret) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "parameter error");
    return eFrameworkunifiedStatusInvldParam;
  }

  void* ctx_temp = malloc(sizeof(CipherContext));
  if (NULL == ctx_temp) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to malloc %lu byte for ctx, errno=%d",
      sizeof(CipherContext), errno);
    return eFrameworkunifiedStatusFail;
  }
  memset(ctx_temp, 0, sizeof(CipherContext));
  CipherContext* pcipher_context = reinterpret_cast<CipherContext*>(ctx_temp);
  pcipher_context->cipher_type = cipher_type;
  pcipher_context->cipher_parameter = *param;
  pcipher_context->key_param = *key;
  *ctx = ctx_temp;
  return eFrameworkunifiedStatusOK;
}

// Encrypt plaintext information
EFrameworkunifiedStatus EncryptUpdate(void* ctx, const uint8_t* in, uint32_t in_len,
                      uint8_t* out, uint32_t out_len, uint32_t* true_length) {
  if (NULL == ctx || NULL == in || NULL == out || NULL == true_length) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
      "ctx, in, out or true_length is NULL, ctx:%p in:%p out:%p true_length:%p",
      ctx, in, out, true_length);
    return eFrameworkunifiedStatusInvldParam;
  }
  if (0 == in_len) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "in_len is equal to 0");
    return eFrameworkunifiedStatusInvldParam;
  }

  CipherContext* pcipher_context = reinterpret_cast<CipherContext*>(ctx);
  if (SYMMETRIC_CIPHER_AES == pcipher_context->cipher_type) {  // symmetric encrypt
    uint32_t block_size = pcipher_context->cipher_parameter.symmetric.block_size;
    if (out_len < in_len + block_size) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "out_len is less than in_len plus block_size");
      return eFrameworkunifiedStatusInvldParam;
    }
    memcpy(out, in, in_len);
    *true_length = in_len;
  } else if (ASYMMETRIC_CIPHER_RSA == pcipher_context->cipher_type) {  // asymmetric encrypt
    if (RSA_PRIVATE_MAX_SIZE_BYTE < in_len) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        "in_len is greater than RSA_PRIVATE_MAX_SIZE_BYTE");
      return eFrameworkunifiedStatusInvldParam;
    }
    if (RSA_PRIVATE_EXPONENT_MAX_SIZE/BITS_PER_BYTE > out_len) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        "out_len is less than RSA_PRIVATE_EXPONENT_MAX_SIZE/BITS_PER_BYTE");
      return eFrameworkunifiedStatusInvldParam;
    }
    // out_len is greater than in_len
    memcpy(out, in, in_len);
    *true_length = in_len;
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
      "the cipher_type isn't support, cipher_type:%d", pcipher_context->cipher_type);
    return eFrameworkunifiedStatusInvldParam;
  }

  return eFrameworkunifiedStatusOK;
}

// Encrypt the final plaintext information
EFrameworkunifiedStatus EncryptFinish(void* ctx, uint8_t* out, uint32_t out_len, uint32_t* true_length) {
  if (NULL == ctx) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "ctx is NULL");
    return eFrameworkunifiedStatusInvldParam;
  }
  CipherContext* pcipher_context = reinterpret_cast<CipherContext*>(ctx);
  if (SYMMETRIC_CIPHER_AES == pcipher_context->cipher_type) {  // symmetric encrypt
    if (NULL == out || NULL == true_length) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        "out or true_length is NULL, out:%p true_length:%p", out, true_length);
      return eFrameworkunifiedStatusInvldParam;
    }
    uint32_t block_size = pcipher_context->cipher_parameter.symmetric.block_size;
    if (out_len < block_size) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "out_len is less than block_size");
      return eFrameworkunifiedStatusInvldParam;
    }
    if (true == pcipher_context->cipher_parameter.symmetric.to_pad) {
      // Padding on, the true_length is equal to block_size.
      *true_length = block_size;
    } else {
      // Padding off, true_length is equal to 0.
      *true_length = 0;
    }
  } else if (ASYMMETRIC_CIPHER_RSA == pcipher_context->cipher_type) {
    // EncryptFinish is useless for RSA asymmetric encrypt. So do nothing.
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
      "the cipher_type isn't support, cipher_type:%d", pcipher_context->cipher_type);
    return eFrameworkunifiedStatusInvldParam;
  }
  return eFrameworkunifiedStatusOK;
}

// Clean up encrypt context information
EFrameworkunifiedStatus EncryptCleanup(void* ctx) {
  if (NULL == ctx) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "ctx is NULL");
    return eFrameworkunifiedStatusInvldParam;
  }
  memset(ctx, 0, sizeof(CipherContext));
  free(ctx);
  return eFrameworkunifiedStatusOK;
}

// Initialize the decrypt context information
EFrameworkunifiedStatus DecryptStart(enum CipherType cipher_type, union CipherParameter* param,
                        union KeyParam *key, void** ctx) {
  if (SYMMETRIC_CIPHER_AES != cipher_type && ASYMMETRIC_CIPHER_RSA != cipher_type) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "the cipher_type isn't support");
    return eFrameworkunifiedStatusInvldParam;
  }
  if (NULL == param || NULL == key || NULL == ctx) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
      "param, key or ctx is NULL, param:%p key:%p ctx:%p", param, key, ctx);
    return eFrameworkunifiedStatusInvldParam;
  }
  bool ret = CheckParameterVaildity(cipher_type, param, key);
  if (true != ret) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "parameter error");
    return eFrameworkunifiedStatusInvldParam;
  }

  void* ctx_temp = malloc(sizeof(CipherContext));
  if (NULL == ctx_temp) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to malloc %lu byte for ctx, errno=%d",
           sizeof(CipherContext), errno);
    return eFrameworkunifiedStatusFail;
  }
  memset(ctx_temp, 0, sizeof(CipherContext));
  CipherContext* pcipher_context = reinterpret_cast<CipherContext*>(ctx_temp);
  pcipher_context->cipher_type = cipher_type;
  pcipher_context->cipher_parameter = *param;
  pcipher_context->key_param = *key;
  *ctx = ctx_temp;
  return eFrameworkunifiedStatusOK;
}

// Decrypt ciphertext information
EFrameworkunifiedStatus DecryptUpdate(void* ctx, const uint8_t* in, uint32_t in_len,
                      uint8_t* out, uint32_t out_len, uint32_t* true_length) {
  if (NULL == ctx || NULL == in || NULL == out || NULL == true_length) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
      "ctx, in, out or true_length is NULL, ctx:%p in:%p out:%p true_length:%p",
      ctx, in, out, true_length);
    return eFrameworkunifiedStatusInvldParam;
  }

  CipherContext* pcipher_context = reinterpret_cast<CipherContext*>(ctx);
  if (SYMMETRIC_CIPHER_AES == pcipher_context->cipher_type) {  // symmetric decrypt
    if (0 == in_len) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "in_len is equal to 0");
      return eFrameworkunifiedStatusInvldParam;
    }
    uint32_t block_size = pcipher_context->cipher_parameter.symmetric.block_size;
    if (out_len < in_len + block_size) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "out_len is less than in_len plus block_size");
      return eFrameworkunifiedStatusInvldParam;
    }
    memcpy(out, in, in_len);
    *true_length = in_len;
  } else if (ASYMMETRIC_CIPHER_RSA == pcipher_context->cipher_type) {  // asymmetric decrypt
    if (RSA_PRIVATE_EXPONENT_MAX_SIZE/BITS_PER_BYTE != in_len) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        "in_len isn't equal to RSA_PRIVATE_EXPONENT_MAX_SIZE/BITS_PER_BYTE");
      return eFrameworkunifiedStatusInvldParam;
    }
    if (SHA256_DIGEST_LENGTH > out_len) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        "out_len is less than SHA256_DIGEST_LENGTH");
      return eFrameworkunifiedStatusInvldParam;
    }
    memcpy(out, in, SHA256_DIGEST_LENGTH);
    *true_length = SHA256_DIGEST_LENGTH;
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
      "the cipher_type isn't support, cipher_type:%d", pcipher_context->cipher_type);
    return eFrameworkunifiedStatusInvldParam;
  }

  return eFrameworkunifiedStatusOK;
}

// Decrypt the final ciphertext information
EFrameworkunifiedStatus DecryptFinish(void* ctx, uint8_t* out, uint32_t out_len, uint32_t* true_length) {
  if (NULL == ctx) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "ctx is NULL");
    return eFrameworkunifiedStatusInvldParam;
  }
  CipherContext* pcipher_context = reinterpret_cast<CipherContext*>(ctx);
  if (SYMMETRIC_CIPHER_AES == pcipher_context->cipher_type) {  // symmetric encrypt
    if (NULL == out || NULL == true_length) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        "out or true_length is NULL, out:%p true_length:%p", out, true_length);
      return eFrameworkunifiedStatusInvldParam;
    }
    uint32_t block_size = pcipher_context->cipher_parameter.symmetric.block_size;
    if (out_len < block_size) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "out_len is less than block_size");
      return eFrameworkunifiedStatusInvldParam;
    }
    if (true == pcipher_context->cipher_parameter.symmetric.to_pad) {
      // Padding on, the true_length is equal to block_size - padding_length. Because security_hal
      // is stub implement, padding_length is unknown. Set true_length to 0.
      *true_length = 0;
    } else {
      // Padding off, true_length is equal to 0.
      *true_length = 0;
    }
  } else if (ASYMMETRIC_CIPHER_RSA == pcipher_context->cipher_type) {
    // EncryptFinish is useless for RSA asymmetric decrypt. So do nothing.
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
      "the cipher_type isn't support, cipher_type:%d", pcipher_context->cipher_type);
    return eFrameworkunifiedStatusInvldParam;
  }
  return eFrameworkunifiedStatusOK;
}

// Clean up decrypt context information
EFrameworkunifiedStatus DecryptCleanup(void* ctx) {
  if (NULL == ctx) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "ctx is NULL");
    return eFrameworkunifiedStatusInvldParam;
  }
  memset(ctx, 0, sizeof(CipherContext));
  free(ctx);
  return eFrameworkunifiedStatusOK;
}

// Initialize hash context information
EFrameworkunifiedStatus HashStart(enum HashType hash_type, void** ctx) {
  if (HASH_TYPE_MD5 > hash_type || HASH_TYPE_SHA512 < hash_type) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "the hash_type isn't support");
    return eFrameworkunifiedStatusInvldParam;
  }
  if (NULL == ctx) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "ctx is NULL");
    return eFrameworkunifiedStatusInvldParam;
  }
  void* ctx_temp = malloc(sizeof(HashContext));
  if (NULL == ctx_temp) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to malloc %lu byte for ctx, errno=%d",
           sizeof(HashContext), errno);
    return eFrameworkunifiedStatusFail;
  }
  memset(ctx_temp, 0, sizeof(HashContext));
  HashContext* phash_context = reinterpret_cast<HashContext*>(ctx_temp);
  phash_context->hash_type = hash_type;
  *ctx = ctx_temp;
  return eFrameworkunifiedStatusOK;
}

// Caculate hash value of input data
EFrameworkunifiedStatus HashUpdate(void* ctx, const uint8_t* in, uint32_t in_len) {
  if (NULL == ctx || NULL == in) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "ctx or in is NULL, ctx:%p in:%p", ctx, in);
    return eFrameworkunifiedStatusInvldParam;
  }
  if (0 == in_len) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "in_len is equal to 0");
    return eFrameworkunifiedStatusInvldParam;
  }
  return eFrameworkunifiedStatusOK;
}

// Caculate final message digest
EFrameworkunifiedStatus HashFinish(void* ctx, uint8_t* out, uint32_t out_len, uint32_t* true_length) {
  if (NULL == ctx || NULL == out || NULL == true_length) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
      "ctx, out or true_length is NULL, ctx:%p out:%p true_length:%p",
      ctx, out, true_length);
    return eFrameworkunifiedStatusInvldParam;
  }
  HashContext* phash_context = reinterpret_cast<HashContext*>(ctx);
  uint32_t digest_length = 0;
  switch (phash_context->hash_type) {
    case HASH_TYPE_MD5:
      digest_length = HASH_TYPE_MD5;
      break;
    case HASH_TYPE_SHA1:
      digest_length = SHA1_DIGEST_LENGTH;
      break;
    case HASH_TYPE_SHA224:
      digest_length = SHA224_DIGEST_LENGTH;
      break;
    case HASH_TYPE_SHA256:
      digest_length = SHA256_DIGEST_LENGTH;
      break;
    case HASH_TYPE_SHA384:
      digest_length = SHA384_DIGEST_LENGTH;
      break;
    case HASH_TYPE_SHA512:
      digest_length = SHA512_DIGEST_LENGTH;
      break;
    default:
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        "the hash_type isn't support, hash_type:%d", phash_context->hash_type);
      return eFrameworkunifiedStatusInvldParam;
  }
  if (out_len < digest_length) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "out_len is less than %u", digest_length);
    return eFrameworkunifiedStatusInvldParam;
  }
  memset(out, 0x00, digest_length);
  *true_length = digest_length;
  return eFrameworkunifiedStatusOK;
}

// Clean up hash context information
EFrameworkunifiedStatus HashCleanup(void* ctx) {
  if (NULL == ctx) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "ctx is NULL");
    return eFrameworkunifiedStatusInvldParam;
  }
  memset(ctx, 0, sizeof(HashContext));
  free(ctx);
  return eFrameworkunifiedStatusOK;
}

// Initialize random number context information
EFrameworkunifiedStatus RandomInit(void** ctx, uint8_t* seed_buffer, uint32_t buffer_len) {
  if (NULL == ctx || NULL == seed_buffer) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
      "ctx or seed_buffer is NULL, ctx:%p seed_buffer:%p", ctx, seed_buffer);
    return eFrameworkunifiedStatusInvldParam;
  }
  if (0 == buffer_len) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "buffer_len is equal to 0");
    return eFrameworkunifiedStatusInvldParam;
  }
  void* ctx_temp = malloc(sizeof(RandomContext));
  if (NULL == ctx_temp) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to malloc %lu byte for ctx, errno=%d",
           sizeof(RandomContext), errno);
    return eFrameworkunifiedStatusFail;
  }
  memset(ctx_temp, 0, sizeof(RandomContext));
  RandomContext* prandom_context = reinterpret_cast<RandomContext*>(ctx_temp);
  prandom_context->seed_buffer = reinterpret_cast<uint8_t*>(malloc(buffer_len));
  if (NULL == prandom_context->seed_buffer) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to malloc %d byte for seed_buffer, errno=%d",
           buffer_len, errno);
    free(ctx_temp);
    ctx_temp = NULL;
    return eFrameworkunifiedStatusFail;
  }
  memcpy(prandom_context->seed_buffer, seed_buffer, buffer_len);
  prandom_context->buffer_len = buffer_len;
  *ctx = ctx_temp;
  return eFrameworkunifiedStatusOK;
}

// Get random number
EFrameworkunifiedStatus RandomGet(void* ctx, uint8_t* out, uint32_t out_len, uint32_t* true_length) {
  if (NULL == ctx || NULL == out || NULL == true_length) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
      "ctx, out or true_length is NULL, ctx:%p out:%p true_length:%p",
      ctx, out, true_length);
    return eFrameworkunifiedStatusInvldParam;
  }
  if (0 == out_len) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "out_len is equal to 0");
    return eFrameworkunifiedStatusInvldParam;
  }
  // Because security_hal is stub implement, don't assignment value to out or true_length.
  return eFrameworkunifiedStatusOK;
}

// Clean up random number context information
EFrameworkunifiedStatus RandomCleanup(void* ctx) {
  if (NULL == ctx) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "ctx is NULL");
    return eFrameworkunifiedStatusInvldParam;
  }
  RandomContext* prandom_context;
  prandom_context = reinterpret_cast<RandomContext*>(ctx);
  if (NULL != prandom_context->seed_buffer) {
    memset(prandom_context->seed_buffer, 0, prandom_context->buffer_len);
    free(prandom_context->seed_buffer);
    prandom_context->seed_buffer = NULL;
  }
  memset(prandom_context, 0, sizeof(RandomContext));
  free(prandom_context);
  prandom_context = NULL;
  return eFrameworkunifiedStatusOK;
}

// Reset Security IC
EFrameworkunifiedStatus ResetSecurityIC(void) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
  return eFrameworkunifiedStatusOK;
}
