//
// Created by idanto on 13/01/2020.
//

#include "../include/ServerHandler.h"


ServerHandler::ServerHandler(ConcurrentDataQueues& queues, ClientInventory& inventory) :
        _connectionHandler(nullptr), _queues(queues), _inventory(inventory) {}

ServerHandler::~ServerHandler() = default;

void ServerHandler::split(std::string& str, std::vector <std::string>& subStrs, std::string delimiter) {
    size_t start = 0U;
    size_t end = str.find(delimiter);
    while (end != std::string::npos) {
        subStrs.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    subStrs.push_back(str.substr(start, str.size() - start + 1));
}

Frame ServerHandler::receiveFrameFromClient() {
    boost::unique_lock <boost::mutex> lock(_queues.mutexToServer);
    Frame frame;
    if (!_queues.framesToServer.empty()) {
        frame = _queues.framesToServer.front();
        _queues.framesToServer.pop();
    }
    return frame;
}

void ServerHandler::sendFrameToClient(Frame frame) {
    boost::unique_lock <boost::mutex> lock(_queues.mutexFromServer);
    _queues.framesFromServer.push(frame);
    _queues.condFromServer.notify_all();
}

void ServerHandler::parseUserFrame(Frame frameFromClient) {
    FrameCommand cmd = frameFromClient.getCommand();
    switch (cmd) {
        case CONNECT: {
            if (_loggedIn) {
                Frame frame = Frame(ERROR, "already logged in. you should logout before logging in again");
                sendFrameToClient(frame);
                return;
            }
            std::string host = frameFromClient.getHeaderVal("host");
            short port = boost::lexical_cast<short>(frameFromClient.getHeaderVal("port"));
            _connectionHandler = new ConnectionHandler(host, port);
            if (!_connectionHandler->connect()) {
                Frame frame = Frame(ERROR, "Could not connect to server");
                sendFrameToClient(frame);
            }
            else {
                _connectionHandler->sendFrameAscii(frameFromClient.toString(), STOMP_DELIMITER.c_str()[0]);
                _loggedIn = true;
            }
            break;
        }
        case DISCONNECT: {
            if(_loggedIn) {
                _connectionHandler->sendFrameAscii(frameFromClient.toString(), STOMP_DELIMITER.c_str()[0]);
            }
            _loggedIn = false;

            break;
        }
        default:
            _connectionHandler->sendFrameAscii(frameFromClient.toString(), STOMP_DELIMITER.c_str()[0]);
    }
}

std::string ServerHandler::extractBookName(std::vector <std::string> pieces, size_t startPos, size_t endPos) {
    std::string book;
    for (size_t i = startPos; i < endPos; i++) {
        book += pieces[i];
    }
    return book;
}

std::string ServerHandler::booksString(std::vector <std::string> books) {
    std::string body = _inventory.getUsername() + ":";
    for (std::string book : books) {
        body += book;
        body += ",";
    }
    body.substr(0, body.size() - 1); // instead of adding a check for last book, just remove last ','
    return body;
}

void ServerHandler::parseMessageFrame(Frame messageFrame) {
    std::string genre = messageFrame.getHeaderVal("destination");
    std::vector <std::string> subStrs;
    std::string body = messageFrame.getBody();
    split(body, subStrs, " ");
    if (subStrs[1] == "wish") {
        std::string book = extractBookName(subStrs, 4, subStrs.size()); // starting from the word 'borrow'
        if (_inventory.isInInventory(genre, book)) {
            Frame ansFrame = Frame(SEND);
            ansFrame.addHeader("destination", genre);
            ansFrame.setBody(_inventory.getUsername() + " has " + book);
            _connectionHandler->sendFrameAscii(ansFrame.toString(), STOMP_DELIMITER.c_str()[0]);
        }
    }
    else if (subStrs[0] == "Returning" && subStrs[subStrs.size() - 1] == _inventory.getUsername()) {
        // starting after 'Returning', ending before 'to'
        std::string book = extractBookName(subStrs, 1, subStrs.size() - 2);
        _inventory.addBook(genre, book);
    }
    else if (subStrs[0] == "Taking" && subStrs[subStrs.size() - 1] == _inventory.getUsername()) {
        // starting after 'Taking', ending before 'from'
        std::string book = extractBookName(subStrs, 1, subStrs.size() - 2);
        _inventory.removeFromInventory(genre, book);
    }
    else if (body == "book status") {
        Frame ansFrame = Frame(SEND);
        ansFrame.addHeader("destination", genre);
        std::string body = booksString(_inventory.getGenreBooks(genre));
        ansFrame.setBody(body);
        _connectionHandler->sendFrameAscii(ansFrame.toString(), STOMP_DELIMITER.c_str()[0]);
    }
    else if (subStrs[0] == _inventory.getUsername()) { return; }
    else if (subStrs[1] == "has" && subStrs[2] != "added") {
        std::string book = extractBookName(subStrs, 2, subStrs.size()); //staring after the word 'has'
        if (_inventory.isInWishList(genre, book)) {
            std::string lender = subStrs[0];
            Frame ansFrame = Frame(SEND);
            ansFrame.addHeader("destination", genre);
            ansFrame.setBody("Taking " + book + " from " + lender);
            _connectionHandler->sendFrameAscii(ansFrame.toString(), STOMP_DELIMITER.c_str()[0]);
            _inventory.borrowBook(book, lender);
            _inventory.removeFromWishList(genre, book);
        }
    }
}

void ServerHandler::parseServerFrame() {
    std::string frameString;
    while(_connectionHandler == nullptr || !_connectionHandler->getFrameAscii(frameString, STOMP_DELIMITER.c_str()[0])) {
        if(_shouldTerminate) return;
    }
    Frame frame = Frame(frameString);
    FrameCommand cmd = frame.getCommand();
    if(!_loggedIn && cmd == RECEIPT) {
        delete _connectionHandler;
        _connectionHandler = nullptr;
        sendFrameToClient(frame);
    }
    switch (cmd) {
        case MESSAGE: {
            parseMessageFrame(frame);
            break;
        }
        default: { // CONNECTED, RECEIPT, and ERROR frames, should be passed to user.
            sendFrameToClient(frame);
        }
    }
}

void ServerHandler::terminate() {
    _shouldTerminate = true;
}

void ServerHandler::run() {
    boost::thread listenToSocketTh(&ServerHandler::listenToSocket, this);
    listenToClient();
    listenToSocketTh.join();

}

void ServerHandler::listenToClient() {
    while (!_shouldTerminate) {
        boost::unique_lock <boost::mutex> lock(_queues.mutexToServer);
        while (_queues.framesToServer.empty() && !_shouldTerminate) _queues.condToServer.wait(lock);
        if(_shouldTerminate) return;
        Frame frameFromClient = _queues.framesToServer.front();
        _queues.framesToServer.pop();
        parseUserFrame(frameFromClient);
    }
}

void ServerHandler::listenToSocket() {
    while (!_shouldTerminate) {
        if (_loggedIn) {
            parseServerFrame();
        }
    }
}
