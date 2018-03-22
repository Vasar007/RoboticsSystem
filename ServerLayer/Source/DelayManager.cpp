#include "DelayManager.h"


namespace vasily
{

DelayManager::DelayManager(printer::Printer& printer, logger::Logger& logger)
    : _printer(printer)
{
    _hasReadTable = readDataTable(logger);
}

bool DelayManager::readDataTable(logger::Logger& logger)
{
    while (!logger.inFile.eof())
    {
        if (logger.hasAnyInputErrors())
        {
            _printer.writeLine(std::cout, "Failed to read data table from file!");
            return false;
        }

        const auto distance = logger.read<long long>();
        const auto time     = logger.read<long long>();
        _distanceToTimeTable.emplace_hint(_distanceToTimeTable.end(), distance, time);
    }

    return !_distanceToTimeTable.empty();
}

std::chrono::milliseconds DelayManager::calculateDuration(const RobotData& firstPoint,
                                                          const RobotData& secondPooint) const
{
    if (!_hasReadTable)
    {
        _printer.writeLine(std::cout, "Not read data table!");
        return std::chrono::milliseconds(0LL);
    }

    // Calculate distance between two points, which contains only first 3 coordinates.
    const long long distance    = utils::distance(firstPoint.coordinates.begin(),
                                                  firstPoint.coordinates.begin() + 2,
                                                  secondPooint.coordinates.begin(), 0LL, 1LL);

    // "low" is not less than (i.e. greater or equal to) "distance",
    // or last if no such element is found.
    const auto low = _distanceToTimeTable.lower_bound(distance);
    
    if (low == _distanceToTimeTable.end())
    {
        const auto it = --_distanceToTimeTable.end();
        _printer.writeLine(std::cout, "Distance:", it->first, " Duration:", it->second.count());
        return it->second;
    }
    if (low == _distanceToTimeTable.begin())
    {
        _printer.writeLine(std::cout, "Distance:", low->first, " Duration:", low->second.count());
        return low->second;
    }
    
    auto prev = low;
    --prev;

    // "prev" is less or equal to distance. 
    if (distance - prev->first < low->first - distance)
    {
        _printer.writeLine(std::cout, "Distance:", prev->first,
                           " Duration:", prev->second.count());
        return prev->second;
    }

    _printer.writeLine(std::cout, "Distance:", low->first, " Duration:", low->second.count());
    return low->second;
}

} // namespace vasily
