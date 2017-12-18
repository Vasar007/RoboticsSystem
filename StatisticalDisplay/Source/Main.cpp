#include <thread>

#include <SFML/Graphics.hpp>

#include "Client.h"
#include "Transmitter.h"


/**
 * \brief               Initialize client and launch it.
 * \param[out] client   Client to work.
 */
void init(vasily::Client& client)
{
	utils::println(std::cout, "Console client for connecting to Fanuc M-20iA v 0.1\n");

	client.init();
	client.launch();
	client.run();
}

int main()
{
	constexpr int	SERVER_PORT				= 9997;
	constexpr int	SERVER_PORT_SENDING		= 9999;
	constexpr int	SERVER_PORT_RECEIVING	= 9998;
	constexpr char	SERVER_IP[]				= "192.168.0.100";

	// MAKE SURE THAT YOU USE RIGHT CLIENT TO WORK WITH ROBOT: 1 - DEBUG, 2 - WORKING WITH ROBOT.
	vasily::Client client(SERVER_PORT_SENDING, SERVER_PORT_RECEIVING, SERVER_IP);
	///vasily::Client client;

	std::thread clientThread(init, std::ref(client));
	clientThread.detach();

	constexpr std::size_t WIDTH		= 1280u;
	constexpr std::size_t HEIGHT	= 720u;

	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Fanuc Diagram Interface");
	window.setFramerateLimit(60u);

	statistic::Transmitter transmitter(WIDTH, HEIGHT);

	while (window.isOpen())
	{
		if (client.isNeedToUpdate)
		{
			transmitter.updateVertices(client.getDuration().count(), client.getRobotData());
			client.isNeedToUpdate = false;
		}

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			else if (event.type == sf::Event::KeyPressed 
					 && sf::Keyboard::isKeyPressed(sf::Keyboard::Delete))
			{
				transmitter.clear();
			}
		}

		window.clear(sf::Color::White);
		window.draw(transmitter);
		window.display();
	}

	return 0;
}
