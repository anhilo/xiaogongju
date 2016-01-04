/**
* File Name :AgentManager/AgentProNet.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : 一 12/28 20:14:42 2015
*/
#include "AgentProNet.h"
#include "GlobalValue.h"
#include "AgentJobMsg.h"

//===========================================================
//   inline  function 
//      m_SendData_CmdTunnel( conn,data, data, len);
#define M_SENDDATA_CMDTUNNEL_ERROR   -1
#define M_SENDDATA_CMDTUNNEL_OK       1
int m_SendData_CmdTunnel(pPCConn conn,char *data,int datalen){
    int nsend = -1;
    int cmdsocket = -1;
    if(conn == NULL || conn->cmd_socket == -1){
        return M_SENDDATA_CMDTUNNEL_ERROR;
    }
    // get cmd_socket
    cmdsocket = conn->cmd_socket;
    // session busylock
    while(NET_SESSION_BUSY_NOW == conn->BusyType){
        MIC_USLEEP(10);
    }
    // session lock it
    conn->BusyType = NET_SESSION_BUSY_NOW;
    // send data
Printf_DEBUG("send data len is %d ",datalen);
    nsend = API_socket_send(cmdsocket,data,datalen);
    // unlock it 
    conn->BusyType = NET_SESSION_UNUSED_NOW;
    // check send state
    if(nsend != datalen){
        return M_SENDDATA_CMDTUNNEL_ERROR;
    }
    return M_SENDDATA_CMDTUNNEL_OK;
}

//===========================================================

int PROTO_SendProto(pPCConn conn,pAgent_proto proto){
    char cmdbuff[MAX_PROTO_BUFLEN];
    if(conn == NULL || proto == NULL ) {
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
    m_SendData_CmdTunnel(conn,cmdbuff,MAX_PROTO_BUFLEN);
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
    int res = API_socket_read_state(conn->cmd_socket,5,0);
    if(res == SOCKET_CAN_READ_STATE ){
        return PROTO_RECVSTATE_CANRECV;
    }
    else if ( SOCKET_OVER_TIME == res ){
        return PROTO_RECVSTATE_OVERTIME;
    }
    return PROTO_RECVSTATE_SOCKET_ERROR;
}

///=======================================================
///   PCConn Functions
///=======================================================
pPCConn PCCONN_CreatePCConnFromSocket(int sock){
    pPCConn conn = (pPCConn)malloc(sizeof(PCConn));
    if(conn == NULL){
        return PCCONN_CREATEPCCONNFROMSOCKET_ERROR;
    }
    conn->ConnType   = CONNTYPE_REVERSE_CONNECT;
    conn->cmd_socket = sock;
    conn->BusyType   = NET_SESSION_UNUSED_NOW;
    return conn;
}

pPCConn PCCONN_CreatePCConnFromIPPort(char *ip,int port,int sock){
    pPCConn conn = PCCONN_CreatePCConnFromSocket(sock);
    if( PCCONN_CREATEPCCONNFROMSOCKET_ERROR == 
        conn){
        return PCCONN_CREATEPCCONNFROMIPPORT_ERROR;
    }
    conn->ConnType   = CONNTYPE_DIRECT_CONNECT;
    strncpy(conn->IPaddr,ip,MAX_IP_ADDR_LEN);
    conn->port       = port;
    conn->BusyType   = NET_SESSION_UNUSED_NOW;
    return conn;
}

int PCCONN_Free(pPCConn conn){
    if(conn == NULL){
        return PCCONN_FREE_ERROR;
    }
    free(conn);
    conn = NULL;
    return PCCONN_FREE_OK;
}

pPCConn PCCONN_Copy(pPCConn conn){
    pPCConn conn1 = NULL;
    if(conn == NULL){
        return PCCONN_COPY_ERROR;
    }
    conn1 = (pPCConn)malloc(sizeof(PCConn));
    if(conn1 == NULL){
        return PCCONN_COPY_ERROR;
    }
    conn1->ConnType   = conn->ConnType;
    conn1->BusyType   = conn->BusyType;
    conn1->port       = conn->port;
    conn1->cmd_socket = conn->cmd_socket;
    memcpy(conn1->IPaddr,conn->IPaddr,MAX_IP_ADDR_LEN);
    return conn1;
}

///=====================================================
int PROTO_SendPCNodeInfo(pPCConn conn,pPCNodeInfo info){
    int jobid = JOB_GetFreshJobID(
        GLOBAL_GetJobList()
    );
    if(jobid == JOB_GETFRESHJOBID_ERROR){
        return PROTO_SENDPROTO_ERROR;
    }
    //pAgent_proto proto = PROTO_CreateProto(
Printf_DEBUG("NEED add COde Here");
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
    
Printf_DEBUG("PROTO_AnalysisPCNodeInfo()");
    if( proto == PROTO_CREATEPROTO_ERROR 
        || info == NULL){
        return PROTO_ANALYSISPCNODEINFO_ERROR;
    }
if(proto->cmdargs == NULL){
    Printf_DEBUG("NULLL?      arglen = %d ",proto->argLen);
    Printf_DEBUG("NULLL????????");
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

int PCCONN_SendData(pPCConn conn,char *data,int datalen){
    int res = m_SendData_CmdTunnel(conn,data,datalen);
    if(res == M_SENDDATA_CMDTUNNEL_ERROR){
        return PCCONN_SENDDATA_ERROR;
    }
    return datalen;
}

int PCCONN_RecvData(pPCConn conn,char *data,int maxlen){
    int nrecv;
    char cmdbuff[MAX_PROTO_BUFLEN];
    if( conn == NULL ){
        return PCCONN_RECVDATA_ERROR;
    }
    nrecv = API_socket_recv(conn->cmd_socket,cmdbuff,MAX_PROTO_BUFLEN);
    if(nrecv == 0){ return PCCONN_RECVDATA_ERROR; }
    if(nrecv > maxlen){
        Printf_Error("nrecv > maxlen");
        return PCCONN_RECVDATA_ERROR;
    }
    memcpy(data,cmdbuff,maxlen);
    return nrecv;
}
