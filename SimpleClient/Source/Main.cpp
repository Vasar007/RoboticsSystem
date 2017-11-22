#include <iostream>
#include <string>

#include "Client.h"


int main()
{
	utils::println("Console client for connecting to Fanuc M-20iA v 0.1\n");

	const int SERVER_PORT		= 9997;
	const std::string SERVER_IP = "192.168.0.103";
	vasily::Client client(SERVER_PORT, SERVER_IP);

	client.init();
	client.launch();
	client.run();

	std::cin.get();
	return 0;
}
