objects = BaseAPI.o EWmain.o SocksBase.o Sock_Tunnel.o \
          ssocksd_pro.o CMD_Protocol.o
EWhere : $(objects)
	cc -o EWhere $(objects)
	rm *.o
clean:
	rm EWhere
