#include <iostream>

#include "ServerLayer.h"


int main()
{
    auto& printer = printer::Printer::getInstance();
    printer(std::cout, "Server layer for connecting from client to Fanuc M-20iA v 0.1\n\n");

    constexpr int	SERVER_SENDING_PORT     = 9999;
    constexpr int	SERVER_RECEIVING_PORT   = 9998;
    constexpr char	SERVER_IP[]             = "192.168.0.101";
    constexpr int   LAYER_PORT              = 8888;
    constexpr int   BACKLOG                 = 10;

    vasily::ServerLayer serverLayer(SERVER_SENDING_PORT, SERVER_RECEIVING_PORT, SERVER_IP,
                                    LAYER_PORT, BACKLOG);

    serverLayer.init();
    serverLayer.launch();
    serverLayer.run();

    return 0;
}
