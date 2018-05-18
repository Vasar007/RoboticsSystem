#include <iostream>

#include <QtCore/QCoreApplication>

#include "ServerLayer.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    auto& printer = printer::Printer::getInstance();
    printer(std::cout, "Server layer for connecting from client to Fanuc M-20iA v 0.1\n\n");

    constexpr int  kServerReceivingPort = 9998;
    constexpr int  kServerSendingPort = 9999;
    constexpr char kServerIP[] = "192.168.0.101";
    constexpr int  kLayerPort = 8888;

    vasily::ServerLayer serverLayer(kServerReceivingPort, kServerSendingPort, kServerIP, kLayerPort);

    ///vasily::ServerLayer serverLayer{};

    serverLayer.launch();

    return a.exec();
}
