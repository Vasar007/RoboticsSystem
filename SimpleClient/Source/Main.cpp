#include <iostream>
#include <string>

#include "Client.h"


int main()
{
	std::cout << "Simple console client by Vasar v 0.1\n" << std::endl;

	const int port = 9999;
	const std::string serverIp = "192.168.0.101";
	Client client(port, serverIp);

	client.run();

	std::system("pause");
	return 0;
}
