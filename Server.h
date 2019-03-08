#pragma once

#include "SocketManager.h"
#include <string>

class Server
{
	SocketManager SocketMan;
	std::string ip;
	unsigned short port;

	void init();
public:
	Server(const Server&) = delete;
	Server operator=(const Server&) = delete;

	Server() { init(); }

	void run();
	bool changePort();
};
