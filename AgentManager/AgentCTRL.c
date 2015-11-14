#include "AgentCTRL.h"
#include "AgentSock.h"

//***************************************
pPCNodeInfo m_self;
int maxid = 1;
//***************************************

int AgentCTRL_Init(char *pcname,int ostype,int nodetype){
    m_self= (pPCNodeInfo)PCNODE_Create();
    if(m_self == NULL){
        return AGENTCTRL_INIT_ERROR;
    }
    int res = PCNODE_SETAllData(
        m_self,
        0,
        ostype,
        pcname,
        nodetype,
        "",
        -1,
        0);
    if(res == PCNODE_SETALLDATA_ERROR){
        return AGENTCTRL_INIT_ERROR;
    }
    res = PCMANAGER_INIT(m_self);
    if(res == PCMANAGER_INIT_ERROR){
        return AGENTCTRL_INIT_ERROR;
    }
    return AGENTCTRL_INIT_OK;
}

int AGENTCTRL_Init_Admin(char *pcname,int ostype){
    return AgentCTRL_Init(pcname,ostype,IAM_ADMIN_NODE);
}

int AGENTCTRL_Init_Node(char *pcname,int ostype){
    return AgentCTRL_Init(pcname,ostype,MYSELF_NODE);
}

//*************************************
#define CMD_PROTO_MAX_LEN        20
#define CMD_LISTENER_RECV_ERROR   1
#define CMD_LISTENER_RECV_OK      2
#define CMD_LISTENER_NOTHING_READ 3
int CMD_Listener(int sock,char *cmd_data){
    char buffer[CMD_PROTO_MAX_LEN];
    if(API_socket_read_state(sock) == SOCKET_CAN_READ_STATE){
        if(SOCKET_RECV_ERROR == 
            API_socket_recv(sock,cmd_data,CMD_PROTO_MAX_LEN)){
            return CMD_LISTENER_RECV_ERROR;
        }
        else{
            return CMD_LISTENER_RECV_OK;
        }
    }
    return CMD_LISTENER_NOTHING_READ;
}

#define CMD_HEAD_SEND_MESSAGE   1
#define CMD_HEAD_NEW_TUNNEL     2
#define CMD_HEAD_BROADCAST      4
typedef struct CMDINFO{
    char cmd;      // 1 byte
    int fromid;    // 4 byte
    int toid;      // 4 byte
    char datalen;  // 1 byte
    char data[CMD_PROTO_MAX_LEN - 10];
}CmdInfo,pCmdInfo;

pCmdInfo parseCmdInfo(char *cmdbuf){
    pCmdInfo cmdinfo= (pCmdInfo)malloc(sizeof(CmdInfo));
    cmdinfo.cmd = cmdbuf[0];
    cmdinfo.fromid = API_m_chartoi(&(cmdbuf[1]),4);
    cmdinfo.toid   = API_m_chartoi(&(cmdbuf[5]),4);
    cmdinfo.datalen= cmdbuf[9];
    memcpy(cmdinfo.data,&(cmdbuf[10]),CMD_PROTO_MAX_LEN-10);
    return cmdinfo;
}

int BuildCmdInfo(pCmdInfo cmd,char *cmdBuf){
    char buffer[CMD_PROTO_MAX_LEN - 10];
    if(cmdBuf == NULL) return 0;
    cmdBuf[0] = cmd.cmd;
    API_m_itochar(cmd.fromid,buffer,4);
    memcpy(&(cmdBuf[1]),buffer,4);
    API_m_itochar(cmd.toid,buffer,4);
    memcpy(&(cmdBuf[5]),buffer,4);
    cmdBuf[9] = cmd.datalen;
    memcpy(&(cmdBuf[10]),cmd.data,CMD_PROTO_MAX_LEN-10);
    return 1;
}

int SendMessage(int nextjump,pCmdInfo cmd){
    pPCNodeInfo node = PCMANAGER_GETNodeInfo(nextjump);
    int next_sock = node->conn.cmd_socket;
    char cmdData[CMD_PROTO_MAX_LEN];
    BuildCmdInfo(cmd,cmdData);
    if( SOCKET_SEND_ERROR == 
        API_socket_send(next_sock,cmdData,CMD_PROTO_MAX_LEN)){
        return 0;
    }
    return 1;
}

int BuildTunnel(int nextjump){
    pPCNodeInfo node = PCMANAGER_GETNextJump(nextjump);
    if(node->conn.LinkType == 
}

#define Parse_and_Do_it_OK      1
#define Parse_Error             2
#define Parse_OK_But_Do_Error   3
int parse_and_do_it(char *cmd_buf,pPCNodeInfo info){
    pCmdInfo cmd = parseCmdInfo(cmd_buf);
    switch(cmd.cmd){
        case CMD_HEAD_SEND_MESSAGE:
            int nextjump = PCMANAGER_GETNextJump(cmd.toinfo);
            if(nextjump == PCMANAGER_GETNEXTJUMP_ERROR){
                return Parse_OK_But_Do_Error;
            }
            SendMessage(nextjump,cmd);
            break;
        case CMD_HEAD_NEW_TUNNEL:
            int nextjump = PCMANAGER_GETNextJump(cmd.toinfo);
            if(nextjump == PCMANAGER_GETNEXTJUMP_ERROR){
                return Parse_OK_But_Do_Error;
            }
//            int nextsock = BuildTunnel(nextjump,cmd_buf);
Printf_DEBUG("Remember Add Code Here");
            break;
        case CMD_HEAD_BROADCAST:
            break;
        default:
            Printf_Error("What???");
            return Parse_Error;
    }
}

int callBack_for_EachNode(pNodeData data){
    char cmd_buf[CMD_PROTO_MAX_LEN];
    pPCNodeInfo node = (pPCNodeInfo)data;
    if(node->id == m_self ->id){
        return 1;
    }
    else {
        int read_state = 
        CMD_Listener(node->conn.cmd_socket,cmd_buf);
        if(read_state == CMD_LISTENER_NOTHING_READ){
            return 1;
        }
        else if(read_state == CMD_LISTENER_RECV_ERROR){
            return 1;
        }
        parse_and_do_it(cmd_data,node);
    }
}

MIC_THREAD_FUN_DEF(travelList_and_setListener,novalue){
    PCMANAGER_Traversal_Neighbor(
}

int Start_CMD_Lister(){
    MIC_THREAD_HANDLE_ID handle;
}
//*************************************
int Distribution_newID(int oldid){
    maxid = (maxid>oldid) ? (maxid) : (oldid);
    if( PCMANAGER_HAVENode(oldid)){
        maxid = maxid +1;
    }
    return maxid;
}

#define SET_MY_ID_ERROR  1
#define SET_MY_ID_OK     2
int SetMyID(int sock){
    int mid = 0;
    int mold_id = 0;

    mid = AgentInfo_RecvID(sock);
    Printf_DEBUG("my id now is %d.",mid);
    if(AGENTINFO_RECVID_ERROR == mid){
        return SET_MY_ID_ERROR;
    }
    if(PCMANAGER_HAVENode(mid)
        == PCMANAGER_HAVANODE_YES){
        // if same id ,change that id
        int idbuf = Distribution_newID(mid);
        if(PCMANAGER_REPLACEID_ERROR == 
            PCMANAGER_ReplaceID(mid,idbuf)){
            return SET_MY_ID_ERROR;
        }
    }
    mold_id = m_self ->id;
    if(PCMANAGER_REPLACEID_ERROR == 
        PCMANAGER_ReplaceID(mold_id,mid)){
        return SET_MY_ID_ERROR;
    }
    m_self->id = mid;
    return SET_MY_ID_OK;
}

int node_conn(int sock,pPCNodeInfo newnode){
    int res = Distribution_newID(newnode->id);
    newnode->id = res;
    AgentInfo_SetID(sock,res);
    return 1;
}

int cb_newnode(int sock){
    int res,mid;
    pPCNodeInfo newnode;
    newnode = AgentInfo_Recv(sock);
    if(newnode == AGENTINFO_RECV_ERROR){
        return 0;
    }
    switch(newnode->conn.LinkType){
    case MYSELF_NODE:
        // add this node 
        Printf_DEBUG("client is MYSELF_NODE  (id=%d)",newnode->id);
        node_conn(sock,newnode);
        res = PCMANAGER_ADDNeighbor(newnode);
        if(res == PCMANAGER_ADDNEIGHBOR_ERROR){
            Printf_Error("ADD Neighbor Error");
AgentInfo_SendNTH(sock);
            return 0;
        }
        break;
    case IAM_ADMIN_NODE:
        // add this node and set it upper
        mid = SetMyID(sock);
        Printf_DEBUG("client is IAM_ADMIN_NODE");
        res = PCMANAGER_ADDNeighbor(newnode);
        if(res == PCMANAGER_ADDNEIGHBOR_ERROR){
            Printf_Error("Add Admin Error");
AgentInfo_SendNTH(sock);
            return 0;
        }
        res = PCMANAGER_SETUpperAdmin(newnode->id);
        if(res == PCMANAGER_SETUPPER_ERROR){
            Printf_Error("Set Upper Error");
AgentInfo_SendNTH(sock);
            return 0;
        }
        break;
    case UNKONWN_NODE:
        // do nothing Print Error
        Printf_Error("UNKNOWN NODE TYPE");
        break;
    }
Printf_OK("the number agent = %d",PCMANAGER_GETNeighborNum());
    if(AgentInfo_Send(sock,m_self)==0){
        return 0;
    }
    return 1;
}

int cb_tunnelask(int sock){
    MyPrintf("new tunnel ask");
    return 1;
}

int cb_tunnelAnswer(int sock){
    MyPrintf("new tunnel answer");
    return 1;
}

int cb_broadcast(int sock){
    MyPrintf("broad cast here");
    return 1;
}

int AGENTCTRL_StartServer(int port,int maxclient){
    int res = AGENTINT_Init(
                cb_newnode,
                cb_tunnelask,
                cb_tunnelAnswer,
                cb_broadcast);
    if(res == AGENTINT_INIT_ERROR){
            return AGENTCTRL_STARTSERVER_ERROR;
    }
    res = AGENTINT_StartServer(port,maxclient);
    if(res == AGENTINT_STARTSERVER_ERROR){
        return AGENTCTRL_STARTSERVER_ERROR;
    }
    return AGENTCTRL_STARTSERVER_OK;
}

int AGENTCTRL_Connect(char *url,int port){
    int sock;
    pPCNodeInfo serverinfo;
    if(m_self == NULL){ return AGENTCTRL_CONNECT_ERROR; }
    switch(m_self->conn.LinkType){
        case NEIGHBOR_NODE:
        case IAM_ADMIN_NODE:
            Printf_DEBUG("ADMIN ");
            sock = AGENTINT_Connect_IAM_NEW_NODE(url,port);
            if(AGENTINFO_SEND_ERROR 
                == AgentInfo_Send(sock,m_self)){
                return AGENTCTRL_CONNECT_ERROR;
            }
            int hisid = Distribution_newID(maxid);
            AgentInfo_SetID(sock,hisid);
            serverinfo = AgentInfo_Recv(sock);
            break;
        case UNKONWN_NODE:
        case MYSELF_NODE:
            sock = AGENTINT_Connect_IAM_NEW_NODE(url,port);
            if(AGENTINFO_SEND_ERROR == 
                AgentInfo_Send(sock,m_self)){
                return AGENTCTRL_CONNECT_ERROR;
            }
            int mid = SetMyID(sock);
       //     PCMANAGER_ReplaceID(m_self->id,mid);
       //     m_self->id = mid;
            serverinfo =AgentInfo_Recv(sock);
            break;
        default:
            Printf_Error("Don't Know LinkType");
            return AGENTCTRL_CONNECT_ERROR;
    }
if(serverinfo != NULL)
    Printf_OK("Server node --> id  = %d , pcname = %s",
        serverinfo->id,serverinfo->PCName);
    return sock;
}


