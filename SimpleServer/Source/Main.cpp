#include <iostream>
#include <string>

#include "ServerImitator.h"


int main()
{
	utils::println("Simulator server Fanuc M-20iA v 0.1\n");

	const int PORT_SENDING	= 9998;
	const int PORT_RECIVING	= 9999;
	const int BACKLOG		= 10;
	
	vasily::ServerImitator server(PORT_SENDING, PORT_RECIVING, BACKLOG);

	server.init();
	server.launch();
	server.run();

	std::cin.get();
	return 0;
}
