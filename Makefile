object_lib = baselib/BaseAPI.c baselib/GlobalFunctions.c \
			baselib/ListNodeCTRL.c baselib/TreeNodeCTRL.c \
			sockslib/Sock_Tunnel.c

AgentLib   =  AgentManager/PCNodeInfo.c \
			AgentManager/PCNodeManager.c \
			AgentManager/AgentConversationProxy.c \
			AgentManager/AgentConversationCTRL.c \
			AgentManager/AgentConnHandle.c \
			AgentManager/AgentTunnelHandle.c \
			AgentManager/AgentMsgHandle.c \
			AgentManager/AgentCMDInfo.c \
			AgentManager/AgentChildSync.c \
			ControlCore/CC_AgentConn.c \
			ControlCore/CCProxy.c 
#			ControlCore/Cmd_Ctrl.c
          

node_admin  = nodeadmin.c
node_client = nodeclient.c
node_server = nodeserver.c
node_test   = nodetest.c

LINUX_LINK = -g -lpthread 
WIN32_LINK = -lwsock32 -lws2_32

out1 = nserver
out2 = nclient
testout = nodetest
out3 = nadmin
winout = nserver.exe nclient.exe nodetest.exe
nuxout = $(out1) $(out2) $(testout) $(out3)

clientobj = $(object_lib) $(AgentLib) $(node_client)
serverobj = $(object_lib) $(AgentLib) $(node_server) 
testobj   = $(object_lib) $(AgentLib) $(node_test) 
adminobj  = $(object_lib) $(AgentLib) $(node_admin) 
objects   = $(object_lib) $(AgentLib) $(node_client) $(node_server) $(node_test) $(node_admin)
EWhere : $(objects)
	cc -o $(out1) $(serverobj) $(LINUX_LINK)
	cc -o $(out2) $(clientobj) $(LINUX_LINK)
	cc -o $(out3) $(adminobj) $(LINUX_LINK)
	cc -o $(testout) $(testobj) $(LINUX_LINK)
#	rm $(objects)
forwin: $(objects)
	cc -o $(out1) $(serverobj) $(WIN32_LINK)
	cc -o $(out2) $(clientobj) $(WIN32_LINK)
	rm $(objects)
clean:
#	rm $(objects) $(out1) $(out2) $(winout)
	rm $(winout) $(nuxout)
