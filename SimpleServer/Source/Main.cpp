#include <iostream>
#include <string>

#include "ServerImitator.h"


int main()
{
	std::cout << "Simulator server FanucM20iA v 0.1\n\n";

	const int PORT_SENDING	= 9998;
	const int PORT_RECIVING	= 9999;
	const int BACKLOG		= 10;
	ServerImitator server(PORT_SENDING, PORT_RECIVING, BACKLOG);

	server.run();

	std::system("pause");
	return 0;
}
