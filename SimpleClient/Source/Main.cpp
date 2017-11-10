#include <iostream>
#include <string>

#include "Client.h"


int main()
{
	std::cout << "Simple console client by Vasar v 0.1\n" << std::endl;

	const int PORT = 9999;
	const std::string SERVER_IP = "192.168.0.101";
	Client client(PORT, SERVER_IP);

	client.run();

	std::system("pause");
	return 0;
}
