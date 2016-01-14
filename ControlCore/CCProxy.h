#ifndef _CCPROXY_H_
#define _CCPROXY_H_

#include "../generic.h"
#include "../AgentManager/AgentProtocol.h"
#include "../AgentManager/GlobalValue.h"
#define CCPROXY_INIT_ERROR -1
#define CCPROXY_INIT_OK     1
int CCProxy_Init(int ostype,char *pcname,int node_type);

// agent connect
#define CCPROXY_LISTENAGENT_ERROR    -1
#define CCPROXY_LISTENAGENT_OK        1
int CCProxy_ListenAgent(int targetid, int rport,int maxnum);


#define CCPROXY_CONNECTAGENT_ERROR    -1
#define CCPROXY_CONNECTAGENT_OK        1
int CCProxy_AgentConnect(int targetid, char *remoteip,int rport);

#define CCPROXY_SENDMSG_OK         1
#define CCPROXY_SENDMSG_ERROR     -1
int CCProxy_SendMsg(int targetid,char *msg,int msglen);
int CCProxy_onNewTunnel(pPCConn conn);

// shell
int CCProxy_startShell(int targetid);
#endif


////************** SHELL    ********************************
//// 1.
//// 在目标节点开 SHELL 隧道
//// 返回socket 套接子
//int CMD_CTRL_startShell(int targetid);
//
//// 2. 
//// 当收到启动 SHELL 请求后，
//int CMD_CTRL_onStartShellHere(int clientsock);
//
////************** Message *********************************
//// 3. 
//// 启动一个指向目标节点的socket 会话
//int CMD_CTRL_sendMsg(int targetid,char *msg,int msglen);
//
//// 4.
//// 当收到发给当前节点的消息时
//int CMD_CTRL_onRecvMsg(char *msg,int msglen);
//
////************** SOCKS   *********************************
//// 5. 
//// 启动一个指向目标服务的socks 会话
//int CMD_CTRL_startSocks(int targetid,int lport);
//
//// 6. 
//// 当收到新的socks 会话请求时
//int CMD_CTRL_onRecvSocksAsk(int clientsock);
//
////************** UPFILES *********************************
//// 7.
//// 向目标节点发送新文件
//int CMD_CTRL_UpFile(int targetid,char *from_local_filepath,
//        char *to_remote_filepath);
//
//// 8. 
//// 当目标节点收到文件上传请求
//int CMD_CTRL_onRecvUpFile(int clientsock);
//
////************** DOWNFILES *******************************
//// 9. 
//// 从目标节点下载文件到本地
//int CMD_CTRL_DownFile(int targetid,char *from_remote_filepath,
//        char *to_local_filepath);
//
//// 10.
//// 当目标节点收到文件下载请求
//int CMD_CTRL_onRecvDownFile(int clientsock);
//
//
////************** LCX_Tran *******************************
////========正向
//// 11.
//// 正向lcx tran 请求
//int CMD_CTRL_Lcx_Tran(int targetid,int lport,char *to_ip,int to_port);
//
//// 12.
//// 当收到正向lcx 请求时
//int CMD_CTRL_Lcx_onRecvLcxTran(int clientsock);
//
////========反向
//// 13.
//// 从目标节点启动反向lcx 请求
//int CMD_CTRL_Lcx_backTran(int targetid,int rport,char *to_ip,int to_port);
//
//// 14.
//// 当收到来自目标节点的lcx请求时
//int CMD_CTRL_onRecvLcxBakTran(int clientsock);
