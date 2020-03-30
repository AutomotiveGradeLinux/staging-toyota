/*
 * @copyright Copyright (c) 2016-2019 TOYOTA MOTOR CORPORATION.
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

/************************************************************************
* File name      : sysup_addr.h
* System name    : Integrated PF
* Subsystem name : Common system
* Title          : Boot-Related Address Definition Header Files
************************************************************************/

#ifndef POSITIONING_BASE_LIBRARY_LIBRARY_INCLUDE_SYSUP_ADDR_H_
#define POSITIONING_BASE_LIBRARY_LIBRARY_INCLUDE_SYSUP_ADDR_H_

/*****************************************************
 *
 *    Initial management area address space definition
 *                              A: Address, S: Size
 ****************************************************/
/* Note : Don't forget to change sysup_addr.inc at the same time! */

/* SDRAM startup identification table (1) A Offset from the beginning 0x0 */
#define ADDR_SYSUP_DRAM1    ((SDRAM_INITIAL_AREA & ADDR_MASK) | ADDR_P2)  // NOLINT(runtime/references)
/* SDRAM startup identification table (1) S */
#define SIZE_SYSUP_DRAM1    0x00000004

/* SDRAM Start Identification Table A  Offset from the beginning 0x4 */
#define ADDR_SYSUP_DRAM_FIX (ADDR_SYSUP_DRAM1 + SIZE_SYSUP_DRAM1)
/* SDRAM Start Identification Table S */
#define SIZE_SYSUP_DRAM_FIX 0x00000004

/* Startup Status Table A Offset from the beginning 0x8 */
#define ADDR_SYSUP_STS      (ADDR_SYSUP_DRAM_FIX + SIZE_SYSUP_DRAM_FIX)
/* Startup Status Table S */
#define SIZE_SYSUP_STS      0x00000002

#define ADDR_SYS_DUMMY1     (ADDR_SYSUP_STS + SIZE_SYSUP_STS)    /* Dummy1 A Offset from the beginning 0xa */
#define SIZE_SYS_DUMMY1     0x00000002                           /* Dummy1 S */

#define ADDR_UPMODE         (ADDR_SYS_DUMMY1 + SIZE_SYS_DUMMY1)  /* Boot Mode Save Table A Offset from the beginning 0xc */
#define SIZE_UPMODE         0x00000004                           /* Boot Mode Save Table S */

#define ADDR_SYSUP_STS_GINI (ADDR_UPMODE + SIZE_UPMODE)  /* System startup status table A Offset from the beginning 0x10 */
#define SIZE_SYSUP_STS_GINI 0x00000002                   /* System startup status table S */

/* Fork Failures Recording Table A  Offset from the beginning 0x12 */
#define ADDR_SYSUP_RST_CNT  (ADDR_SYSUP_STS_GINI + SIZE_SYSUP_STS_GINI)
/* Fork Failures Recording Table S */
#define SIZE_SYSUP_RST_CNT  0x00000002

/* System anomaly process No. table A Offset from the beginning 0x14 */
#define ADDR_SYS_ERR_PNO    (ADDR_SYSUP_RST_CNT + SIZE_SYSUP_RST_CNT)
/* System anomaly process No. table S */
#define SIZE_SYS_ERR_PNO    0x00000004

/* System error reset cause table A  Offset from the beginning 0x18 */
#define ADDR_SYS_ERR_RST    (ADDR_SYS_ERR_PNO + SIZE_SYS_ERR_PNO)
/* System error reset cause table S */
#define SIZE_SYS_ERR_RST    0x00000004

/* System startup failure table A Offset from the beginning 0x1c */
#define ADDR_SYSUP_ERR_STS  (ADDR_SYS_ERR_RST + SIZE_SYS_ERR_RST)
/* System startup failure table S */
#define SIZE_SYSUP_ERR_STS  0x00000004

/* Navigation activation state table A Offset from the beginning 0x20 */
#define ADDR_NAVIUP_STS     (ADDR_SYSUP_ERR_STS + SIZE_SYSUP_ERR_STS)
/* Navigation activation state table S */
#define SIZE_NAVIUP_STS     0x00000002

/* Navigation fork Failed Count Recording Table A Offset from the beginning 0x22*/
#define ADDR_NAVIUP_RST_CNT (ADDR_NAVIUP_STS + SIZE_NAVIUP_STS)
/* Navigation fork Failed Count Recording Table S */
#define SIZE_NAVIUP_RST_CNT 0x00000002

/* Navigation error process No. table A Offset from the beginning 0x24 */
#define ADDR_NAVI_ERR_PNO   (ADDR_NAVIUP_RST_CNT + SIZE_NAVIUP_RST_CNT)
/* Navigation error process No. table S */
#define SIZE_NAVI_ERR_PNO   0x00000004

/* Navigation Error Cause Table A Offset from the beginning 0x28 */
#define ADDR_NAVI_ERR       (ADDR_NAVI_ERR_PNO + SIZE_NAVI_ERR_PNO)
/* Navigation Error Cause Table S */
#define SIZE_NAVI_ERR       0x00000004

#define ADDR_NAVIUP_ERR_STS (ADDR_NAVI_ERR + SIZE_NAVI_ERR)    /* Navigation start failure table A Offset from the beginning 0x2c */
#define SIZE_NAVIUP_ERR_STS 0x00000004                         /* Navigation start failure table S */

/* SDRAM startup identification table (2) A Offset from the beginning 0x30 */
#define ADDR_SYSUP_DRAM2    (ADDR_NAVIUP_ERR_STS + SIZE_NAVIUP_ERR_STS)
/* SDRAM startup identification table (2) S */
#define SIZE_SYSUP_DRAM2    0x00000004

/* SDRAM Backup Status Table A Offset from the beginning 0x34 */
#define ADDR_SYS_BUP_STS    (ADDR_SYSUP_DRAM2 + SIZE_SYSUP_DRAM2)
/* SDRAM Backup Status Table S */
#define SIZE_SYS_BUP_STS    0x00000004

/* NAVI-SYS communication error recording table A Offset from the beginning 0x38 */
#define ADDR_SYSCOM_STS     (ADDR_SYS_BUP_STS + SIZE_SYS_BUP_STS)
/* NAVI-SYS communication error recording table S */
#define SIZE_SYSCOM_STS     0x00000004

/* SDRAM Power Supply Fault Detection Table A  Offset from the beginning 0x3c */
#define ADDR_SYS_PW_STS     (ADDR_SYSCOM_STS + SIZE_SYSCOM_STS)
/* SDRAM Power Supply Fault Detection Table S */
#define SIZE_SYS_PW_STS     0x00000004

/* Hard WDT generation table A  Offset from the beginning 0x40 */
#define ADDR_HWDT_STS       (ADDR_SYS_PW_STS + SIZE_SYS_PW_STS)
/* Hard WDT generation table S */
#define SIZE_HWDT_STS       0x00000004

/* SDRAM Labels Destruct Status Detection Table A Offset from the beginning 0x44 */
#define ADDR_DRAMLABEL_STS  (ADDR_HWDT_STS + SIZE_HWDT_STS)
/* SDRAM Labels Destruct Status Detection Table S */
#define SIZE_DRAMLABEL_STS  0x00000004

#define ADDR_SSB_MNG        (ADDR_DRAMLABEL_STS + SIZE_DRAMLABEL_STS) /* SSB management table A  Offset from the beginning 0x48 */
#define SIZE_SSB_MNG        0x00000004                                /* SSB management table S */

#define ADDR_NMI_END        (ADDR_SSB_MNG + SIZE_SSB_MNG)           /* NMI completion flag A Offset from the beginning 0x4c */
#define SIZE_NMI_END        0x00000004                              /* NMI completion flag S */

/* Opening indication determination table A Offset from the beginning 0x50 */
#define ADDR_OPDSP_CHK      (ADDR_NMI_END + SIZE_NMI_END)
/* Opening indication determination table S */
#define SIZE_OPDSP_CHK      0x00000004

#define ADDR_SYS_FROM_STS   (ADDR_OPDSP_CHK + SIZE_OPDSP_CHK)  /* FROM destruction status table A  Offset from the beginning 0x54 */
#define SIZE_SYS_FROM_STS   0x00000004                         /* FROM destruction status table S */

/* BootLoader status A Offset from the beginning 0x58 */
#define ADDR_BOOT_STS       (ADDR_SYS_FROM_STS + SIZE_SYS_FROM_STS)
/* BootLoader status S */
#define SIZE_BOOT_STS       0x00000004

/* BootLoader Exceptions Recording Table A  Offset from the beginning 0x5c */
#define ADDR_BOOTEXP_LOG    (ADDR_BOOT_STS + SIZE_BOOT_STS)
/* BootLoader Exceptions Recording Table S  */
#define SIZE_BOOTEXP_LOG    0x00000004

/* SDRAM startup identification table (3) A Offset from the beginning0x60 */
#define ADDR_SYSUP_DRAM3    (ADDR_BOOTEXP_LOG + SIZE_BOOTEXP_LOG)
/* SDRAM startup identification table (3) S */
#define SIZE_SYSUP_DRAM3    0x00000004

/* Error Logging Table in BootLoader Drivers A  Offset from the beginning 0x64 */
#define ADDR_BOOTERR_LOG    (ADDR_SYSUP_DRAM3 + SIZE_SYSUP_DRAM3)
/* Error Logging Table in BootLoader Drivers S  */
#define SIZE_BOOTERR_LOG    0x00000004

/* EPC register values at the time of exceptions in the BootLoader A  Offset from the beginning 0x68 */
#define ADDR_BOOT_EPC       (ADDR_BOOTERR_LOG + SIZE_BOOTERR_LOG)
/* EPC register values at the time of exceptions in the BootLoader S  */
#define SIZE_BOOT_EPC       0x00000004

/* Status register when exceptions occur in the BootLoader A  Offset from the beginning 0x6c */
#define ADDR_BOOT_SR        (ADDR_BOOT_EPC + SIZE_BOOT_EPC)
/* Status register when exceptions occur in the BootLoader S  */
#define SIZE_BOOT_SR        0x00000004

/* Cause Register Values When Internal BootLoader Exceptions Occur A  Offset from the beginning 0x70 */
#define ADDR_BOOT_CAUSE     (ADDR_BOOT_SR + SIZE_BOOT_SR)
/* Cause Register Values When Internal BootLoader Exceptions Occur S  */
#define SIZE_BOOT_CAUSE     0x00000004

/* NMI completion flag(Copying) A Offset from the beginning 0x74 */
#define ADDR_NMI_END_CPY    (ADDR_BOOT_CAUSE + SIZE_BOOT_CAUSE)
/* NMI completion flag(Copying) S */
#define SIZE_NMI_END_CPY    0x00000004

/* Target Type A Offset from the beginning 0x78 */
#define ADDR_TARGET         (ADDR_NMI_END_CPY + SIZE_NMI_END_CPY)
/* Target Type S */
#define SIZE_TARGET         0x00000004

#define ADDR_SYS_NR_RST     (ADDR_TARGET + SIZE_TARGET)             /* Normal reset factor table A 0x7c  */
#define SIZE_SYS_NR_RST     0x00000004                              /* Normal reset factor table S */

#define ADDR_NK_LOAD_STS    (ADDR_SYS_NR_RST + SIZE_SYS_NR_RST)     /* NK Load Status Table A 0x80  */
#define SIZE_NK_LOAD_STS    0x00000004                              /* NK Load Status Table S */

#define ADDR_CRC_RETRY      (ADDR_NK_LOAD_STS + SIZE_NK_LOAD_STS)   /* CRC retry count table A 0x84  */
#define SIZE_CRC_RETRY      0x00000004                              /* CRC retry count table S */

#define ADDR_BOOT_VBR       (ADDR_CRC_RETRY + SIZE_CRC_RETRY)       /* VBR-saved table in BootLoaderr A 0x88  */
#define SIZE_BOOT_VBR       0x00000004                              /* VBR-saved table in BootLoaderr S */

#define ADDR_MONI_TYPE      (ADDR_BOOT_VBR + SIZE_BOOT_VBR)         /* Monitor Type Table A 0x8C  */
#define SIZE_MONI_TYPE      0x00000004                              /* Monitor Type Table S */

#define ADDR_B_FLAG         (ADDR_MONI_TYPE + SIZE_MONI_TYPE)       /* B genuine flag A 0x90  */
#define SIZE_B_FLAG         0x00000010                              /* B genuine flag S */

#define ADDR_RGBCABLE_TYPE  (ADDR_B_FLAG + SIZE_B_FLAG)             /* Connection RGB Cable Type Table A 0xA0  */
#define SIZE_RGBCABLE_TYPE  0x0000004                               /* Connection RGB Cable Type Table S */

#define ADDR_RESET_LABEL    (ADDR_RGBCABLE_TYPE + SIZE_RGBCABLE_TYPE) /* Normal reset label area A 0xA4 */
#define SIZE_RESET_LABEL    0x00000010                                /* Normal reset label area S */

#define ADDR_NK_VUP_FLAG    (ADDR_RESET_LABEL + SIZE_RESET_LABEL)   /* NK version upgrade flag A 0xB4 */
#define SIZE_NK_VUP_FLAG    0x00000004                              /* NK version upgrade flag S */

#define ADDR_TRANS_VUP      (ADDR_NK_VUP_FLAG + SIZE_NK_VUP_FLAG)    /* VUP migration flag A */
#define SIZE_TRANS_VUP      0x00000004                               /* VUP migration flag S */

#define ADDR_PRG_SPAR_BOOT  (ADDR_TRANS_VUP + SIZE_TRANS_VUP)       /* Preliminary program start occurrence flag A */
#define SIZE_ADDR_PRG_SPAR_BOOT  0x00000004                         /* Preliminary program start occurrence flag S */

#define ADDR_BOOT_ERR       (ADDR_PRG_SPAR_BOOT + SIZE_ADDR_PRG_SPAR_BOOT)  /* OS Pre-boot Error Information Storage Table A */
#define SIZE_BOOT_ERR       0x00000004                                      /* OS Pre-boot Error Information Storage Table S */

#endif    // POSITIONING_BASE_LIBRARY_LIBRARY_INCLUDE_SYSUP_ADDR_H_

