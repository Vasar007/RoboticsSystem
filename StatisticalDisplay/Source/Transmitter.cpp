#include <iomanip>

#include "Transmitter.h"


namespace statistic
{

Transmitter::Transmitter(const int maxX, const int maxY)
    : _limitX(maxX),
      _limitY(maxY),
      _coordSystem(sf::Lines, 4),
      _deltaTime(sf::Points, _limitX),
      _lastReachedPoint{ { vasily::RobotData::DEFAULT_CORDINATES },
                         { vasily::RobotData::DEFAULT_PARAMETERS } },
      _shapes(sf::Lines, _limitX * _limitY / 100)
{
    _distance.reserve(_NUMBER_FOR_RESERVE);
    _velocity.reserve(_NUMBER_FOR_RESERVE);
    _time.reserve(_NUMBER_FOR_RESERVE);

    _coordSystem.append({ sf::Vector2f(1.0f, 1.0f), sf::Color::Red });
    _coordSystem.append({ sf::Vector2f(1.0f, static_cast<float>(_limitY)), sf::Color::Red });
    _coordSystem.append({ sf::Vector2f(1.0f, static_cast<float>(_limitY - 1)), sf::Color::Red });
    _coordSystem.append({ sf::Vector2f(static_cast<float>(_limitX + 1),
                                       static_cast<float>(_limitY - 1)), sf::Color::Red });

    for (int i = 1; i <= _limitX / 10; ++i)
    {
        _shapes.append({ sf::Vector2f(static_cast<float>(i * 10), static_cast<float>(_limitY - 6)),
                         sf::Color::Red });
        _shapes.append({ sf::Vector2f(static_cast<float>(i * 10), static_cast<float>(_limitY - 1)),
                         sf::Color::Red });
    }
    for (int i = 0; i < _limitY / 10; ++i)
    {
        _shapes.append({ sf::Vector2f(1.0f, static_cast<float>(i * 10)), sf::Color::Red });
        _shapes.append({ sf::Vector2f(6.0f, static_cast<float>(i * 10)), sf::Color::Red });
    }
}

void Transmitter::updateVertices(const double time, const vasily::RobotData& robotData)
{
    {
        const double distance = utils::distance(_lastReachedPoint.coordinates.begin(),
                                                _lastReachedPoint.coordinates.begin() + 2,
                                                robotData.coordinates.begin(), 0.0, 10'000.0);
        _distance.emplace_back(distance);
    }

    if (_velocity.empty())
    {
        _velocity.emplace_back(_distance.back() / time);
    }
    else
    {
        _velocity.emplace_back(_distance.back() / time);
    }

    _time.emplace_back(time);
    _lastReachedPoint = robotData;

    _circles.clear();
    _deltaTime.clear();
    for (std::size_t i = 0u; i < _velocity.size() && i < _distance.size(); ++i)
    {
        // Two types of velocity and distance placement.
        //const auto coordX = static_cast<float>(_velocity.at(i));
        //const auto coordY = static_cast<float>(_limitY - _distance.at(i)) - 1.0f;
        const auto coordX = static_cast<float>(_distance.at(i) * 20);
        const auto coordY = static_cast<float>(_limitY - _velocity.at(i) * 20) - 1.0f;
        _deltaTime.append({ sf::Vector2f(coordX, coordY), sf::Color::Blue });

        sf::CircleShape circle;
        circle.setRadius(2.0f);
        circle.setOutlineColor(sf::Color::Blue);
        circle.setOutlineThickness(2.0f);
        circle.setPosition(coordX, coordY);

        _circles.push_back(std::move(circle));
    }
    /// Command pack for tests:
    /// p|830000 -400000 539000 -180000 0 0 1 2 0|1320000 400000 960000 -180000 0 0 1 2 0|5
    /// 830000 -400000 539000 -180000 0 0 1 2 0
    /// 1320000 400000 960000 -180000 0 0 1 2 0
    /// p|985000 -400000 940000 -180000 0 0 1 2 0|985000 400000 940000 -180000 0 0 1 2 0|800
    /// p|985000 -400000 940000 -180000 0 0 1 2 0|985000 400000 940000 -180000 0 0 1 2 0|10
}

void Transmitter::clear()
{
    _distance.clear();
    _velocity.clear();
    _time.clear();

    _circles.clear();

    _deltaTime.clear();
}

void Transmitter::draw(sf::RenderTarget& target, sf::RenderStates) const
{
    target.draw(_coordSystem);
    target.draw(_shapes);

    target.draw(_deltaTime);

    for (const auto& circle : _circles)
    {
        target.draw(circle);
    }
}

} // namespace statistic
