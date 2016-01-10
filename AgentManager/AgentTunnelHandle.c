/**
* File Name :AgentManager/AgentTunnelHandle.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : Mon 16 Nov 2015 03:27:18 AM PST
*/
#include "AgentTunnelHandle.h"
#include "PCNodeManager.h"
#include "PCNodeInfo.h"
#include "AgentConversationProxy.h"
#include "AgentCMDParse.h"
#include "AgentJobMsg.h"
//=====================================
pfun_CBF_Tunnel cbf_newtunnel = NULL;
//=====================================

#define M_GETTARGETCONN_ERROR       NULL
pPCConn m_getTargetConn(int targetid){
    pPCNodeInfo info = PCMANAGER_GETNextJump(targetid);
    pPCConn conn = NULL;
    if(info == PCMANAGER_GETNEXTJUMP_ERROR){
        return M_GETTARGETCONN_ERROR;
    }
    conn = &(info->conn);
    if(conn == NULL){
        return M_GETTARGETCONN_ERROR;
    }
    return conn;
}

#define M_BUILDDIRECTTUNNEL_ERROR   NULL
pPCConn m_buildDirectTunnel(pPCConn nextconn,int targetid){
    pAgent_proto proto = NULL;
    pPCConn       conn = NULL;
    char targetbuf[4];
    if(nextconn == PCCONN_CONNECT_ERROR){
        goto error_exit;
    }
    conn = PCCONN_Connect(
        nextconn->IPaddr,
        nextconn->port
    );
    if(conn == PCCONN_CONNECT_ERROR){
        goto error_exit;
    }
    
    proto = PROTO_CreateProto();
    if(proto == PROTO_CREATEPROTO_ERROR){
        goto error_exit;
    }
    PROTO_SetCMD(proto,
        CMDTYPE_TRANSMIT,
        CMDID_NEWTUNNEL_ASK,
        -1);
    PROTO_SetAddress(proto,
        PCMANAGER_Get_RootID(),
        targetid);
    API_m_itochar(targetid,targetbuf,4);
    PROTO_SetArgs(proto,
        4,
        targetbuf);
    if(PROTO_SENDPROTO_ERROR == 
        PROTO_SendProto(conn,proto)){
        goto error_exit;
    }
    goto ok_exit;
error_exit:
    conn = M_BUILDDIRECTTUNNEL_ERROR;
ok_exit:
    PROTO_FreeProto(proto);
    return conn;
}

#define M_BUILDREVERSETUNNEL_ERROR    NULL
pPCConn m_buildReverseTunnel(pPCConn nextconn,int targetid){
    int jobid = -1;
    pPCConn conn       = M_BUILDREVERSETUNNEL_ERROR;
    pPCConn *conbuf    = &conn;
    pAgent_proto proto = NULL;
    char    *resultbuf = NULL;
    int       buflen   = sizeof(pPCConn);
    // check conn
    if(nextconn == M_GETTARGETCONN_ERROR){
        goto error_exit;
    }
    // send proto
    jobid = JOB_GetFreshJobID(
        GLOBAL_GetJobList()
    );
    if(jobid == JOB_GETFRESHJOBID_ERROR){
        goto error_exit;
    }
    proto = PROTO_CreateProto();
    PROTO_SetCMD(proto,
        CMDTYPE_TRANSMIT,
        CMDID_NEWTUNNEL_RC_ASK,
        jobid);
    PROTO_SetAddress(proto,
        PCMANAGER_Get_RootID(),
        targetid);
    PROTO_SetArgs(proto,
        0,
        "");
    if(PROTO_SENDPROTO_ERROR == 
        PROTO_SendProto(nextconn,proto)){
        goto error_exit;
    }
    // wait Job
    if(JOB_WAITCLOSEJOB_ERROR == 
        JOB_WaitCloseJob(
            GLOBAL_GetJobList(),
            jobid,
            1000)
    ){
        goto error_exit;
    }
    // get new conn
    JOB_GetResult(
        GLOBAL_GetJobList(),
        jobid,
        &resultbuf,
        &buflen);
    conbuf = &conn;
    memcpy(conbuf,resultbuf,sizeof(pPCConn));
//Printf_DEBUG(" -> 2 <-   The get conn addr is 0x%x",(unsigned int)conn);
    JOB_ReleaseJob(
        GLOBAL_GetJobList(),
        jobid);
//
    Printf_DEBUG("new  reverse tunnel asked here, need send targetid = %d",targetid);
    goto ok_exit;
error_exit:
    conn = M_BUILDREVERSETUNNEL_ERROR;
ok_exit:
    PROTO_FreeProto(proto);
    proto    = NULL;
    conbuf   = NULL;
    return conn;
}

//======================================
//       
//======================================
int AGENT_TUNN_Init(pfun_CBF_Tunnel fun){
    if(fun == NULL){
        return AGENT_TUNN_INIT_ERROR;
    }
    cbf_newtunnel = fun;
    return AGENT_TUNN_INIT_OK;
}

#define M_ON_NEWTUNNEL_HERE_ERROR   -1
#define M_ON_NEWTUNNEL_HERE_OK       1
int m_on_NewTunnel_Here(pPCConn conn){
    int socket = -1;
    if(conn == NULL){
        return M_ON_NEWTUNNEL_HERE_ERROR;
    }
    socket = conn->cmd_socket;
    char cmdmsg[200];
    int nrecv = API_socket_recv(socket,cmdmsg,200);
    cmdmsg[nrecv] = '\0';
    Printf_DEBUG("Recv ----> %s ....",cmdmsg);
    return M_ON_NEWTUNNEL_HERE_OK;
}


pPCConn AGENT_TUNN_BuildTunnel(int targetid){
    pPCConn nextconn = m_getTargetConn(targetid);
    pPCConn conn     = AGENT_TUNN_BUILDTUNNEL_ERROR;
    if(nextconn == M_GETTARGETCONN_ERROR){
Printf_Error("nextconn == M_GETTARGETCONN_ERROR targetid = %d",targetid);
        return AGENT_TUNN_BUILDTUNNEL_ERROR;
    }
    if(nextconn->ConnType == CONNTYPE_DIRECT_CONNECT){
        conn = m_buildDirectTunnel(nextconn,targetid);
        if(M_BUILDDIRECTTUNNEL_ERROR == conn){
Printf_Error("M_BUILDDIRECTTUNNEL_ERROR targetid = %d",targetid);
            return AGENT_TUNN_BUILDTUNNEL_ERROR;
        }
    }
    else if(CONNTYPE_REVERSE_CONNECT == 
        nextconn->ConnType){
        conn = m_buildReverseTunnel(nextconn,targetid);
        if(M_BUILDREVERSETUNNEL_ERROR == 
            conn){
Printf_Error("M_BUILDREVERSETUNNEL_ERROR targetid = %d",targetid);
            return AGENT_TUNN_BUILDTUNNEL_ERROR;
        }
    }
    else{
        Printf_Error("UNKNOWN nextconn -> ConnType");
        return AGENT_TUNN_BUILDTUNNEL_ERROR;
    }
    return conn;
}

int on_reverse_Tunnel_Ask(pAgent_proto upproto,pPCConn upconn){
    pAgent_proto proto = NULL;
    pPCConn       conn = NULL;
    char targetbuf[4];
    if(upconn == PCCONN_CONNECT_ERROR){
        goto error_exit;
    }
    conn = PCCONN_Connect(
        upconn->IPaddr,
        upconn->port
    );
    if(conn == PCCONN_CONNECT_ERROR){
        goto error_exit;
    }
    
    proto = PROTO_CreateProto();
    if(proto == PROTO_CREATEPROTO_ERROR){
        goto error_exit;
    }
    PROTO_SetCMD(proto,
        CMDTYPE_TRANSMIT,
        CMDID_NEWTUNNEL_RC_RPL,
        upproto->jobID);
    PROTO_SetAddress(proto,
        PCMANAGER_Get_RootID(),
        upproto->fromID);

    PROTO_SetArgs(proto,
        upproto->argLen,
        upproto->cmdargs);
    if(PROTO_SENDPROTO_ERROR == 
        PROTO_SendProto(conn,proto)){
        goto error_exit;
    }
    if(upproto -> toID == PCMANAGER_Get_RootID()){
        m_on_NewTunnel_Here(conn);
    }
    else{
        Printf_DEBUG("Bind conn and nextconn");
    }
    goto ok_exit;
error_exit:
    conn = M_BUILDDIRECTTUNNEL_ERROR;
ok_exit:
    PROTO_FreeProto(proto);
    return 1;
}

int on_reverse_Reply(pAgent_proto proto,pPCConn conn){
    int jobid = proto->jobID;
    pPCConn *addr = &conn;
    char *connAddr = (char *)malloc(sizeof(pPCConn));
//Printf_DEBUG(" -> 1 <-   The conn addr is 0x%x",(unsigned int)conn);
    memcpy(connAddr,(char*)addr,sizeof(pPCConn));
    JOB_CloseJob(
        GLOBAL_GetJobList(),
        jobid,
        connAddr);
    addr = NULL;
    free(connAddr);
    return 1;
}

int on_new_tunnel_ask(pAgent_proto proto ,pPCConn conn){
    Printf_DEBUG("New Tunnel asked here !!!!!");
    Printf_OK("Recv msg here ???");
    if(proto -> toID == PCMANAGER_Get_RootID()){
        m_on_NewTunnel_Here(conn);
    }
    else{
        Printf_DEBUG("Bind conn and nextconn");
    }
    return 1;
}
