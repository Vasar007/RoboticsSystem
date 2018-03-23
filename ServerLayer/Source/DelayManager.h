#ifndef DELAY_MANAGER_H
#define DELAY_MANAGER_H

#include <chrono>
#include <map>

#include "Utilities.h"


namespace vasily
{

/**
 * \brief Class used to calculate delays which are based on real experiments.
 */
class DelayManager
{
public:
    /**
     * \brief             Constructor which copies printer reference and using logger reference
     *                    reads data in dictionary (std::map).
     * \param[in] printer Reference to single instance of Printer.
     * \param[out] logger Reference to instance which could read data table.
     */
    DelayManager(printer::Printer& printer, logger::Logger& logger);

    /**
     * \brief              Calculate duration for currrent movement section between two points.
     * \param firstPoint   Fiirst point of movement.
     * \param secondPooint Second point of movement.
     * \return             Approximately duration in milliseconds.
     */
    std::chrono::milliseconds calculateDuration(const RobotData& firstPoint,
                                                const RobotData& secondPooint) const;


private:
    /**
     * \brief Implementation of type-safe output printer.
     */
    printer::Printer& _printer;

    /**
     * \brief Flag used to know if data table successful read or not.
     */
    bool        _hasReadTable;

    /**
     * \brief Data table which keeps compliance with distance and time.
     */
    std::map<long long, std::chrono::milliseconds> _distanceToTimeTable;


    /**
     * \brief            Read data from file into container.
     * \param[in] logger Instance of logger which used to read data from file.
     * \return           True if reading was successful, false otherwise.
     */
    bool readDataTable(logger::Logger& logger);
};

} // namespace vasily

#endif // DELAY_MANAGER_H
