#include "generic.h"
#include "AgentManager/PCNodeInfo.h"
#include "AgentManager/AgentConversationCTRL.h"
#include "AgentManager/PCNodeManager.h"

//int sendMsgTo(int targetid){
//    char buffer[200];
//    sprintf(buffer,"Hello Client ->%d<-, This Msg send by Admin.",targetid);
//    buffer[strlen(buffer)+1] = '\0';
//Printf_DEBUG("start send %d",(int)strlen(buffer));
//    
//    int sock = AGENT_Conversation_Build_SockTunnel(targetid);
//    API_socket_send(sock,buffer,strlen(buffer)+1);
//Printf_DEBUG("send ok");
//    return 1;
//}
int whileMain();
int readline(char *desbuf,char stopchar,int maxlen);

int main(){
//    API_env_init();
//    AGENT_Conversation_Init(10,"This Admin Node",IAM_ADMIN_NODE);
//    AGENT_Conversation_Connect("127.0.0.1",8888);
//
//    Printf_OK("******my id now is %d",PCMANAGER_Get_RootID());
//
//MIC_SLEEP(8);
//    sendMsgTo(3);
//MIC_SLEEP(8);
//    sendMsgTo(2);
//
//    while(1){
//        MIC_SLEEP(10000);
//    }
    whileMain();
    return 0;
}


#define MAX_READLINE_LEN  300
char remoteip[MAX_IP_ADDR_LEN] = "127.0.0.1";
int  remoteport = 8888;
//char localip[]  = "127.0.0.1";
//int  localip    = 9999;
int whileMain(){
    char line[MAX_READLINE_LEN];
    char inputformat[MAX_READLINE_LEN];
    printf(">>>> ");
    readline(line,'\n',MAX_READLINE_LEN);
    while(!startWith(line,"quit")){
        if(startWith(line,"sendmsg")){
            Printf_OK("send msg here");
            Printf_OK(line);

        }
        else if(startWith(line,"connect")){
            Printf_OK("target agent connect new agent");
//            Printf_OK(line);
            sprintf(inputformat,"connect %s%ds %s","%",MAX_IP_ADDR_LEN,"%d");
//            Printf_OK(inputformat,"HelloWorld");
            sscanf(line,inputformat,remoteip,&remoteport);
            Printf_OK("remote ip   :%s\n",remoteip);
            Printf_OK("remote port :%d\n",remoteport);
        }
        else if(startWith(line,"listen")){
            Printf_OK("target agent listen new port here");
            Printf_OK(line);
        }
        else if(startWith(line,"socks")){
            Printf_OK("Start socks server from target agent");
            Printf_OK(line);
        }
        else{
            Printf_OK("unknow cmd");
            Printf_OK(line);
        }
        printf(">>>>");
        readline(line,'\n',MAX_READLINE_LEN);
    }
    return 1;
}

int readline(char *desbuf,char stopchar,int maxlen){
    int i=0;
    int result = 1;
    char c;
    scanf("%c",&c);
    while(c!=stopchar){
        if(i>maxlen){
            result = -1;
            break;
        }
        desbuf[i] = c;
        scanf("%c",&c);
        i++;
    }
    fflush(stdin);
    fflush(stdout);
    desbuf[i] = '\0';
    if(result == 1){
        return i;
    }
    return -1;
}

int startWith(char *string,char *startString){
// startWith("ABCDEFGHIJKLMN","ABC")
//   return 1;
    int i =0;
    if(strlen(startString)>strlen(string)){ return 0; }
    for(i=0;i<strlen(startString);i++){
        if(string[i] != startString[i]){
            return 0;
        }
    }
    return 1;
}
