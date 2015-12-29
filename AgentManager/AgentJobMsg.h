#ifndef _AGENTJOBMSG_H_
#define _AGENTJOBMSG_H_

#include "../generic.h"

#define MAX_DESCRIBE_LEN     50
#define JOB_STATE_UNKNOWN    0 // 未知
#define JOB_STATE_STARTING   1 // 启动
#define JOB_STATE_RUNNING    2 // 运行
#define JOB_STATE_STOPING    3 // 停止
#define JOB_STATE_WAITE_RPL  4 // 等待结果
#define JOB_STATE_PERFECT    5 // 运行结束
#define JOB_STATE_RELEASE    6 // 释放

typedef int (*CBF_Jobfun)(pAgent_proto proto,JobDes job);
typedef struct JOB_DESCRIBE{
    int jobID;   // 工作编号
    int cmdID;   // 工作内容编号(指令编号)
    char describe[MAX_DESCRIBE_LEN];
        // 工作描述
    int jobState;
        // 工作当前状态
    CBF_Jobfun fun;
}JobDes,*pJobDes;

#define JOB_ECHO_ON      1 // 开启工作回显
#define JOB_ECHO_OFF     2 // 关闭工作回显
typedef struct JOB_LIST{
    int MaxJobNum;
    int jobNow;
    int echostate;
    pJobDes joblist;
}JobList,*pJobList;

#define JOB_CREATELIST_ERROR    NULL
pJobList JOB_CreateList();

// 申请一个新的JobID
#define JOB_GetFreshJobID_ERROR  -1
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
        int jobstate,CBF_Jobfun fun);

// 释放 Job 资源
#define JOB_RELEASEJOB_ERROR   -1
#define JOB_RELEASEJOB_OK       1
int JOB_ReleaseJob(pJobList list,int jobid);

// 获取 Job 的摘要信息
#define JOB_GETSUMMARY_ERROR   -1
#define JOB_GETSUMMARY_OK       1
int JOB_GetSummary(pJobList list,int jobid,char *buffer,int maxbuflen);

#endif
