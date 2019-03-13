//this is code I wrote for a personal project ... using it for this project
#include <vector>
#include <algorithm>
#include <functional>
#include <utility>
#include <ctime>
#include <string>
#include <memory>

enum class SocketTypes
{
	UDP,
	TCP,
	RAW
};

#ifdef _WIN32
#pragma once
#include <winsock2.h>
struct WinSock
{
	int socketHandle = -1;
	void initSocket();
};
using Socket = WinSock;
#else
//include unix sock
#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

struct UnixSock
{
	//<data>
	time_t timeSpawned;
	int socketHandle = -1;
	int port;
	int optval; //option value for socketopt()
	std::string hostName;
	std::shared_ptr<struct hostent> host;
	std::shared_ptr<struct sockaddr_in> serverAddress;
	std::shared_ptr<struct sockaddr_in> clientAddress;
	//</data>

	int initSocket(bool isClient = false, SocketTypes type = SocketTypes::UDP);
	inline bool isOkay() { return socketHandle > 0; }
	
	
	//<object concepts>
	UnixSock() = default;
	UnixSock(const UnixSock& other) { *this = objCopy(*this, other); }
	UnixSock(UnixSock&& other) = default;
	
	UnixSock& operator=(const UnixSock& rhs) { *this = objCopy(*this, rhs); return *this; }
	inline bool operator<(const UnixSock& rhs) { return std::asctime(std::localtime(&this->timeSpawned)) < std::asctime(std::localtime(&rhs.timeSpawned)); }
	inline bool operator>(const UnixSock& rhs) { return std::asctime(std::localtime(&this->timeSpawned)) > std::asctime(std::localtime(&rhs.timeSpawned)); }
	~UnixSock()
	{
		socketHandle = -1;
	}
	//</object concepts>

private:
	//assignment function
	std::function<UnixSock(UnixSock&, const UnixSock&)> objCopy = [&](UnixSock modified, const UnixSock notModified)->UnixSock
	{ 
		modified.socketHandle = notModified.socketHandle;
		modified.port = notModified.port;
		modified.optval = notModified.optval;
		modified.hostName = notModified.hostName;
		modified.host.reset(notModified.host.get());
		modified.serverAddress.reset(notModified.serverAddress.get());
		modified.clientAddress.reset(notModified.clientAddress.get());
		modified.timeSpawned = std::time(nullptr);
		return modified;
	};

	void buildNetAddress();
	int bind(/*Only used for server socket initialization*/);
};
using Socket = UnixSock;
#endif

/*
Container for multiple socket objects
*/

class SocketManager
{
	std::vector<Socket> socks;

	const std::string catchError(int errNo) const;
public:
	/*
	return : pair "<socket file descriptor> , <index in array>"
	*/
	const std::pair<int, int> addSocket();


	const Socket getSocketByAddr(const int& sockfd) const;
	const Socket getSocketByIndex(const int& index) const;

};