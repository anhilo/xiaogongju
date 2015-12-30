#ifndef _AGENTJOBMSG_H_
#define _AGENTJOBMSG_H_

#include "../generic.h"

#define MAX_DESCRIBE_LEN     50
#define MAX_RESULT_LEN       50
#define JOB_STATE_UNKNOWN    0 // 未知
#define JOB_STATE_RELEASE    1 // 初始状态
#define JOB_STATE_RUNNING    2 // 正在运行
#define JOB_STATE_WAIT_RPL   3 // 等待结果
#define JOB_STATE_CAN_STOP   4 
        // 可以结束(等到了上游回复)

typedef struct JOB_DESCRIBE{
    int jobID;   // 工作编号
    int cmdID;   
        // 工作内容编号(指令编号)
        // 根据这个指令编号分配回调处理函数
    char describe[MAX_DESCRIBE_LEN];
        // 工作描述
    int jobState;
        // 工作当前状态
    char jobresult[MAX_RESULT_LEN];
        // 设置工作的结果
}JobDes,*pJobDes;

#define JOB_ECHO_ON      1 // 开启工作回显
#define JOB_ECHO_OFF     2 // 关闭工作回显

#define JOB_FRESH_ID_LOCK    1
#define JOB_FRESH_ID_UNLOCK  0
typedef struct JOB_LIST{
    int MaxJobNum;
    int jobNow;
    int echostate;
    int freshlock;
    pJobDes joblist;
}JobList,*pJobList;

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
int JOB_GetResult(pJobList list,int jobid,char *buf,int *buflen);

// 释放 Job 资源
#define JOB_RELEASEJOB_ERROR   -1
#define JOB_RELEASEJOB_OK       1
int JOB_ReleaseJob(pJobList list,int jobid);

// 获取 Job 的摘要信息
#define JOB_GETSUMMARY_ERROR   -1
#define JOB_GETSUMMARY_OK       1
int JOB_GetSummary(pJobList list,int jobid,char *buffer,int maxbuflen);

#endif
