#include <iostream>
#include <string>

#include "ServerImitator.h"


int main()
{
	utils::println("Simulator server Fanuc M-20iA v 0.1\n");

	constexpr int PORT_SENDING		= 9998;
	constexpr int PORT_RECEIVING	= 9999;
	constexpr int BACKLOG			= 10;
	
	vasily::ServerImitator server(PORT_SENDING, PORT_RECEIVING, BACKLOG);

	server.init();
	server.launch();
	server.run();

	std::cin.get();
	return 0;
}
