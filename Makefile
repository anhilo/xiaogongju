object_lib = baselib/BaseAPI.c \
			baselib/GlobalFunctions.c \
			baselib/ListNodeCTRL.c \
			baselib/TreeNodeCTRL.c \
			baselib/PCType.c \
			baselib/File_Base_API.c \
			sockslib/SocksBase.c \
			sockslib/Sock_Tunnel.c 

AgentLib   =  \
			AgentManager/AgentChildSync.c \
			AgentManager/AgentCMDParse.c \
			AgentManager/AgentConnHandle.c \
			AgentManager/AgentConversationCTRL.c \
			AgentManager/AgentConversationProxy.c \
			AgentManager/AgentIDCtrl.c \
			AgentManager/AgentJobMsg.c \
			AgentManager/AgentProNet.c \
			AgentManager/AgentProtocol.c \
			AgentManager/AgentTunnelHandle.c \
			AgentManager/GlobalValue.c \
			AgentManager/PCNodeInfo.c \
			AgentManager/PCNodeManager.c \
			AgentManager/PCConn.c \
			ControlCore/CC_AgentConn.c \
			ControlCore/CCProxy.c \
			ControlCore/CC_ShellCtrl.c \
			ControlCore/CC_Agent_Socks_module.c \
			ControlCore/CC_Msg_module.c \
			ControlCore/CC_LcxCtrl.c  \
			ControlCore/CC_File_Ctrl.c  \
			ControlCore/Cmd_Ctrl.c 


          

node_admin  = nodeadmin.c global_lib.c
node_client = nodeclient.c global_lib.c
node_server = nodeserver.c
node_test   = nodetest.c

LINUX_LINK = -s -lpthread 
WIN32_LINK = -lwsock32 -lws2_32

out1 = nserver
out2 = agent_exe
testout = nodetest
out3 = admin_exe
winout = nserver.exe admin_exe.exe agent_exe.exe
nuxout = $(out1) $(out2) $(testout) $(out3)

clientobj = $(object_lib) $(AgentLib) $(node_client)
serverobj = $(object_lib) $(AgentLib) $(node_server) 
testobj   = $(object_lib) $(AgentLib) $(node_test) 
adminobj  = $(object_lib) $(AgentLib) $(node_admin) 
objects   = $(object_lib) $(AgentLib) $(node_client) $(node_server) $(node_test) $(node_admin)
EWhere : $(objects)
	cc -o $(out2) $(clientobj) $(LINUX_LINK)
	cc -o $(out3) $(adminobj) $(LINUX_LINK)
forwin: $(objects)
	cc -o $(out2) $(clientobj) $(WIN32_LINK)
	cc -o $(out3) $(adminobj) $(WIN32_LINK)
clean:
#	rm $(objects) $(out1) $(out2) $(winout)
	rm $(winout) $(nuxout)
