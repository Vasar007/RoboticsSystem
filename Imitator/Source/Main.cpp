#include <iostream>

#include "ServerImitator.h"


int main()
{
	auto& printer = printer::Printer::getInstance();
	printer.writeLine(std::cout, "Simulator server Fanuc M-20iA v 0.1\n");

	constexpr int kPortSending		= 9998;
	constexpr int kPortReceiving	= 9999;
	constexpr int kBacklog			= 10;
	
	vasily::ServerImitator server(kPortSending, kPortReceiving, kBacklog);

	server.launch();
	server.run();

	return 0;
}
