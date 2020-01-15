//
// Created by idanto on 01/01/2020.
//

#ifndef BOOST_ECHO_CLIENT_CINHANDLER_H
#define BOOST_ECHO_CLIENT_CINHANDLER_H

#include <iostream>
#include "ConcurrentDataQueues.h"
#include "Frame.h"
#include "ClientInventory.h"


const std::string CMD_DELIMITER = " ";

enum UserActionsEnum {
    LOGIN,
    JOIN_GENRE,
    EXIT_GENRE,
    ADD_BOOK,
    BORROW_BOOK,
    RETURN_BOOK,
    GENRE_BOOK_STATUS,
    LOGOUT
};

class UserHandler {
    private:
        bool _shouldTerminate = false;
        ConcurrentDataQueues& _queues;
        ClientInventory& _inventory;
        std::map <UserActionsEnum, FrameCommand> _actionFrameCommandMap;
        std::map<std::string, int> _genreIdMap;
        int nextId = 42; //because it's the meaning of life

        static std::map <UserActionsEnum, FrameCommand> initActionFrameCommandMap() {
            std::map <UserActionsEnum, FrameCommand> actionFrameCommandMap;
            actionFrameCommandMap[LOGIN] = CONNECT;
            actionFrameCommandMap[JOIN_GENRE] = SUBSCRIBE;
            actionFrameCommandMap[EXIT_GENRE] = UNSUBSCRIBE;
            actionFrameCommandMap[ADD_BOOK] = SEND;
            actionFrameCommandMap[BORROW_BOOK] = SEND;
            actionFrameCommandMap[RETURN_BOOK] = SEND;
            actionFrameCommandMap[GENRE_BOOK_STATUS] = SEND;
            actionFrameCommandMap[LOGOUT] = DISCONNECT;
            return actionFrameCommandMap;
        }

        void split(std::string& line, std::vector <std::string>& cmdParams, std::string delimiter);
        UserActionsEnum findCmd(std::string line);
        void parseLine(std::string line);

        Frame receiveFrame();
        void sendFrame(Frame frame);

        void login(std::string& line);
        void joinGenre(std::string& line);
        void exitGenre(std::string& line);
        void addBook(std::string& line);
        void borrowBook(std::string& line);
        void returnBook(std::string& line);
        void bookStatus(std::string& line);
        void logout(std::string& line);

    public:
        UserHandler(ConcurrentDataQueues& queues, ClientInventory& inventory);
        ~UserHandler();
        void operator()();
};


#endif //BOOST_ECHO_CLIENT_CINHANDLER_H
