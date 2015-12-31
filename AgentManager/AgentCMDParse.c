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

//    int cmdType;
//    int cmdID;

#define ON_NEWCONN_ERROR    -1
#define ON_NEWCONN_OK        1
int on_NewConn(pAgent_proto proto,pPCConn conn){
    Printf_DEBUG("new agent connected here");
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
        break;
    default:
        Printf_DEBUG("Unsupport now %d",proto->cmdID);
        break;
    }
    return 1;
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
    case 0:
        Printf_DEBUG("To Admin, transmit it upper");
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

