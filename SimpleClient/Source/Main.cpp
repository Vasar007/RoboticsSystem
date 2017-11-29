//#include <iostream>
//#include <string>
//
//#include "Client.h"
//
//
//int main()
//{
//	utils::println("Console client for connecting to Fanuc M-20iA v 0.1\n");
//
//	constexpr int SERVER_PORT			= 9997;
//	constexpr int SERVER_PORT_SENDING	= 9999;
//	constexpr int SERVER_PORT_RECEIVING = 9998;
//	const std::string SERVER_IP			= "192.168.0.4";
//
//	vasily::Client client(SERVER_PORT_SENDING, SERVER_PORT_RECEIVING, SERVER_IP);
//	//vasily::Client client;
//
//	client.init();
//	client.launch();
//	client.run();
//
//	std::cin.get();
//	return 0;
//}
