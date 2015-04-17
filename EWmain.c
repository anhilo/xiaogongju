#include "generic.h"

#define VERSION_STR "free1.0"
#define NULL_STATE 0
#define SSOCKSD 1
#define RCSOCKS 2
#define RSSOCKS 3
#define LCX_LISTION 4
#define LCX_TRAN 5
#define LCX_SLAVE 6
#define NO_NEEDHELP 0
#define NEEDHELP 1 << 1
#define ABOUT_NOW 1 << 2
#define VERSION 1 << 3
#define FUNNY_FLAG 1 << 4
#define HAS_STATE(m,n) m&n
int cmdstate = NULL_STATE;
int state = NO_NEEDHELP;
char refHost[300] = "127.0.0.1";
int refPort = 1080;
char connHost[300] = "127.0.0.1";
int connPort = 8888;
int listenPort = 8888;
int usec = 10000;

const char *optstring = "s:l:aqht:vd:e:f:g:";
struct option opts[]={
    {"refhost", required_argument,NULL, 'd'},
    {"refport", required_argument,NULL, 'e'},
    {"connhost", required_argument,NULL, 'f'},
    {"connport", required_argument,NULL, 'g'},
    {"state", required_argument,NULL, 's'},
    {"listenport",required_argument,NULL, 'l'},
    {"usectime", required_argument,NULL, 't'},
    {"about", no_argument, NULL, 'a'},
    {"help" , no_argument, NULL, 'h'},
    {"version", no_argument, NULL, 'v'},
    {"qu", no_argument, NULL, 'q'},
    {0,0,0,0}};
int help();
int realDo();
int mainDo();
int about_fun();
int getStatenum();
int quFun();
int main(int argc ,char *argv[]){
    int c,index;
    API_env_init();
    tunn_init_Pool();
    if(argc == 1 ){ help(); }
    while((c=getopt_long(argc,argv,optstring,opts,&index))!=-1)
    {
        switch(c) {
        case 'd'://
            strncpy(refHost,optarg,300);
            break;
        case 'e'://
            refPort = atoi(optarg);
            break;
        case 'f'://
            strncpy(connHost,optarg,300);
            break;
        case 'g'://
            connPort = atoi(optarg);
            break;
        case 's'://
            cmdstate = getStatenum(optarg);
            break;
        case 'l'://
            listenPort = atoi(optarg);
            break;
        case 't'://
            usec = atoi(optarg);
            break;
        case 'h':
            state = state | NEEDHELP;
            break;
        case 'a':
            state = state | ABOUT_NOW;
            break;
        case 'v':
            state = state | VERSION;
            break;
        case 'q':
            state = state | FUNNY_FLAG;
            break;
        case '?'://选项未定义
            state = state | NEEDHELP;
            break;
        default:
            printf("c is %d\n",c);
            break;
        }
    }
    mainDo();
    return 0;
}
int getStatenum(char *opt){
    if(!strcmp("ssocksd",opt)){
        return SSOCKSD;
    }
    else if(!strcmp("rcsocks",opt)){
        return RCSOCKS;
    }
    else if(!strcmp("rssocks",opt)){
        return RSSOCKS;
    }
    else if(!strcmp("lcx_listen",opt)){
        return LCX_LISTION;
    }
    else if(!strcmp("lcx_tran",opt)){
        return LCX_TRAN;
    }
    else if(!strcmp("lcx_slave",opt)){
        return LCX_SLAVE;
    }
    return NULL_STATE;
}

int mainDo(){
    if (HAS_STATE(state,ABOUT_NOW)){
        about_fun();
    }
    if (HAS_STATE(state,VERSION)){
        printf("\nVERSION : %s \n\n",VERSION_STR);
    }
    if (HAS_STATE(state,FUNNY_FLAG)){
        quFun();
    }
    // 从这里开始程序正常状态 判断
    if (HAS_STATE(state,NEEDHELP)){
    // 这里需要帮助
    //printf("NEEDHELP now !!! \n");
        help();
    }
    else{
    // 这里开始程序的正常功能
        realDo();
    // printf("do nothing !\n");
    }
    // 程序执行到这里完全成功。
    return 1;
}

int help(){
    if(cmdstate == NULL_STATE){
        printf("\nVERSION : %s \n",VERSION_STR);
        printf(" ./xxx ([-options] [values])*\n");
        printf(" options :\n");
        printf(" Eg: ./xxx -s ssocksd -h \n");
        printf(" -s state setup the function.You can pick one from the \n");
        printf(" following options:\n");
        printf(" ssocksd , rcsocks , rssocks , \n");
        printf(" lcx_listen , lcx_tran , lcx_slave\n");
        printf(" -l listenport open a port for the service startup.\n");
        printf(" -d refhost set the reflection host address.\n");
        printf(" -e refport set the reflection port.\n");
        printf(" -f connhost set the connect host address .\n");
        printf(" -g connport set the connect port.\n");
        printf(" -h help show the help text, By adding the -s parameter,\n");
        printf(" you can also see the more detailed help.\n");
        printf(" -a about show the about pages\n");
        printf(" -v version show the version. \n");
        printf(" -t usectime set the milliseconds for timeout. The default \n");
        printf(" value is 1000 \n");
        printf(" ......\n");
        printf("\n");
        return 1;
    }
    printf("\n");
    switch(cmdstate){
    case LCX_LISTION:
        printf(" You can create a lcx_listen tunnel like this : \n");
        printf(" ./ew -s lcx_listen --listenPort 1080 --refPort 8888\n");
        printf(" or ./ew -s lcx_listen -l 1080 -e 8888 \n");
        break;
    case LCX_SLAVE:
        printf(" You can create a lcx_slave tunnel like this : \n");
        printf(" ./ew -s lcx_slave --refhost [ref_ip] --refport 1080 -connhost [connIP] --connport 8888\n");
        printf(" or ./ew -s lcx_slave -d [ref_ip] -e 1080 -f [connIP] -g 8888 \n");
        break;
    case LCX_TRAN:
        printf(" You can create a lcx_tran tunnel like this : \n");
        printf(" ./ew -s lcx_tran --listenport 1080 -connhost xxx.xxx.xxx.xxx --connport 8888\n");
        printf(" or ./ew -s lcx_tran -l 1080 -f [connIP] -g 8888 \n");
        break;
    case SSOCKSD:
        printf(" You can create a SOCKS5 server like this : \n");
        printf(" ./ew -s ssocksd --listenport 1080\n");
        printf(" or ./ew -s ssocksd -l 1080\n");
        break;
    case RCSOCKS:
        printf(" You can create a rcsocks tunnel like this : \n");
        printf(" ./ew -s rcsocks --listenPort 1080 --refPort 8888\n");
        printf(" or ./ew -s rcsocks -l 1080 -e 8888\n");
        break;
    case RSSOCKS:
        printf(" You can create a rssocks Server like this : \n");
        printf(" ./ew -s rssocks --refHost xxx.xxx.xxx.xxx --refPort 8888\n");
        printf(" or ./ew -s rssocks -d xxx.xxx.xxx.xxx -e 8888\n");
        break;
    }
    printf("\n");
    return 1;
}

int about_fun(){
    printf("\n");
    printf(" Earthworm is a network agent tool.\n");
    printf(" You can create a Socks5 proxy server in the Intranet or Extranet.\n");
    printf(" You can also create a N level jump Socks server with it.\n");
    printf(" The Readme file is a help document. Please read carefully.\n");
    printf(" You can get help from -h or --help parameters.\n");
    printf(" By adding the -s parameter, you can also see the more detailed help.\n");
    printf(" Eg: ./xxx -h -s ssocksd\n");
    printf(" \n\n");
    printf(" Contributors\n");
    printf(" rootkiter : The creator\n");
    printf("   asky    : <<Linux Programming by \n");
    printf("             Example>>is a great book\n");
    printf(" darksn0w  : Proviede some advice\n");
    printf(" zhuanjia  : Modify the Readme file\n");
    printf(" syc4mor3  : Named for this tool\n");
    printf(" ......\n");
    printf("                http://rootkiter.com/EarthWrom/\n");
    return 1;
}

int quFun(){
    printf("                          _ooOoo_  \n");
    printf("                         o8888888o  \n");
    printf("                         88\" . \"88  \n");
    printf("                         (| -_- |)  \n");
    printf("                          O\\ = /O  \n");
    printf("                      ____/`---'\\____  \n");
    printf("                    .   ' \\\\| |// `.  \n");
    printf("                     / \\\\||| : |||// \\  \n");
    printf("                   / _||||| -:- |||||- \\  \n");
    printf("                     | | \\\\\\ - /// | |  \n");
    printf("                   | \\_| ''\\---/'' | |  \n");
    printf("                    \\ .-\\__ `-` ___/-. /  \n");
    printf("                 ___`. .' /--.--\\ `. . __  \n");
    printf("              .\"\" '< `.___\\_<|>_/___.' >'\"\".  \n");
    printf("             | | : `- \\`.;`\\ _ /`;.`/ - ` : | |  \n");
    printf("               \\ \\ `-. \\_ __\\ /__ _/ .-` / /  \n");
    printf("       ======`-.____`-.___\\_____/___.-`____.-'======  \n");
    printf("                          `=---='   \n");
    printf("..........................................................\n");
    return 1;
}

int realDo(){
    switch(cmdstate){
    case LCX_LISTION:
        lcx_listen(listenPort,refPort,usec);
        break;
    case LCX_SLAVE:
        lcx_slave(refHost,refPort,connHost,connPort,usec);
        break;
    case LCX_TRAN:
        lcx_tran(listenPort,connHost,connPort,usec);
        break;
    case SSOCKSD:
        create_ssocksd_server( listenPort ,usec);
        break;
    case RCSOCKS:
        lcx_listen(listenPort,refPort,usec);
        //create_rcsocks_server(listenPort,refPort,usec);
        break;
    case RSSOCKS:
        create_rssocks_server(refHost,refPort,usec);
        break;
    }
    return 1;
}
