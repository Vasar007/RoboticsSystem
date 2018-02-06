#include <iostream>
#include <thread>

#include <SFML/Graphics.hpp>

#include "Client.h"
#include "Transmitter.h"
#include "Utilities.h"


namespace vasily
{

/**
 * \brief               Initialize client and launch it.
 * \param[out] client   Client to work.
 */
void init(vasily::Client& client)
{
	auto& printer = printer::Printer::getInstance();
	printer(std::cout, "Console client for connecting to Fanuc M-20iA v 0.1\n\n");

	client.launch();
	client.run();
}

} // namespace vasily

int main()
{
	constexpr int	SERVER_PORT				= 8888;
	constexpr int	SERVER_PORT_SENDING		= 9999;
	constexpr int	SERVER_PORT_RECEIVING	= 9998;
	constexpr char	SERVER_IP[]				= "192.168.0.10"; // 192.168.0.101

	constexpr int	ROBOT_SENDING_PORT_TO_SERVER      = 59002;
	constexpr int	ROBOT_RECEIVING_PORT_FROM_SERVER  = 59003;
	constexpr char	ROBOT_SERVER_IP[]                 = "192.168.0.21";

	// MAKE SURE THAT YOU USE RIGHT CLIENT: 2 — DEBUG, 3 — WORKING WITH ROBOT, 1 — UNIVERSAL.
	///vasily::Client client(SERVER_PORT, SERVER_IP, vasily::Client::WorkMode::INDIRECT);
	///vasily::Client client(SERVER_PORT_SENDING, SERVER_PORT_RECEIVING, SERVER_IP,
	///				      vasily::Client::WorkMode::INDIRECT);
	vasily::Client client(ROBOT_SENDING_PORT_TO_SERVER, ROBOT_RECEIVING_PORT_FROM_SERVER,
						  ROBOT_SERVER_IP, vasily::Client::WorkMode::STRAIGHTFORWARD);

	std::thread clientThread(vasily::init, std::ref(client));
	clientThread.detach();

	constexpr unsigned int WIDTH	= 1280u;
	constexpr unsigned int HEIGHT	= 720u;

	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Fanuc Diagram Interface");
	window.setFramerateLimit(60u);

	statistic::Transmitter transmitter(WIDTH, HEIGHT);

	while (window.isOpen())
	{
		if (client.isNeedToUpdate.load())
		{
			transmitter.updateVertices(client.getDuration().count(), client.getRobotData());
			client.isNeedToUpdate.store(false);
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
