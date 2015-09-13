#ifndef _GLOBAL_FUNCTIONS_H_
#define _GLOBAL_FUNCTIONS_H_

#include "../generic.h"

int GLOBAL_INIT_ENV();
//======================================================
#define GLOBAL_ADMIN_ONLINE   1
#define GLOBAL_ADMIN_OFFLINE  2
#define GLOBAL_I_AM_ADMIN_NOW 3
//------------------------------------------------------
int GLOBAL_Admin_State ();
int GLOBAL_Admin_Online();
int CLOBAL_I_Am_Admin  ();
//------------------------------------------------------

//======================================================
//------------------------------------------------------
int GLOBAL_set_usec_time(int usec);
int GLOBAL_get_usec_time();
//------------------------------------------------------
#endif
