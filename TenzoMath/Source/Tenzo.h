#ifndef TENZO_2017
#define TENZO_2017

#include <Windows.h>
#include <iostream>
#include <array>


class Tenzo
{
	HANDLE hSerial = nullptr;
	const LPCTSTR sPortName;

	DWORD iSize{};
	int16_t sReceivedChar{}; // For recieve data from COM port
	char skip{};
	static constexpr char period = 'a'; //static_cast<char>(100000);
	
	static constexpr char sSendChar[5] = {0x1, 0x0, 0x0, 0x0, 0x0};
	static constexpr char sStopChar[5] = {0x7, 0x0, 0x0, 0x0, 0x0};

	int initCOMStrain()
	{
		hSerial = ::CreateFile(sPortName, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING,
							   FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hSerial == INVALID_HANDLE_VALUE)
		{
			if (GetLastError() == ERROR_FILE_NOT_FOUND)
			{
				std::cout << "Serial port does not exist\n";
				return 1001;
			}
			std::cout << "Some other error occurred\n";
			return 1002;
		}
		DCB dcbSerialParams = {0};
		dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
		if (!GetCommState(hSerial, &dcbSerialParams))
		{
			std::cout << "Getting state error\n";
			return 1003;
		}
		dcbSerialParams.BaudRate = CBR_9600;
		dcbSerialParams.ByteSize = 8;
		dcbSerialParams.StopBits = ONESTOPBIT;
		dcbSerialParams.Parity = NOPARITY;
		if (!SetCommState(hSerial, &dcbSerialParams))
		{
			std::cout << "Error setting serial port state\n";
			return 1004;
		}
		std::cout << "Connected successfully!\n";
		WriteFile(hSerial, &sStopChar, 5, &iSize, nullptr);
		return 0;
	}

public:
	explicit Tenzo(const LPCTSTR sName)
		: sPortName(sName)
	{
		initCOMStrain();
	}

	~Tenzo() = default;

	std::array<double, 6> readData()
	{
		WriteFile(hSerial, &sSendChar, 5, &iSize, nullptr);
		std::array<double, 6> data{};
		ReadFile(hSerial, &skip, 1, &iSize, nullptr); // получаем 1 байт кода ответа
		//std::cout << "Skipped: " << skip + '0' - 48 << std::endl;
		for (size_t i = 0; i < 6; ++i)
		{
			ReadFile(hSerial, &sReceivedChar, 2, &iSize, nullptr); // получаем 2 байтa
			data[i] = static_cast<double>(sReceivedChar);
		}
		return data;
	}
};

/*
int main(){
...
	Tenzo name(L"COM3");  // initialize Strain gauge
	while(true){
		std::array<int, 6> tmpData = Tenzo.readData();
	}
...
}
*/


#endif // TENZO_2017
