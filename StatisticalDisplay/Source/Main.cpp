#include <iostream>
#include <thread>

#include <SFML/Graphics.hpp>

#include "Client.h"
#include "Transmitter.h"
#include "Utilities.h"


namespace vasily
{

/**
 * \brief             Initialize client and launch it.
 * \param[out] client Client to work.
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
    constexpr int	kServerPort				= 8888;
    constexpr int	kServerPortSending		= 9999;
    constexpr int	kServerPortReceiving	= 9998;
    constexpr char	kServerIP[]				= "192.168.0.101";

    constexpr int	kRobotSendingPortToServer       = 59002;
    constexpr int	kRobotReceivingPortFromServer   = 59003;
    constexpr char	kRobotServerIP[]                = "192.168.1.21";

    // MAKE SURE THAT YOU USE RIGHT CLIENT: 2 — DEBUG, 3 — WORKING WITH ROBOT, 1 — UNIVERSAL.
    vasily::Client client(kServerPort, kServerIP, vasily::Client::WorkMode::INDIRECT);
    ///vasily::Client client(kServerPortSending, kServerPortReceiving, kServerIP,
    ///				      vasily::Client::WorkMode::STRAIGHTFORWARD);
    ///vasily::Client client(kRobotSendingPortToServer, kRobotReceivingPortFromServer,
    ///					  kRobotServerIP, vasily::Client::WorkMode::STRAIGHTFORWARD);

    std::thread clientThread(vasily::init, std::ref(client));
    clientThread.detach();

    constexpr unsigned int kWidth	= 1280u;
    constexpr unsigned int kHeight	= 720u;

    sf::RenderWindow window(sf::VideoMode(kWidth, kHeight), "Fanuc Diagram Interface");
    window.setFramerateLimit(60u);

    statistic::Transmitter transmitter(kWidth, kHeight);

    while (window.isOpen())
    {
        if (client.isNeedToUpdate.load())
        {
            transmitter.updateVertices(client.getDuration().count(), client.getRobotData());
            client.isNeedToUpdate.store(false);
        }

        sf::Event event{};
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
