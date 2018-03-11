#include <algorithm>
#include <cassert>

#include "Handler.h"
#include "Utilities.h"


namespace vasily
{

Handler::Handler()
	: _state(State::DEFAULT),
	  _mode(Mode::COMMAND),
	  _coorninateSystem(CoordinateSystem::WORLD),
	  _coefficient(10'000),
	  _numberOfIterations(0),
	  _mapping({
		{ "q", State::FORWARD },		{ "a", State::BACK },
		{ "w", State::LEFT },			{ "s", State::RIGHT },
		{ "e", State::UP },				{ "d", State::DOWN },
		{ "r", State::ROLL_PLUS },		{ "f", State::ROLL_MINUS },
		{ "t", State::PITHCH_PLUS },	{ "g", State::PITHCH_MINUS },
		{ "y", State::YAW_PLUS },		{ "h", State::YAW_MINUS },
		{ "c", State::CIRCLIC },		{ "p", State::PARTIAL },
		{ "z", State::HOME },           { "x", State::FROM_FILE } })
{
}

bool Handler::checkChangingMode(const std::string_view letter)
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

bool Handler::checkChangingCoordinateSysytem(const std::string_view letter)
{
	if (const auto [value, check] = utils::parseCoordinateSystem(letter); check)
	{
		setCoordinateSystem(value);
		return true;
	}

	return false;
}

Handler::State Handler::parseCommand(const std::string_view command)
{
	_data = command;
	std::transform(_data.begin(), _data.end(), _data.begin(),
				   [](char c) { return static_cast<char>(::tolower(c)); });

	const std::string letter = _data.substr(0u, 1u);

	if (checkChangingCoordinateSysytem(letter))
	{
		return State::COORDINATE_TYPE;
	}

	if (const auto iter = _mapping.find(letter); iter != _mapping.end())
	{
		return iter->second;
	}

	if (!checkChangingMode(letter))
	{
		_printer.writeLine(std::cout, "ERROR 02: Incorrect input data!");
	}

	return State::DEFAULT;
}

ParsedResult Handler::parseDataAfterCommand()
{
	ParsedResult result;

	switch (_state)
	{
		case State::DEFAULT:
			if (_data == "=")
			{
				return result;
			}
			break;

		case State::COORDINATE_TYPE:
			if (_data.size() == 1u && utils::isCorrectNumber(_data))
			{
				return result;
			}
			break;

		case State::FULL_CONTROL:
			break;

		case State::CIRCLIC:
			[[fallthrough]];
		case State::PARTIAL:
			if (_data.size() > 1u && std::count(_data.begin(), _data.end(), '|') == 3)
			{
				const std::size_t findPosZero = _data.find('|');

				const std::size_t foundPosFirst = _data.find('|', findPosZero + 1u);
				std::string strToParse = _data.substr(findPosZero + 1u, 
													  foundPosFirst - findPosZero - 1u);
				bool flag1;
				result.firstPoint = utils::fromString<RobotData>(strToParse, flag1);

				const std::size_t foundPosSecond = _data.find('|', foundPosFirst + 1u);
				strToParse = _data.substr(foundPosFirst + 1u, foundPosSecond - foundPosFirst - 1u);
				bool flag2;
				result.secondPoint = utils::fromString<RobotData>(strToParse, flag2);

				strToParse = _data.substr(foundPosSecond + 1u);
				bool flag3;
				result.numberOfIterations = utils::fromString<int>(strToParse, flag3);

				if (flag1 && flag2 && flag3)
				{
					return result;
				}
			}
			break;

		case State::HOME:
			[[fallthrough]];
		case State::FROM_FILE:
			if (_data.size() == 1u)
			{
				return result;
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
			if (_data.size() > 1u)
			{
				if (const int coefficient = utils::stringToInt(_data.substr(1u)); coefficient > 0)
				{
					result.coefficient = coefficient;
					setCoefficient(coefficient);
					return result;
				}

				_state = State::DEFAULT;
			}
			else
			{
				return result;
			}
			break;

		default:
			assert(false);
			break;
	}

	_printer.writeLine(std::cout, "ERROR 01: Incorrect input data after literal!");
	result.isCorrect = false;
	return result;
}

void Handler::parseRawData(const std::string& data, RobotData& robotData)
{
	if (data.empty())
	{
		_state = State::DEFAULT;
		return;
	}

	std::string copiedData = data;

	const std::string_view letter = data.substr(0u, 1u);

	if (checkChangingMode(letter))
	{
		copiedData = data.substr(1u);
	}
	if (data.size() == 1u && checkChangingCoordinateSysytem(letter))
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
	if (command.empty())
	{
		_state = State::DEFAULT;
		return;
	}

	_state          = parseCommand(command);
	_parsedResult   = parseDataAfterCommand();

	if (!_parsedResult.isCorrect)
	{
		_state = State::DEFAULT;
	}

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

		case State::HOME:
			break;
		
		case State::FROM_FILE:
			break;

		case State::FORWARD:
			robotData.coordinates.at(X) += _coefficient;
			break;                    
									  
		case State::BACK:             
			robotData.coordinates.at(X) -= _coefficient;
			break;                    
									  
		case State::LEFT:             
			robotData.coordinates.at(Y) += _coefficient;
			break;                    
									  
		case State::RIGHT:            
			robotData.coordinates.at(Y) -= _coefficient;
			break;                    
									  
		case State::UP:               
			robotData.coordinates.at(Z) += _coefficient;
			break;                    
									  
		case State::DOWN:             
			robotData.coordinates.at(Z) -= _coefficient;
			break;                        
									  
		case State::ROLL_PLUS:        
			robotData.coordinates.at(W) += _coefficient;
			break;                    
									  
		case State::ROLL_MINUS:       
			robotData.coordinates.at(W) -= _coefficient;
			break;                    
									  
		case State::PITHCH_PLUS:      
			robotData.coordinates.at(P) += _coefficient;
			break;                    
									  
		case State::PITHCH_MINUS:     
			robotData.coordinates.at(P) -= _coefficient;
			break;                    
									  
		case State::YAW_PLUS:         
			robotData.coordinates.at(R) += _coefficient;
			break;                    
									  
		case State::YAW_MINUS:        
			robotData.coordinates.at(R) -= _coefficient;
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

CoordinateSystem Handler::getCoordinateSystem() const
{
	return _coorninateSystem;
}

void Handler::setCoordinateSystem(const CoordinateSystem coordninateSystem)
{
	_coorninateSystem = coordninateSystem;
}

int Handler::getNumberOfIterations() const
{
	return _numberOfIterations;
}

ParsedResult Handler::getParsedResult() const
{
	return _parsedResult;
}

} // namespace vasily
