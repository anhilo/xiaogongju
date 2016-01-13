/**
* File Name :AgentManager/AgentChildSync.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : Tue 17 Nov 2015 01:01:20 AM PST
*/

#include "AgentChildSync.h"
#include "PCNodeInfo.h"
#include "AgentMsgHandle.h"
#include "PCNodeManager.h"
#include "AgentIDCtrl.h"
#include "AgentCMDParse.h"

#define MAX_SENDCHILDINFO_LEN   200

#define M_SENDCHILDINFO_ERROR   -1
#define M_SENDCHILDINFO_OK       1
int m_SendChildInfo(int fatherid,int childid,
        int ostype,char *pcname){
    char buffer[MAX_SENDCHILDINFO_LEN];
    int namelen = strlen(pcname);
    if(MAX_PCNAME_LEN < namelen){
        return M_SENDCHILDINFO_ERROR;
    }
    int jobid = -1;
    pAgent_proto proto = PROTO_CreateProto();
    if( proto == PROTO_CREATEPROTO_ERROR ){
        return M_SENDCHILDINFO_ERROR;
    }
    API_m_itochar(fatherid,&(buffer[ 0]),4);
    API_m_itochar(childid ,&(buffer[ 4]),4);
    API_m_itochar(ostype  ,&(buffer[ 8]),4);
    API_m_itochar(namelen ,&(buffer[12]),4);
    strncpy(&(buffer[16]),pcname,MAX_PCNAME_LEN);
    PROTO_SetCMD(proto,CMDTYPE_TRANSMIT,
            CMDID_CHILDSYNC_UPPER,
            jobid);
    PROTO_SetAddress(proto,
        PCMANAGER_Get_RootID(),
        AGENTID_UPPER_AGENT);
    PROTO_SetArgs(proto,
        MAX_SENDCHILDINFO_LEN,
        buffer);
    if(CMDPARSE_SENDPROTO_UPPER_ERROR == 
        CMDParse_SendProto_Upper(proto)){
        return M_SENDCHILDINFO_ERROR;
    }
    return M_SENDCHILDINFO_OK;
}

#define M_RECV_AND_ADD_CHILD_ERROR   -1
#define M_RECV_AND_ADD_CHILD_OK       1
int m_Recv_And_Add_Child(pAgent_proto proto){
    pPCNodeInfo info = PCNODE_Create();
    int fatherid,childid,ostype;
    int namelen;
    char pcname[MAX_PCNAME_LEN];
    fatherid = API_m_chartoi(&((proto->cmdargs)[ 0]),4);
    childid  = API_m_chartoi(&((proto->cmdargs)[ 4]),4);
    ostype   = API_m_chartoi(&((proto->cmdargs)[ 8]),4);
    namelen  = API_m_chartoi(&((proto->cmdargs)[12]),4);
    strncpy(pcname, &((proto->cmdargs)[16]),MAX_PCNAME_LEN);
    PCNODE_SETAllData(
        info,                 // node
        childid,              // id
        ostype,               // ostype
        pcname,               // pcname
        MYSELF_NODE_REMOTE,   // NodeType
        CONNTYPE_UNKNOWN,     // ConnType
        "",                   // ipaddr
        -1,                   // port
        -1                    // cmd_socket
    );
    PCMANAGER_ADDRemote(fatherid,info);
    Printf_OK("Recv a new agent info father(%d),child(%d)..",
            fatherid,info->id);
    MIC_USLEEP(1);
    PCNODE_Free(info);
    return M_RECV_AND_ADD_CHILD_OK;
}

#define M_RESETTARGETNEWID_ERROR   -1
#define M_RESETTARGETNEWID_OK       1
int m_resetTargetNewId(int oldid,int newid){
    int res = M_RESETTARGETNEWID_ERROR;
    char newidbuf[4];
    pPCNodeInfo child = PCMANAGER_GETNextJump(oldid);
    pPCConn     conn  = &(child->conn);
    if(conn == NULL || child == NULL){
        goto error_exit;
    }
    pAgent_proto proto = PROTO_CreateProto();
    PROTO_SetCMD(proto,
        CMDTYPE_TRANSMIT,
        CMDID_ID_RESET,
        -1);
    PROTO_SetAddress(proto,
        PCMANAGER_Get_RootID(),
        oldid);
    API_m_itochar(newid,newidbuf,4);
    PROTO_SetArgs(proto,
        4, newidbuf);
    PROTO_SendProto(conn,proto);
    MIC_USLEEP(1);
    
    PCMANAGER_ReplaceID(oldid,newid);
ok_exit:
    res = M_RESETTARGETNEWID_OK;
    goto exit;
error_exit:
    res = M_RESETTARGETNEWID_ERROR;
    goto exit;
exit:
    PCNODE_Free(child);
    conn = NULL;
    return res;
}

int m_ForEachChildAgent_Trigger(pNodeData minfo){
    pPCNodeInfo info = (pPCNodeInfo)minfo;
    if(info ->NodeType == MYSELF_NODE_REMOTE
      || info -> NodeType == BE_MANAGED_NOW
      || info -> NodeType == IAM_ADMIN_NODE
      || info -> NodeType == UPSTREAM_NODE){
        return 1;
    }
Printf_DEBUG("m_ForEachChildAgent_Trigger becalled ");
    int newid = AGENT_ID_ASK();
    // set it id
    m_resetTargetNewId(info->id,newid);
    // send set id info
    // send agent info upper
    m_SendChildInfo(PCMANAGER_Get_RootID(),
        newid,
        info->OSType,
        info->PCName);
    return 1;
}

int ChildNodeInfoSyncTrigger(){
    // Traval every node 
    // and call m_ForEachChildAgent_Trigger
    PCMANAGER_Traversal_Neighbor(
        m_ForEachChildAgent_Trigger);
    return 1;
}

int on_SyncInfoUpper(pAgent_proto proto){
    int res = ON_SYNCINFOUPPER_ERROR;
    int newid = -1;
    if(proto == NULL){
        goto error_exit;
    }
    m_Recv_And_Add_Child(proto);
ok_exit:
    res = ON_SYNCINFOUPPER_OK;
    goto exit;
error_exit:
    res = ON_SYNCINFOUPPER_ERROR;
    goto exit;
exit:
    return res;
}


int SYNC_SendChildInfoUpper(int fatherid,int childid, int ostype,char *pcname){
    int res = m_SendChildInfo(fatherid,childid,ostype,pcname);
    if(res == M_SENDCHILDINFO_ERROR){
        return SYNC_SENDCHILDINFOUPPER_ERROR;
    }
    return SYNC_SENDCHILDINFOUPPER_OK;
}
