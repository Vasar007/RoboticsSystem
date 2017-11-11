#include <iostream>
#include <string>

#include "Server.h"


int main()
{
	std::cout << "Simple console server by Vasar v 0.1\n" << std::endl;

	const int port		= 9999;
	const int backlog	= 10;
	Server server(port, backlog);

	server.run();

	std::system("pause");
	return 0;
}
