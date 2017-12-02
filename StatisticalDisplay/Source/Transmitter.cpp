#include "Transmitter.h"


Transmitter::Transmitter(const int maxX, const int maxY)
	: _limitX(maxX)
	, _limitY(maxY)
	, _time(_limitX)
	, _coordSystem(sf::LineStrip, 4)
	, _deltaTime(sf::LineStrip, _limitX)
{
	_coordSystem.append(sf::Vertex(sf::Vector2f(1, 0), sf::Color::Red));
	_coordSystem.append(sf::Vertex(sf::Vector2f(1, 2 * static_cast<float>(_limitY)), sf::Color::Red));
	_coordSystem.append(sf::Vertex(sf::Vector2f(1, static_cast<float>(_limitY)), sf::Color::Red));
	_coordSystem.append(sf::Vertex(sf::Vector2f(static_cast<float>(_limitX), static_cast<float>(_limitY)), sf::Color::Red));
}

void Transmitter::updateVertices(const double time)
{
	_time.erase(_time.begin());
	_time.emplace_back(time);

	_deltaTime.clear();

	for (int i = 0; i < _limitX; ++i)
	{
		const float coordY = static_cast<float>(_limitY - _time.at(i) * 20.f) - 1.f;
		_deltaTime.append(sf::Vertex(sf::Vector2f(static_cast<float>(i), coordY), sf::Color::Blue));
	}
}

void Transmitter::draw(sf::RenderTarget& target, sf::RenderStates) const
{
	target.draw(_coordSystem);
	target.draw(_deltaTime);
}
