#ifndef STRAIN_GAUGE_H
#define STRAIN_GAUGE_H

#include <array>

#include <Windows.h>


/**
* \brief Main class of communication between user and strain gauge.
*/
class StrainGauge
{
public:
    /**
     * \brief Constructor of Strain gauge with port of connection.
     * \param[in] port Name of the port with sensor in the form: L"COM5".
     */
    explicit StrainGauge(const LPCTSTR port);

    /**
     * \brief Function of the connect to the strain gauge.
     * \return True if successful.
     */
    bool connect();

    /**
     * \brief Function of the getting data array from the strain gauge.
     * \return 3 strengths and 3 moments from the sensor in conventional units.
     */
    std::array<double, 6> readComStrain();

    /**
     * \brief Function of getting period of working and writing it on the screen.
     */
    void whatsPeriod();

    /**
     * \brief Destructor with closing of the port connection.
     */
    virtual ~StrainGauge();

    StrainGauge(const StrainGauge&) = default;
    StrainGauge(StrainGauge&&) = default;
    StrainGauge& operator=(const StrainGauge&) = default;
    StrainGauge& operator=(StrainGauge&&) = default;


private:
    /**
     * \brief Declaration of serial port.
     */
    HANDLE _hSerial{};

    /**
     * \brief Name of the serial port in the form: L"COM5".
     */
    LPCWSTR _sPortName;

    /**
     * \brief Size of buffer for sending data.
     */
    DWORD _iSize{};

    /**
     * \brief Recieved code from the strain gauge.
     */
    int16_t _sReceivedChar{};

    /**
     * \brief Received code with information about the data packet.
     */
    char _skip{};

    /**
     * \brief Array of chars for sensor with info to get data packet.
     */
    char _sSendChar[5];

    /**
     * \brief Function of initializing of COM port.
     * \return True if successful.
     */
    bool initializeCom();
};

#endif // STRAIN_GAUGE_H
