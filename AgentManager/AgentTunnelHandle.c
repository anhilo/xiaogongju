/**
* File Name :AgentManager/AgentTunnelHandle.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : Mon 16 Nov 2015 03:27:18 AM PST
*/
#include "AgentTunnelHandle.h"
#include "PCNodeManager.h"
#include "PCNodeInfo.h"
#include "AgentConversationProxy.h"


int AGENT_TUNN_BuildTunnel(int to_id){
    int target_sock = -1;
    pPCNodeInfo nextnode = PCMANAGER_GETNextJump(to_id);

    Printf_DEBUG("Start Build Tunnel");

if(nextnode != NULL){
    
    Printf_DEBUG("the cmd_sock is ---> %d",
        nextnode->conn.cmd_socket);
}


    if( nextnode != NULL 
        && (nextnode -> conn .ConnType 
         == CONNTYPE_DIRECT_CONNECT)
    ){
        target_sock = API_socket_connect(
            nextnode -> conn.IPaddr,
            nextnode -> conn.port);
        if(target_sock != SOCKET_CONNECT_ERROR){
            return target_sock;
        }
    }
    else if(nextnode != NULL
        && (nextnode -> conn.ConnType
         == CONNTYPE_REVERSE_CONNECT)){
        int cmd_sock = nextnode->conn.cmd_socket;
        if(AGENT_CONVERSATIONPROXY_SENDREVERSEID_OK
            == AGENT_ConversationProxy_SendRcHead
            (cmd_sock)){
            int tunnid = tunn_set_first_pool_and_getid(cmd_sock);
            char tunnelid[4];
            API_m_itochar(tunnid,tunnelid,4);
            int res = API_socket_send(cmd_sock,tunnelid,4);
            if(res != SOCKET_SEND_ERROR){
                target_sock = tunn_wait_second_pool(tunnid, 1000);
                if(target_sock != -1){
                    return target_sock;
                }
            }
        }
        
    }
    return AGENT_TUNN_BUILDTUNNEL_ERROR;
}

void recvtest(int sock){
    char buffer[1000];
    API_socket_recv(sock,buffer,1000);
Printf_DEBUG("%s",buffer);
}


int on_reverse_Tunnel_Ask(int sock,char *ip,int port){
    int reverse_sock = API_socket_connect(ip,port);
    if(reverse_sock == SOCKET_CONNECT_ERROR){
        return 0;
    }
    if( AGENT_CONVERSATIONPROXY_TUNNEL_REPLY_ERROR
        == AGENT_ConversationProxy_Tunnel_Reply(reverse_sock))
    {
        return 0;
    }
    char poolbuf[4];
    API_socket_recv(sock,poolbuf,4);
    API_socket_send(reverse_sock,poolbuf,4);
    recvtest(reverse_sock);
    return 1;
}

int on_reverse_Reply(int sock){
Printf_DEBUG("on_reverse_Reply");
    char poolid_buf[4] ;
    API_socket_recv(sock,poolid_buf,4);
    int  poolid = API_m_chartoi(poolid_buf,4);
    tunn_set_second_pool(poolid,sock);
    Printf_DEBUG("set second pool OK");
//    recvtest(sock);
}
