#ifndef _AGENTCTRL_H_
#define _AGENTCTRL_H_

#include"../generic.h"

// A. 初始化部分
//      1. 初始化自身状态（以及设置新节点连接的回调函数）
#define AGENTCTRL_INIT_ERROR         -1
#define AGENTCTRL_INIT_OK             1
int AGENTCTRL_Init_Admin(char *pcname,int ostype);
int AGENTCTRL_Init_Node (char *pcname,int ostype);
// B. 节点连接部分
//      2. 监听某一端口，等待新节点/隧道连接
#define AGENTCTRL_STARTSERVER_ERROR  -1
#define AGENTCTRL_STARTSERVER_OK      1
int AGENTCTRL_StartServer(int port,int maxclient);
//      3. 主动连接某一节点（根据自身数据进行数据同步）
#define AGENTCTRL_CONNECT_ERROR      -1
int AGENTCTRL_Connect(char *url,int port);
// C. 节点通讯部分
//      4. 建立一条到达某节点的数据隧道
#define AGENTCTRL_BUILDTUNNEL_ERROR  -1
int AGENTCTRL_BuildTunnel(int targetid);
//      5. 节点存活查询
#define AGENTCTRL_NODELIVE_NOW       1
#define AGENTCTRL_NODELOST_NOW      -1
int AGENTCTRL_NodeLive(int id);
// D. 节点信息查询
//      6. 获得某节点状态信息（直接调用PCNodeManager相关方法即可）
#define AGENTCTRL_GETNODEINFO_ERROR -1
#define AGENTCTRL_GETNODEINFO_OK     1
// desid是被查询id，  name和ostype是返回值
int AGENTCTRL_GetNodeInfo(int desid,char *name,int *ostype);
//      7. 获得节点地图信息
#define AGENTCTRL_MAPPRINT_ERROR    -1
#define AGENTCTRL_MAPPRINT_OK        1
int AGENTCTRL_MapPrint();

#endif
