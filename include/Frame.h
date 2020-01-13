//
// Created by idanto on 08/01/2020.
//

#ifndef BOOST_ECHO_CLIENT_FRAME_H
#define BOOST_ECHO_CLIENT_FRAME_H

#include <map>


const std::string STOMP_DELIMITER = "\0";
const int COMMAND_PART = 0;
const int HEADERS_PART = 1;
const int BODY_PART = 2;

enum FrameCommand {
    SEND,
    MESSAGE,
    CONNECT,
    CONNECTED,
    RECEIPT,
    ERROR,
    SUBSCRIBE,
    UNSUBSCRIBE,
    DISCONNECT,
    UNINITIALIZED
};


class Frame {
    private:
        FrameCommand _command = UNINITIALIZED;
        std::map <std::string, std::string> _headers;
        std::string _body = "";

    public:
        Frame();
        Frame(FrameCommand command);
        Frame(FrameCommand command, std::string body);
        Frame(std::string& frameString);

        FrameCommand static strToEnum(const std::string& str);

        FrameCommand getCommand();
        void setCommand(FrameCommand command);
        std::string getBody();
        void setBody(std::string body);
        std::map <std::string, std::string> getHeaders();
        void addHeader(std::string header, std::string value);
        std::string getHeaderVal(std::string header);
        std::string toString();

};


#endif //BOOST_ECHO_CLIENT_FRAME_H
