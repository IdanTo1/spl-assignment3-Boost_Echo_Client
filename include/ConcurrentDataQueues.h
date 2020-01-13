//
// Created by idanto on 11/01/2020.
//

#ifndef BOOST_ECHO_CLIENT_CONCURRENTDATAQUEUES_H
#define BOOST_ECHO_CLIENT_CONCURRENTDATAQUEUES_H

#include <queue>
#include <boost/thread.hpp>
#include "Frame.h"


class ConcurrentDataQueues {
    public:
        std::queue <Frame> framesToServer, framesFromServer;
        boost::mutex mutexToServer, mutexFromServer;
        boost::condition_variable condFromServer, condToServer;

};


#endif //BOOST_ECHO_CLIENT_CONCURRENTDATAQUEUES_H
