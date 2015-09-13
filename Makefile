object_lib = BaseAPI.o SocksBase.o Sock_Tunnel.o \
          ssocksd_pro.o rssocks_pro.o CMD_Protocol.o Lcx_Base.o 

testlib   =  protocol/Node_Conn_Protocol.o \
          protocol/Protocol.o

EWMain_obj = EWmain.o
Test_obj   = testmain.o
node_server = nodeserver.o
node_client = nodeclient.o

LINUX_LINK = -lpthread
WIN32_LINK = -lwsock32 -lws2_32

objects = $(object_lib) $(EWMain_obj) $(Test_obj) $(node_server) $(node_client) $(testlib)
EWhere : $(objects)
	cc -o EWhere    $(EWMain_obj) $(object_lib) $(LINUX_LINK) 
	cc -o testmain  $(Test_obj)   $(object_lib) $(LINUX_LINK)
	cc -o nserver  $(node_server)   $(object_lib) $(LINUX_LINK) $(testlib)
	cc -o nclient  $(node_client)   $(object_lib) $(LINUX_LINK) $(testlib)
	rm *.o
forwin : $(objects)
	cc -o EWhere    $(EWMain_obj) $(object_lib) $(WIN32_LINK)
	cc -o testmain  $(Test_obj)   $(object_lib) $(WIN32_LINK)
	rm *.o
clean:
	rm EWhere
	rm testmain
	rm nserver
	rm nclient
	rm EWhere.exe
	rm testmain.exe
