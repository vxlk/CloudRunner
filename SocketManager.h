#pragma once
#include <vector>
#ifdef _WIN32
#include <winsock2.h>
struct WinSock
{
	int socketHandle;
	void initSocket();
};
using Socket = WinSock;
#else
//include unix sock
#include <sys/socket.h>
struct UnixSock
{

};
using Socket = UnixSock;
#endif



class SocketManager
{
	std::vector<Socket> socks;


public:
	
};