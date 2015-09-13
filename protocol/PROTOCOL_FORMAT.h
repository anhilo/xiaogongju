#ifndef _PROTOCOL_FORMAT_H_
#define _PROTOCOL_FORMAT_H_

//----------------------------------------------------
#include "../generic.h"
#include "Node_Conn_Protocol.h"
#include "Protocol.h"


#define PROTOCOL_MAX_LEN      15      // 协议数据包的长度
#define PROTOCOL_TYPE_OFFSET  0       // 顶层协议类型标示偏移
    #define NEW_NODE_CONN     1       // 新节点连接协议
/**
    +---------------+----------+
    | PROTOCOL_TYPE | xxxxxxxx |
    +---------------+----------+
*/

#endif
