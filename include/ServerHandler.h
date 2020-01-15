//
// Created by idanto on 13/01/2020.
//

#ifndef BOOST_ECHO_CLIENT_SERVERHANDLER_H
#define BOOST_ECHO_CLIENT_SERVERHANDLER_H

#include <algorithm>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include "Frame.h"
#include "ConcurrentDataQueues.h"
#include "ClientInventory.h"
#include "ConnectionHandler.h"


class ServerHandler {
    private:
        bool _shouldTerminate = false;
        bool _loggedIn = false;
        ConnectionHandler* _connectionHandler;
        ConcurrentDataQueues& _queues;
        ClientInventory& _inventory;

        void split(std::string& str, std::vector <std::string>& subStrs, std::string delimiter);
        Frame receiveFrameFromClient();
        void sendFrameToClient(Frame frame);
        void parseUserFrame(Frame frameFromClient);
        void parseServerFrame();
        void parseMessageFrame(Frame messageFrame);
        std::string extractBookName(std::vector <std::string> pieces, size_t startPos, size_t endPos);
        std::string booksString(std::vector <std::string> books);
        Frame stringToFrame(Frame frameString);
    public:
        ServerHandler(ConcurrentDataQueues& queues, ClientInventory& inventory);
        ServerHandler(const ServerHandler& other) = delete;
        ServerHandler operator=(const ServerHandler& other) = delete;
        void terminate();
        ~ServerHandler();
        void run();
};


#endif //BOOST_ECHO_CLIENT_SERVERHANDLER_H
