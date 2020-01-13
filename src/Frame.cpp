//
// Created by idanto on 08/01/2020.
//

#include "../include/Frame.h"


FrameCommand Frame::strToEnum(const std::string& str) {
    if (str == "SEND") {
        return SEND;
    }
    else if (str == "MESSAGE") {
        return MESSAGE;
    }
    else if (str == "CONNECT") {
        return CONNECT;
    }
    else if (str == "CONNECTED") {
        return CONNECTED;
    }
    else if (str == "RECEIPT") {
        return RECEIPT;
    }
    else if (str == "ERROR") {
        return ERROR;
    }
    else if (str == "SUBSCRIBE") {
        return SUBSCRIBE;
    }
    else if (str == "UNSUBSCRIBE") {
        return UNSUBSCRIBE;
    }
    else if (str == "DISCONNECT") {
        return DISCONNECT;
    }
    else {
        return UNINITIALIZED;
    }
}

Frame::Frame(FrameCommand command) : _command(command) {}

Frame::Frame(FrameCommand command, std::string body) : _command(command), _body(body) {}

Frame::Frame(std::string& frameString) {
    std::string line = "";
    std::string delimiter = "\n";
    int framePart = COMMAND_PART;
    size_t start = 0U;
    size_t end = frameString.find(delimiter);
    while (line != STOMP_DELIMITER && framePart != BODY_PART) {
        line = frameString.substr(start, end - start);
        start = end + delimiter.length();
        end = line.find(delimiter, start);
        switch (framePart) {
            case COMMAND_PART: {
                _command = strToEnum(line);
                framePart = HEADERS_PART;
            }
            case HEADERS_PART: {
                size_t portSeparator = line.find(":");
                if (portSeparator == std::string::npos) {
                    framePart = BODY_PART;
                    continue;
                }
                std::string header = line.substr(0, portSeparator);
                std::string headerVal = line.substr(portSeparator, line.size());
                _headers[header] = headerVal;
            }
            default:
                break; // will never happen.
        }
    }
    start = end + delimiter.length();
    _body = frameString.substr(start, frameString.size() - start); // no need for last char in body
}

FrameCommand Frame::getCommand() {
    return _command;
}

void Frame::setCommand(FrameCommand command) {
    _command = command;
}

std::string Frame::getBody() {
    return _body;
}

void Frame::setBody(std::string body) {
    _body = body;
}

std::map <std::string, std::string> Frame::getHeaders() {
    return _headers;
}

void Frame::addHeader(std::string header, std::string value) {
    _headers[header] = value;
}

std::string Frame::getHeaderVal(std::string header) {
    return _headers[header];
}
