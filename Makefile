object_lib = baselib/BaseAPI.o baselib/GlobalFunctions.o \
			baselib/ListNodeCTRL.o baselib/TreeNodeCTRL.o

AgentLib   =  AgentManager/PCNodeInfo.o \
			AgentManager/PCNodeManager.o 
          

node_client = testmain.o
node_server = nodeserver.o

LINUX_LINK = -lpthread
WIN32_LINK = -lwsock32 -lws2_32

clientobj = $(object_lib) $(AgentLib) $(node_client)
serverobj = $(object_lib) $(AgentLib) $(node_server) 
objects   = $(object_lib) $(AgentLib) $(node_client) $(node_server)
EWhere : $(objects)
	cc -o nserver  $(LINUX_LINK) $(serverobj)
	cc -o nclient  $(LINUX_LINK) $(clientobj)
	rm $(objects)
clean:
	rm nserver
	rm nclient
