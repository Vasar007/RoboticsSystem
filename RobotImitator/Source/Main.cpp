#include <iostream>

#include <QtCore/QCoreApplication>

#include "RobotImitator.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    auto& printer = printer::Printer::getInstance();
    printer.writeLine(std::cout, "Simulator server Fanuc M-20iA v 0.1\n");

    constexpr int kReceivingPort = 9998;
    constexpr int kSendingPort = 9999;

    vasily::RobotImitator server(kReceivingPort, kSendingPort);
    server.launch();

    return a.exec();
}
