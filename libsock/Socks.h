#include "../generic.h"

#define SOCKS4_V 0x04
#define SOCKS5_V 0x05


#define CHECK_VERSION_OK           1
#define CHECK_VERSION_ERROR        0
int Check_Version(int sock);

#define GET_TARGET_SOCKET_ERROR   -1
int Get_Target_Socket(int sock);

#define BUILD_TARGET_SOCKET_ERROR -1
int Build_Target_Socket(int sock);
