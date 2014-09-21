.PHONY: clean all init
CC:=g++
CXXFLAGS=-g

all: server client
	
server:server.o
client:client.o
server.o: std.h
client.o: std.h

clean:
	-rm server
	-rm client
	-rm *.o
	-rm tags
	-rm cscope.out
	-rm cscope.in.out
	-rm cscope.po.out

init:
	-ctags -R
	-cscope -bRq
