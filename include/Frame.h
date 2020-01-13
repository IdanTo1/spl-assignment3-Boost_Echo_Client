//
// Created by idanto on 08/01/2020.
//

#ifndef BOOST_ECHO_CLIENT_FRAME_H
#define BOOST_ECHO_CLIENT_FRAME_H

#include <map>


enum FrameCommand {
    SEND,
    MESSAGE,
    CONNECT,
    CONNECTED,
    RECEIPT,
    ERROR,
    SUBSCRIBE,
    UNSUBSCRIBE,
    DISCONNECT
};


class Frame {
    private:
        FrameCommand _command;
        std::map <std::string, std::string> _headers;
        std::string _body = "";

    public:
        Frame(FrameCommand command);
        Frame(FrameCommand command, std::string body);

        FrameCommand getCommand();
        void setCommand(FrameCommand command);
        std::string getBody();
        void setBody(std::string body);
        std::map <std::string, std::string> getHeaders();
        void addHeader(std::string header, std::string value);
        std::string getHeaderVal(std::string header);
        Frame clone();
        std::string toString();

};


#endif //BOOST_ECHO_CLIENT_FRAME_H
