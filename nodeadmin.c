#include "generic.h"
#include "ControlCore/CC_AgentConn.h"
#include "AgentManager/PCNodeInfo.h"

///////////////////////////////////////////////////////////
#define MAX_READLINE_LEN  300
int agentnow = 0;
char remoteip[MAX_IP_ADDR_LEN] = "127.0.0.1";
int  remoteport = 8888;
///////////////////////////////////////////////////////////

int whileMain();
int startWith(char *str1,char *);
int readline(char *desbuf,char stopchar,int maxlen);

int main(){
    CC_Agent_Init(1,"This node is Admin",IAM_ADMIN_NODE);
    CC_Agent_Connect(0,remoteip,remoteport);
    whileMain();
    return 0;
}


//char localip[]  = "127.0.0.1";
//int  localip    = 9999;
char msgbesend[300];
int whileMain(){
    char line[MAX_READLINE_LEN];
    char inputformat[MAX_READLINE_LEN];
    printf(">>>> ");
    readline(line,'\n',MAX_READLINE_LEN);
    while(!startWith(line,"quit")){
        if(startWith(line,"sendmsg")){
            Printf_OK("send msg here");
            sscanf(line,"sendmsg %s",msgbesend);
            CC_Agent_SendMsg(agentnow,msgbesend,strlen(msgbesend));
        }
        else if(startWith(line,"goto")){
            sscanf(line,"goto %d",&agentnow);
            Printf_OK("Current be contral ID is %d",agentnow);
        }
        else if(startWith(line,"shell")){
            int shellport;
            sscanf(line,"shell %d",&shellport);
            Printf_OK("band %d's shell on %d locol port.",agentnow,shellport);
            
            CC_Agent_StartShell(agentnow,shellport,200,10000);
        }
        else if(startWith(line,"lcxtran")){
            int lcxport;
            char rip[MAX_IP_ADDR_LEN];
            int rport;
            sprintf(inputformat,"lcxtran %s %s%ds %s","%d","%",MAX_IP_ADDR_LEN,"%d");
//            Printf_OK(inputformat,"HelloWorld");
            sscanf(line,inputformat,&lcxport,rip,&rport);
            Printf_OK("locolport   :%d\n",lcxport);
            Printf_OK("remote ip   :%s\n",rip);
            Printf_OK("remote port :%d\n",rport);
            CC_Agent_LcxTran(agentnow,lcxport,rip,rport,10000);
        }
        else if(startWith(line,"connect")){
            Printf_OK("target agent connect new agent");
//            Printf_OK(line);
            sprintf(inputformat,"connect %s%ds %s","%",MAX_IP_ADDR_LEN,"%d");
//            Printf_OK(inputformat,"HelloWorld");
            sscanf(line,inputformat,remoteip,&remoteport);
            Printf_OK("remote ip   :%s\n",remoteip);
            Printf_OK("remote port :%d\n",remoteport);
            CC_Agent_Connect(agentnow,remoteip,remoteport);
        }
        else if(startWith(line,"listen")){
            Printf_OK("target agent listen new port here");
            Printf_OK("%s",line);
            sscanf(line,"listen %d",&remoteport);
            CC_Agent_Listen(agentnow,remoteport,30);
        }
        else if(startWith(line,"socks")){
            Printf_OK("Start socks server from target agent");
            int socksport;
            sscanf(line,"socks %d",&socksport);
            Printf_OK("Start Socks from %d port,Server Agent is %d",
                socksport,agentnow);
            CC_Agent_StartSocks(agentnow,socksport,10000);
        }
        else{
            Printf_OK("unknow cmd");
            Printf_OK("%s",line);
        }
        printf(">>>> ");
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
