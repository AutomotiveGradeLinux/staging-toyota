/*
 * @copyright Copyright (c) 2017-2020 TOYOTA MOTOR CORPORATION.
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

#ifndef HAL_API_SECURITY_HAL_H_
#define HAL_API_SECURITY_HAL_H_

#include <stdint.h>
#include <native_service/frameworkunified_types.h>

/**
 * @file security_hal.h
 */

/** @addtogroup update_service
 *  @{
 */
/** @addtogroup security_hal
 *  @ingroup update_service
 *  @{
 */

/**
 * \~english  The max size of RSA modulus
 */
#define RSA_MODULUS_MAX_SIZE 2048
/**
 * \~english  The max size of RSA public exponent
 */
#define RSA_PUBLIC_EXPONENT_MAX_SIZE 2048
/**
 * \~english  The max size of RSA private exponent
 */
#define RSA_PRIVATE_EXPONENT_MAX_SIZE  2048
/**
 * \~english  The minimum size of RSA padding
 */
#define RSA_PADDING_MINIMUM_SIZE 11

/**
 * \~english  Cipher type
 */
enum CipherType {
  /**
   * \~english  AES symmetric cipher
   */
  SYMMETRIC_CIPHER_AES = 1,
  /**
   * \~english  BLOWFISH symmetric cipher
   */
  SYMMETRIC_CIPHER_BLOWFISH,
  /**
   * \~english  CAST5 symmetric cipher
   */
  SYMMETRIC_CIPHER_CAST5,
  /**
   * \~english  DES symmetric cipher
   */
  SYMMETRIC_CIPHER_DES,
  /**
   * \~english  DESEDA symmetric cipher
   */
  SYMMETRIC_CIPHER_DESEDA,
  /**
   * \~english  DESEDA3 symmetric cipher
   */
  SYMMETRIC_CIPHER_DESEDA3,
  /**
   * \~english  IDEA symmetric cipher
   */
  SYMMETRIC_CIPHER_IDEA,
  /**
   * \~english  RC2 symmetric cipher
   */
  SYMMETRIC_CIPHER_RC2,
  /**
   * \~english  RC4 symmetric cipher
   */
  SYMMETRIC_CIPHER_RC4,
  /**
   * \~english  RC5 symmetric cipher
   */
  SYMMETRIC_CIPHER_RC5,
  /**
   * \~english  RSA asymmetric cipher
   */
  ASYMMETRIC_CIPHER_RSA,
  /**
   * \~english  DSA asymmetric cipher
   */
  ASYMMETRIC_CIPHER_DSA,
  /**
   * \~english  ECDSA asymmetric cipher
   */
  ASYMMETRIC_CIPHER_ECDSA
};

/**
 * \~english  Symmetric Cipher Mode
 */
enum SymmetricCipherMode {
  /**
   * \~english  Stream symmetric cipher mode
   */
  SYMMETRIC_CIPHER_MODE_STREAM    = 0x00000001,
  /**
   * \~english  ECB symmetric cipher mode
   */
  SYMMETRIC_CIPHER_MODE_BLOCK_ECB = 0x00000002,
  /**
   * \~english  CBC symmetric cipher mode
   */
  SYMMETRIC_CIPHER_MODE_BLOCK_CBC = 0x00000004,
  /**
   * \~english  CFB symmetric cipher mode
   */
  SYMMETRIC_CIPHER_MODE_BLOCK_CFB = 0x00000008,
  /**
   * \~english  OFB symmetric cipher mode
   */
  SYMMETRIC_CIPHER_MODE_BLOCK_OFB = 0x00000010
};

/**
 * \~english  Symmetric cipher Block Size
 */
enum SymmetricCipherBlockSize {
  /**
   * \~english  Symmetric cipher block size is 4 bytes
   */
  SYMMETRIC_CIPHER_BLOCK_SIZE_4    = 0x00000001,
  /**
   * \~english  Symmetric cipher block size is 8 bytes
   */
  SYMMETRIC_CIPHER_BLOCK_SIZE_8    = 0x00000002,
  /**
   * \~english  Symmetric cipher block size is 16 bytes
   */
  SYMMETRIC_CIPHER_BLOCK_SIZE_16   = 0x00000004,
  /**
   * \~english  Symmetric cipher block size is 32 bytes
   */
  SYMMETRIC_CIPHER_BLOCK_SIZE_32   = 0x00000008
};

/**
 * \~english  Symmetric Cipher Round
 */
enum SymmetricCipherRound {
  /**
   * \~english  Symmetric cipher round is 1
   */
  SYMMETRIC_CIPHER_ROUND_1    = 0x00000001,
  /**
   * \~english  Symmetric cipher round is 2
   */
  SYMMETRIC_CIPHER_ROUND_2    = 0x00000002,
  /**
   * \~english  Symmetric cipher round is 8.5
   */
  SYMMETRIC_CIPHER_ROUND_8_5  = 0x00000004,
  /**
   * \~english  Symmetric cipher round is 10
   */
  SYMMETRIC_CIPHER_ROUND_10   = 0x00000008,
  /**
   * \~english  Symmetric cipher round is 12
   */
  SYMMETRIC_CIPHER_ROUND_12   = 0x00000010,
  /**
   * \~english  Symmetric cipher round is 14
   */
  SYMMETRIC_CIPHER_ROUND_14   = 0x00000020,
  /**
   * \~english  Symmetric cipher round is 16
   */
  SYMMETRIC_CIPHER_ROUND_16   = 0x00000040
};

/**
 * \~english  Asymmetric Padding Mode
 */
enum AsymmetricPaddingMode {
  /**
   * \~english  RSA PKCS1 asymmetric padding mode
   */
  ASYMMETRIC_PADDING_MODE_RSA_PKCS1 = 1,
  /**
   * \~english  RSA SSLV23 asymmetric padding mode
   */
  ASYMMETRIC_PADDING_MODE_RSA_SSLV23,
  /**
   * \~english  RSA NOPADDING asymmetric padding mode
   */
  ASYMMETRIC_PADDING_MODE_RSA_NOPADDING,
  /**
   * \~english  RSA OAEP asymmetric padding mode
   */
  ASYMMETRIC_PADDING_MODE_RSA_OAEP,
  /**
   * \~english  RSA PSS asymmetric padding mode
   */
  ASYMMETRIC_PADDING_MODE_RSA_PSS
};

/**
 * @union CipherParameter
 * \~english @par Brief
 *          Union of cipher parameter
 */
union CipherParameter {
  /**
   * \~english  Struct of symmetric cipher parameter
   */
  struct SymmetricCipherParameter {
    /**
     * \~english Symmetric cipher mode
     */
    enum SymmetricCipherMode  mode;
    /**
     * \~english Symmetric cipher block size
     */
    enum SymmetricCipherBlockSize block_size;
    /**
     * \~english Symmetric cipher round
     */
    enum SymmetricCipherRound round;
    /**
     * \~english Padding ON/OFF. Only used in CBC mode and ECB mode.
     *           Padding ON :
     *               Encrypt:
     *                   1.If plaintext length isn't aligned with block size, the
     *                     ciphertext length is aligned with block size.
     *                   2.If plaintext length is aligned with block size, the
     *                     ciphertext length is equal to plaintext length + 1 block size.
     *               Decrypt:
     *                   1.If ciphertext length isn't aligned with block size, return error.
     *                   2.If ciphertext length is aligned with block size, plaintext length
     *                     is equal to ciphertext length minus padding length. Padding length
     *                     is greater than 0 and not greater than block size.
     *           Padding OFF:
     *               Encrypt:
     *                   1.If plaintext length isn't aligned with block size, return error.
     *                   2.If plaintext length is aligned with block size, the
     *                     ciphertext length is equal to plaintext length.
     *               Decrypt:
     *                   1.If ciphertext length isn't aligned with block size, return error.
     *                   2.If ciphertext length is aligned with block size, the
     *                     plaintext length is equal to ciphertext length.
     */
    bool to_pad;
  } symmetric;

  /**
   * \~english  Struct of asymmetric cipher parameter
   */
  struct AsymmetricCipherParameter {
    /**
     * \~english Asymmetric padding mode
     */
    enum AsymmetricPaddingMode mode;
  } asymmetric;
};


/**
 * \~english Symmetric cipher key type
 */
enum SymmetricCipherKeyType {
  /**
   * \~english Key is managed by hardware/chip
   */
  SYMMETRIC_CIPHER_KEY_TYPE_MANAGED,
  /**
   * \~english Key is provided by user
   */
  SYMMETRIC_CIPHER_KEY_TYPE_USER
};

/**
 * @struct RsaPrivateKey
 * \~english @par Brief
 *                Struct of RSA private key
 */
struct RsaPrivateKey {
  /**
   * \~english Private exponent
   */
  uint8_t d[RSA_PRIVATE_EXPONENT_MAX_SIZE];
  /**
   * \~english Modulus
   */
  uint8_t n[RSA_MODULUS_MAX_SIZE];
  /**
   * \~english The length of private exponent
   */
  uint32_t d_length;
  /**
   * \~english The length of modulus
   */
  uint32_t n_length;
};

/**
 * @union PrivateKey
 * \~english @par Brief
 *          Union of private key
 */
union PrivateKey {
  /**
   * \~english Rsa private key
   */
  struct RsaPrivateKey rsa;
};

/**
 * @struct RsaPublicKey
 * \~english @par Brief
 *          Union of RSA public key
 */
struct RsaPublicKey {
  /**
   * \~english Public exponent
   */
  uint8_t e[RSA_PUBLIC_EXPONENT_MAX_SIZE];
  /**
   * \~english Modulus
   */
  uint8_t n[RSA_MODULUS_MAX_SIZE];
  /**
   * \~english The length of public exponent
   */
  uint32_t e_length;
  /**
   * \~english  The length of modulus
   */
  uint32_t n_length;
};

/**
 * @union PublicKey
 * \~english @par Brief
 *          Union of public key
 */
union PublicKey {
  /**
   * \~english Struct of rsa public key
   */
  struct RsaPublicKey rsa;
};

/**
 * \~english Asymmetric cipher key type
 */
enum AsymmetricCipherKeyType {
  /**
   * \~english Key is managed by hardware/chip
   */
  ASYMMETRIC_CIPHER_KEY_TYPE_MANAGED,
  /**
   * \~english Public key is provided by user
   */
  ASYMMETRIC_CIPHER_KEY_TYPE_USER_PUBLIC,
  /**
   * \~english Private key is provided by user
   */
  ASYMMETRIC_CIPHER_KEY_TYPE_USER_PRIVATE
};

/**
 * @union KeyParam
 * \~english @par Brief
 *          Union of key parameter
 */
union KeyParam {
  /**
   * \~english  Struct of symmetric key paramater
   */
  struct SymmetricKeyParam {
    /**
     * \~english Symmetric cipher key type
     */
    enum SymmetricCipherKeyType key_type;
    /**
     * \~english  Union of symmetric key parameter
     */
    union {
      /**
       * \~english  The symmetric key index
       */
      uint32_t managed_key_index;
      /**
       * \~english  Struct of user key
       */
      struct {
        /**
         * \~english  Key source
         */
        uint8_t* key;
        /**
         * \~english  Key length
         */
        uint32_t key_len;
        /**
         * \~english  Key Initialization vector.
         */
        uint8_t* iv;
        /**
         * \~english  Initialization vector length.The length should be
         *            equal to block size, otherwise return error.
         */
        uint32_t iv_len;
      } user_key;
    } key_param;
  } symmetric;
  /**
   * \~english  Struct of asymmetric key parameter
   */
  struct AsymmetricKeyParam {
    /**
     * \~english Asymmetric cipher key type
     */
    enum AsymmetricCipherKeyType key_type;
    /**
     * \~english  Union of asymmetric key parameter
     */
    union {
      /**
       * \~english  The asymmetric key index
       */
      uint32_t managed_key_index;
      /**
       * \~english  Union of user key
       */
      union {
        /**
         * \~english Public key
         */
        union PublicKey* public_key;
        /**
         * \~english Private key
         */
        union PrivateKey* private_key;
      } user_key;
    } key_param;
  } asymmetric;
};

/**
 * \~english  hash type
 */
enum HashType {
  /**
   * \~english  hash type is md5
   */
  HASH_TYPE_MD5 = 1,
  /**
   * \~english  hash type is sha1
   */
  HASH_TYPE_SHA1,
  /**
   * \~english  hash type is sha224
   */
  HASH_TYPE_SHA224,
  /**
   * \~english  hash type is sha256
   */
  HASH_TYPE_SHA256,
  /**
   * \~english  hash type is sha384
   */
  HASH_TYPE_SHA384,
  /**
   * \~english  hash type is sha512
   */
  HASH_TYPE_SHA512
};

/**
 * \ingroup EncryptStart
 * \~english @par Brief
 *        Initialize the encrypt context information.
 * \~english @param [in] cipher_type
 *        enum CipherType    - Cipher type.
 * \~english @param [in] param
 *        union CipherParameter*      - Cipher parameter
 * \~english @param [in] key
 *        union KeyParam*      - Key parameter
 * \~english @param [out] ctx
 *        void**      - The encrypt information context handle.
 * \~english @retval eFrameworkunifiedStatusOK :          indicates success
 * \~english @retval eFrameworkunifiedStatusFail :        indicates the hardware/handle initialize
 *                                           fail or hardware I/O operation fail or
 *                                           hardware device busy or allocate context handle fail.
 * \~english @retval eFrameworkunifiedStatusInvldParam :  indicates parameter error.
 * \~english @par Prerequisite
 *        - None.
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *        - If the parameter "cipher_type" is less than SYMMETRIC_CIPHER_AES
 *          or greater than ASYMMETRIC_CIPHER_ECDSA [eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "param" is NULL [eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "key" is NULL [eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "ctx" is NULL [eFrameworkunifiedStatusInvldParam]
 *        - If the hardware/handle initialize failed [eFrameworkunifiedStatusFail]
 *        - If the hardware I/O operation failed [eFrameworkunifiedStatusFail]
 *        - If the hardware device is busy [eFrameworkunifiedStatusFail]
 *        - If ctx can not be allocated [eFrameworkunifiedStatusFail]
 * \~english @par Detail
 *        - Initialize the encrypt context information according to cipher type,
 *          cipher parameter and key parameter.
 *        - If EncryptStart() was called successfully, EncryptCleanup() should
 *          be called to clean up encrypt context information.
 *        - The API can be used by multiple processes.
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        EncryptUpdate, EncryptFinish, EncryptCleanup
 */
EFrameworkunifiedStatus EncryptStart(enum CipherType cipher_type, union CipherParameter* param,
                        union KeyParam* key, void** ctx);

/**
 * \ingroup EncryptUpdate
 * \~english @par Brief
 *        Encrypt plaintext information.
 * \~english @param [in] ctx
 *        void*    - The encrypt information context handle.
 * \~english @param [in] in
 *        const uint8_t*      - The plaintext to be encrypted.
 * \~english @param [in] in_len
 *        uint32_t      - The input buffer length
 *                        in_len shall be greater than 0 for symmetric encrypt.
 *                        in_len shall be greater than 0 and not greater than
 *                        RSA_PRIVATE_EXPONENT_MAX_SIZE/8 - RSA_PADDING_MINIMUM_SIZE
 *                        for RSA asymmetric encrypt.
 * \~english @param [out] out
 *        uint8_t*      - The ciphertext after plaintext had been encrypted.
 * \~english @param [in] out_len
 *        uint32_t      - The output buffer length.
 *                        out_len shall not be less than in_len + 1 block size for symmetric encrypt.
 *                        out_len shall not be less than RSA_PRIVATE_EXPONENT_MAX_SIZE/8 for RSA asymmetric encrypt.
 * \~english @param [out] true_length
 *        uint32_t*      - The ciphertext length.
 * \~english @retval eFrameworkunifiedStatusOK :          indicates success
 * \~english @retval eFrameworkunifiedStatusFail :        indicates the hardware/handle initialize fail, hardware I/O operation fail,
 *                                           hardware device busy or allocate context handle fail.
 * \~english @retval eFrameworkunifiedStatusInvldParam :  indicates parameter error.
 * \~english @par Prerequisite
 *        - EncryptStart() was called successfully.
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur
 * \~english @par Conditions of processing failure
 *        - If the parameter "ctx" is NULL [eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "in" is NULL [eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "in_len" is 0 for symmetric encrypt[eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "in_len" is 0 or greater than
 *          RSA_PRIVATE_EXPONENT_MAX_SIZE/8 - RSA_PADDING_MINIMUM_SIZE for RSA asymmetric encrypt[eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "out is NULL [eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "out_len" is less than in_len + 1 block size
 *          for symmetric encrypt [eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "out_len" is less than RSA_PRIVATE_EXPONENT_MAX_SIZE/8
 *          for RSA asymmetric encrypt [eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "true_length" is NULL [eFrameworkunifiedStatusFail]
 *        - If the hardware/handle init failed [eFrameworkunifiedStatusFail]
 *        - If the hardware I/O operation failed [eFrameworkunifiedStatusFail]
 *        - If the hardware device is busy [eFrameworkunifiedStatusFail]
 *        - If ctx can not be allocated [eFrameworkunifiedStatusFail]
 * \~english @par Detail
 *        - Update the plaintext to ciphertext by symmetric or asymmetric encrypt algorithm.
 *        - The API can be used by multiple processes.
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        EncryptStart, EncryptFinish, EncryptCleanup
 */
EFrameworkunifiedStatus EncryptUpdate(void* ctx, const uint8_t* in, uint32_t in_len,
                         uint8_t* out, uint32_t out_len, uint32_t* true_length);

/**
 * \ingroup EncryptFinish
 * \~english @par Brief
 *        Encrypt the final plaintext information.
 * \~english @param [in] ctx
 *        void*    - The encrypt information context handle.
 * \~english @param [out] out
 *        uint8_t*      - The ciphertext after plaintext had been encrypted.
 *                        out is useless for RSA asymmetric encrypt.
 * \~english @param [in] out_len
 *        uint32_t      - The final output buffer length.
 *                        out_len shall not be less than 1 block size for symmetric encrypt.
 *                        out_len is useless for RSA asymmetric encrypt.
 * \~english @param [out] true_length
 *        uint32_t*      - The ciphertext length.
 *                         true_length is useless for RSA asymmetric encrypt.
 * \~english @retval eFrameworkunifiedStatusOK :          indicates success
 * \~english @retval eFrameworkunifiedStatusFail :        indicates the hardware/handle initialize fail, hardware I/O operation fail,
 *                                           hardware device busy or allocate context handle fail.
 * \~english @retval eFrameworkunifiedStatusInvldParam :  indicates parameter error.
 * \~english @par Prerequisite
 *        - EncryptStart() and EncryptUpdate() were called successfully.
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur
 * \~english @par Conditions of processing failure
 *        - If the parameter "ctx" is NULL [eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "out" is NULL [eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "out_len" is less than 1 block size
 *          for symmetric encrypt [eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "true_length" is NULL [eFrameworkunifiedStatusInvldParam]
 *        - If the hardware/handle init failed [eFrameworkunifiedStatusFail]
 *        - If the hardware I/O operation failed [eFrameworkunifiedStatusFail]
 *        - If the hardware device is busy [eFrameworkunifiedStatusFail]
 *        - If ctx can not be allocated [eFrameworkunifiedStatusFail]
 * \~english @par Detail
 *        - Update the final plaintext to ciphertext for symmetric encrypt and do nothing for asymmetric encrypt.
 *        - The API can be used by multiple processes.
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        EncryptStart, EncryptUpdate, EncryptCleanup
 */
EFrameworkunifiedStatus EncryptFinish(void* ctx, uint8_t* out, uint32_t out_len, uint32_t* true_length);

/**
 * \ingroup EncryptCleanup
 * \~english @par Brief
 *        Clean up encrypt context information.
 * \~english @param [in] ctx
 *        void*    - The encrypt information context handle.
 * \~english @retval eFrameworkunifiedStatusOK :          indicates success
 * \~english @retval eFrameworkunifiedStatusInvldParam :  indicates parameter error.
 * \~english @par Prerequisite
 *        - EncryptStart() was called successfully.
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur
 * \~english @par Conditions of processing failure
 *        - If the parameter "ctx" is NULL [eFrameworkunifiedStatusInvldParam]
 * \~english @par Detail
 *        - Clean up encrypt context information.
 *        - The API can be used by multiple processes.
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        EncryptStart, EncryptUpdate, EncryptFinish
 */
EFrameworkunifiedStatus EncryptCleanup(void* ctx);

/**
 * \ingroup DecryptStart
 * \~english @par Brief
 *        Initialize the decrypt context information.
 * \~english @param [in] cipher_type
 *        enum CipherType    - Cipher type.
 * \~english @param [in] param
 *        union CipherParameter*      - Cipher parameter
 * \~english @param [in] key
 *        union KeyParam*      - Key parameter
 * \~english @param [out] ctx
 *        void**      - The decrypt information context handle.
 * \~english @retval eFrameworkunifiedStatusOK :          indicates success
 * \~english @retval eFrameworkunifiedStatusFail :        indicates the hardware/handle initialize fail or hardware I/O
 *                                           operation fail or hardware device busy or allocate context handle fail.
 * \~english @retval eFrameworkunifiedStatusInvldParam :  indicates parameter error.
 * \~english @par Prerequisite
 *        - None.
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *        - If the parameter "cipher_type" is less than SYMMETRIC_CIPHER_AES
 *          or greater than ASYMMETRIC_CIPHER_ECDSA [eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "param" is NULL [eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "key" is NULL [eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "ctx" is NULL [eFrameworkunifiedStatusInvldParam]
 *        - If the hardware/handle initialize failed [eFrameworkunifiedStatusFail]
 *        - If the hardware I/O operation failed [eFrameworkunifiedStatusFail]
 *        - If the hardware device is busy [eFrameworkunifiedStatusFail]
 *        - If ctx can not be allocated [eFrameworkunifiedStatusFail]
 * \~english @par Detail
 *        - Initialize the decrypt context information according to cipher type, cipher parameter and key parameter.
 *        - If DecryptStart() was called successfully, DecryptCleanup() should be called
 *          to clean up decrypt context information.
 *        - The API can be used by multiple processes.
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        DecryptUpdate, DecryptFinish, DecryptCleanup
 */
EFrameworkunifiedStatus DecryptStart(enum CipherType cipher_type, union CipherParameter* param,
                        union KeyParam *key, void** ctx);

/**
 * \ingroup DecryptUpdate
 * \~english @par Brief
 *        Decrypt ciphertext information.
 * \~english @param [in] ctx
 *        void*    - The decrypt information context handle.
 * \~english @param [in] in
 *        const uint8_t*      - The ciphertext to be decrypted.
 * \~english @param [in] in_len
 *        uint32_t      - The input buffer length
 *                        in_len shall be greater than 0 for symmetric decrypt.
 *                        in_len shall be equal to RSA_PRIVATE_EXPONENT_MAX_SIZE/8
 *                        for RSA asymmetric decrypt.
 * \~english @param [out] out
 *        uint8_t*      - The plaintext after ciphertext had been decrypted.
 * \~english @param [in] out_len
 *        uint32_t      - The output buffer length.
 *                        out_len shall not be less than in_len + 1 block size for symmetric decrypt.
 *                        out_len shall not be less than RSA_PRIVATE_EXPONENT_MAX_SIZE/8 - RSA_PADDING_MINIMUM_SIZE
 *                        for RSA asymmetric decrypt.
 * \~english @param [out] true_length
 *        uint32_t*      - The plaintext length.
 * \~english @retval eFrameworkunifiedStatusOK :          indicates success
 * \~english @retval eFrameworkunifiedStatusFail :        indicates the hardware/handle initialize fail, hardware I/O operation fail,
 *                                           hardware device busy or allocate context handle fail.
 * \~english @retval eFrameworkunifiedStatusInvldParam :  indicates parameter error.
 * \~english @par Prerequisite
 *        - DecryptStart() was called successfully.
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur
 * \~english @par Conditions of processing failure
 *        - If the parameter "ctx" is NULL [eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "in" is NULL [eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "in_len" is 0 for symmetric decrypt [eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "in_len" isn't equal to
 *          RSA_PRIVATE_EXPONENT_MAX_SIZE/8 for RSA asymmetric decrypt [eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "out is NULL [eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "out_len" is less than in_len
 *          for symmetric decrypt [eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "out_len" is less than RSA_PRIVATE_EXPONENT_MAX_SIZE/8 - RSA_PADDING_MINIMUM_SIZE
 *          for RSA asymmetric decrypt [eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "true_length" is NULL [eFrameworkunifiedStatusInvldParam]
 *        - If the hardware/handle init failed [eFrameworkunifiedStatusFail]
 *        - If the hardware I/O operation failed [eFrameworkunifiedStatusFail]
 *        - If the hardware device is busy [eFrameworkunifiedStatusFail]
 *        - If ctx can not be allocated [eFrameworkunifiedStatusFail]
 * \~english @par Detail
 *        - Update the ciphertext to plaintext by symmetric or asymmetric decrypt algorithm.
 *        - The API can be used by multiple processes.
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        DecryptStart, DecryptFinish, DecryptCleanup
 */
EFrameworkunifiedStatus DecryptUpdate(void* ctx, const uint8_t* in, uint32_t in_len,
                         uint8_t* out, uint32_t out_len, uint32_t* true_length);

/**
 * \ingroup DecryptFinish
 * \~english @par Brief
 *        Decrypt the final ciphertext information.
 * \~english @param [in] ctx
 *        void*    - The decrypt information context handle.
 * \~english @param [out] out
 *        uint8_t*      - The plaintext after ciphertext had been decrypted.
 *                        out is useless for RSA asymmetric decrypt.
 * \~english @param [in] out_len
 *        uint32_t      - The final output buffer length.
 *                        out_len shall not be less than 1 block size for symmetric decrypt.
 *                        out_len is useless for RSA asymmetric decrypt.
 * \~english @param [out] true_length
 *        uint32_t*      - The plaintext length.
 *                         true_length is useless for RSA asymmetric decrypt.
 * \~english @retval eFrameworkunifiedStatusOK :          indicates success
 * \~english @retval eFrameworkunifiedStatusFail :        indicates the hardware/handle initialize fail, hardware I/O operation fail,
 *                                           hardware device busy or allocate context handle fail.
 * \~english @retval eFrameworkunifiedStatusInvldParam :  indicates parameter error.
 * \~english @par Prerequisite
 *        - DecryptStart() and DecryptUpdate() were called successfully.
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur
 * \~english @par Conditions of processing failure
 *        - If the parameter "ctx" is NULL [eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "out" is NULL [eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "out_len" is less than 1 block size
 *          for symmetric decrypt [eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "true_length" is NULL [eFrameworkunifiedStatusInvldParam]
 *        - If the hardware/handle init failed [eFrameworkunifiedStatusFail]
 *        - If the hardware I/O operation failed [eFrameworkunifiedStatusFail]
 *        - If the hardware device is busy [eFrameworkunifiedStatusFail]
 *        - If ctx can not be allocated [eFrameworkunifiedStatusFail]
 * \~english @par Detail
 *        - Update the final ciphertext to plaintext for symmetric decrypt and do nothing for asymmetric decrypt.
 *        - The API can be used by multiple processes.
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        DecryptStart, DecryptUpdate, DecryptCleanup
 */
EFrameworkunifiedStatus DecryptFinish(void* ctx, uint8_t* out, uint32_t out_len, uint32_t* true_length);

/**
 * \ingroup DecryptCleanup
 * \~english @par Brief
 *        Clean up decrypt context information.
 * \~english @param [in] ctx
 *        void*    - The decrypt information context handle.
 * \~english @retval eFrameworkunifiedStatusOK :          indicates success

 * \~english @retval eFrameworkunifiedStatusInvldParam :  indicates parameter error.

 * \~english @par Prerequisite
 *        - DecryptStart() was called successfully.
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur
 * \~english @par Conditions of processing failure
 *        - If the parameter "ctx" is NULL [eFrameworkunifiedStatusInvldParam]
 * \~english @par Detail
 *        - Clean up decrypt context information.
 *        - The API can be used by multiple processes.
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        DecryptStart, DecryptUpdate, DecryptFinish
 */
EFrameworkunifiedStatus DecryptCleanup(void* ctx);

/**
 * \ingroup HashStart
 * \~english @par Brief
 *        Initialize hash context information.
 * \~english @param [in] hash_type
 *        enum HashType    - Hash type.
 * \~english @param [out] ctx
 *        void**      - The hash information context handle.
 * \~english @retval eFrameworkunifiedStatusOK :          indicates success
 * \~english @retval eFrameworkunifiedStatusFail :        indicates the hardware/handle initialize fail or hardware I/O operation
 *                                           fail or hardware device busy or allocate context handle fail.
 * \~english @retval eFrameworkunifiedStatusInvldParam :  indicates parameter error.
 * \~english @par Prerequisite
 *        - None.
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *        - If the parameter "hash_type" less than HASH_TYPE_MD5
 *          or greater than HASH_TYPE_SHA512 [eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "ctx" is NULL [eFrameworkunifiedStatusInvldParam]
 *        - If the hardware/handle initialize failed [eFrameworkunifiedStatusFail]
 *        - If the hardware I/O operation failed [eFrameworkunifiedStatusFail]
 *        - If the hardware device is busy [eFrameworkunifiedStatusFail]
 *        - If ctx can not be allocated [eFrameworkunifiedStatusFail]
 * \~english @par Detail
 *        - Initialize hash context information according to hash type.
 *        - If HashStart() was called successfully, HashCleanup() should be called
 *          to clean up hash context information.
 *        - The API can be used by multiple processes.
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        HashUpdate, HashFinish, HashCleanup
 */
EFrameworkunifiedStatus HashStart(enum HashType hash_type, void** ctx);

/**
 * \ingroup HashUpdate
 * \~english @par Brief
 *        Caculate hash value of input data.
 * \~english @param [in] ctx
 *        void*    - The hash information context handle.
 * \~english @param [in] in
 *        const uint8_t*      - The input data to be hashed.
 * \~english @param [in] in_len
 *        uint32_t      - The input buffer length
 * \~english @retval eFrameworkunifiedStatusOK :          indicates success
 * \~english @retval eFrameworkunifiedStatusFail :        indicates the hardware/handle initialize fail, hardware I/O operation fail,
 *                                           hardware device busy or allocate context handle fail.
 * \~english @retval eFrameworkunifiedStatusInvldParam :  indicates parameter error.
 * \~english @par Prerequisite
 *        - HashStart() was called successfully.
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur
 * \~english @par Conditions of processing failure
 *        - If the parameter "ctx" is NULL [eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "in" is NULL [eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "in_len" is 0 [eFrameworkunifiedStatusInvldParam]
 *        - If the hardware/handle init failed [eFrameworkunifiedStatusFail]
 *        - If the hardware I/O operation failed [eFrameworkunifiedStatusFail]
 *        - If the hardware device is busy [eFrameworkunifiedStatusFail]
 *        - If ctx can not be allocated [eFrameworkunifiedStatusFail]
 * \~english @par Detail
 *        - Caculate hash value of input data.
 *        - The API can be used by multiple processes.
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        HashStart, HashFinish, HashCleanup
 */
EFrameworkunifiedStatus HashUpdate(void* ctx, const uint8_t* in, uint32_t in_len);

/**
 * \ingroup HashFinish
 * \~english @par Brief
 *        Caculate final message digest.
 * \~english @param [in] ctx
 *        void*    - The hash information context handle.
 * \~english @param [out] out
 *        uint8_t*      - The message digest after all input data had been hashed.
 * \~english @param [in] out_len
 *        uint32_t      - The output buffer length.
 * \~english @param [out] true_length
 *        uint32_t*      - The message digest length.
 * \~english @retval eFrameworkunifiedStatusOK :          indicates success
 * \~english @retval eFrameworkunifiedStatusFail :        indicates the hardware/handle initialize fail, hardware I/O operation fail,
 *                                           hardware device busy or allocate context handle fail.
 * \~english @retval eFrameworkunifiedStatusInvldParam :  indicates parameter error.
 * \~english @par Prerequisite
 *        - HashStart() and HashUpdate() were called successfully.
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur
 * \~english @par Conditions of processing failure
 *        - If the parameter "ctx" is NULL [eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "out" is NULL [eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "out_len" is 0 [eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "true_length" is NULL [eFrameworkunifiedStatusInvldParam]
 *        - If the hardware/handle init failed [eFrameworkunifiedStatusFail]
 *        - If the hardware I/O operation failed [eFrameworkunifiedStatusFail]
 *        - If the hardware device is busy [eFrameworkunifiedStatusFail]
 *        - If ctx can not be allocated [eFrameworkunifiedStatusFail]
 * \~english @par Detail
 *        - Caculate final message digest.
 *        - The API can be used by multiple processes.
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        HashStart, HashUpdate, HashCleanup
 */
EFrameworkunifiedStatus HashFinish(void* ctx, uint8_t* out, uint32_t out_len, uint32_t* true_length);

/**
 * \ingroup HashCleanup
 * \~english @par Brief
 *        Clean up hash context information.
 * \~english @param [in] ctx
 *        void*    - The hash information context handle.
 * \~english @retval eFrameworkunifiedStatusOK :          indicates success
 * \~english @retval eFrameworkunifiedStatusInvldParam :  indicates parameter error.
 * \~english @par Prerequisite
 *        - HashStart() was called successfully.
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur
 * \~english @par Conditions of processing failure
 *        - If the parameter "ctx" is NULL [eFrameworkunifiedStatusInvldParam]
 * \~english @par Detail
 *        - Clean up hash context information.
 *        - The API can be used by multiple processes.
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        HashStart, HashUpdate, HashFinish
 */
EFrameworkunifiedStatus HashCleanup(void* ctx);

/**
 * \ingroup RandomInit
 * \~english @par Brief
 *        Initialize random number context information.
 * \~english @param [out] ctx
 *        void**      - The random number information context handle.
 * \~english @param [in] seed_buffer
 *        uint8_t*    - The random number seed buffer.
 * \~english @param [in] buffer_len
 *        uint32_t    - The random number seed buffer length.
 * \~english @retval eFrameworkunifiedStatusOK :          indicates success
 * \~english @retval eFrameworkunifiedStatusFail :        indicates the hardware/handle initialize fail or hardware I/O operation
 *                                           fail or hardware device busy or allocate context handle fail.
 * \~english @retval eFrameworkunifiedStatusInvldParam :  indicates parameter error.
 * \~english @par Prerequisite
 *        - None.
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *        - If the parameter "ctx" is NULL [eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "seed_buffer" is NULL [eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "buffer_len" is 0 [eFrameworkunifiedStatusInvldParam]
 *        - If the hardware/handle initialize failed [eFrameworkunifiedStatusFail]
 *        - If the hardware I/O operation failed [eFrameworkunifiedStatusFail]
 *        - If the hardware device is busy [eFrameworkunifiedStatusFail]
 *        - If ctx can not be allocated [eFrameworkunifiedStatusFail]
 * \~english @par Detail
 *        - Initialize random number context information according to random number seed.
 *        - If RandomInit() was called successfully, RandomCleanup() should be called to
 *          clean up random number context information. If RandomInit() was called faild,
 *          RandomCleanup() must not be called.
 *        - The API can be used by multiple processes.
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        RandomGet, RandomCleanup
 */
EFrameworkunifiedStatus RandomInit(void** ctx, uint8_t* seed_buffer, uint32_t buffer_len);

/**
 * \ingroup RandomGet
 * \~english @par Brief
 *        Get random number.
 * \~english @param [in] ctx
 *        void*    - The random number information context handle.
 * \~english @param [out] out
 *        uint8_t*      - The random number that caculated by random number seed.
 * \~english @param [in] out_len
 *        uint32_t      - The output buffer length.
 * \~english @param [out] true_length
 *        uint32_t*      - The random number length.
 * \~english @retval eFrameworkunifiedStatusOK :          indicates success
 * \~english @retval eFrameworkunifiedStatusFail :        indicates the hardware/handle initialize fail, hardware I/O operation fail,
 *                                           hardware device busy or allocate context handle fail.
 * \~english @retval eFrameworkunifiedStatusInvldParam :  indicates parameter error.
 * \~english @par Prerequisite
 *        - RandomInit() was called successfully.
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur
 * \~english @par Conditions of processing failure
 *        - If the parameter "ctx" is NULL [eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "out" is NULL [eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "out_len" is 0 [eFrameworkunifiedStatusInvldParam]
 *        - If the parameter "true_length" is NULL [eFrameworkunifiedStatusInvldParam]
 *        - If the hardware/handle init failed [eFrameworkunifiedStatusFail]
 *        - If the hardware I/O operation failed [eFrameworkunifiedStatusFail]
 *        - If the hardware device is busy [eFrameworkunifiedStatusFail]
 *        - If ctx can not be allocated [eFrameworkunifiedStatusFail]
 * \~english @par Detail
 *        - Get random number.
 *        - The API can be used by multiple processes.
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        RandomInit, RandomCleanup
 */
EFrameworkunifiedStatus RandomGet(void* ctx, uint8_t* out, uint32_t out_len, uint32_t* true_length);

/**
 * \ingroup RandomCleanup
 * \~english @par Brief
 *        Clean up random number context information.
 * \~english @param [in] ctx
 *        void*    - The random number information context handle.
 * \~english @retval eFrameworkunifiedStatusOK :          indicates success
 * \~english @retval eFrameworkunifiedStatusInvldParam :  indicates parameter error.
 * \~english @par Prerequisite
 *        - RandomInit() was called successfully.
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur
 * \~english @par Conditions of processing failure
 *        - If the parameter "ctx" is NULL [eFrameworkunifiedStatusInvldParam]
 * \~english @par Detail
 *        - Clean up random number context information.
 *        - The API can be used by multiple processes.
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        RandomInit, RandomGet
 */
EFrameworkunifiedStatus RandomCleanup(void* ctx);

/**
 * \ingroup ResetSecurityIC
 * \~english @par Brief
 *        Security IC Reset.
 * \~english @param none
 * \~english @retval eFrameworkunifiedStatusOK :    indicates success
 * \~english @retval eFrameworkunifiedStatusFail :  indicates the hardware initialize fail, hardware I/O operation fail,
 *                                     hardware device busy.
 * \~english @par Prerequisite
 *        - None.
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur
 * \~english @par Conditions of processing failure
 *        - If the hardware initialize failed [eFrameworkunifiedStatusFail]
 *        - If the hardware I/O operation failed [eFrameworkunifiedStatusFail]
 *        - If the hardware device is busy [eFrameworkunifiedStatusFail]
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @par Detail
 *        - Security IC Reset.
 *        - The API can be used by multiple processes.
 *        - This API does not cleanup context information when EncryptStart(), DecryptStart() or HashStart() is successfull.
 *          Use EncryptCleanup(), DecryptCleanup() or RandomCleanup() to cleanup context information.
 * \~english @see
 *        None
 */
EFrameworkunifiedStatus ResetSecurityIC(void);

/** @}*/  // end of security_hal
/** @}*/  // end of update_service

#endif  // HAL_API_SECURITY_HAL_H_
