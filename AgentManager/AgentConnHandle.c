/**
* File Name :AgentManager/AgentConnHandle.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : 一 11/16 00:13:14 2015
*/
#include "AgentConnHandle.h"
#include "PCNodeManager.h"
#include "PCNodeInfo.h"

//====================================
#define M_INFO_SEND_ERROR     -1
#define M_INFO_SEND_OK         1
int m_Info_send(int sock,pPCNodeInfo info);
#define M_INFO_RECV_ERROR   NULL
pPCNodeInfo m_agentInfo_Recv(int sock);
#define M_SEND_NOTHING_ERROR  -1
#define M_SEND_NOTHING_OK      1
int m_send_nothing(int sock);
#define M_SENDID_ERROR        -1
#define M_SENDID_OK            1
int m_sendID(int sock,int id);
#define M_RECVID_ERROR        -1
int m_recvID(int sock);
//====================================

int AgentConn_Interactive(int sock){
    pPCNodeInfo info = PCMANAGER_Get_RootNode();
    if(info == NULL){return AGENTCONN_INTERACTIVE_ERROR;}
    if(M_INFO_SEND_ERROR == m_Info_send(sock,info)){
        return AGENTCONN_INTERACTIVE_ERROR;
    }
    /////// Add code From here
    return AGENTCONN_INTERACTIVE_OK;
}

int on_Agent_Connect(int sock);


//******************************************************************
#define SEND_INFO_MSG         1
#define SEND_NOTHING_MSG      2
#define RECVOK    1
#define RECVERROR 2

#define SEND_INFO_ERROR  -1
#define SEND_INFO_OK      1
int send_info(int sock,char *buffer,int len){
    int result,nrecv;
    char msg  = SEND_INFO_MSG;
    int nsend = API_socket_send(sock,&msg,1);
    if(nsend == SOCKET_SEND_ERROR){
        result =  SEND_INFO_ERROR;
    }
    else{
        nsend = API_socket_send(sock,buffer,len);
        if(nsend == SOCKET_SEND_ERROR){
            result =  SEND_INFO_ERROR;
        }
        else{
            nrecv = API_socket_recv(sock,&msg,1);
            if(nrecv == SOCKET_RECV_ERROR 
                || msg != RECVOK){
                result =  SEND_INFO_ERROR;
            }
            else{
                result = SEND_INFO_OK;
            }
        }
    }
    return result;
}

int m_send_nothing(int sock){
    int result,nsend,nrecv;
    char msg = SEND_NOTHING_MSG;
    nsend= API_socket_send(sock,&msg,1);
    if(nsend == SOCKET_SEND_ERROR){
        result = SEND_INFO_ERROR;
    }
    else{
        nrecv = API_socket_recv(sock,&msg,1);
        if(nrecv == SOCKET_RECV_ERROR
            || msg != RECVOK){
            result = SEND_INFO_ERROR;
        }
        else{
            result = SEND_INFO_OK;
        }
    }
    return result;
}

#define RECV_INFO_ERROR    -1
#define RECV_INFO_OK        1
int recv_info(int sock,char *buffer,int len){
    int result , nsend,nrecv;
    char msg ,*buf;
    nrecv  =API_socket_recv(sock,&msg,1);
    if(nrecv == SOCKET_RECV_ERROR
        || msg == SEND_NOTHING_MSG){
        result = RECV_INFO_ERROR;
    }
    else{
        buf = (char *)malloc(sizeof(char)*len+10);
        nrecv = API_socket_recv(sock,buf,len);
        if(nrecv == SOCKET_RECV_ERROR){
            result = RECV_INFO_ERROR;
        }
        else{
            msg = RECVOK;
            API_socket_send(sock,&msg,1);
            result = RECV_INFO_OK;
        }
        memcpy(buffer,buf,len);
        free(buf);
    }
    buf = NULL;
    return result;
}

int m_sendID(int sock,int id){
    char buffer[20];
    API_m_itochar(id,buffer,4);
    if(send_info(sock,buffer,4)
        ==SEND_INFO_ERROR){
        return M_SENDID_ERROR;
    }
    return M_SENDID_OK;
}

int m_recvID(int sock){
    char buffer[20];
    if(recv_info(sock,buffer,4) 
        == RECV_INFO_ERROR){
        return M_RECVID_ERROR;
    }
    return API_m_chartoi(buffer,4);
}

int m_Info_send(int sock,pPCNodeInfo info){
    char buffer[1000];
    char idbuf[4],ostype[4],nodetype[4];
    int result = M_INFO_SEND_OK;

    if(info == NULL){
        result = M_INFO_SEND_ERROR;
    }
    else{
        API_m_itochar (info->id       ,idbuf   ,4);
        API_m_itochar (info->OSType   ,ostype  ,4);
        API_m_itochar (info->NodeType  ,nodetype,4);
        strncpy(buffer, info->PCName , MAX_PCNAME_LEN);
        // send id
        if( SEND_INFO_ERROR == send_info(sock ,idbuf,4)
            // send pcname
            || SEND_INFO_ERROR == send_info(sock,buffer,MAX_PCNAME_LEN)
            // send ostype
            || SEND_INFO_ERROR == send_info(sock,ostype,  4)
            // send nodetype
            || SEND_INFO_ERROR == send_info(sock,nodetype,4)){
            result =  M_INFO_SEND_ERROR;
        }
    }
    // send error
    if(result == M_INFO_SEND_ERROR){
        Printf_Error("Send info Error");
    }
    return  result;
}

#define BUF_ERROR   -1
#define BUF_OK       1
pPCNodeInfo m_agentInfo_Recv(int sock){
    pPCNodeInfo info = PCNODE_Create();
    char pcname[MAX_PCNAME_LEN+1];
    char id[4],ostype[4],linktype[4];
    int  mid,mostype,mlinktype;
    int result = 1;
    if(info == NULL){
        result =  BUF_ERROR;
    }
    else{
        // recv id
        if( RECV_INFO_ERROR == recv_info(sock,id,4) 
            // recv name
            || RECV_INFO_ERROR == recv_info(sock,pcname,MAX_PCNAME_LEN)
            // recv ostype
            || RECV_INFO_ERROR == recv_info(sock,ostype,4)
            // recv nodetype
            || RECV_INFO_ERROR == recv_info(sock,linktype,4)){
            result = BUF_ERROR;
        }
        else{
            mid       = API_m_chartoi(id,4);
            mostype   = API_m_chartoi(ostype,4);
            mlinktype = API_m_chartoi(linktype,4);
            int res = PCNODE_SETAllData(
                info,
                mid,
                mostype,
                pcname,
                mlinktype,
                -1,
                "",
                -1,
                0);
            if(res == PCNODE_SETALLDATA_ERROR){
                result =  BUF_ERROR;
            }
        }
    }

    if(result == BUF_ERROR){
        return M_INFO_RECV_ERROR;
    }
Printf_OK("id = %d,ostype = %d , nodetype = %d, pcname = %s",
        info-> id,info->OSType,info->NodeType,info->PCName);
    return info;
}

//******************************************************************
