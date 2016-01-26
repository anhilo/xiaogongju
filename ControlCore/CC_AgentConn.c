/**
* File Name :ControlCore/CC_AgentConn.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : 三 12/23 23:36:54 2015
*/
#include "CC_AgentConn.h"
#include "CCProxy.h"
#include "../AgentManager/AgentConversationCTRL.h"

int CC_Agent_Init(int ostype,char *pcname,int node_type){
    int res = CCProxy_Init(ostype,pcname,node_type);
    if(CCPROXY_INIT_ERROR == res){
        return CC_AGENT_INIT_ERROR;
    }
    API_Init_File_ENV();
    return CC_AGENT_INIT_OK;
}

int CC_Agent_Listen(int target,int lport,int maxnum){
    int res = CCProxy_ListenAgent(target,lport,maxnum);
    if( CCPROXY_LISTENAGENT_ERROR == res){
        return CC_AGENT_LISTEN_ERROR;
    }
    return CC_AGENT_LISTEN_OK;
}

int CC_Agent_Connect(int target,char *remoteip,int rport){
    int res = CCProxy_AgentConnect(target,remoteip,rport);
    if(CCPROXY_CONNECTAGENT_ERROR == res){
        return CC_AGENT_CONNECT_ERROR;
    }
    return CC_AGENT_CONNECT_OK;
}

int CC_Agent_SendMsg(int target,char *msg,int msglen){
    int res = CCProxy_SendMsg(target,msg,msglen);
    if(res == CCPROXY_SENDMSG_ERROR){
        return CC_AGENT_SENDMSG_ERROR;
    }
    return CC_AGENT_SENDMSG_OK;
}

int CC_Agent_StartShell(int target,int lport,int maxnum,int usec){
    int res = 
        CCProxy_startShell(target,lport,maxnum);
    if(res == CCPROXY_STARTSHELL_ERROR){
        return CC_AGENT_STARTSHELL_ERROR;
    }
    return CC_AGENT_STARTSHELL_OK;
}

int CC_Agent_LcxTran(int target,int lport,char *rip,int rport,int usec){
    int res = CCProxy_LcxTran(target,lport,rip,rport,usec);
    if(CCPROXY_LCXTRAN_ERROR == res){
        return CC_AGENT_LCXTRAN_ERROR;
    }
    return CC_AGENT_LCXTRAN_OK;
}

int CC_Agent_StartSocks(int target,int lport,int usec){
    int res = CCProxy_StartSocks(target,lport,usec);
    if(CCPROXY_STARTSOCKS_ERROR == res){
        return CC_AGENT_STARTSOCKS_ERROR;
    }
    return CC_AGENT_STARTSOCKS_OK;
}

int CC_Agent_ShowMap(){
    int res = AGENT_Conversation_ShowMap();
    if(AGENT_CONVERSATION_SHOWMAP_ERROR == res ){
        return CC_AGENT_SHOWMAP_ERROR;
    }
    return CC_AGENT_SHOWMAP_OK;
}

int CC_Agent_Upfile(int target,char *from_lfile,char *to_rfile){
    int res = CCPROXY_Upfile(target,from_lfile,to_rfile);
    if(CCPROXY_UPFILE_ERROR == res){
        return  CC_AGENT_UPFILE_ERROR;
    }
    return CC_AGENT_UPFILE_OK;
}
