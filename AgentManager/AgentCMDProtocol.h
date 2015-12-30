#ifndef _AGENTCMDPROTOCOL_H_
#define _AGENTCMDPROTOCOL_H_

#include "../generic.h"
/////  第一字段 指令类型
#define CMDTYPE_NEWCONN   1 // 节点连接协议
#define CMDTYPE_TRANSMIT  2 // 节点间交互协议

/////  第二字段 指令编号
#define CMDID_JOB_RPLY         1 // 工作回馈 
#define CMDID_NEWTUNNEL_ASK    2 // 新隧道请求(直连)
#define CMDID_NEWTUNNEL_RC_ASK 3 // 新隧道请求(反向)
#define CMDID_NEWTUNNEL_RC_RPL 4 // 新隧道响应(反向)
#define CMDID_NEWID_ASK        5 // 向上级请求新ID
#define CMDID_NEWID_RPL        6 // 新ID分配响应
#define CMDID_CHILDSYNC_UPPER  7 // 向上级更新节点信息
#define CMDID_NEWNODE_HERE     8 // 新节点连接

#define CMDPROTO_PARSE_AND_DO_ERROR     -1
#define CMDPROTO_PARSE_AND_DO_OK         1
int CMDPROTO_Parse_And_Do(pAgent_proto proto,pPCConn conn);

#endif
