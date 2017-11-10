#include <iostream>
#include <string>

#include "Server.h"


int main()
{
	std::cout << "Simple console server by Vasar v 0.1\n" << std::endl;

	const int PORT		= 9999;
	const int BACKLOG	= 10;
	Server server(PORT, BACKLOG);

	server.run();

	std::system("pause");
	return 0;
}
