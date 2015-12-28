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

typedef int (*CBF_Jobfun)(pAgent_proto proto);
typedef struct JOB_DESCRIBE{
    int jobID;   // 工作编号
    int cmdID;   // 工作内容编号(指令编号)
    char describe[MAX_DESCRIBE_LEN];
        // 工作描述
    int jobState;
        // 工作当前状态
    CBF_Jobfun fun;
}JobDes,*pJobDes;

#define JOB_ECHO_ON    1 // 开启工作回显
#define JOB_ECHO_OFF   2 // 关闭工作回显
typedef struct JOB_LIST{
    int MaxJobNum;
    int jobNow;
    int echostate;
    pJobDes joblist[MAX_JOB_LIST];
}JobList,*pJobList;

pJobList JOB_CreateList();

// 申请一个新的JobID
int JOB_GetFreshJobID(pJobList list); 

// 设置 Job 描述
int JOB_SetJOB(pJobList list,int jobid,
        char *describe);

// 配置 Job 状态
int JOB_SetJOBState(pJobList list,int jobid,
        int jobstate);

// 释放 Job 资源
int JOB_ReleaseJob(pJobList list,int jobid);

// 获取 Job 的摘要信息
char *JOB_GetSummary(pJobList list,int jobid);

#endif
