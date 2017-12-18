#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <vector>

#include <SFML/Graphics.hpp>

#include "Utilities.h"


/**
 * \brief Namespace that contains statistical tools.
 */
namespace statistic
{
	
/**
 * \brief Class draw dependencies from client data.
 */
class Transmitter : public sf::Drawable
{
private:
	/**
	 * \brief Max width to draw.
	 */
	const int _limitX;

	/**
	 * \brief Max height to draw.
	 */
	const int _limitY;

	/**
	 * \brief Container that keeps distance between every two last points.
	 */
	std::vector<double> _distance;

	/**
	 * \brief Container that keeps velocity of robot movement.
	 */
	std::vector<double> _velocity;

	/**
	* \brief Container that keeps duration of robot movement.
	*/
	std::vector<double> _time;

	/**
	 * \brief Array which contains coordinate axis.
	 */
	sf::VertexArray     _coordSystem;

	/**
	 * \brief Array which contains vertex that depends on duration time.
	 */
	sf::VertexArray     _deltaTime;

	/**
	 * \brief Keep last reached robot's point.
	 */
	vasily::RobotData   _lastReachedPoint;

	/**
	 * \brief Circles graphically represent the dependence on the chart.
	 */
	std::vector<sf::CircleShape> _circles;

	/**
	 * \brief The marks on the coordinate axes.
	 */
	sf::VertexArray _shapes;


	/**
	 * \brief               Draw coordinate axis and a jagged curve.
	 * \param[out] target   Window to draw.
	 * \param[in] states    Additional drawing parameteres.
	 */
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;


public:
	/**
	 * \brief           Constructor.
	 * \param[in] maxX  Window width.
	 * \param[in] maxY  Window height.
	 */
	Transmitter(const int maxX, const int maxY);

	/**
	 * \brief               Update vertices depend on sent time.
	 * \param[in] time      Current duration time.
	 * \param[in] robotData Current received data.
	 */
	void updateVertices(const double time, const vasily::RobotData& robotData);

	/**
	 * \brief Cleat all containers.
	 */
	void clear();
};

}

#endif // TRANSMITTER_H
