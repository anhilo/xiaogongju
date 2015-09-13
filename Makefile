objects = BaseAPI.o EWmain.o SocksBase.o Sock_Tunnel.o \
          ssocksd_pro.o rssocks_pro.o CMD_Protocol.o Lcx_Base.o \
		  libsock/Socks.o
EWhere : $(objects)
	cc -o EWhere $(objects) -lpthread -s
	rm $(objects)
forwin : $(objects)
	cc -o EWhere $(objects) -lwsock32 -s
	rm $(objects)
clean:
	rm EWhere
	rm EWhere.exe
