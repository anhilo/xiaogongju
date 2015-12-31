#ifndef _AGENTJOBMSG_H_
#define _AGENTJOBMSG_H_

#include "../generic.h"
#include "GlobalValue.h"

#define JOB_CREATELIST_ERROR    NULL
pJobList JOB_CreateList();

// 申请一个新的JobID
#define JOB_GETFRESHJOBID_ERROR  -1
int JOB_GetFreshJobID(pJobList list); 

// 设置 Job 描述
#define JOB_SETJOB_ERROR    -1
#define JOB_SETJOB_OK        1
int JOB_SetJOB(pJobList list,int jobid,
        char *describe);

// 配置 Job 状态
#define JOB_SETJOBSTATE_ERROR  -1 
#define JOB_SETJOBSTATE_OK      1 
int JOB_SetJOBState(pJobList list,int jobid,
        int jobstate);

// 设置 Job 结果
#define JOB_CLOSEJOB_ERROR  -1
#define JOB_CLOSEJOB_OK      1
int JOB_CloseJob(pJobList list,int jobid,
        char *jobResult);

// 等待 Job 结束
#define JOB_WAITCLOSEJOB_ERROR  -1
#define JOB_WAITCLOSEJOB_OK      1
int JOB_WaitCloseJob(pJobList list,int jobid,
        int sec);


#define JOB_GETRESULT_ERROR    -1
#define JOB_GETRESULT_OK        1
int JOB_GetResult(pJobList list,int jobid,char **buf,int *buflen);

// 释放 Job 资源
#define JOB_RELEASEJOB_ERROR   -1
#define JOB_RELEASEJOB_OK       1
int JOB_ReleaseJob(pJobList list,int jobid);

// 获取 Job 的摘要信息
#define JOB_GETSUMMARY_ERROR  NULL
char *JOB_GetSummary(pJobList list,int jobid,char *buffer,int maxbuflen);

#endif
