#include "SocketManager.h"
#include <iostream>

const std::pair<int, int> SocketManager::addSocket(bool isClient, SocketTypes type)
{
    this->socks.emplace_back();
    
    //init new socket
    if (socks[this->socks.size()-1].initSocket(isClient, type) == -1)
        return SocketManager::BAD_SOCK();
    else
        return std::make_pair(
                                socks[this->socks.size()-1].initSocket(isClient, type),
                                this->socks.size()-1
                             );
    
}

int UnixSock::initSocket(bool isClient, SocketTypes type)
{
    //set up a socket
    switch (type)
    {
        case SocketTypes::RAW:
            /* code */
            break;
        case SocketTypes::UDP:
            this->socketHandle = socket(AF_INET, SOCK_DGRAM, 0);
            break;
        case SocketTypes::TCP:

            break;
        default:
            break;
    }
    
    
    
    return this->socketHandle;
    


}

const std::string SocketManager::checkIfValid(int sockfd) const
{
    return "";
}

const std::string SocketManager::catchError(int errNo) const
{
    //return error string
    return "";
}