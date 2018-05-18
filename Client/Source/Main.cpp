#include <iostream>

#include <QtCore/QCoreApplication>

#include "Client.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    auto& printer = printer::Printer::getInstance();
    printer(std::cout, "Console client for connecting to Fanuc M-20iA v 0.1\n\n");

    constexpr int  kServerPort = 8888;
    constexpr int  kServerReceivingPort = 9998;
    constexpr int  kServerSendingPort = 9999;
    constexpr char kServerIP[] = "192.168.0.101";

    // Make sure that you use right client: 1 - debug, 2 - for layer, 3 - for robot.
    ///vasily::Client client(kServerReceivingPort, kServerSendingPort, kServerIP);

    vasily::Client client(kServerPort, kServerIP);

    ///vasily::Client client{};

    client.launch();
    client.run();

    return a.exec();
}
