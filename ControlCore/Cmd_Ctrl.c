/**
* File Name :ControlCore/Cmd_Ctrl.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : Mon 14 Dec 2015 02:24:44 AM PST
*/

#include"Cmd_Ctrl.h"
#include"CCProxy.h"

int CMD_CTRL_ListenAgent(int targetid,
    int rport){
    int res = CCProxy_ListenAgent(targetid,rport);
    if(res == CCPROXY_LISTENAGENT_ERROR){
        return CMD_CTRL_LISTENAGENT_ERROR;
    }
    return CMD_CTRL_LISTENAGENT_OK;
}

int CMD_CTRL_ConnectAgent(int targetid,
    char *remote_ip,int remote_port){
    int res = CCProxy_Agentconnect(
                targetid,
                remote_ip,
                remote_port);
    if( CCPROXY_CONNECTAGENT_ERROR == res){
        return CMD_CTRL_CONNECTAGENT_ERROR;
    }
    return CMD_CTRL_CONNECTAGENT_OK;
}
