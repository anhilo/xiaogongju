/**
* File Name :ControlCore/CC_AgentConn.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : 三 12/23 23:36:54 2015
*/
#include "CC_AgentConn.h"
#include "CCProxy.h"

int CC_Agent_Init(int ostype,char *pcname,int node_type){
    int res = CCProxy_Init(ostype,pcname,node_type);
    if(CCPROXY_INIT_ERROR == res){
        return CC_AGENT_INIT_ERROR;
    }
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
