/**
* File Name :AgentSock.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : 三 11/11 00:30:07 2015
*/

#include"AgentSock.h"

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

int send_nothing(int sock){
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
//******************************************************************

int AgentInfo_SendNTH(int sock){
    return send_nothing(sock);
}

int AgentInfo_Send(int sock,pPCNodeInfo info){
    char buffer[1000];
    char idbuf[4],ostype[4],nodetype[4];
    int result = SEND_INFO_OK;

    if(info == NULL){
        result = AGENTINFO_SEND_ERROR;
    }
    else{
        API_m_itochar (info->id           ,idbuf   ,4);
        API_m_itochar (info->OSType       ,ostype  ,4);
        API_m_itochar (info->conn.LinkType,nodetype,4);
        strncpy(buffer, info->PCName , MAX_PCNAME_LEN);
        // send id
        if( SEND_INFO_ERROR == send_info(sock ,idbuf,4)
            // send pcname
            || SEND_INFO_ERROR == send_info(sock,buffer,MAX_PCNAME_LEN)
            // send ostype
            || SEND_INFO_ERROR == send_info(sock,ostype,  4)
            // send nodetype
            || SEND_INFO_ERROR == send_info(sock,nodetype,4)){
            result =  AGENTINFO_SEND_ERROR;
        }
    }
    // send error
    if(result == AGENTINFO_SEND_ERROR){
        Printf_Error("Send info Error");
    }
    return  result;
}

#define AGENTBUF_ERROR  -1
#define AGENTBUF_OK      1
pPCNodeInfo AgentInfo_Recv(int sock){
    pPCNodeInfo info = PCNODE_Create();
    char pcname[MAX_PCNAME_LEN+1];
    char id[4],ostype[4],linktype[4];
    int  mid,mostype,mlinktype;
    int result = 1;
    if(info == NULL){
        result =  AGENTBUF_ERROR;
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
            result = AGENTBUF_ERROR;
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
                "",
                -1,
                0);
            if(res == PCNODE_SETALLDATA_ERROR){
                result =  AGENTBUF_ERROR;
            }
            else{
                result = AGENTBUF_OK;
            }
        }
    }

    if(result == AGENTBUF_ERROR){
        return AGENTINFO_RECV_ERROR;
    }
Printf_OK("id = %d,ostype = %d , linktype = %d, pcname = %s",
        info-> id,info->OSType,info->conn.LinkType,info->PCName);
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

