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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <libgen.h>

#include <expat.h>

#define BUFSIZE 4096
#define OUTFILE "tskm_auto_build.h"
#define TMPFILE "parsexml.tmp"

typedef uint32_t        ELE_STATE_t;
#define ELE_NONE                0x00000000U
#define ELE_TSKM_CFG            0x10000000U
#define ELE_SERVICE_LIST        0x11000000U
#define ELE_SERVICE             0x11100000U
#define ELE_SUBGID_LIST         0x11110000U

#define ELE_COMMON_GSTEP        0x00100000U
#define ELE_COMMON_EXEC         0x00010000U
#define ELE_COMMON_REQ          0x00020000U

#define ELE_NORMAL_WAKEUP       0x12000000U
#define ELE_NORMAL_W_GSTEP      (ELE_NORMAL_WAKEUP|ELE_COMMON_GSTEP)
#define ELE_NORMAL_W_EXEC       (ELE_NORMAL_WAKEUP|ELE_COMMON_GSTEP|ELE_COMMON_EXEC)
#define ELE_NORMAL_W_REQ        (ELE_NORMAL_WAKEUP|ELE_COMMON_GSTEP|ELE_COMMON_REQ)

#define ELE_NORMAL_SHUTDOWN     0x13000000U
#define ELE_NORMAL_D_GSTEP      (ELE_NORMAL_SHUTDOWN|ELE_COMMON_GSTEP)
#define ELE_NORMAL_D_REQ        (ELE_NORMAL_SHUTDOWN|ELE_COMMON_GSTEP|ELE_COMMON_REQ)

#define ELE_VUP_WAKEUP          0x14000000U
#define ELE_VUP_W_GSTEP         (ELE_VUP_WAKEUP|ELE_COMMON_GSTEP)
#define ELE_VUP_W_EXEC          (ELE_VUP_WAKEUP|ELE_COMMON_GSTEP|ELE_COMMON_EXEC)
#define ELE_VUP_W_REQ           (ELE_VUP_WAKEUP|ELE_COMMON_GSTEP|ELE_COMMON_REQ)

#define ELE_VUP_SHUTDOWN        0x15000000U
#define ELE_VUP_D_GSTEP         (ELE_VUP_SHUTDOWN|ELE_COMMON_GSTEP)
#define ELE_VUP_D_REQ           (ELE_VUP_SHUTDOWN|ELE_COMMON_GSTEP|ELE_COMMON_REQ)


#define ELE_MASK0               0xF0000000U
#define ELE_MASK1               0x0F000000U
#define ELE_MASK2               0x00F00000U
#define ELE_MASK3               0x000F0000U

#define PARSE_ASSERT_EXIT(x)  \
        if(!(x)){ \
          printf("ASSERT %s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__); \
          exit(-1); \
        }

#define VALUE_NAME_MAX 255
typedef struct {
  int  step;
  char gstepIdStr[VALUE_NAME_MAX];
  int  execSvcNum;
  int  reqNum;
  char nextStepCondStr[VALUE_NAME_MAX];
  char execSvcName[VALUE_NAME_MAX];
  char reqTableName[VALUE_NAME_MAX];
} PARSE_GSTEP_t;


typedef struct {
  char svcid[VALUE_NAME_MAX];
  char name[VALUE_NAME_MAX];
  char path[VALUE_NAME_MAX];
  char type[VALUE_NAME_MAX];
  char life_cycle[VALUE_NAME_MAX];
  char retry_cnt[VALUE_NAME_MAX];
  char cpu_assign[VALUE_NAME_MAX];
  char prio[VALUE_NAME_MAX];
  char policy[VALUE_NAME_MAX];
  char user[VALUE_NAME_MAX];
  char runtime_limit[VALUE_NAME_MAX];
  char cpu_limit[VALUE_NAME_MAX];
  char mem_limit[VALUE_NAME_MAX];
  int  subgidNum;
  char args[VALUE_NAME_MAX];
  char shutdown_wait[VALUE_NAME_MAX];
} PARSE_SVC_t;

/***************************************
 * Context
 **************************************/
typedef struct {
  FILE* fp;     // Output file pointer
  FILE* tmpFp;  // Temporary file
  ELE_STATE_t   state;

  PARSE_SVC_t   svc;

  int   svcNum; // Number of services
  PARSE_GSTEP_t gstep;
} PARSE_CTX_t;


/***************************************
 * entryTskmCfg
 **************************************/
void
entryTskmCfg(PARSE_CTX_t* p_ctx,const XML_Char* atts[]) {
  p_ctx->state = ELE_TSKM_CFG;
}

/***************************************
 * exitTskmCfg
 **************************************/
void
exitTskmCfg(PARSE_CTX_t* p_ctx) {
  p_ctx->state = ELE_NONE;
}


/***************************************
 * entryServiceList
 **************************************/
void
entryServiceList(PARSE_CTX_t* p_ctx,const XML_Char* atts[]) {
  p_ctx->state = ELE_SERVICE_LIST;

  p_ctx->tmpFp = fopen(TMPFILE,"w");

  fprintf(p_ctx->tmpFp,
          "static TSKM_SVC_ATTR_t serviceAttr[]={"
          "\n");
}

/***************************************
 * exitServiceList
 **************************************/
void
exitServiceList(PARSE_CTX_t* p_ctx) {
  fprintf(p_ctx->tmpFp,
          "};"
          "\n\n");

  fprintf(p_ctx->tmpFp, "#define TSKM_SVC_NUM (%d) \n\n",p_ctx->svcNum);

  fprintf(p_ctx->tmpFp, "static TSKM_SVC_CTX_t serviceList[TSKM_SVC_NUM]; \n\n");

  fclose(p_ctx->tmpFp);

  fprintf(p_ctx->fp,"\n\n");

  p_ctx->state = ELE_TSKM_CFG;
}

/***************************************
 * handleServiceList
 **************************************/
void
entryService(PARSE_CTX_t* p_ctx,const XML_Char *name,const XML_Char* atts[]) {
  PARSE_ASSERT_EXIT(strcmp(name,"service") == 0);
  int ii;
  char* tp;
  p_ctx->state = ELE_SERVICE;

  memset(&p_ctx->svc,0,sizeof(p_ctx->svc));

  for(ii=0; atts[ii]; ii+=2) {
    const char* attr = atts[ii];
    const char* value = atts[ii+1];
    if(strcmp(attr,"svcid")==0) {
      strcpy(p_ctx->svc.svcid,value);
    } else if(strcmp(attr,"name")==0) {
      strcpy(p_ctx->svc.name,value);
    } else if(strcmp(attr,"path")==0) {
      strcpy(p_ctx->svc.path,value);
    } else if(strcmp(attr,"type")==0) {
      const char* typeName = (strcmp(value,"native")==0) ? "TSKM_SVC_TYPE_NATIVE" : "TSKM_SVC_TYPE_UNKNONW";
      strcpy(p_ctx->svc.type,typeName);
    } else if(strcmp(attr,"prio")==0) {
      strcpy(p_ctx->svc.prio,value);
    } else if(strcmp(attr,"policy")==0) {
      const char* polName  = (strcmp(value,"fifo")==0) ? "TSKM_SVC_POLICY_FIFO" :
                             (strcmp(value,"tss")==0)  ? "TSKM_SVC_POLICY_TSS"  :
                             (strcmp(value,"rr")==0)   ? "TSKM_SVC_POLICY_RR"   : "ERROR";
      strcpy(p_ctx->svc.policy,polName);
    } else if(strcmp(attr,"life_cycle")==0) {
      const char* lcName  = (strcmp(value,"always")==0)             ?  "TSKM_SVC_LC_ALWAYS" :
                            (strcmp(value,"always_recoverable")==0) ?  "TSKM_SVC_LC_ALWAYS_RECOVERABLE" :
                            (strcmp(value,"dynamic")==0)            ?  "TSKM_SVC_LC_DYNAMIC" : "ERROR";
      strcpy(p_ctx->svc.life_cycle,lcName);
    } else if(strcmp(attr,"retry_cnt")==0) {
      strcpy(p_ctx->svc.retry_cnt,value);
    } else if(strcmp(attr,"cpu_assign")==0) {
      const char* caName  = (strcmp(value,"cpu0")==0) ? "TSKM_SVC_ASSIGN_CPU_0" :
                            (strcmp(value,"cpu1")==0) ? "TSKM_SVC_ASSIGN_CPU_1" :
                            (strcmp(value,"auto")==0) ? "TSKM_SVC_ASSIGN_CPU_AUTO" : "ERROR";
      strcpy(p_ctx->svc.cpu_assign,caName);
    } else if(strcmp(attr,"user")==0) {
      strcpy(p_ctx->svc.user,value);
    } else if(strcmp(attr,"runtime_limit")==0) {
      strcpy(p_ctx->svc.runtime_limit,value);
    } else if(strcmp(attr,"cpu_limit")==0) {
      strcpy(p_ctx->svc.cpu_limit,value);
    } else if(strcmp(attr,"mem_limit")==0) {
      strcpy(p_ctx->svc.mem_limit,value);
    } else if(strcmp(attr,"args")==0) {
      strcpy(p_ctx->svc.args,value);
    } else if(strcmp(attr,"shutdown_wait")==0) {
      const char* swStr  = (strcmp(value,"yes")==0) ? "TSKM_TRUE"  :
                           (strcmp(value,"no")==0)  ? "TSKM_FALSE" : "ERROR";
      strcpy(p_ctx->svc.shutdown_wait,swStr);
    }
  }

  fprintf(p_ctx->fp,"char const *svcArgs%d[] = {",p_ctx->svcNum);
  fprintf(p_ctx->fp,"\"%s\",",p_ctx->svc.path);

  // Output arguments
  tp = strtok(p_ctx->svc.args," ");
  while(tp != NULL) {
    fprintf(p_ctx->fp,"\"%s\",",tp);
    tp = strtok(NULL," ");
  }

  fprintf(p_ctx->fp,"NULL");
  fprintf(p_ctx->fp,"};\n");
}

/***************************************
 * exitService
 **************************************/
void
exitService(PARSE_CTX_t* p_ctx) {
  uint32_t ii;
  fprintf(p_ctx->tmpFp,"{" );

  fprintf(p_ctx->tmpFp,"%s",p_ctx->svc.svcid);
  fprintf(p_ctx->tmpFp,", (const char *)\"%s\"",p_ctx->svc.name);
  fprintf(p_ctx->tmpFp,", (const char *)\"%s\"",p_ctx->svc.path);
  fprintf(p_ctx->tmpFp,", (char**)svcArgs%d",p_ctx->svcNum);
  fprintf(p_ctx->tmpFp,", %s",p_ctx->svc.type);
  fprintf(p_ctx->tmpFp,", %s",p_ctx->svc.prio);
  fprintf(p_ctx->tmpFp,", %s",p_ctx->svc.policy);
  fprintf(p_ctx->tmpFp,", %s",p_ctx->svc.life_cycle);
  fprintf(p_ctx->tmpFp,", (uint32_t)%s",p_ctx->svc.retry_cnt);
  fprintf(p_ctx->tmpFp,", %s",p_ctx->svc.cpu_assign);
  fprintf(p_ctx->tmpFp,", (const char *)\"%s\"",p_ctx->svc.user);
  fprintf(p_ctx->tmpFp,", %s",(p_ctx->svc.runtime_limit[0] != '\0') ? p_ctx->svc.runtime_limit : "0");
  fprintf(p_ctx->tmpFp,", %s",(p_ctx->svc.cpu_limit[0]     != '\0') ? p_ctx->svc.cpu_limit     : "0");
  fprintf(p_ctx->tmpFp,", %s",(p_ctx->svc.mem_limit[0]     != '\0') ? p_ctx->svc.mem_limit     : "0");
  fprintf(p_ctx->tmpFp,", %s",p_ctx->svc.shutdown_wait);
  fprintf(p_ctx->tmpFp,", %d",p_ctx->svc.subgidNum);
  if(p_ctx->svc.subgidNum) {
    fprintf(p_ctx->tmpFp,", subgidList%d",p_ctx->svcNum);
  } else {
    fprintf(p_ctx->tmpFp,", NULL");
  }

  fprintf(p_ctx->tmpFp,"},\n");

  p_ctx->svcNum++;

  p_ctx->state = ELE_SERVICE_LIST;
}

/***************************************
 * entrySubgidList
 **************************************/
void
entrySubgidList(PARSE_CTX_t* p_ctx,const XML_Char* atts[]) {
  p_ctx->state = ELE_SUBGID_LIST;

  fprintf(p_ctx->fp,"static gid_t subgidList%d[] = {",p_ctx->svcNum);


}

/***************************************
 * handleSubgidList
 **************************************/
void
handleSubgidList(PARSE_CTX_t* p_ctx,const XML_Char *name,const XML_Char* atts[]) {
  PARSE_ASSERT_EXIT(strcmp(name,"subgid") == 0);
  int32_t ii;

  if(p_ctx->svc.subgidNum > 0) {
    fprintf(p_ctx->fp,",");
  }
  for(ii=0; atts[ii]; ii+=2) {
    const char* attr = atts[ii];
    const char* value = atts[ii+1];
    if(strcmp(attr,"gid")==0) {
      fprintf(p_ctx->fp,"%s",value);
    } else {
      PARSE_ASSERT_EXIT(0);
    }
  }
  p_ctx->svc.subgidNum++;
}

/***************************************
 * exitSubgidList
 **************************************/
void
exitSubgidList(PARSE_CTX_t* p_ctx) {

  fprintf(p_ctx->fp,"};\n");

  p_ctx->state = ELE_SERVICE;
}


/***************************************
 * entryNormalWakeup
 **************************************/
void
entryNormalWakeup(PARSE_CTX_t* p_ctx,const XML_Char* atts[]) {
  p_ctx->state = ELE_NORMAL_WAKEUP;
  memset(&p_ctx->gstep,0,sizeof(p_ctx->gstep));

  sprintf(p_ctx->gstep.reqTableName,"wakeupReqList");
  sprintf(p_ctx->gstep.execSvcName,"wakeupExecSvcId");
  p_ctx->tmpFp = fopen(TMPFILE,"a");
  PARSE_ASSERT_EXIT(p_ctx->tmpFp);

  fprintf(p_ctx->tmpFp,"TSKM_GSTEP_t wakeupGstep[]={\n");
}

/***************************************
 * exitNormalWakeup
 **************************************/
void
exitNormalWakeup(PARSE_CTX_t* p_ctx) {
  fprintf(p_ctx->tmpFp,"};\n\n");
  fclose(p_ctx->tmpFp);

  p_ctx->state = ELE_TSKM_CFG;
}

/***************************************
 * entryVupWakeup
 **************************************/
void
entryVupWakeup(PARSE_CTX_t* p_ctx,const XML_Char* atts[]) {
  p_ctx->state = ELE_VUP_WAKEUP;
  memset(&p_ctx->gstep,0,sizeof(p_ctx->gstep));

  sprintf(p_ctx->gstep.reqTableName,"wakeupReqListVup");
  sprintf(p_ctx->gstep.execSvcName,"wakeupExecSvcIdVup");
  p_ctx->tmpFp = fopen(TMPFILE,"a");
  PARSE_ASSERT_EXIT(p_ctx->tmpFp);

  fprintf(p_ctx->tmpFp,"TSKM_GSTEP_t wakeupGstepVup[]={\n");
}

/***************************************
 * exitVupWakeup
 **************************************/
void
exitVupWakeup(PARSE_CTX_t* p_ctx) {
  fprintf(p_ctx->tmpFp,"};\n\n");
  fclose(p_ctx->tmpFp);

  p_ctx->state = ELE_TSKM_CFG;
}

void
entryGstep(PARSE_CTX_t* p_ctx,const XML_Char* atts[]) {

  p_ctx->state &= ~ELE_MASK2;
  p_ctx->state |= ELE_COMMON_GSTEP;

  PARSE_GSTEP_t *p_gstep = &p_ctx->gstep;

  if(atts[0] && strcmp(atts[0],"stepid") == 0) {
    sprintf(p_gstep->gstepIdStr,"%s",atts[1]);
  } else {
    sprintf(p_gstep->gstepIdStr,"TSKM_GSTEP_NONE");
  }
  p_gstep->execSvcNum = 0;
  p_gstep->reqNum = 0;
  sprintf(p_gstep->nextStepCondStr,"INI_INITCOMP_NONE");
}

void
handleGstep(PARSE_CTX_t* p_ctx,const XML_Char *name,const XML_Char* atts[]) {
  PARSE_ASSERT_EXIT(strcmp(name,"next_trans_condition") == 0);
  PARSE_GSTEP_t *p_gstep = &p_ctx->gstep;

  if(atts[0] && strcmp(atts[0],"cond") == 0) {
    sprintf(p_gstep->nextStepCondStr,"%s",atts[1]);
  } else {
    PARSE_ASSERT_EXIT(0);
  }
}

void
exitGstep(PARSE_CTX_t* p_ctx) {
  PARSE_GSTEP_t *p_gstep = &p_ctx->gstep;

  fprintf(p_ctx->tmpFp,"{");
  fprintf(p_ctx->tmpFp,"%s,",p_gstep->gstepIdStr);
  fprintf(p_ctx->tmpFp,"%d,",p_gstep->execSvcNum);
  if(p_gstep->execSvcNum) {
    fprintf(p_ctx->tmpFp,"%s%d,",p_gstep->execSvcName,p_gstep->step);
  } else {
    fprintf(p_ctx->tmpFp,"NULL,");
  }
  fprintf(p_ctx->tmpFp,"%d,",p_gstep->reqNum);
  if(p_gstep->reqNum) {
    fprintf(p_ctx->tmpFp,"%s%d,",p_gstep->reqTableName,p_gstep->step);
  } else {
    fprintf(p_ctx->tmpFp,"NULL,");
  }
  fprintf(p_ctx->tmpFp,"%s",p_gstep->nextStepCondStr);
  fprintf(p_ctx->tmpFp,"},\n");

  p_gstep->step++;

  p_ctx->state &= ~ELE_MASK2;
}

void
entryExec(PARSE_CTX_t* p_ctx,const XML_Char* atts[]) {
  p_ctx->state &= ~ELE_MASK3;
  p_ctx->state |= ELE_COMMON_EXEC;

  PARSE_GSTEP_t *p_gstep = &p_ctx->gstep;
  fprintf(p_ctx->fp,
          "static TSKM_SVCID_t %s%d[] = {",p_gstep->execSvcName,p_gstep->step);
}
void
handleExec(PARSE_CTX_t* p_ctx,const XML_Char *name,const XML_Char* atts[]) {
  PARSE_ASSERT_EXIT(strcmp(name,"exec_svc") == 0);
  PARSE_GSTEP_t *p_gstep = &p_ctx->gstep;
  int ii;

  if(p_gstep->execSvcNum > 0) {
    fprintf(p_ctx->fp,",");
  }

  for(ii=0; atts[ii]; ii+=2) {
    const char* attr = atts[ii];
    const char* value = atts[ii+1];
    PARSE_ASSERT_EXIT(strcmp(attr,"svcid") == 0);
    fprintf(p_ctx->fp,"%s",value);
  }
  p_gstep->execSvcNum++;
}
void
exitExec(PARSE_CTX_t* p_ctx) {
  fprintf(p_ctx->fp,"};\n\n");
  p_ctx->state &= ~ELE_MASK3;
}

void
entryReq(PARSE_CTX_t* p_ctx,const XML_Char* atts[]) {
  p_ctx->state &= ~ELE_MASK3;
  p_ctx->state |= ELE_COMMON_REQ;
  PARSE_GSTEP_t *p_gstep = &p_ctx->gstep;

  fprintf(p_ctx->fp,"static TSKM_GSTEP_REQ_INFO_t %s%d[] ={",p_gstep->reqTableName,p_gstep->step);
}
void
handleReq(PARSE_CTX_t* p_ctx,const XML_Char *name,const XML_Char* atts[]) {
  PARSE_GSTEP_t *p_gstep = &p_ctx->gstep;
  PARSE_ASSERT_EXIT(strcmp(name,"request") == 0);
  int ii;

  fprintf(p_ctx->fp,"  {");

  for(ii=0; atts[ii]; ii+=2) {
    const char* attr = atts[ii];
    const char* value = atts[ii+1];
    if(strcmp(attr,"local_step") == 0) {
      if(strcmp(value,"shm") == 0) {
        fprintf(p_ctx->fp,"TSKM_LSTEP_SHM");
      } else if(strcmp(value,"bupchk") == 0) {
        fprintf(p_ctx->fp,"TSKM_LSTEP_BUPCHK");
      } else if(strcmp(value,"last") == 0) {
        fprintf(p_ctx->fp,"TSKM_LSTEP_LAST");
      } else if(strcmp(value,"all") == 0) {
        fprintf(p_ctx->fp,"TSKM_LSTEP_ALL");
      } else {
        fprintf(p_ctx->fp,"%s",value);
      }
    } else {
      fprintf(p_ctx->fp,"%s",value);  // Output of ","
    }
    fprintf(p_ctx->fp,"%s",(atts[ii+2])?",":"");  // Outoput of ","
  }
  fprintf(p_ctx->fp,"},");
  p_gstep->reqNum++;
}
void
exitReq(PARSE_CTX_t* p_ctx) {
  fprintf(p_ctx->fp,"};\n\n");
  p_ctx->state &= ~ELE_MASK3;
}

/***************************************
 * entryNormalShutdown
 **************************************/
void
entryNormalShutdown(PARSE_CTX_t* p_ctx,const XML_Char* atts[]) {
  p_ctx->state = ELE_NORMAL_SHUTDOWN;

  memset(&p_ctx->gstep,0,sizeof(p_ctx->gstep));
  sprintf(p_ctx->gstep.reqTableName,"downReqList");
  p_ctx->tmpFp = fopen(TMPFILE,"a");
  PARSE_ASSERT_EXIT(p_ctx->tmpFp);

  fprintf(p_ctx->tmpFp,"static TSKM_GSTEP_t downGstep[]={\n");
}

/***************************************
 * exitNormalShutdown
 **************************************/
void
exitNormalShutdown(PARSE_CTX_t* p_ctx) {

  fprintf(p_ctx->tmpFp,"};\n\n");
  fclose(p_ctx->tmpFp);

  p_ctx->state = ELE_TSKM_CFG;
}

/***************************************
 * entryVupShutdown
 **************************************/
void
entryVupShutdown(PARSE_CTX_t* p_ctx,const XML_Char* atts[]) {
  p_ctx->state = ELE_VUP_SHUTDOWN;
  memset(&p_ctx->gstep,0,sizeof(p_ctx->gstep));

  sprintf(p_ctx->gstep.reqTableName,"downReqListVup");
  p_ctx->tmpFp = fopen(TMPFILE,"a");
  PARSE_ASSERT_EXIT(p_ctx->tmpFp);

  fprintf(p_ctx->tmpFp,"static TSKM_GSTEP_t downGstepVup[]={\n");
}

/***************************************
 * exitVupShutdown
 **************************************/
void
exitVupShutdown(PARSE_CTX_t* p_ctx) {
  fprintf(p_ctx->tmpFp,"};\n\n");
  fclose(p_ctx->tmpFp);

  p_ctx->state = ELE_TSKM_CFG;
}

/***************************************
 * elementStart
 **************************************/
void
elementStart(void *userData, const XML_Char *name, const XML_Char *atts[]) {
  PARSE_CTX_t *p_ctx = (PARSE_CTX_t*)userData;
#if 0
  int ii;
  printf("[ELEMENT] %s Start!\n", name);
  for(ii=0; atts[ii]; ii+=2) {
    printf("  %s:%s \n", atts[ii],atts[ii+1]);
  }
#endif
  switch(p_ctx->state) {
  case ELE_NONE:
    if(strcmp(name,"tskm_cfg") == 0) {
      entryTskmCfg(p_ctx,atts);
    } else {
      PARSE_ASSERT_EXIT(0);
    }
    break;
  case ELE_TSKM_CFG:
    if(strcmp(name,"service_list") == 0) {
      entryServiceList(p_ctx,atts);
    } else if(strcmp(name,"normal_wakeup") == 0) {
      entryNormalWakeup(p_ctx,atts);
    } else if(strcmp(name,"normal_shutdown") == 0) {
      entryNormalShutdown(p_ctx,atts);
    } else if(strcmp(name,"vup_wakeup") == 0) {
      entryVupWakeup(p_ctx,atts);
    } else if(strcmp(name,"vup_shutdown") == 0) {
      entryVupShutdown(p_ctx,atts);
    } else {
      PARSE_ASSERT_EXIT(0);
    }
    break;
  case ELE_SERVICE_LIST:
    if(strcmp(name,"service") == 0) {
      entryService(p_ctx,name,atts);
    }
    break;
  case ELE_SERVICE:
    if(strcmp(name,"subgid_list") == 0) {
      entrySubgidList(p_ctx,atts);
    }
    break;
  case ELE_SUBGID_LIST:
    handleSubgidList(p_ctx,name,atts);
    break;
  case ELE_NORMAL_WAKEUP:
  case ELE_NORMAL_SHUTDOWN:
  case ELE_VUP_WAKEUP:
  case ELE_VUP_SHUTDOWN:
    if(strcmp(name,"global_step") == 0) {
      entryGstep(p_ctx,atts);
    } else {
      PARSE_ASSERT_EXIT(0);
    }
    break;
  case ELE_NORMAL_W_GSTEP:
  case ELE_NORMAL_D_GSTEP:
  case ELE_VUP_W_GSTEP:
  case ELE_VUP_D_GSTEP:
    if(strcmp(name,"exec_list") == 0) {
      entryExec(p_ctx,atts);
    } else if(strcmp(name,"request_list") == 0) {
      entryReq(p_ctx,atts);
    } else {
      handleGstep(p_ctx,name,atts);
    }
    break;
  case ELE_NORMAL_W_EXEC:
  case ELE_VUP_W_EXEC:
    handleExec(p_ctx,name,atts);
    break;
  case ELE_NORMAL_W_REQ:
  case ELE_NORMAL_D_REQ:
  case ELE_VUP_W_REQ:
  case ELE_VUP_D_REQ:
    handleReq(p_ctx,name,atts);
    break;
  }
}


/***************************************
 * elementEnd
 **************************************/
void
elementEnd(void *userData, const XML_Char *name) {
  PARSE_CTX_t *p_ctx = (PARSE_CTX_t*)userData;
  switch(p_ctx->state) {
  case ELE_NONE:
    PARSE_ASSERT_EXIT(0);
    break;
  case ELE_TSKM_CFG:
    if(strcmp(name,"tskm_cfg") == 0) {
      exitTskmCfg(p_ctx);
    } else {
      PARSE_ASSERT_EXIT(0);
    }
    break;
  case ELE_SERVICE_LIST:
    if(strcmp(name,"service_list") == 0) {
      exitServiceList(p_ctx);
    }
    break;
  case ELE_SERVICE:
    if(strcmp(name,"service") == 0) {
      exitService(p_ctx);
    }
    break;
  case ELE_SUBGID_LIST:
    if(strcmp(name,"subgid_list") == 0) {
      exitSubgidList(p_ctx);
    }
    break;
  case ELE_NORMAL_WAKEUP:
    if(strcmp(name,"normal_wakeup") == 0) {
      exitNormalWakeup(p_ctx);
    }
    break;
  case ELE_NORMAL_SHUTDOWN:
    if(strcmp(name,"normal_shutdown") == 0) {
      exitNormalShutdown(p_ctx);
    }
    break;
  case ELE_VUP_WAKEUP:
    if(strcmp(name,"vup_wakeup") == 0) {
      exitVupWakeup(p_ctx);
    }
    break;
  case ELE_VUP_SHUTDOWN:
    if(strcmp(name,"vup_shutdown") == 0) {
      exitVupShutdown(p_ctx);
    }
    break;
  case ELE_NORMAL_D_GSTEP:
  case ELE_NORMAL_W_GSTEP:
  case ELE_VUP_D_GSTEP:
  case ELE_VUP_W_GSTEP:
    if(strcmp(name,"global_step") == 0) {
      exitGstep(p_ctx);
    }
    break;
  case ELE_NORMAL_W_EXEC:
  case ELE_VUP_W_EXEC:
    if(strcmp(name,"exec_list") == 0) {
      exitExec(p_ctx);
    }
    break;
  case ELE_NORMAL_D_REQ:
  case ELE_NORMAL_W_REQ:
  case ELE_VUP_D_REQ:
  case ELE_VUP_W_REQ:
    if(strcmp(name,"request_list") == 0) {
      exitReq(p_ctx);
    }
    break;
  }
}
/***************************************
 * parseXmlFile
 **************************************/
int
parseXmlFile(const char* file,XML_Parser parser) {
  int ret = -1;
  FILE* fp = fopen(file, "r");
  if (fp == NULL) {
    goto ERROR;
  }

  while(1) {
    char *buf = (char*) XML_GetBuffer(parser, BUFSIZE);
    if (!buf) {
      goto ERROR;
    }

    size_t nread = fread(buf, sizeof(char), BUFSIZE, fp);
    if (ferror(fp)) {
      goto ERROR;
    }

    if (!XML_ParseBuffer(parser, nread, feof(fp))) {
      goto ERROR;
    }

    if (feof(fp)) {
      break;
    }
  }
  ret = 0;
ERROR:
  if(fp) {
    fclose(fp);
  }
  return ret;
}

/***************************************
 * externalHandler
 **************************************/
int
externalHandler(XML_Parser parser,
                const XML_Char* content, const XML_Char* base,
                const XML_Char* systemId, const XML_Char* publicId) {
  printf("parse %s \n",systemId);

  int ret = XML_ERROR_EXTERNAL_ENTITY_HANDLING;
  XML_Parser extparser = NULL;

  extparser = XML_ExternalEntityParserCreate(parser, content, NULL);
  if(extparser ==NULL) {
    goto ERROR;
  }

  if(parseXmlFile(systemId,extparser) != 0) {
    goto ERROR;
  }
  ret = XML_STATUS_OK;
ERROR:
  if(extparser) {
    XML_ParserFree(extparser);
  }
  return ret;
}

/***************************************
 * usage
 **************************************/
void
usage(const char* cmd) {
  printf("usage:%s xmlfile\n",cmd);
}

/***************************************
 * main
 **************************************/
int
main (int argc, char *argv[]) {
  struct stat statinfo;
  int ret=-1;
  int xmlRet;
  const char* inpath;
  char  tmpstr1[255];
  char  tmpstr2[255];
  const char* workdir;
  const char* infile;
  XML_Parser parser;
  PARSE_CTX_t ctx;

  if(argc < 2) {
    usage(argv[0]);
    goto ERROR;
  }

  inpath = argv[1];
  if(0 != stat(inpath,&statinfo)) {
    fprintf(stderr, "%s:%s",strerror(errno),inpath);
    goto ERROR;
  }
  memset(&ctx,0,sizeof(ctx));

  strcpy(tmpstr1,inpath);
  strcpy(tmpstr2,inpath);
  workdir=dirname(tmpstr1);
  infile=basename(tmpstr2);

  if(0 != chdir(workdir)) {
    fprintf(stderr, "%s:%s",strerror(errno),workdir);
    goto ERROR;
  }

  ctx.fp = fopen(OUTFILE, "w");
  if (ctx.fp == NULL) {
    goto ERROR;
  }
  fprintf(ctx.fp,"//This file is created automatically from %s.\n",inpath);
  fprintf(ctx.fp,"//So you shall not modify this file immediately.\n");

  /* create XML parser */
  if ((parser = XML_ParserCreate(NULL)) == NULL) {
    fprintf(stderr, "parser creation error\n");
    goto ERROR;
  }
  XML_SetUserData(parser,&ctx);
  XML_SetParamEntityParsing(parser,XML_PARAM_ENTITY_PARSING_ALWAYS);  // Allow External Entities
  XML_SetExternalEntityRefHandler(parser,externalHandler);
  XML_SetElementHandler(parser, elementStart, elementEnd);

  if(parseXmlFile(infile,parser) != 0) {
    goto ERROR;
  }
  ret = 0;

ERROR:
  if(parser) {
    XML_ParserFree(parser);
  }
  if(ctx.fp) {
    fclose(ctx.fp);
  }
  char buf[255];
  sprintf(buf,"cat %s >> %s",TMPFILE,OUTFILE);
  printf("%s\n",buf);
  system(buf);
  unlink(TMPFILE);
  return ret;
}

