#ifndef TRAJECTORY_MANAGER_H
#define TRAJECTORY_MANAGER_H

#include "Handler.h"


/**
 * \brief My woring namespace (Danila Gusev).
 */
namespace danila
{

/**
 * \brief Class for creating trajectory from declaration.
 */
class TrajectoryManager
{
public:
    /**
     * \brief Default constructor.
     */
    TrajectoryManager() = default;

    /**
     * \brief                   Method for generating positional trajectory.
     * \param[in] parsedResult  Parsed data for generating trajectory.
     * \return                  List of points in positional trajectory.
     * \code
     * Enter command: o|1 2 3 4 5 6 10 2 0|10 20 30 40 50 60 10 2 0
     * \endcode
     */
    std::vector<vasily::RobotData> positionalMovement(
        const vasily::ParsedResult& parsedResult) const noexcept;

    /**
     * \brief                   Method for generating partial trajectory.
     * \param[in] parsedResult  Parsed data for generating trajectory.
     * \return                  List of points in partial trajectory.
     * \code
     * Enter command: p|3|1 2 3 4 5 6 10 2 0|10 20 30 40 50 60 10 2 0
     * \endcode
    */
    std::vector<vasily::RobotData> partialMovement(
        const vasily::ParsedResult& parsedResult) const;

    /**
     * \brief                   Method for generating circlic trajectory.
     * \param[in] parsedResult  Parsed data for generating trajectory.
     * \return                  List of points in circlic trajectory.
     * \code
     * Enter command: c|3|1 2 3 4 5 6 10 2 0|10 20 30 40 50 60 10 2 0
     * \endcode
     */
    std::vector<vasily::RobotData> circlicMovement(
        const vasily::ParsedResult& parsedResult) const;
};

} // namespace danila

#endif // TRAJECTORY_MANAGER_H
