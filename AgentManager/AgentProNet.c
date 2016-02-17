/**
* File Name :AgentManager/AgentProNet.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : 一 12/28 20:14:42 2015
*/
#include "AgentProNet.h"
#include "GlobalValue.h"
#include "AgentJobMsg.h"
#include "AgentCMDParse.h"

//===========================================================

int PROTO_SendProto(pPCConn conn,pAgent_proto proto){
    char cmdbuff[MAX_PROTO_BUFLEN];
    if(conn == NULL || proto == NULL ) {
if(conn == NULL){
    Printf_Error("conn is NULL");
}
if(proto == NULL){
    Printf_Error("proto is NULL");
}
        return PROTO_SENDPROTO_ERROR;
    }
    if(proto->argLen + 24 > MAX_PROTO_BUFLEN){
        Printf_Error("PROTOBUFLEN_OVERFLOW !!!!!");
        return PROTO_SENDPROTO_ERROR;
    }
    API_m_itochar(proto->cmdType,&(cmdbuff[0])     ,4);
    API_m_itochar(proto->cmdID  ,&(cmdbuff[4])     ,4);
    API_m_itochar(proto->jobID  ,&(cmdbuff[8])     ,4);
    API_m_itochar(proto->fromID ,&(cmdbuff[12])    ,4);
    API_m_itochar(proto->toID   ,&(cmdbuff[16])    ,4);
    API_m_itochar(proto->argLen ,&(cmdbuff[20])    ,4);
    memcpy(&(cmdbuff[24]),proto->cmdargs,proto->argLen+1);

////////////// recode here
    PCCONN_SendData(conn,cmdbuff,MAX_PROTO_BUFLEN);
    return PROTO_SENDPROTO_OK;
}

pAgent_proto PROTO_RecvProto(pPCConn conn){
    int nrecv;
    char cmdbuff[MAX_PROTO_BUFLEN];
    pAgent_proto proto = PROTO_CreateProto();
    if(proto == PROTO_CREATEPROTO_ERROR){
        free(proto);
        proto = NULL;
        return PROTO_RECVPROTO_ERROR;
    }
    nrecv = PCCONN_RecvData(conn,cmdbuff,MAX_PROTO_BUFLEN);
    if(nrecv != MAX_PROTO_BUFLEN){
        Printf_Error("PROTO_RecvProto recv len error");
        return PROTO_RECVPROTO_ERROR;
    }
    proto->cmdType = API_m_chartoi(&(cmdbuff[ 0]),4);
    proto->cmdID   = API_m_chartoi(&(cmdbuff[ 4]),4);
    proto->jobID   = API_m_chartoi(&(cmdbuff[ 8]),4);
    proto->fromID  = API_m_chartoi(&(cmdbuff[12]),4);
    proto->toID    = API_m_chartoi(&(cmdbuff[16]),4);
    proto->argLen  = API_m_chartoi(&(cmdbuff[20]),4);
    PROTO_SetArgs(proto,proto->argLen,&(cmdbuff[24]));
    return proto;
}


int PROTO_RecvState(pPCConn conn){
    if( conn == NULL){
        return PROTO_RECVSTATE_SOCKET_ERROR;
    }
    int res = API_socket_read_state(conn->cmd_socket,0,5);
    if(res == SOCKET_CAN_READ_STATE ){
        return PROTO_RECVSTATE_CANRECV;
    }
    else if ( SOCKET_OVER_TIME == res ){
        return PROTO_RECVSTATE_OVERTIME;
    }
    return PROTO_RECVSTATE_SOCKET_ERROR;
}


///=====================================================
int PROTO_SendPCNodeInfo(pPCConn conn,pPCNodeInfo info){
    int jobid = JOB_GetFreshJobID(
        GLOBAL_GetJobList()
    );
    if(jobid == JOB_GETFRESHJOBID_ERROR){
        return PROTO_SENDPROTO_ERROR;
    }
    pAgent_proto proto = PROTO_CreateProto();
    PROTO_SetCMD(proto,
        CMDTYPE_NEWCONN,     // 新节点连接
        CMDID_NEWNODE_HERE,  // 新节点连接
        jobid);
    PROTO_SetAddress(proto,
        -1,-1);              // 无须设置消息来回
    PROTO_SetArgs(proto,
        0,"");
    PROTO_SendProto(conn,proto);
    return 1;
}

pPCNodeInfo PROTO_AnalysisPCNodeInfo(pPCConn conn,pAgent_proto proto){
    int nrecv;
    char *cmdbuff;
    pPCNodeInfo info = PCNODE_Create();
    char idbuf[4],ostypebuf[4];
    char nodetypebuf[4],namelenbuf[4];
    char pcname [MAX_PCNAME_LEN];
    int id,ostype,nodetype,namelen;
    
    if( proto == PROTO_CREATEPROTO_ERROR 
        || info == NULL){
        return PROTO_ANALYSISPCNODEINFO_ERROR;
    }
    if(proto->cmdargs == NULL){
        return PROTO_ANALYSISPCNODEINFO_ERROR;
    }
    cmdbuff = proto->cmdargs;
//    memcpy(cmdbuff,proto->cmdargs,MAX_ARG_LEN);
    memcpy(idbuf      , &(cmdbuff[ 0]),4);
    memcpy(ostypebuf  , &(cmdbuff[ 4]),4);
    memcpy(nodetypebuf, &(cmdbuff[ 8]),4);
    memcpy(namelenbuf , &(cmdbuff[12]),4);
namelen  = API_m_chartoi(namelenbuf ,4);
    memcpy(pcname    , &(cmdbuff[16]),namelen);
    id       = API_m_chartoi(idbuf      ,4);
    ostype   = API_m_chartoi(ostypebuf  ,4);
    nodetype = API_m_chartoi(nodetypebuf,4);
    PCNODE_SETAllData(info,
        id,
        ostype,
        pcname,
        nodetype,
        conn->ConnType,
        conn->IPaddr,
        conn->port,
        conn->cmd_socket
    );
    return info;
}

