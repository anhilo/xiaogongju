/**
* File Name :AgentManager/AgentIDCtrl.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : Tue 29 Dec 2015 10:05:33 PM PST
*/
#include "AgentIDCtrl.h"
#include "AgentCMDProtocol.h"

// 从管理员获得ID
int Ask_Id_Upper(pJobList list){
    pPCNodeInfo info = PCMANAGER_Get_FatherNode();
    pAgent_proto proto = NULL;
    int result;
    int jobid;
    if(info == PCMANAGER_GET_FATHERNODE_ERROR)
    {
        result = -1;
    }
    else{
        // 新建任务
        jobid = JOB_GetFreshJobID(list);
        // 设置任务状态
        JOB_SetJOBState(list,jobid,JOB_STATE_WAITE_RPL);

        // 新建传输协议
        PROTO_CreateProto(proto,CMDTYPE_TRANSMIT,CMDID_NEWID_ASK,jobid);
        // 从当前节点发送给管理员
        PROTO_SetAddress(proto,PCMANAGER_Get_RootID(),0);
        // 无参数内容
        PROTO_SetArgs(proto,0,"");
        
        if(PROTO_SENDPROTO_ERROR == 
            PROTO_SendProto(info->conn,proto)){
            result = -1;
        }
        else {
            if(JOB_WAITCLOSEJOB_ERROR == 
              JOB_WaitCloseJob(list,jobid,10)){
                result = -1;
            }
            else{
                char *buf  = NULL;
                int buflen =    0;
                JOB_GetResult(list,jobid,&buf,&buflen);
                result = API_m_chartoi(buf,4);
            }
        }
    }
    PCNODE_Free(info);
    PROTO_FreeProto(proto);
    return result;
}

int AGENT_ID_ASK(){
    int newid = 0;
    int state = PCMANAGER_Manager_State();
    switch(state){
    case PCMANAGER_MANAGER_STATE_TRUE:
        newid = Ask_Id_Upper();
        if(-1 == newid){
            return AGENT_ID_ASK_ERROR;
        }
        break;
    case PCMANAGER_MANAGER_IAM_ADMIN:
    case PCMANAGER_MANAGER_STATE_FALSE:
        newid = PCMANAGER_Get_Fresh_ID();
        if(PCMANAGER_GET_FRESH_ID_ERROR
            == newid){
            return AGENT_ID_ASK_ERROR;
        }
        break;
    }
    return newid;
}


int sendIDBack(pPCConn conn,
    int targetid,
    int jobid,
    int newid){
    pAgent_proto proto = NULL;
    char buf[4];
    API_m_itochar(newid,buf,4);
    PROTO_CreateProto(proto,CMDTYPE_TRANSMIT,
        CMDID_NEWID_RPL,jobid);
    PROTO_SetAddress(proto,PCMANAGER_Get_RootID(),targetid);
    PROTO_SetArgs(proto,4,buf);
    PROTO_SendProto(conn,proto);
    return 1;
}

int on_AGENT_ID_Ask(pAgent_proto proto,pPCConn conn){
    int newid = -1;
    if(proto->toID == PCMANAGER_Get_RootID()){
        newid = AGENT_ID_ASK();
        if(newid == -1){
            return -1;
        }
    }
    if(1 == sendIDBack(conn,
        proto->fromID,
        proto->jobID,
        newid)){
        return 1;
    }
    return -1;
}
