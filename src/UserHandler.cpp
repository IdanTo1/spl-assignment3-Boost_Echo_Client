//
// Created by idanto on 01/01/2020.
//

#include "../include/UserHandler.h"


UserHandler::UserHandler(ConcurrentDataQueues& queues, ClientInventory& inventory) :
        _queues(queues), _inventory(inventory), _actionFrameCommandMap(initActionFrameCommandMap()) {}

UserActionsEnum UserHandler::findCmd(std::string line) {
    std::string cmd = line.substr(0, line.find(" "));
    if (cmd == "login") {
        return LOGIN;
    }
    else if (cmd == "join") {
        return JOIN_GENRE;
    }
    else if (cmd == "exit") {
        return EXIT_GENRE;
    }
    else if (cmd == "add") {
        return ADD_BOOK;
    }
    else if (cmd == "borrow") {
        return BORROW_BOOK;
    }
    else if (cmd == "return") {
        return RETURN_BOOK;
    }
    else if (cmd == "status") {
        return GENRE_BOOK_STATUS;
    }
    else { // cmd == logout. valid input guaranteed.
        return LOGOUT;
    }
}

void UserHandler::split(std::string& line, std::vector <std::string>& cmdParams, std::string delimiter) {
    size_t start = 0U;
    size_t end = line.find(delimiter);
    while (end != std::string::npos) {
        cmdParams.push_back(line.substr(start, end - start));
        start = end + delimiter.length();
        end = line.find(delimiter, start);
    }
    cmdParams.push_back(line.substr(start, line.size() - start + 1));
}

Frame UserHandler::receiveFrame() {
    boost::unique_lock <boost::mutex> lock(_queues.mutexFromServer);
    while (_queues.framesFromServer.empty()) _queues.condFromServer.wait(lock);
    Frame ans = _queues.framesFromServer.front();
    _queues.framesFromServer.pop();
    return ans;
}

void UserHandler::sendFrame(Frame frame) {
    boost::unique_lock <boost::mutex> lock(_queues.mutexToServer);
    _queues.framesToServer.push(frame);
    _queues.condToServer.notify_all();
}

void UserHandler::login(std::string& line) {
    std::vector <std::string> cmdParams;
    split(line, cmdParams, CMD_DELIMITER);
    size_t portSeparator = cmdParams[1].find(":");
    std::string host = cmdParams[1].substr(0, portSeparator);
    std::string port = cmdParams[1].substr(portSeparator, cmdParams[1].size());
    std::string username = cmdParams[2];
    _inventory.setUsername(username);
    std::string password = cmdParams[3];

    Frame frame = Frame(_actionFrameCommandMap[LOGIN]);
    frame.addHeader("accept-version", "1.2");
    frame.addHeader("host", host);
    frame.addHeader("port", port);
    frame.addHeader("login", username);
    frame.addHeader("passcode", password);
    sendFrame(frame);

    Frame ans = receiveFrame();
    FrameCommand andCmd = ans.getCommand();
    if (andCmd == CONNECTED) {
        std::cout << "Login successful" << std::endl;
    }
    else if (andCmd == ERROR) {
        std::cout << ans.getBody() << std::endl;
    }
}

void UserHandler::joinGenre(std::string& line) {
    std::vector <std::string> cmdParams;
    split(line, cmdParams, CMD_DELIMITER);
    std::string genre = cmdParams[1];
    _genreIdMap[genre] = nextId;

    Frame frame(_actionFrameCommandMap[JOIN_GENRE]);
    frame.addHeader("destination", genre);
    frame.addHeader("id", std::to_string(nextId));
    frame.addHeader("receipt", std::to_string(nextId));
    sendFrame(frame);

    Frame ans = receiveFrame();
    FrameCommand ansCmd = ans.getCommand();
    if (ansCmd == RECEIPT) {
        std::cout << "Joined club " + genre << std::endl;
    }
    else if (ansCmd == ERROR) {
        std::cout << ans.getBody() << std::endl;
    }
    nextId++;
}

void UserHandler::exitGenre(std::string& line) {
    std::vector <std::string> cmdParams;
    split(line, cmdParams, CMD_DELIMITER);
    std::string genre = cmdParams[1];
    int genreId = _genreIdMap[genre];

    Frame frame(_actionFrameCommandMap[EXIT_GENRE]);
    frame.addHeader("id", std::to_string(genreId));
    frame.addHeader("receipt", std::to_string(genreId));
    sendFrame(frame);

    Frame ans = receiveFrame();
    FrameCommand ansCmd = ans.getCommand();
    if (ansCmd == RECEIPT) {
        std::cout << "Exited club " + genre << std::endl;
    }
    else if (ansCmd == ERROR) {
        std::cout << ans.getBody() << std::endl;
    }
}

void UserHandler::addBook(std::string& line) {
    std::vector <std::string> cmdParams;
    split(line, cmdParams, CMD_DELIMITER);
    std::string genre = cmdParams[1];
    std::string book = cmdParams[2];

    Frame frame = Frame(_actionFrameCommandMap[ADD_BOOK]);
    frame.addHeader("destination", genre);
    frame.setBody(_inventory.getUsername() + " has added the book " + book);
    sendFrame(frame);
    _inventory.addBook(genre, book);
}

void UserHandler::borrowBook(std::string& line) {
    std::vector <std::string> cmdParams;
    split(line, cmdParams, CMD_DELIMITER);
    std::string genre = cmdParams[1];
    std::string book = cmdParams[2];

    Frame frame = Frame(_actionFrameCommandMap[BORROW_BOOK]);
    frame.addHeader("destination", genre);
    frame.setBody(_inventory.getUsername() + " wish to borrow " + book);
    sendFrame(frame);
}

void UserHandler::returnBook(std::string& line) {
    std::vector <std::string> cmdParams;
    split(line, cmdParams, CMD_DELIMITER);
    std::string genre = cmdParams[1];
    std::string book = cmdParams[2];

    Frame frame = Frame(_actionFrameCommandMap[RETURN_BOOK]);
    frame.addHeader("destination", genre);
    frame.setBody("Returning " + book + " to " + _inventory.returnBook(book));
    sendFrame(frame);
}

void UserHandler::bookStatus(std::string& line) {
    std::vector <std::string> cmdParams;
    split(line, cmdParams, CMD_DELIMITER);
    std::string genre = cmdParams[1];

    Frame frame = Frame(_actionFrameCommandMap[GENRE_BOOK_STATUS]);
    frame.addHeader("destination", genre);
    frame.setBody("book status");
    sendFrame(frame);
}

void UserHandler::logout(std::string& line) {
    Frame frame = Frame(_actionFrameCommandMap[LOGOUT]);
    frame.addHeader("receipt", std::to_string(nextId));
    sendFrame(frame);

    Frame ans = receiveFrame();
    FrameCommand ansCmd = ans.getCommand();
    if (ansCmd == ERROR) {
        std::cout << ans.getBody() << std::endl;
    }
}

void UserHandler::parseLine(std::string line) {
    if (line == "bye") {
        _shouldTerminate = true;
        return;
    }
    UserActionsEnum cmd = findCmd(line);
    switch (cmd) {
        case LOGIN: {
            login(line);
            break;
        }
        case JOIN_GENRE: {
            joinGenre(line);
            break;
        }
        case EXIT_GENRE: {
            exitGenre(line);
            break;
        }
        case ADD_BOOK: {
            addBook(line);
            break;
        }
        case BORROW_BOOK: {
            borrowBook(line);
            break;
        }
        case RETURN_BOOK: {
            returnBook(line);
            break;
        }
        case GENRE_BOOK_STATUS: {
            bookStatus(line);
            break;
        }
        default: // Logout
            logout(line);
    }
}

void UserHandler::operator()() {
    while (!_shouldTerminate) {
        const short bufSize = 1024;
        char buf[bufSize];
        std::cin.getline(buf, bufSize);
        std::string line(buf);
        parseLine(line);
    }
}