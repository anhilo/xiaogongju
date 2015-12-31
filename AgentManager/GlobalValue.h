#ifndef _GLOBALVALUE_H_
#define _GLOBALVALUE_H_

#include "../generic.h"

///////////////////////////////////////////
//         PCConn
///////////////////////////////////////////
#define MAX_IP_ADDR_LEN    20

#define CONNTYPE_DIRECT_CONNECT   1
#define CONNTYPE_REVERSE_CONNECT  2
#define NET_SESSION_BUSY_NOW      1
#define NET_SESSION_UNUSED_NOW    2
typedef struct PCConnInfo{
    int ConnType;
    int BusyType;   // BusyType
    char IPaddr[MAX_IP_ADDR_LEN];
    int port;
    int cmd_socket;
}PCConn,*pPCConn;

///////////////////////////////////////////
//         JOB List
///////////////////////////////////////////
#define MAX_DESCRIBE_LEN     50
#define MAX_RESULT_LEN       50
#define JOB_STATE_UNKNOWN    0 // 未知
#define JOB_STATE_RELEASE    1 // 初始状态
#define JOB_STATE_RUNNING    2 // 正在运行
#define JOB_STATE_WAIT_RPL   3 // 等待结果
#define JOB_STATE_CAN_STOP   4 
        // 可以结束(等到了上游回复)

typedef struct JOB_DESCRIBE{
    int jobID;   // 工作编号
    int cmdID;   
        // 工作内容编号(指令编号)
        // 根据这个指令编号分配回调处理函数
    char describe[MAX_DESCRIBE_LEN];
        // 工作描述
    int jobState;
        // 工作当前状态
    char jobresult[MAX_RESULT_LEN];
        // 设置工作的结果
}JobDes,*pJobDes;

#define JOB_ECHO_ON      1 // 开启工作回显
#define JOB_ECHO_OFF     2 // 关闭工作回显

#define JOB_FRESH_ID_LOCK    1
#define JOB_FRESH_ID_UNLOCK  0
typedef struct JOB_LIST{
    int MaxJobNum;
    int jobNow;
    int echostate;
    int freshlock;
    pJobDes joblist;
}JobList,*pJobList;


///////////////////////////////////////////
//         PCNodeInfo
///////////////////////////////////////////
#define MAX_PCNAME_LEN     50

#define PC_OS_LINUX        1
#define PC_OS_WINDOW       2
#define PC_OS_ARMLINUX     3
#define PC_OS_MACOSX       4
#define PC_OS_UNKNOW       5

// NodeType 
#define IAM_ADMIN_NODE      1 // Neighbor
#define MYSELF_NODE         2 // Neighbor
#define UPSTREAM_NODE       3 // REMOTE_CHILD
#define MYSELF_NODE_REMOTE  4 // REMOTE_CHILD
#define UNKONWN_NODE        5 // UNKNOWN
typedef struct PCNodeData{
    int id;
    int OSType; // OS_Type // 
    char PCName[MAX_PCNAME_LEN];
    int NodeType ;
    PCConn conn;
}PCNodeInfo,*pPCNodeInfo;


///////////////////////////////////////////
//         Functions
///////////////////////////////////////////
#define GLOBAL_SETJOBLIST_ERROR    -1
#define GLOBAL_SETJOBLIST_OK        1
int GLOBAL_SetJobList(pJobList list);

#define GLOBAL_GETJOBLIST_ERROR  NULL
pJobList GLOBAL_GetJobList();

#endif
