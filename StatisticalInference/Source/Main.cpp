#include <thread>

#include <SFML/Graphics.hpp>

#include "Utilities.h"
#include "WinsockInterface.h"
#include "Client.h"
#include "Transmitter.h"


constexpr int SERVER_PORT			= 9997;
constexpr int SERVER_PORT_SENDING	= 9999;
constexpr int SERVER_PORT_RECEIVING = 9998;
const std::string SERVER_IP			= "192.168.0.4";

///vasily::Client client(SERVER_PORT_SENDING, SERVER_PORT_RECEIVING, SERVER_IP);
vasily::Client client;

void init()
{
	utils::println("Console client for connecting to Fanuc M-20iA v 0.1\n");

	client.init();
	client.launch();
	client.run();
}

int main()
{
	std::thread clientThread(init);
	clientThread.detach();

	constexpr int WIDTH		= 1280;
	constexpr int HEIGHT	= 720;

	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Fanuc Diagram Interface");
	window.setFramerateLimit(60u);

	Transmitter transmitter(WIDTH, HEIGHT);

	while (window.isOpen())
	{
		transmitter.updateVertices(client.getDuration().count());

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		window.clear(sf::Color::White);
		window.draw(transmitter);
		window.display();
	}

	return 0;
}