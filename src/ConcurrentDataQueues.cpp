//
// Created by idanto on 11/01/2020.
//

#include "../include/ConcurrentDataQueues.h"


ConcurrentDataQueues::ConcurrentDataQueues(std::queue <Frame>* framesToServer,
                                           std::queue <Frame>* frameFromServer) :
        framesToServer(*framesToServer), framesFromServer(*frameFromServer), mutexToServer(),
        mutexFromServer(), condToServer(), condFromServer() {}