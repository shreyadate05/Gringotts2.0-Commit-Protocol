# Makefile for the socket programming example
#
server_objects  = GringottsServer.o Socket.o Coordinator.o
backend_objects = GringottsBackend.o Socket.o Cohort.o 
client_objects  = GringottsClient.o Socket.o Client.o 

all : server client backend

server: $(server_objects)
	g++ -pthread -o server $(server_objects)

client: $(client_objects)
	g++ -pthread -o client $(client_objects)

backend: $(backend_objects)
	g++ -pthread -o client $(backend_objects)
	
Socket: Socket.cpp
GringottsServer: Coordinator.cpp
GringottsBackend: Cohort.cpp
GringottsClient: Client.cpp

server_main: GringottsServer.cpp
client_main: GringottsClient.cpp
backend_main: GringottsBackend.cpp

clean:
	rm -rf *.o server client backend
