/**
* File Name :AgentSock.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : 三 11/11 00:30:07 2015
*/

#include"AgentSock.h"

#define RECVOK    1
#define RECVERROR 2

#define SEND_INFO_ERROR  -1
#define SEND_INFO_OK      1
int send_info(int sock,char *buffer,int len){
    char buf[20];
    int nsend = API_socket_send(sock,buffer,len);
    if(nsend == SOCKET_SEND_ERROR){
        return SEND_INFO_ERROR;
    }
    int nrecv = API_socket_recv(sock,buf,1);
    if(nrecv != 1 
        || nrecv == SOCKET_RECV_ERROR
        || buf[0] != RECVOK ){
        return SEND_INFO_ERROR;
    }
    return SEND_INFO_OK;
}

#define RECV_INFO_ERROR    -1
#define RECV_INFO_OK        1
int recv_info(int sock,char *recvdata,int len){
    char res[20];
    char *buf = (char *)malloc(sizeof(char)*len+1);
    int nrecv = API_socket_recv(sock,buf,len);
    if(nrecv == SOCKET_SEND_ERROR 
        || nrecv > len){
        res[0] = RECVERROR;
        API_socket_send(sock,res,1);
        return RECV_INFO_ERROR;
    }
    res[0] = RECVOK;
    API_socket_send(sock,res,1);
    memcpy(recvdata,buf,len);
    free(buf);
    buf = NULL;
    return RECV_INFO_OK;
}

int AgentInfo_Send(int sock,pPCNodeInfo info){
    char buffer[1000];
    // send id
    API_m_itochar  (info->id    ,buffer,4);
    if(send_info(sock    ,buffer,4)
            ==SEND_INFO_ERROR){
        return AGENTINFO_SEND_ERROR;
    }
    // send pcname
    if(send_info(sock,info->PCName,MAX_PCNAME_LEN) 
        == SEND_INFO_ERROR){
        return AGENTINFO_SEND_ERROR;
    }
    // send ostype
    API_m_itochar  (info->OSType,buffer,4);
    if( send_info(sock  ,buffer,4)
        == SEND_INFO_ERROR){
        return AGENTINFO_SEND_ERROR;
    }
    // send nodetype
    API_m_itochar  (info->conn.LinkType,buffer,4);
    if(send_info(sock    ,buffer,4)
        == SEND_INFO_ERROR){
        return AGENTINFO_SEND_ERROR;
    }
    return  AGENTINFO_SEND_OK;
}

pPCNodeInfo AgentInfo_Recv(int sock){
    pPCNodeInfo info = PCNODE_Create();
    char buf[MAX_PCNAME_LEN+MAX_IP_ADDR_LEN];
    char pcname[MAX_PCNAME_LEN+1];
    int  id,ostype,linktype;
    if(info == NULL) 
        return AGENTINFO_RECV_ERROR;
    //recv id
    recv_info(sock,buf,4);
    id = API_m_chartoi(buf,4);
    MyPrintf("newnode id = %d.",id);
    // recv name
    recv_info(sock,pcname,MAX_PCNAME_LEN);
    MyPrintf("newnode name = %s.",pcname);
    // recv ostype
    recv_info(sock,buf,4);
    ostype = API_m_chartoi(buf,4);
    MyPrintf("newnode OSType = %d.",ostype);
    // recv nodetype
    recv_info(sock,buf,4);
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
        return AGENTINFO_RECV_ERROR;
    }
    return info;
}

int AgentInfo_SetID(int sock,int id){
    char buffer[20];
    API_m_itochar(id,buffer,4);
    if(send_info(sock,buffer,4)
        ==SEND_INFO_ERROR){
        return AGENTINFO_RECVID_ERROR;
    }
    return AGENTINFO_SETID_OK;
}

int AgentInfo_RecvID(int sock){
    char buffer[20];
    if(recv_info(sock,buffer,4) 
        == RECV_INFO_ERROR){
        return AGENTINFO_RECVID_ERROR;
    }
    return API_m_chartoi(buffer,4);
}

