#include <iostream>
#include <string>

#include "Client.h"


int main()
{
	std::cout << "Console client for connecting to FanucM20iA v 0.1\n\n";

	const int SERVER_PORT		= 9997;
	const std::string SERVER_IP = "192.168.0.117";
	Client client(SERVER_PORT, SERVER_IP);

	client.init();
	client.launch();
	client.run();

	std::system("pause");
	return 0;
}
