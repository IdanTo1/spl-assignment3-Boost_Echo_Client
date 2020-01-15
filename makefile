CC = g++
CFLAGS = -c -g -Wall -Weffc++ -std=c++11
LFLAGS = -lboost_system -pthread -L/usr/libc

# All Targets
all: StompBookClubClient

# Linker
StompBookClubClient: bin/StompBookClubClient.o
	$(CC) -o bin/StompBookClubClient bin/StompBookClubClient.o bin/ConnectionHandler.o bin/UserHandler.o \
		bin/ServerHandler.o bin/ConcurrentDataQueues.o bin/Frame.o bin/ClientInventory.o $(LFLAGS)

# Source Files
bin/StompBookClubClient.o: src/StompBookClubClient.cpp include/StompBookClubClient.h \
		bin/ConnectionHandler.o bin/UserHandler.o bin/ServerHandler.o
	$(CC) $(CFLAGS) -o bin/StompBookClubClient.o src/StompBookClubClient.cpp

bin/ConnectionHandler.o: src/ConnectionHandler.cpp include/ConnectionHandler.h
	$(CC) $(CFLAGS) -o bin/ConnectionHandler.o src/ConnectionHandler.cpp

bin/ServerHandler.o: src/ServerHandler.cpp include/ServerHandler.h bin/ConcurrentDataQueues.o bin/Frame.o \
	bin/ClientInventory.o bin/ConnectionHandler.o
	$(CC) $(CFLAGS) -o bin/ServerHandler.o src/ServerHandler.cpp

bin/UserHandler.o: src/UserHandler.cpp include/UserHandler.h bin/ConcurrentDataQueues.o bin/Frame.o bin/ClientInventory.o
	$(CC) $(CFLAGS) -o bin/UserHandler.o src/UserHandler.cpp

bin/ConcurrentDataQueues.o: src/ConcurrentDataQueues.cpp include/ConcurrentDataQueues.h bin/Frame.o
	$(CC) $(CFLAGS) -o bin/ConcurrentDataQueues.o src/ConcurrentDataQueues.cpp

bin/Frame.o: src/Frame.cpp include/Frame.h
	g++ $(CFLAGS) -o bin/Frame.o src/Frame.cpp

bin/ClientInventory.o: src/ClientInventory.cpp include/ClientInventory.h
	g++ $(CFLAGS) -o bin/ClientInventory.o src/ClientInventory.cpp

.PHONY: clean, all
# clean the build directory.
clean:
	rm -f bin/*
