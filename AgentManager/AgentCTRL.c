#include "AgentCTRL.h"
#include "PCNodeManager.h"
#include "PCNodeInfo.h"
#include "AgentInteractive.h"

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

#define DEFAULT_NODE_ID   255
int agentCtrl_sendPCNode(int sock,pPCNodeInfo info){
    char buffer[1000];
    // send id
    API_m_itochar  (info->id    ,buffer,4);
    API_socket_send(sock        ,buffer,4);
    // send pcname
    API_socket_send(sock,info->PCName,MAX_PCNAME_LEN);
    // send ostype
    API_m_itochar  (info->OSType,buffer,4);
    API_socket_send(sock        ,buffer,4);
    // send nodetype
    API_m_itochar  (info->conn.LinkType,buffer,4);
    API_socket_send(sock               ,buffer,4);
    return 1;
}

#define AGENTCTRL_RECVPCNODE_ERROR  NULL
pPCNodeInfo agentCtrl_recvPCNode(int sock){
    pPCNodeInfo info = PCNODE_Create();
    char buf[MAX_PCNAME_LEN+MAX_IP_ADDR_LEN];
    char pcname[MAX_PCNAME_LEN+1];
    int  id,ostype,linktype;
    if(info == NULL) return AGENTCTRL_RECVPCNODE_ERROR;
    //recv id
    API_socket_recv(sock,buf,4);
    id = API_m_chartoi(buf,4);
    MyPrintf("newnode id = %d.",id);
    // recv name
    API_socket_recv(sock,pcname,MAX_PCNAME_LEN);
    MyPrintf("newnode name = %s.",pcname);
    // recv ostype
    API_socket_recv(sock,buf,4);
    ostype = API_m_chartoi(buf,4);
    MyPrintf("newnode OSType = %d.",ostype);
    // recv nodetype
    API_socket_recv(sock,buf,4);
    linktype = API_m_chartoi(buf,4);
    MyPrintf("newnode linktype = %d.",linktype);
    
    int res = PCNODE_SETAllData(
        info,
        id,
        ostype,
        pcname,
        linktype,
        "",
        -1,
        0);
    if(res == PCNODE_SETALLDATA_ERROR){
        return AGENTCTRL_RECVPCNODE_ERROR;
    }
    return info;
}

int Distribution_newID(int oldid){
    maxid = (maxid>oldid) ? (maxid) : (oldid);
    if( PCMANAGER_HAVENode(oldid)){
        maxid = maxid +1;
    }
    return maxid;
}

#define set_Your_id 1
int resetHisID(int sock,int id){
    char buffer[20];
    buffer[0] = set_Your_id;
    API_m_itochar(id,&buffer[1],4);
    if(API_socket_send(sock,buffer,5)
        == SOCKET_SEND_ERROR){
        return 0;
    }
    return 1;
}

#define ASK_MY_ID_ERROR  1
#define ASK_MY_ID_OK     2
int askMyID(int sock){
    char buffer[20];
    int mid = 0;
    int mold_id = 0;
    if(API_socket_recv(sock,buffer,5) 
        == SOCKET_RECV_ERROR){
        return ASK_MY_ID_ERROR;
    }
    if(buffer[0] == set_Your_id){
        mid = API_m_chartoi(&buffer[1],4);
        if(PCMANAGER_HAVENode(mid)
            == PCMANAGER_HAVANODE_YES){
            int idbuf = Distribution_newID(mid);
            PCMANAGER_ReplaceID(mid,idbuf);
        }
        mold_id = m_self ->id;
        PCMANAGER_ReplaceID(mold_id,mid);
        return ASK_MY_ID_OK;
    }
    return ASK_MY_ID_ERROR;
}

int node_conn(int sock,pPCNodeInfo newnode){
    int res = Distribution_newID(newnode->id);
    newnode->id = res;
    resetHisID(sock,res);
    return 1;
}

int cb_newnode(int sock){
    int res,mid;
    pPCNodeInfo newnode = agentCtrl_recvPCNode(sock);
    if(newnode == AGENTCTRL_RECVPCNODE_ERROR){
        return 0;
    }
    switch(newnode->conn.LinkType){
    case MYSELF_NODE:
        // add this node 
        Printf_DEBUG("client is MYSELF_NODE  (id=%d)",newnode->id);
        node_conn(sock,newnode);
        res = PCMANAGER_ADDNeighbor(newnode);
        if(res == PCMANAGER_ADDNEIGHBOR_ERROR){
            return 0;
        }
        agentCtrl_sendPCNode(sock,m_self);
        break;
    case IAM_ADMIN_NODE:
        // add this node and set it upper
        mid = askMyID(sock);
        Printf_DEBUG("client is IAM_ADMIN_NODE");
        res = PCMANAGER_ADDNeighbor(newnode);
        if(res == PCMANAGER_ADDNEIGHBOR_ERROR){
            return 0;
        }
        res = PCMANAGER_SETUpperAdmin(newnode->id);
        if(res == PCMANAGER_SETUPPER_ERROR){
            return 0;
        }
        break;
    case UNKONWN_NODE:
        // do nothing Print Error
        Printf_Error("UNKNOWN NODE TYPE");
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
            sock = AGENTINT_Connect_IAM_NEW_NODE(url,port);
            agentCtrl_sendPCNode(sock,m_self);
            int hisid = Distribution_newID(maxid);
            resetHisID(sock,hisid);
            agentCtrl_sendPCNode(sock,serverinfo);
            break;
        case UNKONWN_NODE:
        case MYSELF_NODE:
            sock = AGENTINT_Connect_IAM_NEW_NODE(url,port);
            agentCtrl_sendPCNode(sock,m_self);
            int mid = askMyID(sock);
            PCMANAGER_ReplaceID(m_self->id,mid);
            m_self->id = mid;
            serverinfo=agentCtrl_recvPCNode(sock);
            break;
        default:
            Printf_Error("Don't Know LinkType");
            return AGENTCTRL_CONNECT_ERROR;
    }
    Printf_OK("Server node --> id  = %d , pcname = %s",
        serverinfo->id,serverinfo->PCName);
    return sock;
}


