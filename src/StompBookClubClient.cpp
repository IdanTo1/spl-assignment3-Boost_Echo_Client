//
// Created by idanto on 01/01/2020.
//

#include "../include/StompBookClubClient.h"


int main(int argc, char* argv[]) {
    std::queue <Frame>* frameToServer = new std::queue<Frame>;
    std::queue <Frame>* frameFromServer = new std::queue<Frame>;
    ConcurrentDataQueues queues(frameToServer, frameFromServer);

    std::map <std::string, std::vector<std::string>>* inventory =
            new std::map <std::string, std::vector<std::string>>;
    std::map <std::string, std::string>* borrowMap = new std::map<std::string, std::string>;
    std::map <std::string, std::vector<std::string>>* wishList =
            new std::map <std::string, std::vector<std::string>>;
    ClientInventory clientInventory(*inventory, *borrowMap, *wishList);

    UserHandler userHandler(queues, clientInventory);
    ServerHandler serverHandler(queues, clientInventory);

    boost::thread userHandlerTh(&UserHandler::run, &userHandler);
    boost::thread serverHandlerTh(&ServerHandler::run, &serverHandler);

    userHandlerTh.join();
    serverHandler.terminate();
    queues.condToServer.notify_all();
    serverHandlerTh.join();
    delete frameToServer;
    delete frameFromServer;
    delete inventory;
    delete borrowMap;
    delete wishList;
    return 0;
}
