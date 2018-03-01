#include <iostream>

#include "StrainGauge.h"


bool StrainGauge::initializeCom()
{
    _hSerial = CreateFile(_sPortName, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL, nullptr);

    if (_hSerial == INVALID_HANDLE_VALUE)
    {
        if (GetLastError() == ERROR_FILE_NOT_FOUND)
        {
            std::cout << "Serial port does not exist\n";
            return false;
        }
        std::cout << "Some other error occurred\n";
        return false;
    }
    return true;
}

StrainGauge::StrainGauge(const LPCWSTR port)
    : _sPortName(port),
      _sSendChar{ 0x1, 0x0, 0x0, 0x0, 0x0 }
{
    initializeCom();
    connect();
}

bool StrainGauge::connect()
{
    DCB dcbSerialParams;
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(_hSerial, &dcbSerialParams))
    {
        std::cout << "Getting state error\n";
        return false;
    }
    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if (!SetCommState(_hSerial, &dcbSerialParams))
    {
        std::cout << "Error setting serial port state\n";
        return false;
    }
    char sStopChar[5] = { 0x7, 0x0, 0x0, 0x0, 0x0 };
    WriteFile(_hSerial, &sStopChar, 5, &_iSize, nullptr);

    std::cout << "Connected successfully!\n";
    return true;
}

std::array<double, 6> StrainGauge::readComStrain()
{
    ///char period = 0x000186A0;

    WriteFile(_hSerial, &_sSendChar, 5, &_iSize, nullptr);

    // Recieve 1 byte of answer.
    ReadFile(_hSerial, &_skip, 1, &_iSize, nullptr);
    // std::cout << "Skipped: " << skip + '0' - 48 << '\n';
    
    std::array<double, 6> data{};
    for (double& elem : data)
    {
        // Recieve 2 bytes of actual data.
        ReadFile(_hSerial, &_sReceivedChar, 2, &_iSize, nullptr);
        elem = static_cast<double>(_sReceivedChar);
    }
    return data;
}

void StrainGauge::whatsPeriod()
{
    uint32_t period{};

    char sStopChar[5] = { 0x7, 0x0, 0x0, 0x0, 0x0 };
    WriteFile(_hSerial, &sStopChar, 5, &_iSize, nullptr);

    char sSendChar[5] = { 0x8, 0x0, 0x0, 0x0, 0x0 };
    WriteFile(_hSerial, &sSendChar, 5, &_iSize, nullptr);

    // Recieve 1 byte of answer.
    ReadFile(_hSerial, &_skip, 1, &_iSize, nullptr);
    // std::cout << "Skipped: " << _skip + '0' - 48 << '\n';

    // Recieve 2 bytes of actual data.
    ReadFile(_hSerial, &period, 4, &_iSize, nullptr);
    std::cout << "Period: " << period << '\n';
}

StrainGauge::~StrainGauge()
{
    CloseHandle(_hSerial);
}
