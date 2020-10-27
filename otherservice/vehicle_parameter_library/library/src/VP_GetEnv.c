/*
 * @copyright Copyright (c) 2016-2020 TOYOTA MOTOR CORPORATION.
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

/* ====================================================================== */
/**
 * @file VP_GetEnv.c
 * @brief API to get vehicle parameter environment variable
 */
/* ====================================================================== */


#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <other_service/VP_GetEnv.h>
#include <native_service/ns_backup.h>
#include <native_service/ns_backup_id.h>

#include "VP_FuncCheck_CanRcv_private.h"

// copy from vpsvc.h
#define VPSVC_COUNTRY_MAX (0x10)          // Maximal number of country codes for 1 destination

// copy from VehicleSens_Vpupdate.cpp

#define VP_VEHICLEPARAMETERLIBRARY_DEST_LEN_MIN  5           // At least 5 characters for 1 destination(e.g. "I:104")
#define VP_VEHICLEPARAMETERLIBRARY_DEST_NUM_MAX  (VP_MAX_LENGTH/VP_VEHICLEPARAMETERLIBRARY_DEST_LEN_MIN)
#define DS_PACK2_LEN_MAX    2+1           // Max. digits of destination package 2(+\0)

typedef struct {
  char dest_bdb[2];                       // Destination symbol
  char ds_pack2[DS_PACK2_LEN_MAX];        // Destination package 2
  uint8_t coutry_num;                     // Number of Country Codes
  uint16_t country_no[VPSVC_COUNTRY_MAX]; // Country Code
} VP_VEHICLEPARAMETERLIBRARY_DEST_C_CODE_t;

VP_VEHICLEPARAMETERLIBRARY_DEST_C_CODE_t g_vp_vehicleparameterlibrary_dest[VP_VEHICLEPARAMETERLIBRARY_DEST_NUM_MAX]; // Destination information get from environment variables
static uint32_t g_vp_vehicleparameterlibrary_dest_num;   // Number of destination information get from environment variables

static void AnalyzeVpVehicleparameterlibraryDestEnv(void) {
  char p_env_variable[VP_MAX_LENGTH] = {};
  char dest_buf[VP_MAX_LENGTH] = { 0 }; // Destination data for environment variables
  char *c_code_ascii;                   // Country Code(ASCII)
  char *dest_bdb_buf;                   // Destination symbol
  uint32_t dest_num = 0;                // Number of destinations
  int country_num = 0;                  // Number of Country Codes per Destination
  char *token1, *saveptr1;              // Destination break token
  char *token2, *saveptr2;              // Delimiter tokens in each destination
  VP_VEHICLEPARAMETERLIBRARY_DEST_C_CODE_t *p_dest;
  uint16_t *p_country;

  g_vp_vehicleparameterlibrary_dest_num = 0;
  memset(&g_vp_vehicleparameterlibrary_dest, 0x00, sizeof(VP_VEHICLEPARAMETERLIBRARY_DEST_C_CODE_t));

  // Get environment variables
  VP_GetEnv(VP_VEHICLEPARAMETERLIBRARY_DEST_C_CODE, p_env_variable);
  if (0 == strncmp(p_env_variable, "", VP_MAX_LENGTH)) {
    DEBUG_PRINT("VP_VEHICLEPARAMETERLIBRARY_DEST_C_CODE is NULL");
    return;
  }

  DEBUG_PRINTF("VP_VEHICLEPARAMETERLIBRARY_DEST_C_CODE=[%s]\n", p_env_variable);

  // Begin analysis  First, separate with ","
  token1 = strtok_r(p_env_variable, ",", &saveptr1);
  while (token1 != NULL) {
    snprintf(dest_buf, VP_MAX_LENGTH, "%s", token1);
    DEBUG_PRINTF("dest_buf\t[%s]", dest_buf);
    p_dest = &g_vp_vehicleparameterlibrary_dest[dest_num];

    // Get the country code  000-999
    country_num = 0;
    c_code_ascii = strchr(dest_buf, (int32_t)(':'));
    if (c_code_ascii != NULL) {
      c_code_ascii += 1;
      token2 = strtok_r(c_code_ascii, "/", &saveptr2);
      while (token2 != NULL) {
        p_country = &(p_dest->country_no[country_num]);
        errno = 0;
        long int val = strtol(token2, (char **)NULL, 10);
        if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
            || (errno != 0 && val == 0)) {
          DEBUG_PRINTF("country No format error [%s]", token2);
          return;
        }
        *p_country = (uint16_t)val;

        if (country_num >= VPSVC_COUNTRY_MAX) {
          DEBUG_PRINTF("country num over [%s]", token2);
          return;
        }
        country_num++;
        token2 = strtok_r(NULL, "/", &saveptr2);
      }
      p_dest->coutry_num = (uint8_t)(country_num);
      DEBUG_PRINTF("country_num\t[%d]", country_num);
    }

    // Get destination symbol (may not be available)
    dest_bdb_buf = strchr(dest_buf, (int32_t)('&'));
    if (dest_bdb_buf != NULL) {
      p_dest->dest_bdb[0] = *(dest_bdb_buf + 1);
      DEBUG_PRINTF("dest_bdb\t[%s]", p_dest->dest_bdb);
    } else {
      DEBUG_PRINT("dest_bdb is noting");
    }

    // Get destination packages
    token2 = strtok_r(dest_buf, "&:", &saveptr2);
    snprintf(p_dest->ds_pack2, DS_PACK2_LEN_MAX, "%s", token2);
    DEBUG_PRINTF("ds_pack2\t[%s]\n", p_dest->ds_pack2);

    dest_num++;
    token1 = strtok_r(NULL, ",", &saveptr1);
  }

  g_vp_vehicleparameterlibrary_dest_num = dest_num;
}

// based VP_DspackToCountryCode
// dest [in] Destination codes read from the bkup_manager
// country_code [out] Country code string, separated by ";" for multi-country codes
static BOOL VP_DspackToCountryCode(uint8_t* dest, char *country_code) {
  BOOL ret = FALSE;
  VP_VEHICLEPARAMETERLIBRARY_DEST_C_CODE_t *p;
  typedef struct {
    char dest_bdb[2];                // Destination symbol
    char ds_pack2[DS_PACK2_LEN_MAX]; // Destination package 2
  } VEHICLE_CAN_DEST_t;
  VEHICLE_CAN_DEST_t can_dest = {};

  AnalyzeVpVehicleparameterlibraryDestEnv();

  // Setting CAN data
  can_dest.dest_bdb[0] = (int8_t)dest[0]; // Get destination symbol

  if (isalpha(dest[2]) != 0) { // Get destination packages
    can_dest.ds_pack2[0] = (int8_t)dest[2];
  } else {
    DEBUG_PRINTF("ds_pack2 is neither Alpha character[%c]", (int32_t)dest[2]);
    return ret;
  }

  // Comparison with vehicle parameter
  for (uint32_t i = 0; i < g_vp_vehicleparameterlibrary_dest_num; i++) {
    p = &g_vp_vehicleparameterlibrary_dest[i];
    // Determination of destination packages
    if (strncmp(p->ds_pack2, can_dest.ds_pack2, sizeof(can_dest.ds_pack2)) != 0) {
      continue;
    }

    // Determination of destination symbol
    if (p->dest_bdb[0] == 0) {
      ret = TRUE; // When the destination code judgment is not required
      break;
    }

    if (strncmp(p->dest_bdb, can_dest.dest_bdb, sizeof(can_dest.dest_bdb)) == 0) {
      ret = TRUE;
      break;
    }
  }

  // Returned as a country code string
  if (ret == TRUE) {
    snprintf(country_code, VP_MAX_LENGTH, "%03d", p->country_no[0]);
    for (uint32_t i = 1; i < p->coutry_num; i++) {
      char buf[VP_MAX_LENGTH] = {};
      snprintf(buf, VP_MAX_LENGTH, "%s;%03d", country_code, p->country_no[i]);
      strncpy(country_code, buf, VP_MAX_LENGTH);
    }
  }
  return ret;
}
// end of copy from VehicleSens_Vpupdate.cpp

// copy from vehicle unit
typedef struct {
  uint8_t uc_hv;             /* hv */
  uint8_t uc_hv_status;      /* hv status */
  uint8_t uc_2wd4wd;         /* 2wd4wd */
  uint8_t uc_2wd4wd_status;  /* 2wd4wd status */
  uint8_t uc_dest[3];        /* Destination */
  uint8_t uc_dest_status;    /* Destination status */
  uint8_t uc_stwheel;        /* STEERING_WHEEL */
  uint8_t uc_stwheel_status; /* STEERING_WHEEL status */
  uint8_t uc_reserve[6];     /* Reserve */
} VEHICLESENS_NON_VOLATILE_DATA;

/* ====================================================================== */
/**
 * @fn
 * void VP_GetVp_CWORD31_Destination(char *pEnvBuff)
 * @breaf Get environment variables
 * @param[out] (pEnvBuff) Start address of the area to store the get environment variable value
 * @return None
 * @detail Read and analyze D_BK_ID_VEHICLE_STABLE_DATA from BackupManager.
 *         The country code string is stored in the argument and returned.
 */
/* ====================================================================== */
static void VP_GetVp_CWORD31_Destination(char *pEnvBuff)
{
  int32_t ret_api = BKUP_RET_NORMAL;
  VEHICLESENS_NON_VOLATILE_DATA pstback_up_data;
  memset(&pstback_up_data, 0x00, sizeof(VEHICLESENS_NON_VOLATILE_DATA));
  char env_string[VP_MAX_LENGTH];

  VP_GetEnv(VP__CWORD31__TELEMATICS_FUNCTION, env_string);

  if ('\0' == env_string[0]) {
    *pEnvBuff = '\0';
    return;
  } else {
    // read from backup
    ret_api = Backup_DataRd(D_BK_ID_VEHICLE_STABLE_DATA,
                            0,
                            &pstback_up_data,
                            sizeof(VEHICLESENS_NON_VOLATILE_DATA));
    if (BKUP_RET_NORMAL != ret_api) {
      *pEnvBuff = '\0';
      return;
    }
  }

  if (0 == strncmp(env_string, "country_ID", strlen("country_ID"))) {
    uint32_t country_code = 0;
    country_code += (pstback_up_data.uc_dest[1]); // countory_code 3rd and 2nd digit
    country_code = (country_code << 4);           // shift
    country_code += (pstback_up_data.uc_dest[2] >> 4); // countory_code 1st digit
    snprintf(pEnvBuff, VP_MAX_LENGTH, "%x", country_code);
  } else if(0 == strncmp(env_string, "CAN_judge", strlen("CAN_judge"))) {
    VP_DspackToCountryCode(pstback_up_data.uc_dest, pEnvBuff);
  } else {
    *pEnvBuff = '\0';
  }
}

/* ====================================================================== */
/**
 * @fn
 * void VP_GetEnv(char *pEnvStr, char *pEnvBuff )
 * @breaf Get Environment Variable
 * @param[in]  (pEnvStr) Pointer to the string of the environment variable name to be gotten
 * @param[out] (pEnvBuff) Start address of the area to store the gotten value of the environment variable
 * @return None
 * @detail Read the value (string) of the environment variable specified by the argument and stores it in the specified address.
 */
/* ====================================================================== */
void VP_GetEnv(const char *pEnvStr, char *pEnvBuff )
{
  const char *env_string;
  size_t length;

  if ((pEnvStr != NULL) && (pEnvBuff != NULL)) {
    if (0 == strncmp(pEnvStr, VP__CWORD31__DESTINATION, strlen(VP__CWORD31__DESTINATION))) {
      VP_GetVp_CWORD31_Destination(pEnvBuff);
    } else {
      env_string = getenv(pEnvStr);

      if (env_string == NULL) {
        *pEnvBuff = '\0';
      } else {
        length = strlen( env_string );
        if (length < ((size_t)VP_MAX_LENGTH)) {
          (void)strcpy(pEnvBuff, env_string);
        } else {
          (void)strncpy(pEnvBuff, env_string, (VP_MAX_LENGTH - 1));
          pEnvBuff[ VP_MAX_LENGTH - 1 ] = '\0';
        }
      }
    }
  } else {
    if(pEnvBuff != NULL) {
      *pEnvBuff = '\0';
    }
  }
}
