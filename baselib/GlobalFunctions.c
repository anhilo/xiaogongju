#include "GlobalFunctions.h"
int admin_state    ;
int local_usec_time;

int init_admin_state();

int GLOBAL_INIT_ENV(){
    init_admin_state();
    GLOBAL_set_usec_time(100000);
    return 1;
}

int init_admin_state(){
    admin_state = GLOBAL_ADMIN_OFFLINE;
    return 1;
}

int GLOBAL_Admin_State (){
    return admin_state;
}

int GLOBAL_Admin_Online(){
    admin_state = GLOBAL_ADMIN_ONLINE;
    return 1;
}

int CLOBAL_I_Am_Admin  (){
    admin_state = GLOBAL_I_AM_ADMIN_NOW;
    return 1;
}


int GLOBAL_set_usec_time(int usec){
    local_usec_time = usec;
    return 1;
}
    
int GLOBAL_get_usec_time(){
    return local_usec_time;
}
