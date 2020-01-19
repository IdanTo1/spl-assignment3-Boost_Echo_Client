//
// Created by idanto on 11/01/2020.
//

#include "../include/ClientInventory.h"


ClientInventory::ClientInventory(std::map <std::string, std::vector<std::string>>& inventory,
                                 std::map <std::string, std::string>& borrowMap,
                                 std::map <std::string, std::vector<std::string>>& wishList) :
        _inventory(inventory), _borrowMap(borrowMap), _wishList(wishList),
        _inventoryMutex(), _borrowMutex(), _wishListMutex() {}

void ClientInventory::clear() {
    boost::lock_guard <boost::mutex> inventoryLock(_inventoryMutex);
    boost::lock_guard <boost::mutex> borrowLock(_borrowMutex);
    boost::lock_guard <boost::mutex> wishListLock(_wishListMutex);
    _inventory.clear();
    _borrowMap.clear();
    _wishList.clear();
}

void ClientInventory::addBook(std::string genre, std::string book) {
    boost::lock_guard <boost::mutex> lock(_inventoryMutex);
    _inventory[genre].push_back(book);
}

std::vector <std::string> ClientInventory::getGenreBooks(std::string genre) {
    boost::lock_guard <boost::mutex> lock(_inventoryMutex);
    return _inventory[genre];
}

bool ClientInventory::isInInventory(std::string genre, std::string book) {
    boost::lock_guard <boost::mutex> lock(_inventoryMutex);
    std::vector <std::string> genreInventory = _inventory.at(genre);
    return (std::find(genreInventory.begin(), genreInventory.end(), book) != genreInventory.end());
}

void ClientInventory::removeFromInventory(std::string genre, std::string book) {
    /*
    * no need to check if it = genreInventory.end(), because isInInventory will be called before this method
    * and only one thread removes elements from this vector.
    * in addition, an other client won't take a book from this client, if it didn't have it before.
    */
    boost::lock_guard <boost::mutex> lock(_inventoryMutex);
    std::vector <std::string>& genreInventory = _inventory.at(genre);
    auto it = std::find(genreInventory.begin(), genreInventory.end(), book);
    genreInventory.erase(it);
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
    boost::lock_guard <boost::mutex> lock(_borrowMutex);
    std::string lender = _borrowMap.at(book);
    _borrowMap.erase(book);
    return lender;
}

void ClientInventory::addToWishList(std::string genre, std::string book) {
    boost::lock_guard <boost::mutex> lock(_wishListMutex);
    _wishList[genre].push_back(book);
}

bool ClientInventory::isInWishList(std::string genre, std::string book) {
    boost::lock_guard <boost::mutex> lock(_wishListMutex);
    std::vector <std::string> genreWishList = _wishList[genre];
    return (std::find(genreWishList.begin(), genreWishList.end(), book) != genreWishList.end());
}

void ClientInventory::removeFromWishList(std::string genre, std::string book) {
    /*
    * no need to check if it = genreWishList.end(), because isInWishList will be called before this method
    * and only one thread removes elements from this vector.
    * in addition, we may assume no 2 users ask for the same book at the same time, so if user wishes to have
    * this book, it will be in the wish list prior to the check.
    */
    boost::lock_guard <boost::mutex> lock(_wishListMutex);
    std::vector <std::string>& genreWishList = _wishList.at(genre);
    auto it = std::find(genreWishList.begin(), genreWishList.end(), book);
    genreWishList.erase(it);
}
