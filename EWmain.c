#include "generic.h"

int main(int argc ,char *argv[]){
    API_env_init();
//    create_ssocksd_server(1080,100000);
    printf("%s,%s",argv[1],argv[1]);
    if(!strcmp("1",argv[1])){
        create_rssocks_server("192.168.150.159",8888,10000);
    }
    else if(!strcmp("2",argv[1])){
        lcx_listen(1080,8888,10000);
    }
    return 0;
}
