/**
* File Name :AgentManager/AgentProtocol.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : 一 12/28 19:57:38 2015
*/
#include "AgentProtocol.h"

int PROTO_Init(){   // init cbfnode list
    return PROTO_INIT_OK;
}

int PROTO_CreateProto(pAgent_proto proto,int cmdtype,int cmdid,int jobid){
    proto = (pAgent_proto)malloc(sizeof(Agent_proto));
    if(proto == NULL){
        return PROTO_CREATEPROTO_ERROR;
    }
    proto->cmdType = cmdtype;
    proto->cmdID   = cmdid;
    proto->jobID   = jobid;
    proto->fromID  = -1;
    proto->toID    = -1;
    proto->argLen  = 0;
    proto->cmdargs = NULL;
    return PROTO_CREATEPROTO_OK;
}

int PROTO_SetAddress (pAgent_proto proto,int fromid,int toid){
    if(proto == NULL){
        return PROTO_SETADDRESS_ERROR;
    }
    proto->fromID = fromid;
    proto->toID   = toid;
    return PROTO_SETADDRESS_OK;
}

int PROTO_SetArgs (pAgent_proto proto,int arglen,char *cmdargs){
    if(proto == NULL 
        || arglen > MAX_ARG_LEN){
        return PROTO_SETARGS_ERROR;
    }
    if(proto->cmdargs != NULL){
        free(proto->cmdargs);
        proto->cmdargs = NULL;
    }
    proto->cmdargs = (char *)malloc(sizeof(arglen+4));
    memcpy(proto->cmdargs,cmdargs,arglen);
    proto->argLen = arglen;
    return PROTO_SETARGS_OK;
}

int PROTO_FreeProto(pAgent_proto proto){
    if(proto == NULL){
        return PROTO_FREEPROTO_ERROR;
    }
    free(proto->cmdargs);
    proto->cmdargs = NULL;
    free(proto);
    return PROTO_FREEPROTO_OK;
}
