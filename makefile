# makefile

all: dataserver client

reqchannel.o: reqchannel.h reqchannel.cpp
	g++-4.7 -c -g reqchannel.cpp
	
semaphore.o: semaphore.h semaphore.cpp
	g++-4.7 -c -g semaphore.cpp
	
BoundedBuffer.o: BoundedBuffer.h BoundedBuffer.cpp
	g++-4.7 -c -g BoundedBuffer.cpp

dataserver: dataserver.cpp reqchannel.o 
	g++-4.7 -g -o dataserver dataserver.cpp reqchannel.o -lpthread

client: client.cpp reqchannel.o semaphore.o BoundedBuffer.o
	g++-4.7 -g -pthread -o client client.cpp reqchannel.o semaphore.o BoundedBuffer.o
