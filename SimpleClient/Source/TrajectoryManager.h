#ifndef TRAJECTORY_MANAGER_H
#define TRAJECTORY_MANAGER_H

#include "Handler.h"

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
     *  * \code
	 * Enter command: o|1 2 3 4 5 6 10 2 0|10 20 30 40 50 60 10 2 0
	 * \endcode
	
     */
    std::vector<vasily::RobotData> PositionalMovement(
        const vasily::ParsedResult& parsedResult) const noexcept;

    /**
     * \brief                   Method for generating partial trajectory.
     * \param[in] parsedResult  Parsed data for generating trajectory.
     * \return                  List of points in partial trajectory.
     * \code
	 * Enter command: p|3|1 2 3 4 5 6 10 2 0|10 20 30 40 50 60 10 2 0
	 * \endcode
	
    */
    std::vector<vasily::RobotData> PartialMovement(
        const vasily::ParsedResult& parsedResult) const noexcept;

    /**
     * \brief                   Method for generating circlic trajectory.
     * \param[in] parsedResult  Parsed data for generating trajectory.
     * \return                  List of points in circlic trajectory.
     * \code
	 * Enter command: c|3|1 2 3 4 5 6 10 2 0|10 20 30 40 50 60 10 2 0
	 * \endcode
	
    */
    std::vector<vasily::RobotData> CirclicMovement(
        const vasily::ParsedResult& parsedResult) const noexcept;
};

} // namespace danila

#endif // TRAJECTORY_MANAGER_H