/**
* File Name :AgentManager/AgentJobMsg.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : 二 12/29 22:57:18 2015
*/
#include "AgentJobMsg.h"

#define MAX_JOB_LIST   100
pJobList JOB_CreateList(){
    pJobDes jobdes   = NULL;
    pJobList jobList = (pJobList)malloc(sizeof(JobList));

    if(NULL == jobList ){
        return JOB_CREATELIST_ERROR;
    }
    jobdes = (pJobDes)malloc(sizeof(JobDes));
    if(NULL == jobdes){
        free(jobList);
        joblist = NULL;
        return JOB_CREATELIST_ERROR;
    }
    for(int i=0;i<MAX_JOB_LIST;i++){
        jobdes[i].jobID    = -1;
        jobdes[i].cmdID    = -1;
        memset(jobdes[i].describe,0,MAX_DESCRIBE_LEN);
        jobdes[i].jobState = JOB_STATE_RELEASE;
        jobdes[i].fun      = NULL;
    }
    jobList -> joblist     = jobdes;
    jobList -> MaxJobNum   = MAX_JOB_LIST;
    jobList -> jobNow      = 0;
    jobList -> echostate   = JOB_ECHO_ON;
    return jobList;
}

// 申请一个新的JobID
int JOB_GetFreshJobID(pJobList list){
    int times = -1;
    int i = 0;
    while(times < 1000){
        for(i=0;i<MAX_JOB_LIST;i++){
            if(((list->joblist)[i]).jobState == JOB_STATE_RELEASE){
                list->jobNow++;
                return i;
            }
        }
        MIC_USLEEP(10);
    }
    return JOB_GetFreshJobID_ERROR;
}

// 设置 Job 描述
int JOB_SetJOB(pJobList list,int jobid,
        char *describe){
    if(list == NULL || list->joblist){
        return JOB_SETJOB_ERROR;
    }
    ((list->joblist)[jobid]).jobID = jobid;
    strncpy((list->joblist)[jobid].describe , describe,MAX_DESCRIBE_LEN);
    return JOB_SETJOB_OK;
}

// 配置 Job 资源
int JOB_SetJOBState(pJobList list,int jobid,
        int jobstate,CBF_Jobfun fun){
    if(list == NULL || list->joblist){
        return JOB_SETJOBSTATE_ERROR;
    }
    ((list->joblist)[jobid]).jobState = jobstate;
    ((list->joblist)[jobid]).fun      = fun;
    return JOB_SETJOBSTATE_OK;
}

// 释放 Job 资源
int JOB_ReleaseJob(pJobList list,int jobid){
    if(list == NULL || list->joblist){
        return JOB_SETJOBSTATE_ERROR;
    }
    ((list->joblist)[jobid]).fun      = NULL;
    ((list->joblist)[jobid]).jobState = JOB_STATE_RELEASE;
    return JOB_RELEASEJOB_OK;
}

// 获取 Job 的摘要信息
char *JOB_GetSummary(pJobList list,int jobid,char *buffer,int maxbuflen){
    if(maxbuflen>300){return JOB_GETSUMMARY_ERROR;}
    ssprintf(buffer,"describe :%s , %d",
        ((list->joblist)[jobid]).describe,
        ((list->joblist)[jobid]).jobState);
    return JOB_GETSUMMARY_OK;
}
