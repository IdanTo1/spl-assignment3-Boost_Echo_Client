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
        boost::mutex _inventoryMutex, _borrowMutex;
    public:
        void addBook(std::string genre, std::string book);
        void setUsername(std::string name);
        std::string getUsername();
        void borrowBook(std::string book, std::string lender);
        std::string returnBook(std::string book);
};


#endif //BOOST_ECHO_CLIENT_CLIENTINVENTORY_H
