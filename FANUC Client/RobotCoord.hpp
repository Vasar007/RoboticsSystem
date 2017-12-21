#pragma once

#ifndef CONTIENERS

#define CONTIENERS

#include<string>
#include<algorithm>

// структура общения с Robot FANUC
struct RobotCoord 
{
	int _xr, _yr, _zr, _uw, _up, _uz, _segTime, _typeOfMoving, _control;
	RobotCoord()
	{
		_xr = _yr = _zr = _uw = _up = _uz = _segTime = _typeOfMoving = _control = -1;
	}

	// преобразуют структуру в строку
	std::string toString() const
	{
		char locBuf[128];
		std::fill(locBuf, locBuf + 128, 0);
		sprintf_s(locBuf, "%d %d %d %d %d %d %d %d %d ", _xr, _yr, _zr, _uw, _up, _uz, _segTime, _typeOfMoving, _control);
		return static_cast<std::string>(locBuf);
	}

	// рассчет модуля вектора между кординатами
	double difference(const RobotCoord& rc) const
	{
		return sqrt((_xr - rc._xr)*(_xr - rc._xr) +
			(_yr - rc._yr)*(_yr - rc._yr) +
			(_zr - rc._zr)*(_zr - rc._zr) +
			(_uw - rc._uw)*(_uw - rc._uw) +
			(_up - rc._up)*(_up - rc._up) +
			(_uz - rc._uz)*(_uz - rc._uz));
	}
};

inline bool operator!=(const RobotCoord& rc, const RobotCoord& rc2)
{
	return rc._control != rc2._control ||
		rc._segTime != rc2._segTime ||
		rc._typeOfMoving != rc2._typeOfMoving ||
		rc._up != rc2._up ||
		rc._uw != rc2._uw ||
		rc._uz != rc2._uz ||
		rc._xr != rc2._xr ||
		rc._yr != rc2._yr ||
		rc._zr != rc2._zr;
}

inline std::istream& operator >> (std::istream& in, RobotCoord& rc)
{
	in >> rc._xr >> rc._yr >> rc._zr >> rc._uw >> rc._up >> rc._uz >> rc._segTime >> rc._typeOfMoving >> rc._control;
	return in;
}


#endif //  !CONTIENERS

