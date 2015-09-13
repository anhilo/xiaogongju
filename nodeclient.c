#include "generic.h"
#include "protocol/Protocol.h"

int main(){
    API_env_init();
//    GLOBAL_Admin_Online();
    NodeConn_Node_Connect("127.0.0.1",9999);
    return 0;
}
