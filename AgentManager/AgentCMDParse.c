/**
* File Name :AgentManager/AgentCMDProtocol.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : Tue 29 Dec 2015 07:31:43 PM PST
*/

#include "AgentCMDParse.h"
#include "GlobalValue.h"
#include "AgentIDCtrl.h"
#include "AgentJobMsg.h"
#include "PCNodeManager.h"
#include "AgentConnHandle.h"
#include "AgentChildSync.h"

//    int cmdType;
//    int cmdID;

#define ON_NEWCONN_ERROR    -1
#define ON_NEWCONN_OK        1
int on_NewConn(pAgent_proto proto,pPCConn conn){
    Printf_DEBUG("new agent connected here");
    if( ON_NEWAGENT_CONNECT_ERROR == 
        on_NewAgent_Connect(proto,conn)){
        return ON_NEWCONN_ERROR;
    }
    return ON_NEWCONN_OK;
}

int on_MyMsgHere(pAgent_proto proto,pPCConn conn){
    switch(proto->cmdID){
    case CMDID_NEWID_ASK:
        Printf_DEBUG("fresh new id and give it back");
        on_AGENT_ID_Ask(proto,conn);
        break;
    case CMDID_NEWID_RPL:
        Printf_DEBUG("get a new id replay how to use it ?");
        JOB_CloseJob(GLOBAL_GetJobList(),
            proto->jobID,
            proto->cmdargs);
    case CMDID_CHILDSYNC_UPPER:
        Printf_DEBUG("sync agent info upper");
        on_SyncInfoUpper(proto);
        break;
    default:
        Printf_DEBUG("Unsupport now %d",proto->cmdID);
        break;
    }
    return 1;
}

int CMDParse_SendProto_Upper(pAgent_proto proto){
    int result = CMDPARSE_SENDPROTO_UPPER_ERROR;
    if(PCMANAGER_Get_RootID() == 0){
        // no upper , I'm admin now
        goto error_exit;
    }
    pPCNodeInfo father = PCMANAGER_Get_FatherNode();
    if(father == PCMANAGER_GET_FATHERNODE_ERROR){
        // get father info error
        goto error_exit;
    }
    pPCConn conn = &(father->conn);
    if(conn == PCCONN_COPY_ERROR){
        goto error_exit;
    }
    PROTO_SendProto(conn,proto);
    MIC_USLEEP(1);
ok_exit:
    result = CMDPARSE_SENDPROTO_UPPER_OK;
    goto exit;
error_exit:
    result = CMDPARSE_SENDPROTO_UPPER_ERROR;
    goto exit;
exit:
    PCNODE_Free(father);
    conn   = NULL;
    father = NULL;
    return result;
}

#define ON_TRANSMIT_ERROR   -1
#define ON_TRANSMIT_OK       1
int on_Transmit(pAgent_proto proto,pPCConn conn){
    Printf_DEBUG("normal cmd info");

    if(PCMANAGER_Get_RootID() == proto->toID){
        Printf_DEBUG("This is my Msg");
        on_MyMsgHere(proto,conn);
        goto exit;
    }

    switch(proto->toID){
    case AGENTID_ADMIN: // To Admin, transmit it upper
        Printf_DEBUG("To Admin, transmit it upper");
        break;
    case AGENTID_UPPER_AGENT: 
        // analysis it && To All upper Agent
        Printf_DEBUG("To All upper Agent && analysis it ");
        on_MyMsgHere(proto,conn);
        CMDParse_SendProto_Upper(proto);
        break;
    default:
        Printf_DEBUG("Need find this agent from tree %d", proto->toID);
        break;
    }
exit:
    return ON_TRANSMIT_OK;
}

int CMDParse_And_Do(pAgent_proto proto,pPCConn conn){
    int result = CMDPARSE_AND_DO_ERROR;
    switch(proto->cmdType){
    case CMDTYPE_NEWCONN:
        if(ON_NEWCONN_OK == on_NewConn(proto,conn)){
            result = CMDPARSE_AND_DO_OK;
        }
        break;
    case CMDTYPE_TRANSMIT:
        if(ON_TRANSMIT_OK == on_Transmit(proto,conn)){
            result = CMDPARSE_AND_DO_OK;
        }
        break;
    default:
        Printf_Error("UNKNOWN proto->cmdType %d",proto->cmdType);
        break;
    }
    return result;
}

