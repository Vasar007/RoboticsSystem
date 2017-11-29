#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <vector>

#include <SFML/Graphics.hpp>

class Transmitter : public sf::Drawable
{
private:
	const int _limitX;
	const int _limitY;

	std::vector<double> _time;

	sf::VertexArray _coordSystem;

	sf::VertexArray _deltaTime;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
	Transmitter(const int maxX, const int maxY);

	void updateVertices(const double time);
};

#endif // TRANSMITTER_H
