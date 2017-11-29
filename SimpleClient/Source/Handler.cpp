#include <algorithm>
#include <cassert>

#include "Utilities.h"
#include "Handler.h"


namespace vasily
{

Handler::Handler()
	: _state(State::DEFAULT)
	, _mode(Mode::COMMAND)
	, _coorninateSystem(CoorninateSystem::WORLD)
	, _data()
	, _coefficient(10'000)
	, _numberOfIterations(0u)
	, _parsedResult()
	, _mapping({
		{ "q", State::FORWARD },		{ "a", State::BACK },
		{ "w", State::LEFT },			{ "s", State::RIGHT },
		{ "e", State::UP },				{ "d", State::DOWN },
		{ "r", State::ROLL_PLUS },		{ "f", State::ROLL_MINUS },
		{ "t", State::PITHCH_PLUS },	{ "g", State::PITHCH_MINUS },
		{ "y", State::YAW_PLUS },		{ "h", State::YAW_MINUS },
		{ "c", State::CIRCLIC },		{ "p", State::PARTIAL } })
{
}

bool Handler::checkChangingMode(const std::string& letter)
{
	if (letter == "=")
	{
		switch (_mode)
		{
			case Mode::READING:
				setMode(Mode::COMMAND);
				break;

			case Mode::COMMAND:
				setMode(Mode::READING);
				break;

			default:
				assert(false);
				break;
		}

		return true;
	}

	return false;
}

bool Handler::checkChangingCoordinateSysytem(const std::string& letter)
{
	if (const int type = utils::stringToInt(letter); type > 0)
	{
		switch (type)
		{
			case 1:
				setCoordinateSystem(CoorninateSystem::JOINT);
				break;

			case 2:
				setCoordinateSystem(CoorninateSystem::WORLD);
				break;

			default:
				utils::println("ERROR 01: Incorrect coordinate system!");
				break;
		}

		return true;
	}

	return false;
}

Handler::State Handler::parseCommand(const std::string_view command)
{
	_data = command;
	std::transform(_data.begin(), _data.end(), _data.begin(),
		[](char c) { return static_cast<char>(::tolower(c)); });

	const std::string letter = _data.substr(0, 1);

	const bool flag = checkChangingMode(letter);
	if (checkChangingCoordinateSysytem(letter))
	{
		return State::COORDINATE_TYPE;
	}

	if (const auto iter = _mapping.find(letter); iter != _mapping.end())
	{
		return iter->second;
	}

	if (!flag)
	{
		utils::println("ERROR 03: Incorrect input data!");
	}

	return State::DEFAULT;
}

ParsedResult Handler::parseDataAfterCommand()
{
	ParsedResult result;

	switch (_state)
	{
		case State::DEFAULT:
			break;

		case State::COORDINATE_TYPE:
			break;

		case State::FULL_CONTROL:
			break;

		case State::CIRCLIC:
			[[fallthrough]];
		case State::PARTIAL:
			if (_data.size() > 1u && std::count(_data.begin(), _data.end(), '|') == 2)
			{
				const std::size_t foundPosFirst = _data.find('|');
				std::string strToParse = _data.substr(1u, foundPosFirst - 1u);
				bool flag1;
				result.mFirstPoint = utils::fromString<RobotData>(strToParse, flag1);

				const std::size_t foundPosSecond = _data.find('|', foundPosFirst + 1u);
				strToParse = _data.substr(foundPosFirst + 1u,
					foundPosSecond - 1u);
				bool flag2;
				result.mSecondPoint = utils::fromString<RobotData>(strToParse,
					flag2);

				strToParse = _data.substr(foundPosSecond + 1u);
				bool flag3;
				result.mNumberOfIterations = utils::fromString<std::size_t>(strToParse, flag3);

				if (flag1 && flag2 && flag3)
				{
					return result;
				}
			}
			break;

		case State::FORWARD:
			[[fallthrough]];
		case State::BACK:
			[[fallthrough]];
		case State::LEFT:
			[[fallthrough]];
		case State::RIGHT:
			[[fallthrough]];
		case State::UP:
			[[fallthrough]];
		case State::DOWN:
			[[fallthrough]];
		case State::ROLL_PLUS:
			[[fallthrough]];
		case State::ROLL_MINUS:
			[[fallthrough]];
		case State::PITHCH_PLUS:
			[[fallthrough]];
		case State::PITHCH_MINUS:
			[[fallthrough]];
		case State::YAW_PLUS:
			[[fallthrough]];
		case State::YAW_MINUS:
			if (_data.size() > 1)
			{
				if (const int coefficient = utils::stringToInt(_data.substr(1)); coefficient > 0)
				{
					result.mCoefficient = coefficient;
					setCoefficient(coefficient);
					return result;
				}

				_state = State::DEFAULT;
			}
			break;

		default:
			assert(false);
			break;
	}

	utils::println("ERROR 02: Incorrect input data after literal!");
	return result;
}

void Handler::parseRawData(const std::string& data, RobotData& robotData)
{
	std::string copiedData = data;

	const std::string letter = data.substr(0u, 1u);

	if (checkChangingMode(letter))
	{
		copiedData = data.substr(1u);
	}
	if (checkChangingCoordinateSysytem(letter) && data.size() == 1u)
	{
		_state = State::COORDINATE_TYPE;
		return;
	}

	bool flag;
	robotData = utils::fromString<RobotData>(copiedData, flag);

	_state = flag ? State::FULL_CONTROL : State::DEFAULT;
}

void Handler::appendCommand(const std::string_view command, RobotData& robotData)
{
	_state = parseCommand(command);

	_parsedResult = parseDataAfterCommand();

	switch (_state)
	{
		case State::DEFAULT:
			break;

		case State::COORDINATE_TYPE:
			break;

		case State::CIRCLIC:
			break;

		case State::PARTIAL:
			break;

		case State::FORWARD:
			robotData.mCoordinates.at(X) += _coefficient;
			break;                    
									  
		case State::BACK:             
			robotData.mCoordinates.at(X) -= _coefficient;
			break;                    
									  
		case State::LEFT:             
			robotData.mCoordinates.at(Y) += _coefficient;
			break;                    
									  
		case State::RIGHT:            
			robotData.mCoordinates.at(Y) -= _coefficient;
			break;                    
									  
		case State::UP:               
			robotData.mCoordinates.at(Z) += _coefficient;
			break;                    
									  
		case State::DOWN:             
			robotData.mCoordinates.at(Z) -= _coefficient;
			break;                        
									  
		case State::ROLL_PLUS:        
			robotData.mCoordinates.at(W) += _coefficient;
			break;                    
									  
		case State::ROLL_MINUS:       
			robotData.mCoordinates.at(W) -= _coefficient;
			break;                    
									  
		case State::PITHCH_PLUS:      
			robotData.mCoordinates.at(P) += _coefficient;
			break;                    
									  
		case State::PITHCH_MINUS:     
			robotData.mCoordinates.at(P) -= _coefficient;
			break;                    
									  
		case State::YAW_PLUS:         
			robotData.mCoordinates.at(R) += _coefficient;
			break;                    
									  
		case State::YAW_MINUS:        
			robotData.mCoordinates.at(R) -= _coefficient;
			break;

		default:
			assert(false);
			break;
	}
}

int Handler::getCoefficient() const
{
	return _coefficient;
}

void Handler::setCoefficient(const int coefficient)
{
	_coefficient = coefficient;
}

Handler::State Handler::getCurrentState() const
{
	return _state;
}

Handler::Mode Handler::getCurrentMode() const
{
	return _mode;
}

void Handler::setMode(const Mode mode)
{
	_mode = mode;
}

Handler::CoorninateSystem Handler::getCoordinateSystem() const
{
	return _coorninateSystem;
}

void Handler::setCoordinateSystem(const CoorninateSystem coorninateType)
{
	_coorninateSystem = coorninateType;
}

int Handler::getNumberOfIterations() const
{
	return _numberOfIterations;
}

ParsedResult Handler::getParsedResult() const
{
	return _parsedResult;
}

}
