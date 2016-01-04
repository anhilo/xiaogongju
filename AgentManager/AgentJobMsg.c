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
    int i  = 0;

    if(NULL == jobList ){
        return JOB_CREATELIST_ERROR;
    }
    jobdes = (pJobDes)malloc(sizeof(JobDes)*MAX_JOB_LIST);
    if(NULL == jobdes){
        free(jobList);
        jobList = NULL;
        return JOB_CREATELIST_ERROR;
    }
    for(i=0;i<MAX_JOB_LIST;i++){
        jobdes[i].jobID    = -1;
        jobdes[i].cmdID    = -1;
        memset(jobdes[i].describe,0,MAX_DESCRIBE_LEN);
        jobdes[i].jobState = JOB_STATE_RELEASE;
    }
    jobList -> joblist     = jobdes;
    jobList -> MaxJobNum   = MAX_JOB_LIST;
    jobList -> jobNow      = 0;
    jobList -> freshlock   = JOB_FRESH_ID_UNLOCK;
    jobList -> echostate   = JOB_ECHO_ON;
    return jobList;
}

// 申请一个新的JobID
int JOB_GetFreshJobID(pJobList list){
    int times = -1;
    int i = 0;
    int result = JOB_GETFRESHJOBID_ERROR;
    times = 0;
    while(JOB_FRESH_ID_LOCK == list->freshlock){
        MIC_USLEEP(50);
        times ++;
        if(times > 1000){
            return result;
        }
    }
    list->freshlock = JOB_FRESH_ID_LOCK;
    times = 0;
    while(times < 1000){
        for(i=0;i<MAX_JOB_LIST;i++){
            if(((list->joblist)[i]).jobState == JOB_STATE_RELEASE){
                ((list->joblist)[i]).jobState = JOB_WAITCLOSEJOB_OK;
                list->jobNow++;
                result = i;
                goto exit;
            }
        }
        MIC_USLEEP(10);
    }
exit:
    list->freshlock = JOB_FRESH_ID_UNLOCK;
    return i;
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

// 配置 Job 状态
int JOB_SetJOBState(pJobList list,int jobid,
        int jobstate){
    if(list == NULL || list->joblist){
        return JOB_SETJOBSTATE_ERROR;
    }
    ((list->joblist)[jobid]).jobState = jobstate;
    return JOB_SETJOBSTATE_OK;
}

// 设置 Job 结果
int JOB_CloseJob(pJobList list,int jobid,
        char *jobResult){
    if(list == NULL || list->joblist){
        return JOB_CLOSEJOB_ERROR;
    }
    memcpy(((list->joblist)[jobid]).jobresult,
        jobResult,MAX_RESULT_LEN);
    ((list->joblist)[jobid]).jobState = JOB_STATE_CAN_STOP;
    return JOB_CLOSEJOB_OK;
}

// 释放 Job 资源
int JOB_ReleaseJob(pJobList list,int jobid){
    if(list == NULL || list->joblist){
        return JOB_SETJOBSTATE_ERROR;
    }
    ((list->joblist)[jobid]).jobState = JOB_STATE_RELEASE;
    return JOB_RELEASEJOB_OK;
}

// 获取 Job 的摘要信息
char *JOB_GetSummary(pJobList list,int jobid,char *buffer,int maxbuflen){
    if(maxbuflen>300){return JOB_GETSUMMARY_ERROR;}
    sprintf(buffer,"describe :%s , %d",
        ((list->joblist)[jobid]).describe,
        ((list->joblist)[jobid]).jobState);
    return buffer;
}

int JOB_WaitCloseJob(pJobList list,int jobid,
        int sec){
    int times =0;
    while((times++)<sec){
        if(((list->joblist)[jobid]).jobState == 
            JOB_STATE_CAN_STOP){
            return JOB_WAITCLOSEJOB_OK;
        }
        MIC_SLEEP(1);
    }
    return JOB_WAITCLOSEJOB_ERROR;
}

int JOB_GetResult(pJobList list,int jobid,char **buf,int *buflen){
    if(buf == NULL){
        return JOB_GETRESULT_ERROR;
    }
    (*buf) = (char *)malloc(sizeof(char)*MAX_RESULT_LEN);
//    pJobDes jobdes= (list->joblist);
    memcpy((*buf),(list->joblist)[jobid].jobresult,MAX_RESULT_LEN);
    *buflen = MAX_RESULT_LEN;
    return JOB_GETRESULT_OK;
}
