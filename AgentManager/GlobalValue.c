/**
* File Name :AgentManager/GlobalValue.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : Wed 30 Dec 2015 02:41:42 AM PST
*/
#include "GlobalValue.h"
//=====================================
pJobList joblist = NULL;
//=====================================

int GLOBAL_SetJobList(pJobList list){
    if(list != NULL){
        Printf_Error("GLOBAL_SetJobList NULL joblist");
        return GLOBAL_SETJOBLIST_ERROR;
    }
    joblist = list;
    return GLOBAL_SETJOBLIST_OK;
}

pJobList GLOBAL_GetJobList(){
    if(joblist == NULL){
        Printf_Error("GLOBAL_GetJobList NULL joblist");
        return GLOBAL_GETJOBLIST_ERROR;
    }
n
    return joblist;
}
