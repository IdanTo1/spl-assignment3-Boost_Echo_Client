//
// Created by idanto on 08/01/2020.
//

#include "../include/Frame.h"


Frame::Frame(FrameCommand command) : _command(command) {}

Frame::Frame(FrameCommand command, std::string body) : _command(command), _body(body) {}

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
