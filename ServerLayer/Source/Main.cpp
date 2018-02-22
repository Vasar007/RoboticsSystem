#include <iostream>

#include "ServerLayer.h"


int main()
{
    auto& printer = printer::Printer::getInstance();
    printer(std::cout, "Server layer for connecting from client to Fanuc M-20iA v 0.1\n\n");

    constexpr int	kServerSendingPort      = 9999;
    constexpr int	kServerReceivingPort    = 9998;
    constexpr char	kServerIP[]             = "192.168.0.102"; 
    constexpr int   kLayerPort              = 8888;
    constexpr int   kBacklog                = 10;

    constexpr int	kRobotSendingPortToServer       = 59002;
    constexpr int	kRobotReceivingPortFromServer   = 59003;
    constexpr char	kRobotServerIP[]                = "192.168.1.21";

    ///vasily::ServerLayer serverLayer(kServerSendingPort, kServerReceivingPort, kServerIP,
    ///                               kLayerPort, kBacklog);
    vasily::ServerLayer serverLayer(kRobotSendingPortToServer, kRobotReceivingPortFromServer,
                                    kRobotServerIP, kLayerPort, kBacklog);

    serverLayer.launch();
    serverLayer.run();

    return 0;
}
