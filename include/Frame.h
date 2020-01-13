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

        const FrameCommand getCommand() const;
        void setCommand(FrameCommand command);
        const std::string getBody() const;
        void setBody(std::string body);
        const std::map <std::string, std::string> getHeaders() const;
        void addHeader(std::string header, std::string value);
        const std::string getHeaderVal(const std::string& header) const;
        std::string toString();

};


#endif //BOOST_ECHO_CLIENT_FRAME_H
