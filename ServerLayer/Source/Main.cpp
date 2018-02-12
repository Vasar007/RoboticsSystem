#include <iostream>

#include "ServerLayer.h"


int main()
{
    auto& printer = printer::Printer::getInstance();
    printer(std::cout, "Server layer for connecting from client to Fanuc M-20iA v 0.1\n\n");

    constexpr int	SERVER_SENDING_PORT     = 9999;
    constexpr int	SERVER_RECEIVING_PORT   = 9998;
    constexpr char	SERVER_IP[]             = "192.168.1.108"; 
    constexpr int   LAYER_PORT              = 8888;
    constexpr int   BACKLOG                 = 10;

    constexpr int	ROBOT_SENDING_PORT_TO_SERVER        = 59002;
    constexpr int	ROBOT_RECEIVING_PORT_FROM_SERVER    = 59003;
    constexpr char	ROBOT_SERVER_IP[]                   = "192.168.1.21";

    ///vasily::ServerLayer serverLayer(SERVER_SENDING_PORT, SERVER_RECEIVING_PORT, SERVER_IP,
    ///                                LAYER_PORT, BACKLOG);
    vasily::ServerLayer serverLayer(ROBOT_SENDING_PORT_TO_SERVER, ROBOT_RECEIVING_PORT_FROM_SERVER,
                                    ROBOT_SERVER_IP, LAYER_PORT, BACKLOG);

    serverLayer.launch();
    serverLayer.run();

    return 0;
}
