//
// Created by idanto on 11/01/2020.
//

#include "../include/ClientInventory.h"


void ClientInventory::addBook(std::string genre, std::string book) {
    boost::lock_guard <boost::mutex> lock(_inventoryMutex);
    _inventory[genre].push_back(book);
}

std::string ClientInventory::getUsername() {
    return _username;
}

void ClientInventory::setUsername(std::string name) {
    _username = name;
}

void ClientInventory::borrowBook(std::string book, std::string lender) {
    boost::lock_guard <boost::mutex> lock(_borrowMutex);
    _borrowMap[book] = lender;
}

std::string ClientInventory::returnBook(std::string book) {
    std::string lender = _borrowMap[book];
    _borrowMap.erase(book);
    return lender;
}
