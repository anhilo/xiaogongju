/**
* File Name :AgentManager/AgentCMDInfo.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : Mon 16 Nov 2015 02:13:49 AM PST
*/

#include "AgentCMDInfo.h"

pCmdInfo AGENTCMD_parse(char *cmdbuf){
    pCmdInfo cmdinfo= (pCmdInfo)malloc(sizeof(CmdInfo));
    if(cmdinfo == NULL){
        return AGENTCMD_PARSE_ERROR;
    }
    cmdinfo->cmd = cmdbuf[0];
    cmdinfo->fromid = API_m_chartoi(&(cmdbuf[1]),4);
    cmdinfo->toid   = API_m_chartoi(&(cmdbuf[5]),4);
    cmdinfo->datalen= cmdbuf[9];
    memcpy(cmdinfo->data,&(cmdbuf[10]),CMD_PROTO_MAX_LEN-10);
    return cmdinfo;
}

int AGENTCMD_CmdInfo(pCmdInfo cmd,char *cmdBuf){
    char buffer[CMD_PROTO_MAX_LEN - 10];
    if(cmdBuf == NULL) 
        return AGENTCMD_BUILDCMDINFO_ERROR;
    cmdBuf[0] = cmd->cmd;
    API_m_itochar(cmd->fromid,buffer,4);
    memcpy(&(cmdBuf[1]),buffer,4);
    API_m_itochar(cmd->toid,buffer,4);
    memcpy(&(cmdBuf[5]),buffer,4);
    cmdBuf[9] = cmd->datalen;
    memcpy(&(cmdBuf[10]),cmd->data,CMD_PROTO_MAX_LEN-10);
    return AGENTCMD_BUILDCMDINFO_OK;
}

int AGENTCMD_sendMSG(int sock,pCmdInfo cmd){
    char cmdData[CMD_PROTO_MAX_LEN];
    AGENTCMD_CmdInfo(cmd,cmdData);
    if( SOCKET_SEND_ERROR == 
        API_socket_send(sock,cmdData,CMD_PROTO_MAX_LEN)){
        return AGENTCMD_SENDMSG_ERROR;
    }
    return AGENTCMD_SENDMSG_OK;
}
