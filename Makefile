object_lib = baselib/BaseAPI.o baselib/GlobalFunctions.o \
			baselib/ListNodeCTRL.o baselib/TreeNodeCTRL.o

AgentLib   =  AgentManager/PCNodeInfo.o \
			AgentManager/PCNodeManager.o \
			AgentManager/AgentConversationProxy.o \
			AgentManager/AgentConversationCTRL.o \
			AgentManager/AgentConnHandle.o
			#AgentManager/AgentCTRL.o \
			AgentManager/AgentSock.o \
			AgentManager/AgentInteractive.o
          

node_client = testmain.o
node_server = nodeserver.o

LINUX_LINK = -lpthread
WIN32_LINK = -lwsock32 -lws2_32

out1 = nserver
out2 = nclient

clientobj = $(object_lib) $(AgentLib) $(node_client)
serverobj = $(object_lib) $(AgentLib) $(node_server) 
objects   = $(object_lib) $(AgentLib) $(node_client) $(node_server)
EWhere : $(objects)
	cc -o $(out1) $(serverobj) $(LINUX_LINK)
	cc -o $(out2) $(clientobj) $(LINUX_LINK)
	rm $(objects)
forwin: $(objects)
	cc -o $(out1) $(serverobj) $(WIN32_LINK)
	cc -o $(out2) $(clientobj) $(WIN32_LINK)
	rm $(objects)
clean:
	rm $(objects) $(out1) $(out2)
