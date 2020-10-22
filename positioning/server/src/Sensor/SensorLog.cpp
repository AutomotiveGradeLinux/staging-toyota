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

/**
 * @file
 *   SensorLog.cpp
 * @brief
 *   Sensor Logging service functionality
 */

/*---------------------------------------------------------------------------------*
 * Include Files                                                                   *
 *---------------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#include <positioning_hal.h>
#include <unistd.h>
#include "SensorLog.h"
#include "Sensor_Common_API.h"
#include <zlib.h>
#include "POS_private.h"

/*---------------------------------------------------------------------------------*
 * Definition                                                                      *
 *---------------------------------------------------------------------------------*/
/* Sensor Log Type */
typedef enum {
    SENSLOG_TYPE_SYS_INPUT = 0,                 /* Sensor Log Type-SYS (INPUT)  */
    SENSLOG_TYPE_SYS_OUTPUT,                    /* Sensor Log Type-SYS (OUTPUT) */
    SENSLOG_TYPE_GPS_INPUT,                     /* Sensor Log Type-GPS (INPUT)  */
    SENSLOG_TYPE_GPS_OUTPUT,                    /* Sensor Log Type-GPS (OUTPUT) */
    SENSLOG_TYPE_NAVI_INPUT,                    /* Sensor Log Type-NAVI(INPUT)  */
    SENSLOG_TYPE_NAVI_OUTPUT,                   /* Sensor Log Type-NAVI(OUTPUT) */
    SENSLOG_TYPE_POS_OUTPUT,                    /* Sensor Log Type-POS (OUTPUT) */
    SENSLOG_TYPE_CMD_INPUT,                     /* Sensor Log Type-CMD (INPUT)  */
    SENSLOG_TYPE_CMD_OUTPUT,                    /* Sensor Log Type-CMD (OUTPUT) */

    SENSLOG_TYPE_COUNT                          /* Sensor Log Type-Counting     */
} SENSLOG_TYPE;

#define SENSLOG_VALIDFLAG_ON            (1)     /* Effective specific flag ON              */
#define SENSLOG_VALIDFLAG_OFF           (0)     /* Enable flag OFF             */
#define SENSLOG_FILEPATH_SIZE           (128)   /* Output path size            */
#define SENSLOG_FILENAME_SIZE           (64)    /* Output file name size      */
#define SENSLOG_CONFIG_TEXT_SIZE        (128)   /* config file size/line */
#define SENSLOG_LEN_MIN                 (1)     /* Record Length-Minimum         */
#define SENSLOG_LEN_MAX                 (900)   /* Record Length-Maximum         */
#define SENSLOG_LEN_DEFAULT             (300)   /* Record length-default value   */
#define SENSLOG_GEN_MIN                 (1)     /* Number of generations-minimum value             */
#define SENSLOG_GEN_MAX                 (999)   /* Number of generations-maximum value             */
#define SENSLOG_GEN_DEFAULT             (100)   /* Number of generations-default value       */
#define SENSLOG_BUF_MIN                 (2272)  /* Buffer Size-Minimum         */
#define SENSLOG_BUF_MAX                 (11360) /* Buffer size-maximum value         */
#define SENSLOG_BUF_DEFAULT             (4544)  /* Buffer size-default value   */
#define SENSLOG_OUTPUTFLAG_NEW          (0)     /* Output Flag-New */
#define SENSLOG_OUTPUTFLAG_ADD          (1)     /* Output Flag-Add */
#define SENSLOG_PNAME_MAX               (16)    /* Maximum number of characters of process name */

#define SENSLOG_SEMAPHO_NAME            ("SENSLOG_SEMAPHO")   /* Semaphore name(MAX 32Byte) */

#define SENSLOG_CONFIG_FILE_PATH_1      "/nv/BS/vs/positioning/rwdata/"  /* Sensor log-Config filepath-1      */
#define SENSLOG_CONFIG_FILE_PATH_2      "/mnt/sda1/"       /* Sensor log-Config filepath-2      */
#define SENSLOG_CONFIG_FILE_NAME_SYS    "POS_sys_log.cfg"  /* Sensor log-Config filename-SYS      */
#define SENSLOG_CONFIG_FILE_NAME_GPS    "POS_gps_log.cfg"  /* Sensor log-Config filename-GPS      */
#define SENSLOG_CONFIG_FILE_NAME_NAVI   "POS_nav_log.cfg"  /* Sensor log-Config filename-NAVI     */
#define SENSLOG_CONFIG_FILE_NAME_POS    "POS_pos_log.cfg"  /* Sensor log-Config filename-POS      */
#define SENSLOG_CONFIG_FILE_NAME_CMD    "POS_cmd_log.cfg"  /* Sensor log-Config filename-CMD      */
#define SENSLOG_CONFIG_KEY_LEN          "POS_log_len"      /* Sensor log-config file-Record length */
#define SENSLOG_CONFIG_KEY_GEN          "POS_log_gen"      /* Sensor log-config file-Number of generations     */
#define SENSLOG_CONFIG_KEY_BUF          "POS_log_buf"      /* Sensor log-config file-Buffer size */
#define SENSLOG_NAV_SVINFO_FILE_NAME    "POS_NAV_SVINFO"   /* Sensor log-NAV-SVINFOFile name(Log output when file exists)*/
#define SENSLOG_SEQ_START               ">>>>"             /* Sensor log sequence(Start) */
#define SENSLOG_SEQ_END                 "<<<<"             /* Sensor log sequence(End)   */
#define SENSLOG_SEQ_SIZE                (4)                /* Sensor Log Sequence Size       */

/*---------------------------------------------------------------------------------*
 * Structre                                                                        *
 *---------------------------------------------------------------------------------*/
/*!
   @brief   Structure to create SensorLog data
*/
typedef struct {
    uint16_t us_len;  /* Number of records */
    uint16_t uc_gen;  /* Number of generations */
} SENSLOG_GEN;

/*!
   @brief   Structure to create SensorLog data
*/
typedef struct {
    uint8_t      uc_valid_flag;                       /* Output enable flag            */
    SENSLOG_GEN  st_gen;                              /* Generation information                  */
    uint16_t     us_file_rec_count;                    /* Number of file output records    */
    uint16_t     us_rec_count;                        /* Number of output records            */
    uint16_t     us_gen_count;                        /* Number of generations                    */
    uint8_t      uc_file_path[SENSLOG_FILEPATH_SIZE]; /* Output path                  */
    uint8_t      uc_file_name[SENSLOG_FILENAME_SIZE]; /* Output file name            */
    uint8_t      uc_gen_fname[SENSLOG_FILENAME_SIZE]; /* Generation information output file name    */
    uint8_t      uc_text_buf[SENSLOG_BUF_MAX];        /* Buffering area(Static)  */
    uint32_t     ul_text_buf_size;                     /* Buffering area size  */
    uint32_t     ul_text_buf_len;                      /* Data size in the buffer  */
    uint8_t      uc_output_flag;                      /* Output flag */
    uint8_t      uc_temp_buf[SENSLOG_BUF_MAX];        /* Output data temporary area        */
    uint8_t      uc_positioninglog_buf[SENSLOG_BUF_MAX];      /* FRAMEWORKUNIFIEDLOG outputs          */
} SENSLOG_INFO;

/*!
   @brief   Structure to get System time
*/
typedef struct {
    uint16_t us_year;
    uint16_t us_month;
    uint16_t us_day_of_week;
    uint16_t us_day;
    uint16_t us_hour;
    uint16_t us_minute;
    uint16_t us_second;
    uint16_t us_milli_seconds;
} SENSLOG_SYSTEMTIME;

/*!
   @brief   Structure to create SensorLog data
*/
typedef struct {
    uint16_t            us_data_type;                 /* Data type         */
    SENSLOG_SYSTEMTIME  st_sys_time;                  /* Current time           */
    DID                 ul_did;                      /* Data ID           */
    int8_t              c_pname[SENSLOG_PNAME_MAX];  /* Destination information           */
    uint8_t             uc_unit_type;                 /* Hardware configuration type(GRADE1 / GRADE2) */
    uint8_t             uc_result;                   /* Send/Receive result(Success:0 / Fail:1) */
    uint16_t            us_data_size;                 /* Size of the data       */
} SENSLOG_DATA_HEADER;


/*!
   @brief   Structure to create SensorLog ID table
*/
typedef struct {
    uint16_t            us_data_type;                 /* Data type         */
    DID                 ul_did;                      /* Data ID           */
    uint16_t            us_file_type;                 /* Sensor Log Type     */
    uint16_t            us_write_type;                /* FRAMEWORKUNIFIEDLOG Output Type     */
} SENSLOG_ID_TBL;

/*---------------------------------------------------------------------------------*
 * Local Function Prototype                                                        *
 *---------------------------------------------------------------------------------*/
static void SensLogGetConfig(const uint8_t*, SENSLOG_INFO*);
static void SensLogGetConfigVal(uint8_t*, uint8_t*);
static void SensLogCheckNAVSVINFOFile(void);
static void SensLogWrite(uint16_t, uint16_t, uint16_t, DID did, PNO pno, uint8_t*, uint16_t, uint8_t, uint8_t, uint8_t);
static uint16_t SensLogGetFileType(const SENSLOG_ID_TBL*, uint16_t*, uint16_t*, DID did);
static void SensLogOutputFile(uint8_t);
static void SensLogOutputGenFile(uint8_t);
static void SensLogGetSystemTime(SENSLOG_SYSTEMTIME*);
static void Num2String2Digit(uint8_t* buf, uint32_t num);
static void Num2String3Digit(uint8_t* buf, uint32_t num);
static void Num2String4Digit(uint8_t* buf, uint32_t num);
static void Num2HexString(uint8_t* buf, uint8_t digits, uint32_t num);
static void SensLogmakeHeader(uint8_t* buf, SENSLOG_DATA_HEADER data_header);

/*---------------------------------------------------------------------------------*
 * Grobal Value                                                                    *
 *---------------------------------------------------------------------------------*/
/* Sensor log information */
static SENSLOG_INFO g_sens_log_info[SENSLOG_TYPE_COUNT];    // NOLINT(readability/nolint)
static uint8_t   g_navsv_info_flag = FALSE;
uint8_t   g_mount_path[SENSLOG_FILEPATH_SIZE];  /* Mount path */
UNIT_TYPE g_unit_type = UNIT_TYPE_NONE;         /* HW configuration type   */
SemID     g_sem_id = 0;                         /* Lock semaphore ID */

/* Sensor log type definition table */
const SENSLOG_ID_TBL kSensLogInputTbl[] = {
    /* Data type                DID                                Sensor Log Type             FRAMEWORKUNIFIEDLOG Output Type */
    {SENSLOG_DATA_I_SYS,         0,                                 SENSLOG_TYPE_SYS_INPUT,    POS_SENSLOG_TYPE_SYS  },
    {SENSLOG_DATA_I_SYS_STS,     0,                                 SENSLOG_TYPE_SYS_INPUT,    POS_SENSLOG_TYPE_SYS  },
    {SENSLOG_DATA_I_GPS,         0,                                 SENSLOG_TYPE_GPS_INPUT,    POS_SENSLOG_TYPE_GPS  },
    {SENSLOG_DATA_I_LOC,         POSHAL_DID_GPS_CUSTOMDATA_NAVI,    SENSLOG_TYPE_NAVI_INPUT,   POS_SENSLOG_TYPE_NAV  },
    {SENSLOG_DATA_I_SPEED,       VEHICLE_DID_MOTION_SPEED_NAVI,     SENSLOG_TYPE_NAVI_INPUT,   POS_SENSLOG_TYPE_NAV  },
    {SENSLOG_DATA_I_TIME,        VEHICLE_DID_SETTINGTIME,           SENSLOG_TYPE_NAVI_INPUT,   POS_SENSLOG_TYPE_NAV  },
    {SENSLOG_DATA_I_TIME,        POSHAL_DID_GPS_TIME,               SENSLOG_TYPE_GPS_INPUT,    POS_SENSLOG_TYPE_GPS  },
    {SENSLOG_DATA_I_GPSINF,      0,                                 SENSLOG_TYPE_NAVI_INPUT,   POS_SENSLOG_TYPE_NAV  },
    {SENSLOG_DATA_I_GPSRST,      0,                                 SENSLOG_TYPE_CMD_INPUT,    POS_SENSLOG_TYPE_CMD  },
    {SENSLOG_DATA_I_GPSSET,      0,                                 SENSLOG_TYPE_CMD_INPUT,    POS_SENSLOG_TYPE_CMD  },
    {SENSLOG_DATA_I_NAVSVINFO,   0,                                 SENSLOG_TYPE_GPS_INPUT,    POS_SENSLOG_TYPE_GPS  },
    {SENSLOG_DATA_I_SYS_ABNORMAL, 0,                                SENSLOG_TYPE_SYS_INPUT,    POS_SENSLOG_TYPE_SYS  },
    {SENSLOG_DATA_I_UNSPECIFIED, 0,                                 SENSLOG_TYPE_COUNT,        POS_SENSLOG_TYPE_NONE }
};

const SENSLOG_ID_TBL kSensLogOutputTbl[] = {
    /* Data type                DID                                Sensor Log Type             FRAMEWORKUNIFIEDLOG Output Type */
    {SENSLOG_DATA_O_SYS,         POSHAL_DID_SPEED_PULSE,               SENSLOG_TYPE_SYS_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_SYS,         POSHAL_DID_SPEED_KMPH,                SENSLOG_TYPE_SYS_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_SYS,         POSHAL_DID_GYRO_X,                    SENSLOG_TYPE_SYS_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_SYS,         POSHAL_DID_GYRO_Y,                    SENSLOG_TYPE_SYS_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_SYS,         POSHAL_DID_GYRO_Z,                    SENSLOG_TYPE_SYS_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_SYS,         POSHAL_DID_GSNS_X,                    SENSLOG_TYPE_SYS_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_SYS,         POSHAL_DID_GSNS_Y,                    SENSLOG_TYPE_SYS_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_SYS,         POSHAL_DID_GSNS_Z,                    SENSLOG_TYPE_SYS_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_SYS,         POSHAL_DID_GPS_ANTENNA,               SENSLOG_TYPE_SYS_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_SYS,         POSHAL_DID_SNS_COUNTER,               SENSLOG_TYPE_SYS_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_SYS,         POSHAL_DID_SPEED_PULSE_FST,           SENSLOG_TYPE_SYS_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_SYS,         POSHAL_DID_GYRO_X_FST,                SENSLOG_TYPE_SYS_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_SYS,         POSHAL_DID_GYRO_Y_FST,                SENSLOG_TYPE_SYS_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_SYS,         POSHAL_DID_GYRO_Z_FST,                SENSLOG_TYPE_SYS_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_SYS,         VEHICLE_DID_GPS__CWORD82__NMEA,             SENSLOG_TYPE_GPS_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_SYS,         POSHAL_DID_GPS__CWORD82___CWORD44_GP4,          SENSLOG_TYPE_GPS_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_SYS,         POSHAL_DID_GPS__CWORD82__FULLBINARY,        SENSLOG_TYPE_GPS_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_SYS,         POSHAL_DID_GPS_NMEA,                  SENSLOG_TYPE_GPS_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_SYS,         POSHAL_DID_REV,                       SENSLOG_TYPE_SYS_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_SYS,         POSHAL_DID_REV_FST,                   SENSLOG_TYPE_SYS_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_SYS,         POSHAL_DID_GYRO_TEMP,                 SENSLOG_TYPE_SYS_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_SYS,         POSHAL_DID_GYRO_TEMP_FST,             SENSLOG_TYPE_SYS_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_SYS,         POSHAL_DID_GSNS_X_FST,                SENSLOG_TYPE_SYS_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_SYS,         POSHAL_DID_GSNS_Y_FST,                SENSLOG_TYPE_SYS_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_SYS,         POSHAL_DID_GSNS_Z_FST,                SENSLOG_TYPE_SYS_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_SYS,         POSHAL_DID_PULSE_TIME,                SENSLOG_TYPE_SYS_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_SYS,         POSHAL_DID_GPS_CLOCK_DRIFT,           SENSLOG_TYPE_GPS_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_SYS,         POSHAL_DID_GPS_CLOCK_FREQ,            SENSLOG_TYPE_GPS_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_SYS_PKG,     0,                                 SENSLOG_TYPE_SYS_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_GPS,         0,                                 SENSLOG_TYPE_CMD_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_LONLAT_N,    VEHICLE_DID_LOCATION_LONLAT_NAVI,  SENSLOG_TYPE_NAVI_OUTPUT,  POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_LONLAT_G,    VEHICLE_DID_LOCATION_LONLAT,       SENSLOG_TYPE_GPS_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_ALT,         VEHICLE_DID_LOCATION_ALTITUDE,     SENSLOG_TYPE_GPS_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_SPEED_N,     VEHICLE_DID_MOTION_SPEED_NAVI,     SENSLOG_TYPE_NAVI_OUTPUT,  POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_SPEED_P,     VEHICLE_DID_MOTION_SPEED_INTERNAL, SENSLOG_TYPE_POS_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_HEAD_N,      VEHICLE_DID_MOTION_HEADING_NAVI,   SENSLOG_TYPE_NAVI_OUTPUT,  POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_HEAD_G,      VEHICLE_DID_MOTION_HEADING,        SENSLOG_TYPE_GPS_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_TIME_SETREQ, VEHICLE_DID_SETTINGTIME,           SENSLOG_TYPE_GPS_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_TIME,        POSHAL_DID_GPS_TIME,               SENSLOG_TYPE_GPS_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_GPSINF,      VEHICLE_DID_NAVIINFO_DIAG_GPS,     SENSLOG_TYPE_NAVI_OUTPUT,  POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_TIME_CS,     0,                                 SENSLOG_TYPE_GPS_OUTPUT,   POS_SENSLOG_TYPE_NONE },
    {SENSLOG_DATA_O_GPSRST,      0,                                 SENSLOG_TYPE_GPS_OUTPUT,   POS_SENSLOG_TYPE_NONE },

    {SENSLOG_DATA_O_UNSPECIFIED, 0,                                 SENSLOG_TYPE_COUNT,        POS_SENSLOG_TYPE_NONE }
};

/*---------------------------------------------------------------------------------*
 * Function                                                                        *
 *---------------------------------------------------------------------------------*/
/**
 * @brief
 *   Sensor Log Initial Processing
 *
 * @param[in]  p_mount_path Mount path
 */
void SensLogInitialize(uint8_t *p_mount_path) {
    static SENSLOG_INFO st_sens_log_info; /* Coverity CID:23609 compliant */
    uint8_t      uc_hw[16];                       /* HW name */
    uint8_t      ret = 0;
    uint8_t      i;
    static uint8_t tzset_flag = 0; /* Not initialized */

    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "+");

    /* Not initialized */
    if (tzset_flag == 0) {
        tzset();
        tzset_flag = 1; /* Initialization complete */
    }

    /* Initialization of sensor log information structure */
    memset(&g_sens_log_info, 0, sizeof(g_sens_log_info));

    /* Get mount path */
    memset(&g_mount_path[0], 0, sizeof(g_mount_path));
    if (NULL == p_mount_path) {
        snprintf(reinterpret_cast<char*>(&g_mount_path[0]), sizeof(g_mount_path), SENSLOG_CONFIG_FILE_PATH_2);
    } else {
        snprintf(reinterpret_cast<char*>(&g_mount_path[0]), sizeof(g_mount_path), "%s/", p_mount_path);
    }
    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "mount path[%s]", g_mount_path);

    /* HW judgment */
    memset(&uc_hw[0], 0, sizeof(uc_hw));
    g_unit_type = GetEnvSupportInfo();
    if (UNIT_TYPE_GRADE1 == g_unit_type) {
        /* GRADE1 */
        snprintf(reinterpret_cast<char*>(&uc_hw[0]), sizeof(uc_hw), "GRADE1");
    } else if (UNIT_TYPE_GRADE2 == g_unit_type) {
      /*
       *  Note.
       *  This feature branches processing depending on the unit type.
       */
    } else {
        /* Environment error */
        /* Do not output sensor log */
        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Environment Error.");
        ret = 1;
    }

    /* Get Semaphore ID */
    g_sem_id = _pb_CreateSemaphore(const_cast<char *>(SENSLOG_SEMAPHO_NAME));
    if (g_sem_id == 0) {  // LCOV_EXCL_BR_LINE 200: can not return zero
        // LCOV_EXCL_START 8: invalid
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Semaphore create Error.(ID = 0)");
        ret = 1;
        // LCOV_EXCL_STOP
    }

    if (ret == 0) {
        /* Read config files */
        /* SYS */
        memset(&st_sens_log_info, 0, sizeof(SENSLOG_INFO));
        SensLogGetConfig((const uint8_t*)SENSLOG_CONFIG_FILE_NAME_SYS, &st_sens_log_info);
        if (st_sens_log_info.uc_valid_flag == SENSLOG_VALIDFLAG_ON) {
            st_sens_log_info.us_gen_count = 1;
            memcpy(&g_sens_log_info[SENSLOG_TYPE_SYS_INPUT],  &st_sens_log_info, sizeof(SENSLOG_INFO));
            memcpy(&g_sens_log_info[SENSLOG_TYPE_SYS_OUTPUT], &st_sens_log_info, sizeof(SENSLOG_INFO));
            snprintf(reinterpret_cast<char*>(&(g_sens_log_info[SENSLOG_TYPE_SYS_INPUT].uc_file_name[0])),
                     sizeof(g_sens_log_info[SENSLOG_TYPE_SYS_INPUT].uc_file_name), "POS_%s_sys_i_%%03d.log", uc_hw);
            snprintf(reinterpret_cast<char*>(&(g_sens_log_info[SENSLOG_TYPE_SYS_OUTPUT].uc_file_name[0])),
                     sizeof(g_sens_log_info[SENSLOG_TYPE_SYS_OUTPUT].uc_file_name), "POS_%s_sys_o_%%03d.log", uc_hw);
            snprintf(reinterpret_cast<char*>(&(g_sens_log_info[SENSLOG_TYPE_SYS_INPUT].uc_gen_fname[0])),
                     sizeof(g_sens_log_info[SENSLOG_TYPE_SYS_INPUT].uc_gen_fname), "POS_%s_sys_i_gen.dat", uc_hw);
            snprintf(reinterpret_cast<char*>(&(g_sens_log_info[SENSLOG_TYPE_SYS_OUTPUT].uc_gen_fname[0])),
                     sizeof(g_sens_log_info[SENSLOG_TYPE_SYS_OUTPUT].uc_gen_fname), "POS_%s_sys_o_gen.dat", uc_hw);
            SensLogOutputGenFile(SENSLOG_TYPE_SYS_INPUT);
            SensLogOutputGenFile(SENSLOG_TYPE_SYS_OUTPUT);
        }

        /* GPS */
        memset(&st_sens_log_info, 0, sizeof(SENSLOG_INFO));
        SensLogGetConfig((const uint8_t*)SENSLOG_CONFIG_FILE_NAME_GPS, &st_sens_log_info);
        if (st_sens_log_info.uc_valid_flag == SENSLOG_VALIDFLAG_ON) {
            st_sens_log_info.us_gen_count = 1;
            memcpy(&g_sens_log_info[SENSLOG_TYPE_GPS_INPUT],  &st_sens_log_info, sizeof(SENSLOG_INFO));
            memcpy(&g_sens_log_info[SENSLOG_TYPE_GPS_OUTPUT], &st_sens_log_info, sizeof(SENSLOG_INFO));
            snprintf(reinterpret_cast<char*>(&(g_sens_log_info[SENSLOG_TYPE_GPS_INPUT].uc_file_name[0])),
                     sizeof(g_sens_log_info[SENSLOG_TYPE_GPS_INPUT].uc_file_name), "POS_%s_gps_i_%%03d.log", uc_hw);
            snprintf(reinterpret_cast<char*>(&(g_sens_log_info[SENSLOG_TYPE_GPS_OUTPUT].uc_file_name[0])),
                     sizeof(g_sens_log_info[SENSLOG_TYPE_GPS_OUTPUT].uc_file_name), "POS_%s_gps_o_%%03d.log", uc_hw);
            snprintf(reinterpret_cast<char*>(&(g_sens_log_info[SENSLOG_TYPE_GPS_INPUT].uc_gen_fname[0])),
                     sizeof(g_sens_log_info[SENSLOG_TYPE_GPS_INPUT].uc_gen_fname), "POS_%s_gps_i_gen.dat", uc_hw);
            snprintf(reinterpret_cast<char*>(&(g_sens_log_info[SENSLOG_TYPE_GPS_OUTPUT].uc_gen_fname[0])),
                     sizeof(g_sens_log_info[SENSLOG_TYPE_GPS_OUTPUT].uc_gen_fname), "POS_%s_gps_o_gen.dat", uc_hw);
            SensLogOutputGenFile(SENSLOG_TYPE_GPS_INPUT);
            SensLogOutputGenFile(SENSLOG_TYPE_GPS_OUTPUT);
        }

        /* NAVI */
        memset(&st_sens_log_info, 0, sizeof(SENSLOG_INFO));
        SensLogGetConfig((const uint8_t*)SENSLOG_CONFIG_FILE_NAME_NAVI, &st_sens_log_info);
        if (st_sens_log_info.uc_valid_flag == SENSLOG_VALIDFLAG_ON) {
            st_sens_log_info.us_gen_count = 1;
            memcpy(&g_sens_log_info[SENSLOG_TYPE_NAVI_INPUT],  &st_sens_log_info, sizeof(SENSLOG_INFO));
            memcpy(&g_sens_log_info[SENSLOG_TYPE_NAVI_OUTPUT], &st_sens_log_info, sizeof(SENSLOG_INFO));
            snprintf(reinterpret_cast<char*>(&(g_sens_log_info[SENSLOG_TYPE_NAVI_INPUT].uc_file_name[0])),
                     sizeof(g_sens_log_info[SENSLOG_TYPE_NAVI_INPUT].uc_file_name), "POS_%s_nav_i_%%03d.log", uc_hw);
            snprintf(reinterpret_cast<char*>(&(g_sens_log_info[SENSLOG_TYPE_NAVI_OUTPUT].uc_file_name[0])),
                     sizeof(g_sens_log_info[SENSLOG_TYPE_NAVI_OUTPUT].uc_file_name), "POS_%s_nav_o_%%03d.log", uc_hw);
            snprintf(reinterpret_cast<char*>(&(g_sens_log_info[SENSLOG_TYPE_NAVI_INPUT].uc_gen_fname[0])),
                     sizeof(g_sens_log_info[SENSLOG_TYPE_NAVI_INPUT].uc_gen_fname), "POS_%s_nav_i_gen.dat", uc_hw);
            snprintf(reinterpret_cast<char*>(&(g_sens_log_info[SENSLOG_TYPE_NAVI_OUTPUT].uc_gen_fname[0])),
                     sizeof(g_sens_log_info[SENSLOG_TYPE_NAVI_OUTPUT].uc_gen_fname), "POS_%s_nav_o_gen.dat", uc_hw);
            SensLogOutputGenFile(SENSLOG_TYPE_NAVI_INPUT);
            SensLogOutputGenFile(SENSLOG_TYPE_NAVI_OUTPUT);
        }

        /* POS */
        memset(&st_sens_log_info, 0, sizeof(SENSLOG_INFO));
        SensLogGetConfig((const uint8_t*)SENSLOG_CONFIG_FILE_NAME_POS, &st_sens_log_info);
        if (st_sens_log_info.uc_valid_flag == SENSLOG_VALIDFLAG_ON) {
            st_sens_log_info.us_gen_count = 1;
            memcpy(&g_sens_log_info[SENSLOG_TYPE_POS_OUTPUT],  &st_sens_log_info, sizeof(SENSLOG_INFO));
            snprintf(reinterpret_cast<char*>(&(g_sens_log_info[SENSLOG_TYPE_POS_OUTPUT].uc_file_name[0])),
                     sizeof(g_sens_log_info[SENSLOG_TYPE_POS_OUTPUT].uc_file_name), "POS_%s_pos_o_%%03d.log", uc_hw);
            snprintf(reinterpret_cast<char*>(&(g_sens_log_info[SENSLOG_TYPE_POS_OUTPUT].uc_gen_fname[0])),
                     sizeof(g_sens_log_info[SENSLOG_TYPE_POS_OUTPUT].uc_gen_fname), "POS_%s_pos_o_gen.dat", uc_hw);
            SensLogOutputGenFile(SENSLOG_TYPE_POS_OUTPUT);
        }

        /* CMD */
        memset(&st_sens_log_info, 0, sizeof(SENSLOG_INFO));
        SensLogGetConfig((const uint8_t*)SENSLOG_CONFIG_FILE_NAME_CMD, &st_sens_log_info);
        if (st_sens_log_info.uc_valid_flag == SENSLOG_VALIDFLAG_ON) {
            st_sens_log_info.us_gen_count = 1;
            memcpy(&g_sens_log_info[SENSLOG_TYPE_CMD_INPUT],  &st_sens_log_info, sizeof(SENSLOG_INFO));
            memcpy(&g_sens_log_info[SENSLOG_TYPE_CMD_OUTPUT], &st_sens_log_info, sizeof(SENSLOG_INFO));
            snprintf(reinterpret_cast<char*>(&(g_sens_log_info[SENSLOG_TYPE_CMD_INPUT].uc_file_name[0])),
                     sizeof(g_sens_log_info[SENSLOG_TYPE_CMD_INPUT].uc_file_name), "POS_%s_cmd_i_%%03d.log", uc_hw);
            snprintf(reinterpret_cast<char*>(&(g_sens_log_info[SENSLOG_TYPE_CMD_OUTPUT].uc_file_name[0])),
                     sizeof(g_sens_log_info[SENSLOG_TYPE_CMD_OUTPUT].uc_file_name), "POS_%s_cmd_o_%%03d.log", uc_hw);
            snprintf(reinterpret_cast<char*>(&(g_sens_log_info[SENSLOG_TYPE_CMD_INPUT].uc_gen_fname[0])),
                     sizeof(g_sens_log_info[SENSLOG_TYPE_CMD_INPUT].uc_gen_fname), "POS_%s_cmd_i_gen.dat", uc_hw);
            snprintf(reinterpret_cast<char*>(&(g_sens_log_info[SENSLOG_TYPE_CMD_OUTPUT].uc_gen_fname[0])),
                     sizeof(g_sens_log_info[SENSLOG_TYPE_CMD_OUTPUT].uc_gen_fname), "POS_%s_cmd_o_gen.dat", uc_hw);
            SensLogOutputGenFile(SENSLOG_TYPE_CMD_INPUT);
            SensLogOutputGenFile(SENSLOG_TYPE_CMD_OUTPUT);
        }

        /* NAV-SVINFO */
        SensLogCheckNAVSVINFOFile();
    }

    for (i = 0; i < SENSLOG_TYPE_COUNT; i++) {
        if (g_sens_log_info[i].uc_valid_flag == SENSLOG_VALIDFLAG_ON) {
            FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, \
                "g_sens_log_info[%d].uc_valid_flag:%d", i, g_sens_log_info[i].uc_valid_flag);
            FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, \
                "g_sens_log_info[%d].st_gen.us_len:%d", i, g_sens_log_info[i].st_gen.us_len);
            FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, \
                "g_sens_log_info[%d].st_gen.uc_gen:%d", i, g_sens_log_info[i].st_gen.uc_gen);
            FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, \
                "g_sens_log_info[%d].us_file_rec_count:%d", i, g_sens_log_info[i].us_file_rec_count);
            FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, \
                "g_sens_log_info[%d].us_rec_count:%d", i, g_sens_log_info[i].us_rec_count);
            FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, \
                "g_sens_log_info[%d].us_gen_count:%d", i, g_sens_log_info[i].us_gen_count);
            FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, \
                "g_sens_log_info[%d].uc_file_path:%s", i, g_sens_log_info[i].uc_file_path);
            FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, \
                "g_sens_log_info[%d].uc_file_name:%s", i, g_sens_log_info[i].uc_file_name);
            FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, \
                "g_sens_log_info[%d].uc_gen_fname:%s", i, g_sens_log_info[i].uc_gen_fname);
            FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, \
                "g_sens_log_info[%d].ul_text_buf_size:%d", i, g_sens_log_info[i].ul_text_buf_size);
        }
    }
    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "-");

    return;
}

/**
 * @brief
 *   Sensor log stop processing
 */
void SensLogTerminate(void) {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "+");

    /* Initialization of sensor log information structure */
    memset(&g_sens_log_info, 0, sizeof(g_sens_log_info));

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "-");

    return;
}

/**
 * @brief
 *   Sensor log saving process
 */
void SensLogStore(void) {  // LCOV_EXCL_START 8 : dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    uint8_t     i;

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "+");

    for (i = 0; i < SENSLOG_TYPE_COUNT; i++) {
        /* Output buffering log */
        if ((g_sens_log_info[i].uc_valid_flag == SENSLOG_VALIDFLAG_ON) &&
                (g_sens_log_info[i].ul_text_buf_len > 0)) {
            SensLogOutputFile(i);
        }
    }

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "-");
    return;
}
// LCOV_EXCL_STOP

/**
 * @brief
 *   Sensor log config file information acquisition
 *
 * @param[in]  uc_config_file  Config filename
 * @param[out] st_sens_log_info Sensor log information
 */
static void SensLogGetConfig(const uint8_t* uc_config_file, SENSLOG_INFO* st_sens_log_info) {
    FILE*   fp;
    uint8_t uc_file_path[SENSLOG_FILEPATH_SIZE];
    uint8_t uc_file_name[SENSLOG_FILEPATH_SIZE + SENSLOG_FILENAME_SIZE];
    uint8_t ucBuf[SENSLOG_CONFIG_TEXT_SIZE];
    uint8_t uc_val[SENSLOG_CONFIG_TEXT_SIZE];
    uint8_t flag = 0;

    FRAMEWORKUNIFIEDLOG(ZONE_28, __FUNCTION__, "+");

    if ((uc_config_file == NULL) || (st_sens_log_info == NULL)) {  // LCOV_EXCL_BR_LINE 200: can not NULL
        // LCOV_EXCL_START 8: invalid
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        /* Unprocessed if the argument is invalid. */
        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Parameter Error.");
        // LCOV_EXCL_STOP
    } else {
        memset(&uc_file_path, 0, sizeof(uc_file_path));
        memset(&uc_file_name, 0, sizeof(uc_file_name));

        /* Refer to the config files in the built-in memory. */
        snprintf(reinterpret_cast<char*>(&uc_file_path[0]), sizeof(uc_file_path), \
            SENSLOG_CONFIG_FILE_PATH_1);
        snprintf(reinterpret_cast<char*>(&uc_file_name[0]), sizeof(uc_file_name), \
            "%s%s", uc_file_path, uc_config_file);
        fp = fopen(reinterpret_cast<char*>(&uc_file_name[0]), "r");
        if (NULL == fp) {
            /* If there are no config files in the built-in memory, refer to the USB memory. */
            snprintf(reinterpret_cast<char*>(&uc_file_path[0]), sizeof(uc_file_path), \
                "%s", (const char*)&g_mount_path[0]);
            snprintf(reinterpret_cast<char*>(&uc_file_name[0]), sizeof(uc_file_name), \
                "%s%s", uc_file_path, uc_config_file);
            fp = fopen(reinterpret_cast<char*>(&uc_file_name[0]), "r");
        }

        if (NULL == fp) {
            /* Do not print when there are no config files.(Console log output)*/
            FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, \
                "Not SensorLog config file. [%s]", uc_file_name);
        } else {
            /* Read config files */
            for (;;) {
                if (fgets(reinterpret_cast<char*>(&ucBuf), SENSLOG_CONFIG_TEXT_SIZE, fp) == NULL) {
                    /* EOF detection(Fail-safe:Normally it does not pass because it is break at the flag judgment place.)*/
                    break;
                }
                memset(&uc_val, 0, sizeof(uc_val));
                if ((strncmp(reinterpret_cast<char*>(&ucBuf), SENSLOG_CONFIG_KEY_LEN, \
                    strlen(SENSLOG_CONFIG_KEY_LEN)) == 0) && ((flag & 0x01) != 0x01)) {
                    /* Get record length */
                    SensLogGetConfigVal(ucBuf, uc_val);
                    st_sens_log_info->st_gen.us_len = static_cast<uint16_t>(atoi((const char *)uc_val));
                    flag ^= 0x01;
                } else if ((strncmp(reinterpret_cast<char*>(&ucBuf), SENSLOG_CONFIG_KEY_GEN, \
                    strlen(SENSLOG_CONFIG_KEY_GEN)) == 0) && ((flag & 0x02) != 0x02)) {
                    /* Get number of generations */
                    SensLogGetConfigVal(ucBuf, uc_val);
                    st_sens_log_info->st_gen.uc_gen = static_cast<uint16_t>(atoi((const char *)uc_val));
                    flag ^= 0x02;
                } else {
                    /* nop */
                }

                if (flag == 0x03) {
                    break;
                }
            }
            fclose(fp);

            FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, \
                "SensorLog read : file[%s] : config[0x%02x]", uc_file_name, flag);

            if ((st_sens_log_info->st_gen.us_len < SENSLOG_LEN_MIN) \
                || (st_sens_log_info->st_gen.us_len > SENSLOG_LEN_MAX)) {
                /* The default value is applied if it is outside the valid range. */
                st_sens_log_info->st_gen.us_len = SENSLOG_LEN_DEFAULT;
            }
            if ((st_sens_log_info->st_gen.uc_gen < SENSLOG_GEN_MIN) \
                || (st_sens_log_info->st_gen.uc_gen > SENSLOG_GEN_MAX)) {
                /* The default value is applied if it is outside the valid range. */
                st_sens_log_info->st_gen.uc_gen = SENSLOG_GEN_DEFAULT;
            }
            st_sens_log_info->ul_text_buf_size = SENSLOG_BUF_MAX; /* Static area(Maximum security) */

            st_sens_log_info->uc_valid_flag = SENSLOG_VALIDFLAG_ON;
            st_sens_log_info->uc_output_flag = SENSLOG_OUTPUTFLAG_NEW;
            /* The log output path should be the same path as the config file. */
            memcpy(st_sens_log_info->uc_file_path, uc_file_path, SENSLOG_FILEPATH_SIZE);
        }
    }
    FRAMEWORKUNIFIEDLOG(ZONE_28, __FUNCTION__, "-");

    return;
}

/**
 * @brief
 *   Sensor log config file setting value acquisition
 *
 * @param[in]  uc_text  Config setting information
 * @param[out] uc_val   Config setting
 */
static void SensLogGetConfigVal(uint8_t* uc_text, uint8_t* uc_val) {
    uint8_t  ucBuf[SENSLOG_CONFIG_TEXT_SIZE];
    uint16_t i = 0;
    uint16_t j = 0;

    FRAMEWORKUNIFIEDLOG(ZONE_28, __FUNCTION__, "+");

    if ((uc_text == NULL) || (uc_val == NULL)) {  // LCOV_EXCL_BR_LINE 200: can not NULL
        // LCOV_EXCL_START 8: invalid
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        /* Unprocessed if the argument is invalid. */
        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Parameter Error.");
        // LCOV_EXCL_STOP
    } else {
        memset(ucBuf, 0, sizeof(ucBuf));
        while ((*uc_text != ':') && (*uc_text != '\0')) {
            uc_text++;
            i++;
            if (i >= (SENSLOG_CONFIG_TEXT_SIZE - 1)) {
                break;
            }
        }
        while ((*uc_text != '\r') && (*uc_text != '\n') && (*uc_text != '\0')) {
            uc_text++;
            i++;
            ucBuf[j++] = *uc_text;
            if (i >= SENSLOG_CONFIG_TEXT_SIZE) {  // LCOV_EXCL_BR_LINE 200: can not exceed size
                // LCOV_EXCL_START 8: invalid
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                memset(&ucBuf, 0, sizeof(ucBuf));
                break;
                // LCOV_EXCL_STOP
            }
        }
        memcpy(uc_val, ucBuf, sizeof(ucBuf));
    }
    FRAMEWORKUNIFIEDLOG(ZONE_28, __FUNCTION__, "-");

    return;
}

/**
 * @brief
 *   Acquisition of sensor log NAV-SVINFO file information
 *
 * @param[in] none
 */
static void SensLogCheckNAVSVINFOFile(void) {
    FILE*   fp;
    const uint8_t* uc_config_file;
    uint8_t uc_file_path[SENSLOG_FILEPATH_SIZE];
    uint8_t uc_file_name[SENSLOG_FILEPATH_SIZE + SENSLOG_FILENAME_SIZE];

    FRAMEWORKUNIFIEDLOG(ZONE_28, __FUNCTION__, "+");

    uc_config_file = (const uint8_t*)SENSLOG_NAV_SVINFO_FILE_NAME;

    memset(&uc_file_path, 0, sizeof(uc_file_path));
    memset(&uc_file_name, 0, sizeof(uc_file_name));

    /* Refer to the config files in the built-in memory. */
    snprintf(reinterpret_cast<char*>(&uc_file_path[0]), sizeof(uc_file_path), SENSLOG_CONFIG_FILE_PATH_1);
    snprintf(reinterpret_cast<char*>(&uc_file_name[0]), sizeof(uc_file_name), "%s%s", uc_file_path, uc_config_file);
    fp = fopen(reinterpret_cast<char*>(&uc_file_name[0]), "r");
    if (NULL == fp) {
        /* If there are no config files in the built-in memory, refer to the USB memory. */
        snprintf(reinterpret_cast<char*>(&uc_file_path[0]), sizeof(uc_file_path), \
            "%s", (const char*)&g_mount_path[0]);
        snprintf(reinterpret_cast<char*>(&uc_file_name[0]), sizeof(uc_file_name), \
            "%s%s", uc_file_path, uc_config_file);
        fp = fopen(reinterpret_cast<char*>(&uc_file_name[0]), "r");
    }

    if (NULL == fp) {
        /* Do not print when there are no config files.(Console log output)*/
        FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "Not SensorLog config file. [%s]", uc_file_name);
        g_navsv_info_flag = FALSE;
    } else {
        fclose(fp);
        FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "SensorLog read : file[%s]", uc_file_name);
        g_navsv_info_flag = TRUE;
    }
    FRAMEWORKUNIFIEDLOG(ZONE_28, __FUNCTION__, "-");

    return;
}

/**
 * @brief
 *   Acquisition of sensor log NAV-SVINFO file information
 *
 * @param[in] none
 *
 * @return Destination sensor log file type
 */
uint8_t SensLogGetNavSvinfoFlag(void) {  // LCOV_EXCL_START 8: dead code.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return g_navsv_info_flag;
}
// LCOV_EXCL_STOP
/**
 * @brief
 *   Sensor log output(Input information)
 *
 *   Writes the specified log (input information) to the buffer area
 *
 * @param[in]  us_data_type   Data type
 * @param[in]  ul_did        Data ID
 * @param[in]  us_pno        Destination PNO
 * @param[in]  p_data        Write data pointer
 * @param[in]  us_size       Write data size
 * @param[in]  uc_result     Reception result
 * @param[in]  u_write_flag   ability to write sensor log
 * @param[in]  u_write_abnormal_flag   When an error occurs ability to write sensor log
 */
void SensLogWriteInputData(uint16_t us_data_type, DID ul_did, PNO us_pno, uint8_t *p_data, \
        uint16_t us_size, uint8_t uc_result, uint8_t u_write_flag, uint8_t u_write_abnormal_flag) {
    uint16_t                file_type = 0;
    uint16_t                data_type = us_data_type;
    uint16_t                write_type = POS_SENSLOG_TYPE_NONE;
    RET_API                 lret_sem = RET_ERROR;

    /* Sensor log file type determination */
    file_type = SensLogGetFileType(kSensLogInputTbl, &data_type, &write_type, ul_did);

    /* For the target type,Perform exclusive control */
    /* Currently, only GPS input can be written from multiple threads. */
    if (file_type == SENSLOG_TYPE_GPS_INPUT) {
        /* Semaphore ID determination */
        if (g_sem_id != 0) {  // LCOV_EXCL_BR_LINE 200: can not zero
            lret_sem = _pb_SemLock(g_sem_id); /* Semaphore Lock */
            if (lret_sem != RET_NORMAL) {  // LCOV_EXCL_BR_LINE 200: can not return error
                // LCOV_EXCL_START 8: invalid
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "SemLock Error.");
                // LCOV_EXCL_STOP
            } else {
                /* Sensor log output */
                SensLogWrite(file_type, data_type, write_type, ul_did, us_pno, p_data, \
                    us_size, uc_result, u_write_flag, u_write_abnormal_flag);
                (void)_pb_SemUnlock(g_sem_id); /* Semaphore unlock */
            }
        }
    } else {
        /* Sensor log output */
        SensLogWrite(file_type, data_type, write_type, ul_did, us_pno, p_data, us_size, \
            uc_result, u_write_flag, u_write_abnormal_flag);
    }

    return;
}

/**
 * @brief
 *   Sensor log output(Outputs the information)
 *
 *   Writes the specified log (output information) to the buffer area
 *
 * @param[in]  us_data_type   Data type
 * @param[in]  ul_did        Data ID
 * @param[in]  us_pno        Destination PNO
 * @param[in]  p_data        Write data pointer
 * @param[in]  us_size       Write data size
 * @param[in]  uc_result     Reception result
 */
void SensLogWriteOutputData(uint16_t us_data_type, DID ul_did, PNO us_pno, uint8_t *p_data, \
        uint16_t us_size, uint8_t uc_result) {
    uint16_t                file_type = 0;
    uint16_t                data_type = us_data_type;
    uint16_t                write_type = POS_SENSLOG_TYPE_NONE;
    RET_API                 lret_sem = RET_ERROR;

    /* Sensor log file type determination */
    file_type = SensLogGetFileType(kSensLogOutputTbl, &data_type, &write_type, ul_did);

    /* For the target type,Perform exclusive control */
    /* Currently, only GPS output can be written from multiple threads. */
    if (file_type == SENSLOG_TYPE_GPS_OUTPUT) {
        /* Semaphore ID determination */
        if (g_sem_id != 0) {  // LCOV_EXCL_BR_LINE 200: can not zero
            lret_sem = _pb_SemLock(g_sem_id);  /* Semaphore Lock */
            if (lret_sem != RET_NORMAL) {  // LCOV_EXCL_BR_LINE 200: can not return error
                // LCOV_EXCL_START 8: invalid
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "SemLock Error.");
                // LCOV_EXCL_STOP
            } else {
                /* Sensor log output */
                SensLogWrite(file_type, data_type, write_type, ul_did, us_pno, p_data, \
                    us_size, uc_result, SENSLOG_ON, SENSLOG_OFF);
                (void)_pb_SemUnlock(g_sem_id);  /* Semaphore unlock */
            }
        }
    } else {
        /* Sensor log output */
        SensLogWrite(file_type, data_type, write_type, ul_did, us_pno, p_data, us_size, \
            uc_result, SENSLOG_ON, SENSLOG_OFF);
    }
    return;
}

/**
 * @brief
 *   Convert number to ASCII code(For two digits,Fill up to zero)
 *
 * @param[out] buf   Data saving
 * @param[in]  num   Numbers to be converted
 */
static void Num2String2Digit(uint8_t* buf, uint32_t num) {
    int8_t c;
    if (num >= 100) {  // LCOV_EXCL_BR_LINE 200: can not exceed  size
        // LCOV_EXCL_START 8: invalid
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        *buf++ = '0';
        *buf = '0';
        // LCOV_EXCL_STOP
    } else {
        c = static_cast<int8_t>(num / 10);
        *buf++ = static_cast<uint8_t>(c + '0');
        *buf = static_cast<uint8_t>(num - (c * 10) + '0');
    }
}

/**
 * @brief
 *   Convert number to ASCII code(For 3 digits,Fill up to zero)
 *
 * @param[out] buf   Data saving
 * @param[in]  num   Numbers to be converted
 */
static void Num2String3Digit(uint8_t* buf, uint32_t num) {
    int8_t c;
    if (num >= 1000) {  // LCOV_EXCL_BR_LINE 200: can not exceed  size
        // LCOV_EXCL_START 8: invalid
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        *buf++ = '0';
        *buf++ = '0';
        *buf = '0';
        // LCOV_EXCL_STOP
    } else {
        c = static_cast<int8_t>(num / 100);
        *buf++ = static_cast<uint8_t>(c + '0');
        num = num - (c * 100);
        c = static_cast<int8_t>(num / 10);
        *buf++ = static_cast<uint8_t>(c + '0');
        *buf = static_cast<uint8_t>(num - (c * 10) + '0');
    }
}

/**
 * @brief
 *   Convert number to ASCII code(For 4 digits,Fill up to zero)
 *
 * @param[out] buf   Data saving
 * @param[in]  num   Numbers to be converted
 */
static void Num2String4Digit(uint8_t* buf, uint32_t num) {
    int8_t c;
    if (num >= 10000) {  // LCOV_EXCL_BR_LINE 200: can not exceed  size
        // LCOV_EXCL_START 8: invalid
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        *buf++ = '0';
        *buf++ = '0';
        *buf++ = '0';
        *buf = '0';
        // LCOV_EXCL_STOP
    } else {
        c = static_cast<int8_t>(num / 1000);
        *buf++ = static_cast<uint8_t>(c + '0');
        num = num - (c * 1000);
        c = static_cast<int8_t>(num / 100);
        *buf++ = static_cast<uint8_t>(c + '0');
        num = num - (c * 100);
        c = static_cast<int8_t>(num / 10);
        *buf++ = static_cast<uint8_t>(c + '0');
        *buf = static_cast<uint8_t>(num - (c * 10) + '0');
    }
}

/**
 * @brief
 *   Convert digits to hexadecimal ASCII code(Fill up to zero)
 *
 * @param[out] buf    Data saving
 * @param[in]  Number of digits to be converted to digits
 * @param[in]  num    Numbers to be converted
 */
static void Num2HexString(uint8_t* buf, uint8_t digits, uint32_t num) {
    uint8_t* p = buf + digits;
    uint8_t calc;
    int32_t i;

    /* Only within the displayable range,Convert  */
    if ((digits == 8) || (num <  (uint32_t)(1 << (4 *digits)))) {
        while (num) {
            calc = num % 16;
            if (10 > calc) {
                calc = static_cast<uint8_t>(calc + 0x30);  // 0x30 is 0 character
            } else {
                calc = static_cast<uint8_t>(calc + (0x61 - 0x0A));  // 0x41 is the letter of a
            }
            *(--p) = calc;
            num /= 16;
            digits--;
        }
    }

    /* Insert 0 in the remaining significant digits.  */
    for (i = 0; i < digits; i++) {
        *(--p) = '0';
    }
}

/**
 * @brief
 *   Sensor Log Header Creation
 *
 * @param[out] buf        Data saving
 * @param[in]  DataHeader data headers
 * @note For buf, Up to 62 bytes are used. As 64 bytes are allocated in the upper digit, Without current problems, dataHeader.c_pname size increase,
 *  Need to be reviewed when changing the format.
 */
static void SensLogmakeHeader(uint8_t* buf, SENSLOG_DATA_HEADER dataHeader) {
    /* "%02d %04d/%02d/%02d %02d:%02d:%02d.%03d %08x %s %01x %01x %04d " */
    int8_t *p;
    uint16_t i = 0;
    Num2String2Digit(buf, dataHeader.us_data_type);
    buf += 2;
    *(buf++) = ' ';
    Num2String4Digit(buf, dataHeader.st_sys_time.us_year);
    buf += 4;
    *(buf++) = '/';
    Num2String2Digit(buf, dataHeader.st_sys_time.us_month);
    buf += 2;
    *(buf++) = '/';
    Num2String2Digit(buf, dataHeader.st_sys_time.us_day);
    buf += 2;
    *(buf++) = ' ';
    Num2String2Digit(buf, dataHeader.st_sys_time.us_hour);
    buf += 2;
    *(buf++) = ':';
    Num2String2Digit(buf, dataHeader.st_sys_time.us_minute);
    buf += 2;
    *(buf++) = ':';
    Num2String2Digit(buf, dataHeader.st_sys_time.us_second);
    buf += 2;
    *(buf++) = '.';
    Num2String3Digit(buf, dataHeader.st_sys_time.us_milli_seconds);
    buf += 3;
    *(buf++) = ' ';
    Num2HexString(buf, 8, dataHeader.ul_did);
    buf += 8;
    *(buf++) = ' ';
    p = dataHeader.c_pname;
    for (i = 0; i < sizeof(dataHeader.c_pname); i++) {
        if (*p == 0) {
            break;
        }
        *(buf++) = *(p++);
    }
    *(buf++) = ' ';
    Num2HexString((buf++), 1, dataHeader.uc_unit_type);
    *(buf++) = ' ';
    Num2HexString((buf++), 1, dataHeader.uc_result);
    *(buf++) = ' ';
    Num2String4Digit(buf, dataHeader.us_data_size);
    buf += 4;
    *(buf++) = ' ';

    return;
}

/**
 * @brief
 *   Sensor log output
 *
 *   Write specified log to buffer area(Common process)
 *
 * @param[in]  us_file_type   File type
 * @param[in]  us_data_type   Data type
 * @param[in]  us_write_type  FRAMEWORKUNIFIEDLOG Output Type
 * @param[in]  ul_did        Data ID
 * @param[in]  us_pno        Destination PNO
 * @param[in]  p_data        Write data pointer
 * @param[in]  us_size       Write data size
 * @param[in]  uc_result     Reception result
 * @param[in]  uWriteFlag   ability to write sensor log
 * @param[in]  u_write_abnormal_flag   When an error occursability to write sensor log
 */
static void SensLogWrite(uint16_t us_file_type, uint16_t us_data_type, uint16_t us_write_type, \
        DID ul_did, PNO us_pno, uint8_t *p_data, uint16_t us_size, uint8_t uc_result, uint8_t uWriteFlag, \
        uint8_t u_write_abnormal_flag) {
    uint32_t                len = 0;
    SENSLOG_DATA_HEADER     dataHeader;
    uint16_t                headSize = sizeof(SENSLOG_DATA_HEADER);
    uint16_t                i = 0;
    uint8_t                 workBuf[64];
    uint16_t                workBufLen = 0;
    PCSTR                   pPname = NULL;
    uint32_t                ulTempBufLen;
    uint32_t                ulPositioninglogBufLen = SENSLOG_BUF_MAX;
    uint32_t                retComp;

    if ((p_data == NULL) || (us_size == 0) || (us_size > SENSLOG_BUF_MIN)) {  // LCOV_EXCL_BR_LINE 200: can not NULL
        /* Unprocessed if write specified log data is invalid */
        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, 
                                          "Parameter Error. File[%d] Data[%d] DID[%d] Size[%d]",
                                          us_file_type, us_data_type, ul_did, us_size);
    } else {
        /* Sensor log file type determination */
        if (us_file_type < SENSLOG_TYPE_COUNT) {
            if ((g_sens_log_info[us_file_type].uc_valid_flag == SENSLOG_VALIDFLAG_ON) ||
                (us_write_type != POS_SENSLOG_TYPE_NONE)) {
                /* Setting of header information */
                memset(&dataHeader, 0x00, sizeof(dataHeader));
                dataHeader.us_data_type = us_data_type;
                SensLogGetSystemTime(&dataHeader.st_sys_time);
                dataHeader.ul_did = ul_did;
                pPname = _pb_CnvPno2Name(us_pno);
                if (pPname == NULL) {
                    dataHeader.c_pname[0] = '-';
                } else {
                    snprintf(reinterpret_cast<char *>(&(dataHeader.c_pname[0])), \
                        sizeof(dataHeader.c_pname), "%s", pPname);
                }
                dataHeader.uc_unit_type = (uint8_t)g_unit_type;
                dataHeader.uc_result = uc_result;
                dataHeader.us_data_size = us_size;

                /* Buffer storage(Header)*/
                memset(&workBuf[0], 0x00, sizeof(workBuf));
                SensLogmakeHeader(workBuf, dataHeader);
                workBufLen = static_cast<uint16_t>(strlen(reinterpret_cast<char *>(&(workBuf[0]))));
                memcpy(reinterpret_cast<char *>(&(g_sens_log_info[us_file_type].uc_temp_buf[0])), \
                    &workBuf[0], workBufLen);

                /* Buffer storage(Data portion)*/
                for (i = 0; i < us_size; i++) {
                    Num2HexString(&(g_sens_log_info[us_file_type].uc_temp_buf[workBufLen + i * 3]), 2, *p_data);
                    g_sens_log_info[us_file_type].uc_temp_buf[workBufLen + i * 3 + 2] = ' ';
                    g_sens_log_info[us_file_type].uc_temp_buf[workBufLen + i * 3 + 3] = 0; /* NULL */
                    p_data++;
                }
                g_sens_log_info[us_file_type].uc_temp_buf[workBufLen + (us_size * 3)]     = '\r';
                g_sens_log_info[us_file_type].uc_temp_buf[workBufLen + (us_size * 3) + 1] = '\n';
                ulTempBufLen = workBufLen + (us_size * 3) + 2;

                /* FRAMEWORKUNIFIEDLOG out */
                if ((us_file_type == SENSLOG_TYPE_NAVI_INPUT) || (us_file_type == SENSLOG_TYPE_NAVI_OUTPUT)) {
                    if (g_unit_type != UNIT_TYPE_GRADE1) {
                        /* Sensor log is not output when an error occurs except for _CWORD80_ in NAV. */
                        u_write_abnormal_flag = 0;
                    }
                }
                if ((us_write_type != POS_SENSLOG_TYPE_NONE) && (u_write_abnormal_flag == SENSLOG_ON)) {
                    /* Data compression */
                    retComp = compress2(reinterpret_cast<Bytef*>(&(g_sens_log_info[us_file_type]. \
                                        uc_positioninglog_buf[SENSLOG_SEQ_SIZE + sizeof(ulPositioninglogBufLen)])), \
                                        reinterpret_cast<uLongf*>(&ulPositioninglogBufLen), \
                                        reinterpret_cast<Bytef*>(&(g_sens_log_info[us_file_type].uc_temp_buf[0])), \
                                        ulTempBufLen, Z_DEFAULT_COMPRESSION);

                    if (retComp == Z_OK) {
                        memcpy(&(g_sens_log_info[us_file_type].uc_positioninglog_buf[0]), \
                                SENSLOG_SEQ_START, SENSLOG_SEQ_SIZE);
                        memcpy(&(g_sens_log_info[us_file_type].uc_positioninglog_buf[SENSLOG_SEQ_SIZE]), \
                            &ulPositioninglogBufLen, sizeof(ulPositioninglogBufLen));
                        memcpy(&(g_sens_log_info[us_file_type].uc_positioninglog_buf[SENSLOG_SEQ_SIZE \
                            + sizeof(ulPositioninglogBufLen) + ulPositioninglogBufLen]), SENSLOG_SEQ_END, SENSLOG_SEQ_SIZE);
                        g_sens_log_info[us_file_type].uc_positioninglog_buf[SENSLOG_SEQ_SIZE + sizeof(ulPositioninglogBufLen) \
                            + ulPositioninglogBufLen + SENSLOG_SEQ_SIZE] = '\n';
                        ulPositioninglogBufLen = static_cast<uint32_t>(SENSLOG_SEQ_SIZE + sizeof(ulPositioninglogBufLen) \
                            + ulPositioninglogBufLen + SENSLOG_SEQ_SIZE + 1);
                        POS_SENSLOG(us_write_type, (PCSTR)&(g_sens_log_info[us_file_type].uc_positioninglog_buf[0]), \
                            ulPositioninglogBufLen);
                    } else {
                        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, \
                            "compress error[retComp = %d]", retComp);
                    }
                }

                if ((g_sens_log_info[us_file_type].uc_valid_flag == SENSLOG_VALIDFLAG_ON) \
                        && (uWriteFlag == SENSLOG_ON)) {
                    /* For executing file output */
                    /* Buffer size determination */
                    len = g_sens_log_info[us_file_type].ul_text_buf_len;
                    if (g_sens_log_info[us_file_type].ul_text_buf_size <= (len + ((headSize + us_size) * 3))) {
                        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "[%d]:DataSize[%d]/BufLen[%d]",
                                                          us_data_type, us_size, len);

                        /* In case of buffer upper limit,Write File */
                        SensLogOutputFile(static_cast<uint8_t>(us_file_type));

                        /* Buffer clear */
                        memset(&(g_sens_log_info[us_file_type].uc_text_buf[0]), 0x00,
                            g_sens_log_info[us_file_type].ul_text_buf_size);
                        len = 0;
                    }

                    /* Buffer storage */
                    memcpy(reinterpret_cast<char*>(&(g_sens_log_info[us_file_type].uc_text_buf[len])), \
                        &g_sens_log_info[us_file_type].uc_temp_buf[0], ulTempBufLen);
                    g_sens_log_info[us_file_type].ul_text_buf_len = len + ulTempBufLen;
                    g_sens_log_info[us_file_type].us_rec_count++;

                    /* Determining whether the number of file write records is the upper limit (the number of records per file) */
                    if (g_sens_log_info[us_file_type].st_gen.us_len <= g_sens_log_info[us_file_type].us_rec_count) {
                        FRAMEWORKUNIFIEDLOG(ZONE_28, __FUNCTION__, "[%d]:RecCnt[%d/%d]",
                            us_data_type, g_sens_log_info[us_file_type].us_rec_count, \
                            g_sens_log_info[us_file_type].st_gen.us_len);

                        /* In case of buffer upper limit,Write File */
                        SensLogOutputFile(static_cast<uint8_t>(us_file_type));

                        /* Buffer clear */
                        memset(&(g_sens_log_info[us_file_type].uc_text_buf[0]), 0x00, \
                            g_sens_log_info[us_file_type].ul_text_buf_size);
                        g_sens_log_info[us_file_type].ul_text_buf_len = 0;
                        g_sens_log_info[us_file_type].us_rec_count = 0;
                        /* Update the number of generations */
                        g_sens_log_info[us_file_type].us_gen_count++;
                        if (g_sens_log_info[us_file_type].st_gen.uc_gen < g_sens_log_info[us_file_type].us_gen_count) {
                            g_sens_log_info[us_file_type].us_gen_count = 1;
                        }
                        g_sens_log_info[us_file_type].uc_output_flag = SENSLOG_OUTPUTFLAG_NEW;
                    }
                }
            }
        }
    }

    return;
}

/**
 * @brief
 *   Sensor log file type determination
 *
 *   Determine the type of the output destination sensor log file
 *
 * @param[in]   pstTbl       Sensor log type definition table
 * @param[in]   Pus_data_type Data type
 * @param[out]  pus_write_type FRAMEWORKUNIFIEDLOG Output Type
 * @param[in]   ul_did        Data ID
 *
 * @return Destination sensor log file type
 */
static uint16_t SensLogGetFileType(const SENSLOG_ID_TBL *pstTbl, uint16_t *pus_data_type, \
        uint16_t *pus_write_type, DID ul_did) {
    uint16_t        file_type = SENSLOG_TYPE_COUNT;
    uint16_t        cnt = 0;

    /* Determination of output log file type */
    /* DID,Prioritize the beginning of the table for both data types. */
    /* If DID is specified, it is judged by DID.*/
    if (ul_did != 0) {
        for (cnt = 0; pstTbl[cnt].us_file_type != SENSLOG_TYPE_COUNT; cnt++) {
            if (pstTbl[cnt].ul_did == ul_did) {
                *pus_data_type = pstTbl[cnt].us_data_type;
                file_type = pstTbl[cnt].us_file_type;
                *pus_write_type = pstTbl[cnt].us_write_type;
                break;
            }
        }
    }
    /* If no DID is specified or cannot be found, judge according to the data type. */
    if (file_type == SENSLOG_TYPE_COUNT) {
        for (cnt = 0; pstTbl[cnt].us_file_type != SENSLOG_TYPE_COUNT; cnt++) {
            if (pstTbl[cnt].us_data_type == *pus_data_type) {
                file_type = pstTbl[cnt].us_file_type;
                *pus_write_type = pstTbl[cnt].us_write_type;
                break;
            }
        }
    }
    return file_type;
}

/**
 * @brief
 *   Sensor log file output
 *
 *   Write the log data in the buffer area to a file
 *
 * @param[in]  ucFileType   File type
 */
static void SensLogOutputFile(uint8_t ucFileType) {
    FILE    *fp;
    int     fd;
    uint8_t  uc_file_name_base[SENSLOG_FILEPATH_SIZE + SENSLOG_FILENAME_SIZE];
    uint8_t  uc_file_name[SENSLOG_FILEPATH_SIZE + SENSLOG_FILENAME_SIZE];

    if (ucFileType < SENSLOG_TYPE_COUNT) {  // LCOV_EXCL_BR_LINE 200: can not exceed  type
        /* File path generation */
        snprintf(reinterpret_cast<char*>(&uc_file_name_base[0]), sizeof(uc_file_name_base),
                  "%s%s",
                  g_sens_log_info[ucFileType].uc_file_path, g_sens_log_info[ucFileType].uc_file_name);
        snprintf(reinterpret_cast<char*>(&uc_file_name[0]), sizeof(uc_file_name),
                  reinterpret_cast<char*>(&uc_file_name_base[0]),
                  g_sens_log_info[ucFileType].us_gen_count);

        /* Writing to a File */
        if (g_sens_log_info[ucFileType].uc_output_flag == SENSLOG_OUTPUTFLAG_NEW) {
            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, \
                "SensLog create file. : file[%s] : uc_output_flag[%d]", \
                uc_file_name, g_sens_log_info[ucFileType].uc_output_flag);

            /* Export New File(Write From Beginning)*/
            fp = fopen(reinterpret_cast<char*>(&uc_file_name[0]), "w+");
            g_sens_log_info[ucFileType].uc_output_flag = SENSLOG_OUTPUTFLAG_ADD;

            /* Update generation information file */
            SensLogOutputGenFile(ucFileType);
        } else {
            /* Append export */
            fp = fopen(reinterpret_cast<char*>(&uc_file_name[0]), "a+");
        }

        if (NULL != fp) {  // LCOV_EXCL_BR_LINE 200: can not NULL
            /* Log file output(ASCII)*/
            fd = fileno(fp);
            fwrite(&(g_sens_log_info[ucFileType].uc_text_buf[0]), g_sens_log_info[ucFileType].ul_text_buf_len, 1, fp);
            fflush(fp);    /* Flush the userspace buffers provided by the C library  */
            fdatasync(fd); /* Flush cache of files referenced by fd to disk */
            fclose(fp); /* Coverity CID:23371 compliant */
        } else {
            // LCOV_EXCL_START 8: invalid
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, \
                "SensLog fopen fail. : file[%s] : uc_output_flag[%d]", \
                uc_file_name, g_sens_log_info[ucFileType].uc_output_flag);
            // LCOV_EXCL_STOP
        }
    }
    return;
}

/**
 * @brief
 *   Generation information file output
 *
 *   Holds the number of generations being output in the generation information file.
 *
 * @param[in]  ucFileType   File type
 */
static void SensLogOutputGenFile(uint8_t ucFileType) {
    FILE     *fp;
    int      fd;
    uint8_t  uc_file_name_base[SENSLOG_FILEPATH_SIZE + SENSLOG_FILENAME_SIZE];
    uint8_t  uc_file_name[SENSLOG_FILEPATH_SIZE + SENSLOG_FILENAME_SIZE];

    if (ucFileType < SENSLOG_TYPE_COUNT) {  // LCOV_EXCL_BR_LINE 200: can not exceed  type
        /* File path generation */
        snprintf(reinterpret_cast<char*>(&uc_file_name_base[0]), sizeof(uc_file_name_base),
                  "%s%s",
                  g_sens_log_info[ucFileType].uc_file_path, g_sens_log_info[ucFileType].uc_gen_fname);
        snprintf(reinterpret_cast<char*>(&uc_file_name[0]), sizeof(uc_file_name),
                  reinterpret_cast<char*>(&uc_file_name_base[0]),
                  g_sens_log_info[ucFileType].us_gen_count);

        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "SensLog create file. : file[%s] : us_gen_count[%d]",
                      uc_file_name, g_sens_log_info[ucFileType].us_gen_count);

        /* Export New File(Write From Beginning)*/
        fp = fopen(reinterpret_cast<char*>(&uc_file_name[0]), "w+");
        if (NULL != fp) {  // LCOV_EXCL_BR_LINE 200: can not NULL
            /* Generation control number output */
            fd = fileno(fp);
            fprintf(fp, "%03d", g_sens_log_info[ucFileType].us_gen_count);
            fflush(fp);    /* Flush the userspace buffers provided by the C library  */
            fdatasync(fd); /* Flush cache of files referenced by fd to disk */
            fclose(fp); /* Coverity CID:23372 compliant */
        } else {
            // LCOV_EXCL_START 8: invalid
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, \
                "SensLog fopen fail. : file[%s] : us_gen_count[%d]", \
                uc_file_name, g_sens_log_info[ucFileType].us_gen_count);
            // LCOV_EXCL_STOP
        }
    }
    return;
}

/**
 * @brief
 *   Get system time
 *
 *   Get the system time(Millisecond)
 *
 * @param[out] st_sys_time    System time
 */
void SensLogGetSystemTime(SENSLOG_SYSTEMTIME *st_sys_time) {
    time_t time_sec = 0;
    struct timespec tp = {0};
    struct tm time_cal= {0};

    if (NULL != st_sys_time) {  // LCOV_EXCL_BR_LINE 200: not NULL
        if (0 == clock_gettime(CLOCK_REALTIME, &tp)) {
            time_sec = tp.tv_sec;
            localtime_r(&time_sec, &time_cal);
            st_sys_time->us_year = static_cast<uint16_t>((WORD)time_cal.tm_year + 1900);
            st_sys_time->us_month = static_cast<uint16_t>((WORD)time_cal.tm_mon + 1);
            st_sys_time->us_day_of_week    = (WORD)time_cal.tm_wday;
            st_sys_time->us_day          = (WORD)time_cal.tm_mday;
            st_sys_time->us_hour         = (WORD)time_cal.tm_hour;
            st_sys_time->us_minute       = (WORD)time_cal.tm_min;
            st_sys_time->us_second       = (WORD)time_cal.tm_sec;
            st_sys_time->us_milli_seconds = (WORD)(static_cast<double>(tp.tv_nsec) * 1e-6); /* Calculating milliseconds */
        }
    }
    return;
}

/* end of file */
