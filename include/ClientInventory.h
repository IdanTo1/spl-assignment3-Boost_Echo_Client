//
// Created by idanto on 11/01/2020.
//

#ifndef BOOST_ECHO_CLIENT_CLIENTINVENTORY_H
#define BOOST_ECHO_CLIENT_CLIENTINVENTORY_H

#include <map>
#include <boost/thread.hpp>


class ClientInventory {
    private:
        std::string _username = "";
        std::map <std::string, std::vector<std::string>> _inventory;
        std::map <std::string, std::string> _borrowMap;
        std::map <std::string, std::vector<std::string>> _wishList;
        boost::mutex _inventoryMutex, _borrowMutex, _wishListMutex;
    public:
        ClientInventory(std::map <std::string, std::vector<std::string>>& inventory,
                        std::map <std::string, std::string>& borrowMap,
                        std::map <std::string, std::vector<std::string>>& wishList);

        void addBook(std::string genre, std::string book);
        std::vector <std::string> getGenreBooks(std::string genre);

        bool isInInventory(std::string genre, std::string book);
        void removeFromInventory(std::string genre, std::string book);

        void setUsername(std::string name);
        std::string getUsername();

        void addToWishList(std::string genre, std::string book);
        bool isInWishList(std::string genre, std::string book);
        void removeFromWishList(std::string genre, std::string book);

        void borrowBook(std::string book, std::string lender);
        std::string returnBook(std::string book);
};


#endif //BOOST_ECHO_CLIENT_CLIENTINVENTORY_H
