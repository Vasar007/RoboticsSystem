#ifndef ROBOT_DATA_H
#define ROBOT_DATA_H

#include <array>
#include <cassert>


namespace vasily
{

/**
 * \brief Array of coordinates type.
 */
enum class CoordinateSystem
{
    JOINT = 0,
    JGFRM = 1,
    WORLD = 2,

    INVALID = 3
};


/**
 * \brief Simple structure which contains values and parameteres to exchanging.
 */
struct RobotData
{
    /**
     * \brief Constant number of values to avoid "magic number".
     */
    static constexpr std::size_t NUMBER_OF_COORDINATES	= 6u;
    
    /**
     * \brief Constant number of parameters to avoid "magic number".
     */
    static constexpr std::size_t NUMBER_OF_PARAMETERS	= 3u;
    
    /**
     * \brief Constant number to calculate real distance and to avoid "magic number".
     */
    static constexpr double TRANSFER_NUMBER	= 10'000.0;

    /**
     * \brief Default coordinates.
     */
    static constexpr std::array<int, NUMBER_OF_COORDINATES>	DEFAULT_CORDINATES
                                                            { 985'000, 0, 940'000, -180'000, 0, 0 };

    /**
     * \brief Default robot parameters.
     */
    static constexpr std::array<int, NUMBER_OF_PARAMETERS>	DEFAULT_PARAMETERS
                                                            { 1, 2, 0 };

    /**
     * \brief Array which contains only coordinates or degrees.
     */
    std::array<int, NUMBER_OF_COORDINATES>	coordinates;

    /**
     * \brief	Array which contains some parameters.
     * \details	Segtime (!= 0), type of moving(0 - FINE/ 2 - NOVAR+NODECEL), start/stop (0/1)
     */
    std::array<int, NUMBER_OF_PARAMETERS>	parameters;


    /**
     * \brief                     Constructor for creating structure from two arrays.
     * \param[in] initCoordinates Array that contains coordinates.
     * \param[in] initParameters  Array that contains parameters.
     */
    constexpr RobotData(
        const std::array<int, NUMBER_OF_COORDINATES>& initCoordinates = DEFAULT_CORDINATES,
        const std::array<int, NUMBER_OF_PARAMETERS>& initParameters = DEFAULT_PARAMETERS) noexcept
        : coordinates(initCoordinates),
          parameters(initParameters)
    {
    }

    /**
     * \brief				  Constructor for creating structure from 9 numbers.
     * \param[in] x			  1st coordinate.
     * \param[in] y			  2nd coordinate.
     * \param[in] z			  3rd coordinate.
     * \param[in] w			  4th coordinate.
     * \param[in] p			  5th coordinate.
     * \param[in] r			  6th coordinate.
     * \param[in] segtime	  Segtime.
     * \param[in] movingType  Type of moving: Nodecel or Fine.
     * \param[in] lastCommand Special command for robot.
     */
    constexpr RobotData(const int x, const int y, const int z, const int w, const int p, 
                        const int r, const int segtime, const int movingType, 
                        const int lastCommand) noexcept
        : coordinates{ x, y, z, w, p, r },
          parameters{ segtime, movingType, lastCommand }
    {
    }

    /**
     * \brief Default destructor.
     */
                ~RobotData() noexcept                          = default;

    /**
     * \brief			Default copy constructor.
     * \param[in] other Other object.
     */
    constexpr RobotData(const RobotData& other)                = default;

    /**
     * \brief			Default copy assignment operator.
     * \param[in] other Other object.
     * \return			Return copied structure.
     */
    constexpr RobotData& operator=(const RobotData& other)     = default;

    /**
     * \brief			 Default move constructor.
     * \param[out] other Other object.
     */
    constexpr RobotData(RobotData&& other) noexcept            = default;

    /**
     * \brief			 Default move assignment operator.
     * \param[out] other Other object.
     * \return			 Return moved structure.
     */
    constexpr RobotData& operator=(RobotData&& other) noexcept = default;


    /**
     * \brief	Convert object to string.
     * \return	String which contains all data of this object.
     */
    std::string toString() const;

    /**
     * \brief	Calculate length of vector which contains 6 coordinates.
     * \detail  Fucntion uses to calculate length in centimeters!
     * \return	Square root of sum of coordinates squares.
     */
    double length() const;

    /**
     * \brief			Check two structures on full equality.
     * \param[in] other Other object to compare.
     * \return			True if structures are equal, false otherwise.
     */
    constexpr bool isEqual(const RobotData& other) const noexcept
    {
        for (std::size_t i = 0u; i < NUMBER_OF_PARAMETERS; ++i)
        {
            if (this->parameters.at(i) != other.parameters.at(i))
            {
                return false;
            }
        }
        return *this == other;
    }

    /**
     * \brief Reset coordinates and parameters values to default.
     */
    constexpr void returnToDefault() noexcept
    {
        coordinates = DEFAULT_CORDINATES;
        parameters  = DEFAULT_PARAMETERS;
    }


    /**
     * \brief  Create object with default coordinates and parameters.
     * \return Object with default values.
     */
    static constexpr RobotData getDefaultPosition() noexcept
    {
        return { DEFAULT_CORDINATES, DEFAULT_PARAMETERS };
    }

    /**
     * \brief		  Check coordinate arrays of two structures on equality.
     * \param[in] lhs Left-hand side object.
     * \param[in] rhs Right-hand side object.
     * \return		  True if coordinates are equal, false otherwise.
     */
    friend constexpr bool operator==(const RobotData& lhs, const RobotData& rhs) noexcept
    {
        for (std::size_t i = 0u; i < RobotData::NUMBER_OF_COORDINATES; ++i)
        {
            if (lhs.coordinates.at(i) != rhs.coordinates.at(i))
            {
                return false;
            }
        }
        return true;
    }
                
    /**
     * \brief		  Check coordinate arrays of two structures on equality.
     * \param[in] lhs Left-hand side object.
     * \param[in] rhs Right-hand side object.
     * \return		  True if coordinates are not equal, false otherwise.
     */
    friend constexpr bool operator!=(const RobotData& lhs, const RobotData& rhs) noexcept
    {
        return !(lhs == rhs);
    }
                
    /**
     * \brief         Compare coordinate arrays of two structures.
     * \param[in] lhs Left-hand side object.
     * \param[in] rhs Right-hand side object.
     * \return        True if lhs object is less than rhs object, false otherwise.
     */
    friend constexpr bool operator<(const RobotData& lhs, const RobotData& rhs) noexcept
    {
        for (std::size_t i = 0u; i < RobotData::NUMBER_OF_COORDINATES; ++i)
        {
            if (lhs.coordinates.at(i) > rhs.coordinates.at(i))
            {
                return false;
            }
        }
        return !(lhs == rhs);
    }
                
    /**
     * \brief         Compare coordinate arrays of two structures.
     * \param[in] lhs Left-hand side object.
     * \param[in] rhs Right-hand side object.
     * \return        True if lhs object is greater than rhs object, false otherwise.
     */
    friend constexpr bool operator>(const RobotData& lhs, const RobotData& rhs) noexcept
    {
        return !(lhs < rhs) && !(lhs == rhs);
    }

    /**
     * \brief         Sum coordinate of lhs and rhs objects.
     * \param[in] lhs Left-hand side object.
     * \param[in] rhs Right-hand side object.
     * \return        Result of sum puts in lhs object.
     */
    friend constexpr RobotData& operator+=(RobotData& lhs, const RobotData& rhs) noexcept
    {
        for (std::size_t i = 0u; i < RobotData::NUMBER_OF_COORDINATES; ++i)
        {
            lhs.coordinates.at(i) += rhs.coordinates.at(i);
        }

        return lhs;
    }

    /**
     * \brief         Subtract coordinate of lhs and rhs objects.
     * \param[in] lhs Left-hand side object.
     * \param[in] rhs Right-hand side object.
     * \return        Result of subtract puts in lhs object.
     */
    friend constexpr RobotData& operator-=(RobotData& lhs, const RobotData& rhs) noexcept
    {
        for (std::size_t i = 0u; i < RobotData::NUMBER_OF_COORDINATES; ++i)
        {
            lhs.coordinates.at(i) -= rhs.coordinates.at(i);
        }

        return lhs;
    }

    /**
     * \brief         Multiply coordinate of lhs and rhs objects.
     * \param[in] lhs Left-hand side object.
     * \param[in] rhs Right-hand side object.
     * \return        Result of multiplication puts in lhs object.
     */
    friend constexpr RobotData& operator*=(RobotData& lhs, const RobotData& rhs) noexcept
    {
        for (std::size_t i = 0u; i < RobotData::NUMBER_OF_COORDINATES; ++i)
        {
            lhs.coordinates.at(i) *= rhs.coordinates.at(i);
        }

        return lhs;
    }

    /**
     * \brief         Divide coordinate of lhs and rhs objects.
     * \param[in] lhs Left-hand side object.
     * \param[in] rhs Right-hand side object.
     * \return        Result of division puts in lhs object.
     */
    friend constexpr RobotData& operator/=(RobotData& lhs, const RobotData& rhs) noexcept
    {
        for (std::size_t i = 0u; i < RobotData::NUMBER_OF_COORDINATES; ++i)
        {
            lhs.coordinates.at(i) /= rhs.coordinates.at(i);
        }

        return lhs;
    }

    /**
     * \brief     Sum coordinate of lhs and rhs objects.
     * \tparam T  Number type for sum.
     * \param lhs Left-hand side object.
     * \param rhs Right-hand side object.
     * \return    Result of sum puts in lhs object.
     */
    template <class T>
    friend constexpr std::enable_if_t<std::is_arithmetic_v<T>, RobotData&>
        operator+=(RobotData& lhs, const T& rhs)
    {
        for (std::size_t i = 0u; i < RobotData::NUMBER_OF_COORDINATES; ++i)
        {
            lhs.coordinates.at(i) = static_cast<int>(lhs.coordinates.at(i) + rhs);
        }

        return lhs;
    }

    /**
     * \brief     Subtract coordinate of lhs and rhs objects.
     * \tparam T  Number type for sustract.
     * \param lhs Left-hand side object.
     * \param rhs Right-hand side object.
     * \return    Result of sustract puts in lhs object.
     */
    template <class T>
    friend constexpr std::enable_if_t<std::is_arithmetic_v<T>, RobotData&>
        operator-=(RobotData& lhs, const T& rhs)
    {
        for (std::size_t i = 0u; i < RobotData::NUMBER_OF_COORDINATES; ++i)
        {
            lhs.coordinates.at(i) = static_cast<int>(lhs.coordinates.at(i) - rhs);
        }

        return lhs;
    }

    /**
     * \brief     Multiply coordinate of lhs and rhs objects.
     * \tparam T  Number type for multiplication.
     * \param lhs Left-hand side object.
     * \param rhs Right-hand side object.
     * \return    Result of division puts in lhs object.
     */
    template <class T>
    friend constexpr std::enable_if_t<std::is_arithmetic_v<T>, RobotData&>
        operator*=(RobotData& lhs, const T& rhs)
    {
        for (std::size_t i = 0u; i < RobotData::NUMBER_OF_COORDINATES; ++i)
        {
            lhs.coordinates.at(i) = static_cast<int>(lhs.coordinates.at(i) * rhs);
        }

        return lhs;
    }

    /**
     * \brief         Divide coordinate of lhs and rhs objects.
     * \tparam T      Number type for division.
     * \param[in] lhs Left-hand side object.
     * \param[in] rhs Right-hand side object.
     * \return        Result of division puts in lhs object.
     */
    template <class T>
    friend constexpr std::enable_if_t<std::is_arithmetic_v<T>, RobotData&>
        operator/=(RobotData& lhs, const T& rhs)
    {
        assert(rhs != 0);

        for (std::size_t i = 0u; i < RobotData::NUMBER_OF_COORDINATES; ++i)
        {
            if (lhs.coordinates.at(i) == 0)
            {
                continue;
            }

            lhs.coordinates.at(i) = static_cast<int>(lhs.coordinates.at(i) / rhs);
            if (lhs.coordinates.at(i) == 0)
            {
                lhs.coordinates.at(i) = 1;
            }
        }

        return lhs;
    }

    /**
     * \brief         Sum coordinate of lhs and rhs objects.
     * \param[in] lhs Left-hand side object.
     * \param[in] rhs Right-hand side object.
     * \return        Result of sum.
     */
    friend constexpr RobotData& operator+(const RobotData& lhs, const RobotData& rhs) noexcept
    {
        RobotData result = lhs;

        return result += rhs;
    }

    /**
     * \brief         Subtract coordinate rhs object from lhs object.
     * \param[in] lhs Left-hand side object.
     * \param[in] rhs Right-hand side object.
     * \return        Result of sustraction.
     */
    friend constexpr RobotData& operator-(const RobotData& lhs, const RobotData& rhs) noexcept
    {
        RobotData result = lhs;

        return result -= rhs;
    }

    /**
     * \brief         Multiply coordinate rhs object from lhs object.
     * \param[in] lhs Left-hand side object.
     * \param[in] rhs Right-hand side object.
     * \return        Result of multiplication.
     */
    friend constexpr RobotData& operator*(const RobotData& lhs, const RobotData& rhs) noexcept
    {
        RobotData result = lhs;

        return result *= rhs;
    }

    /**
     * \brief         Divide coordinate rhs object from lhs object.
     * \param[in] lhs Left-hand side object.
     * \param[in] rhs Right-hand side object.
     * \return        Result of division.
     */
    friend constexpr RobotData& operator/(const RobotData& lhs, const RobotData& rhs) noexcept
    {
        RobotData result = lhs;

        return result /= rhs;
    }

    /**
     * \brief         Sum coordinate of lhs and rhs objects.
     * \param[in] lhs Left-hand side object.
     * \param[in] rhs Right-hand side object.
     * \return        Result of sum.
     */
    template <class T>
    friend constexpr std::enable_if_t<std::is_arithmetic_v<T>, RobotData&>
        operator+(const RobotData& lhs, const T& rhs) noexcept
    {
        RobotData result = lhs;

        return result += rhs;
    }

    /**
     * \brief         Subtract coordinate rhs object from lhs object.
     * \param[in] lhs Left-hand side object.
     * \param[in] rhs Right-hand side object.
     * \return        Result of sustraction.
     */
    template <class T>
    friend constexpr std::enable_if_t<std::is_arithmetic_v<T>, RobotData&>
        operator-(const RobotData& lhs, const T& rhs) noexcept
    {
        RobotData result = lhs;

        return result -= rhs;
    }

    /**
    * \brief         Multiply coordinate of lhs and rhs objects.
    * \tparam T      Number type for multiplication.
    * \param[in] lhs Left-hand side object.
    * \param[in] rhs Right-hand side object.
    * \return        Result of multiplication.
    */
    template <class T>
    friend constexpr std::enable_if_t<std::is_arithmetic_v<T>, RobotData&>
        operator*(const RobotData& lhs, const T& rhs)
    {
        RobotData result = lhs;

        return result *= rhs;
    }

    /**
     * \brief         Divide coordinate of lhs and rhs objects.
     * \tparam T      Number type for division.
     * \param[in] lhs Left-hand side object.
     * \param[in] rhs Right-hand side object.
     * \return        Result of division.
     */
    template <class T>
    friend constexpr std::enable_if_t<std::is_arithmetic_v<T>, RobotData&>
        operator/(const RobotData& lhs, const T& rhs)
    {
        assert(rhs != 0);

        RobotData result = lhs;

        return result /= rhs;
    }

    /**
     * \brief               Operator overloading for istream.
     * \param[in] in        Refrence to the original istream.
     * \param[in] robotData Object for records.
     * \return              A reference to the original istream object.
     */
    friend std::istream& operator>>(std::istream& in, RobotData& robotData);

    /**
     * \brief               Operator overloading for ostream.
     * \param[in] out       Refrence to the original ostream.
     * \param[in] robotData Object for output.
     * \return              A reference to the original ostream object.
     */
    friend std::ostream& operator<<(std::ostream& out, const RobotData& robotData);

    /**
     * \brief               Operator overloading for wostream.
     * \param[in] out       Refrence to the original wostream.
     * \param[in] robotData Object for output.
     * \return              A reference to the original wostream object.
     */
    friend std::wostream& operator<<(std::wostream& out, const RobotData& robotData);
};

} // namespace vasily

#endif // ROBOT_DATA_H
