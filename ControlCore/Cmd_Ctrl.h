#ifndef _CMD_CTRL_H_
#define _CMD_CTRL_H_

#include "../generic.h"

#define NODETYPE_ADMIN_NODE  1
#define NODETYPE_NORMAL_NODE 2

#define OSTYPE_LINUX_OS       1
#define OSTYPE_ARMLINUX_OS    2
#define OSTYPE_WINDOW_OS      3
#define OSTYPE_MACOSX_OS      4
#define OSTYPE_UNKNOW_OS      5

//************** AgentCtrl  ******************************
#define CMD_CTRL_LISTENAGENT_ERROR -1
#define CMD_CTRL_LISTENAGENT_OK     1
int CMD_CTRL_ListenAgent(int targetid,
    int rport);

#define CMD_CTRL_CONNECTAGENT_ERROR  -1
#define CMD_CTRL_CONNECTAGENT_OK      1
int CMD_CTRL_ConnectAgent(int targetid,
    char *remote_ip,int remote_port);

//************** SHELL    ********************************
// 1.
// 在目标节点开 SHELL 隧道
// 返回socket 套接子
int CMD_CTRL_startShell(int targetid);

//************** Message *********************************
// 2. 
// 启动一个指向目标节点的socket 会话
int CMD_CTRL_sendMsg(int targetid,char *msg,int msglen);

//************** SOCKS   *********************************
// 3. 
// 启动一个指向目标服务的socks 会话
int CMD_CTRL_startSocks(int targetid,int lport);


//************** UPFILES *********************************
// 4.
// 向目标节点发送新文件
int CMD_CTRL_UpFile(int targetid,char *from_local_filepath,
        char *to_remote_filepath);


//************** DOWNFILES *******************************
// 5. 
// 从目标节点下载文件到本地
int CMD_CTRL_DownFile(int targetid,char *from_remote_filepath,
        char *to_local_filepath);

//************** LCX_Tran *******************************
//========正向
// 6.
// 正向lcx tran 请求
int CMD_CTRL_Lcx_Tran(int targetid,int lport,char *to_ip,int to_port);

//========反向
// 7.
// 从目标节点启动反向lcx 请求
int CMD_CTRL_Lcx_backTran(int targetid,int rport,char *to_ip,int to_port);


#endif
