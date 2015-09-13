#include "generic.h"
#include "protocol/Protocol.h"

int main(){
    API_env_init();
    GLOBAL_Admin_Online();
    PRO_start_server(9999,20);
    return 0;
}
