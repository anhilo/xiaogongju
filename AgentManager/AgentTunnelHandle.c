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

int SendTunnelProto(int sock,int targetid){
    char cmd_buf[4];
    API_m_itochar(targetid,cmd_buf,4);
    API_socket_send(sock,cmd_buf,4);
    return 1;
}

int AGENT_TUNN_BuildTunnel(int to_id){
    int target_sock = -1;
    int result_flag = 0;
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
Printf_DEBUG("Build Tunnel OK -1111---> %d",target_sock);
// Send NEW_TUNNEL_ASK
            AGENT_ConversationProxy_SendRcHead(target_sock);
            result_flag = 1;
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
Printf_DEBUG("poolid = %d",tunnid);
            char tunnelid[4];
            API_m_itochar(tunnid,tunnelid,4);
            int res = API_socket_send(cmd_sock,tunnelid,4);
            if(res != SOCKET_SEND_ERROR){
                target_sock = tunn_wait_second_pool(tunnid, 1000);
                if(target_sock != -1){
                    result_flag = 1;
                }
            }
        }
        
    }
    if(result_flag == 1){
    // get New Tunnel sock ok
    //  send new tunnel protocal???
        SendTunnelProto(target_sock,to_id);
        return target_sock;
    }
    return AGENT_TUNN_BUILDTUNNEL_ERROR;
}

void recvtest(int sock){
    char buffer[1000];
    int len = API_socket_recv(sock,buffer,1000);
//Printf_DEBUG("%d -- > %d",len,buffer[0]);
//    len = API_socket_recv(sock,buffer,1000);
Printf_DEBUG("%d -- > %s",len,buffer);
}

int on_newTunnel_recv(int sock){
    char cmd_buf[4];
    API_socket_recv(sock,cmd_buf,4);
    int targetid = API_m_chartoi(cmd_buf,4);
    if(targetid == PCMANAGER_Get_RootID()){
        recvtest(sock);
    }
    else{
        int target_sock = AGENT_TUNN_BuildTunnel(targetid);
        tunn_sock_to_sock(sock,target_sock,1000);
    }
    return 0;
}

int on_new_tunnel_ask(int sock){
    on_newTunnel_recv(sock);
    return 1;
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
    on_newTunnel_recv(reverse_sock);
    return 1;
}

int on_reverse_Reply(int sock){
    char poolid_buf[4] ;
    API_socket_recv(sock,poolid_buf,4);
    int  poolid = API_m_chartoi(poolid_buf,4);
    tunn_set_second_pool(poolid,sock);
    return 1;
}
