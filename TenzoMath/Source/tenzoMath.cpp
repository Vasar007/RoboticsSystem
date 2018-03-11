#include <fstream>
#include <string>
#include <iostream>

#include "TenzoMath.h"


namespace nikita
{

TenzoMath::TenzoMath()
    : _positions({
        {
            { 0, 0, 0, 0, -90,   0 },
            { 0, 0, 0, 0,   0,   0 },
            { 0, 0, 0, 0,   0, 180 },
            { 0, 0, 0, 0,  90, 180 },
            { 0, 0, 0, 0,   0, 270 },
            { 0, 0, 0, 0,   0,  90 },
        }}),
      _tenzoData(L"COM13")
{
    _g = cv::Mat(3, 1, cv::DataType<double>::type);
    _g.at<double>(0, 0) = 0.;
    _g.at<double>(1, 0) = 0.;
    _g.at<double>(2, 0) = -1;

    _fgmax = cv::Mat(3, 3, cv::DataType<double>::type);

    _tmax = cv::Mat(3, 3, cv::DataType<double>::type);

    _tmaxNeg = cv::Mat(3, 3, cv::DataType<double>::type);

    _fgmaxNeg = cv::Mat(3, 3, cv::DataType<double>::type);

    _collectedData = cv::Mat(6, 6, cv::DataType<double>::type);
}

std::array<double, 6> TenzoMath::swapData(const std::array<double, 6>& data) const
{
    return std::array<double, 6>{ data[1], -data[0], -data[2], -data[4], data[3], data[5] };
}

std::array<int, 6> TenzoMath::convertToInt(const std::array<double, 6>& coord)
{
    std::array<int, 6> convert{};
    for (std::size_t i = 0; i < coord.size(); ++i)
    {
        convert.at(i) = static_cast<int>(coord.at(i) * 1000.0);
    }
    return convert;
}

std::array<double, 6> TenzoMath::convertToDouble(const std::array<int, 6>& coord)
{
    std::array<double, 6> convert{};
    for (std::size_t i = 0; i < coord.size(); ++i)
    {
        convert.at(i) = static_cast<double>(coord.at(i)) / 1000.0;
    }
    return convert;
}

std::string TenzoMath::toString(const std::array<double, 6>& coord) const
{
    std::array<int, 6> convert = convertToInt(coord);

    std::stringstream stringStream;

    std::copy(convert.begin(), convert.end(), std::ostream_iterator<int>(stringStream, " "));

    return stringStream.str();
}

void TenzoMath::collectData(const std::size_t index)
{
    std::cin.get();

    std::array<double, 6> tmp = swapData(_tenzoData.readComStrain());
    for (std::size_t i = 0; i < 6; ++i)
    {
        std::cout << tmp[i] << ' ';
        _collectedData.at<double>(static_cast<int>(index), static_cast<int>(i)) = tmp[i];
    }
    std::cout << '\n';
}

std::array<int, 6> TenzoMath::getPosition(const std::size_t index) const
{
    return convertToInt(_positions.at(index));
    //return _posCartesian.at(index);
}

void TenzoMath::doCalibration()
{
    _forcesBias[0] = (_collectedData.at<double>(1, 0) + _collectedData.at<double>(2, 0)) / 2.0;
    _forcesBias[1] = (_collectedData.at<double>(4, 1) + _collectedData.at<double>(5, 1)) / 2.0;
    _forcesBias[2] = (_collectedData.at<double>(0, 2) + _collectedData.at<double>(3, 2)) / 2.0;

    _torquesBias[0] = (_collectedData.at<double>(1, 3) + _collectedData.at<double>(2, 3)) / 2.0;
    _torquesBias[1] = (_collectedData.at<double>(4, 4) + _collectedData.at<double>(5, 4)) / 2.0;
    _torquesBias[2] = (_collectedData.at<double>(0, 5) + _collectedData.at<double>(3, 5)) / 2.0;

    _fgmax.at<double>(0, 0) = _collectedData.at<double>(2, 0) - _forcesBias[0];
    _fgmax.at<double>(0, 1) = _collectedData.at<double>(2, 1) - _forcesBias[1];
    _fgmax.at<double>(0, 2) = _collectedData.at<double>(2, 2) - _forcesBias[2];

    _fgmax.at<double>(1, 0) = _collectedData.at<double>(4, 0) - _forcesBias[0];
    _fgmax.at<double>(1, 1) = _collectedData.at<double>(4, 1) - _forcesBias[1];
    _fgmax.at<double>(1, 2) = _collectedData.at<double>(4, 2) - _forcesBias[2];

    _fgmax.at<double>(2, 0) = _collectedData.at<double>(0, 0) - _forcesBias[0];
    _fgmax.at<double>(2, 1) = _collectedData.at<double>(0, 1) - _forcesBias[1];
    _fgmax.at<double>(2, 2) = _collectedData.at<double>(0, 2) - _forcesBias[2];

    _tmax.at<double>(0, 0) = _collectedData.at<double>(2, 3) - _torquesBias[0];
    _tmax.at<double>(0, 1) = _collectedData.at<double>(2, 4) - _torquesBias[1];
    _tmax.at<double>(0, 2) = _collectedData.at<double>(2, 5) - _torquesBias[2];

    _tmax.at<double>(1, 0) = _collectedData.at<double>(4, 3) - _torquesBias[0];
    _tmax.at<double>(1, 1) = _collectedData.at<double>(4, 4) - _torquesBias[1];
    _tmax.at<double>(1, 2) = _collectedData.at<double>(4, 5) - _torquesBias[2];

    _tmax.at<double>(2, 0) = _collectedData.at<double>(0, 3) - _torquesBias[0];
    _tmax.at<double>(2, 1) = _collectedData.at<double>(0, 4) - _torquesBias[1];
    _tmax.at<double>(2, 2) = _collectedData.at<double>(0, 5) - _torquesBias[2];

    _fgmaxNeg.at<double>(0, 0) = _collectedData.at<double>(1, 0) - _forcesBias[0];
    _fgmaxNeg.at<double>(0, 1) = _collectedData.at<double>(1, 1) - _forcesBias[1];
    _fgmaxNeg.at<double>(0, 2) = _collectedData.at<double>(1, 2) - _forcesBias[2];

    _fgmaxNeg.at<double>(1, 0) = _collectedData.at<double>(5, 0) - _forcesBias[0];
    _fgmaxNeg.at<double>(1, 1) = _collectedData.at<double>(5, 1) - _forcesBias[1];
    _fgmaxNeg.at<double>(1, 2) = _collectedData.at<double>(5, 2) - _forcesBias[2];

    _fgmaxNeg.at<double>(2, 0) = _collectedData.at<double>(3, 0) - _forcesBias[0];
    _fgmaxNeg.at<double>(2, 1) = _collectedData.at<double>(3, 1) - _forcesBias[1];
    _fgmaxNeg.at<double>(2, 2) = _collectedData.at<double>(3, 2) - _forcesBias[2];

    _tmaxNeg.at<double>(0, 0) = _collectedData.at<double>(1, 3) - _torquesBias[0];
    _tmaxNeg.at<double>(0, 1) = _collectedData.at<double>(1, 4) - _torquesBias[1];
    _tmaxNeg.at<double>(0, 2) = _collectedData.at<double>(1, 5) - _torquesBias[2];

    _tmaxNeg.at<double>(1, 0) = _collectedData.at<double>(5, 3) - _torquesBias[0];
    _tmaxNeg.at<double>(1, 1) = _collectedData.at<double>(5, 4) - _torquesBias[1];
    _tmaxNeg.at<double>(1, 2) = _collectedData.at<double>(5, 5) - _torquesBias[2];

    _tmaxNeg.at<double>(2, 0) = _collectedData.at<double>(3, 3) - _torquesBias[0];
    _tmaxNeg.at<double>(2, 1) = _collectedData.at<double>(3, 4) - _torquesBias[1];
    _tmaxNeg.at<double>(2, 2) = _collectedData.at<double>(3, 5) - _torquesBias[2];

    std::ofstream out("calibData.txt");
    if (!out)
    {
        std::cout << "file is not opened\n";
    }

    out << _forcesBias[0] << ' ' << _forcesBias[1] << ' ' << _forcesBias[2] << ' ' << _torquesBias[0] << ' '
        << _torquesBias[1] << ' ' << _torquesBias[2] << '\n';

    std::cout << _forcesBias[0] << ' ' << _forcesBias[1] << ' ' << _forcesBias[2] << ' ' << _torquesBias[0] << ' '
        << _torquesBias[1] << ' ' << _torquesBias[2] << '\n';

    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            out << _fgmax.at<double>(i, j) << ' ';
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            out << _fgmaxNeg.at<double>(i, j) << ' ';
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            out << _tmax.at<double>(i, j) << ' ';
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            out << _tmaxNeg.at<double>(i, j) << ' ';
    out.close();

}

std::array<double, 6> TenzoMath::gravCompensation(const cv::Mat& p6, std::array<double, 6>& rawData)
{
    cv::Mat fgmaxCurr(3, 3, CV_64F);
    cv::Mat tmaxCurr(3, 3, CV_64F);
    std::array<double, 6> newData{};

    cv::Mat currRot(3, 3, cv::DataType<double>::type);
    p6(cv::Rect(0, 0, 3, 3)).copyTo(currRot);
    cv::Mat gravProjection = currRot.t() * _g;

    if (gravProjection.at<double>(0, 0) > 0)
    {
        fgmaxCurr.at<double>(0, 0) = _fgmax.at<double>(0, 0) * gravProjection.at<double>(0, 0);
        fgmaxCurr.at<double>(0, 1) = _fgmax.at<double>(0, 1) * gravProjection.at<double>(0, 0);
        fgmaxCurr.at<double>(0, 2) = _fgmax.at<double>(0, 2) * gravProjection.at<double>(0, 0);

        tmaxCurr.at<double>(0, 0) = _tmax.at<double>(0, 0) * gravProjection.at<double>(0, 0);
        tmaxCurr.at<double>(0, 1) = _tmax.at<double>(0, 1) * gravProjection.at<double>(0, 0);
        tmaxCurr.at<double>(0, 2) = _tmax.at<double>(0, 2) * gravProjection.at<double>(0, 0);
    }
    else
    {
        fgmaxCurr.at<double>(0, 0) = -_fgmaxNeg.at<double>(0, 0) * gravProjection.at<double>(0, 0);
        fgmaxCurr.at<double>(0, 1) = -_fgmaxNeg.at<double>(0, 1) * gravProjection.at<double>(0, 0);
        fgmaxCurr.at<double>(0, 2) = -_fgmaxNeg.at<double>(0, 2) * gravProjection.at<double>(0, 0);

        tmaxCurr.at<double>(0, 0) = -_tmaxNeg.at<double>(0, 0) * gravProjection.at<double>(0, 0);
        tmaxCurr.at<double>(0, 1) = -_tmaxNeg.at<double>(0, 1) * gravProjection.at<double>(0, 0);
        tmaxCurr.at<double>(0, 2) = -_tmaxNeg.at<double>(0, 2) * gravProjection.at<double>(0, 0);
    }

    if (gravProjection.at<double>(1, 0) > 0)
    {
        fgmaxCurr.at<double>(1, 0) = _fgmax.at<double>(1, 0) * gravProjection.at<double>(1, 0);
        fgmaxCurr.at<double>(1, 1) = _fgmax.at<double>(1, 1) * gravProjection.at<double>(1, 0);
        fgmaxCurr.at<double>(1, 2) = _fgmax.at<double>(1, 2) * gravProjection.at<double>(1, 0);

        tmaxCurr.at<double>(1, 0) = _tmax.at<double>(1, 0) * gravProjection.at<double>(1, 0);
        tmaxCurr.at<double>(1, 1) = _tmax.at<double>(1, 1) * gravProjection.at<double>(1, 0);
        tmaxCurr.at<double>(1, 2) = _tmax.at<double>(1, 2) * gravProjection.at<double>(1, 0);
    }
    else
    {
        fgmaxCurr.at<double>(1, 0) = -_fgmaxNeg.at<double>(1, 0) * gravProjection.at<double>(1, 0);
        fgmaxCurr.at<double>(1, 1) = -_fgmaxNeg.at<double>(1, 1) * gravProjection.at<double>(1, 0);
        fgmaxCurr.at<double>(1, 2) = -_fgmaxNeg.at<double>(1, 2) * gravProjection.at<double>(1, 0);

        tmaxCurr.at<double>(1, 0) = -_tmaxNeg.at<double>(1, 0) * gravProjection.at<double>(1, 0);
        tmaxCurr.at<double>(1, 1) = -_tmaxNeg.at<double>(1, 1) * gravProjection.at<double>(1, 0);
        tmaxCurr.at<double>(1, 2) = -_tmaxNeg.at<double>(1, 2) * gravProjection.at<double>(1, 0);
    }

    if (gravProjection.at<double>(2, 0) > 0)
    {
        fgmaxCurr.at<double>(2, 0) = _fgmax.at<double>(2, 0) * gravProjection.at<double>(2, 0);
        fgmaxCurr.at<double>(2, 1) = _fgmax.at<double>(2, 1) * gravProjection.at<double>(2, 0);
        fgmaxCurr.at<double>(2, 2) = _fgmax.at<double>(2, 2) * gravProjection.at<double>(2, 0);

        tmaxCurr.at<double>(2, 0) = _tmax.at<double>(2, 0) * gravProjection.at<double>(2, 0);
        tmaxCurr.at<double>(2, 1) = _tmax.at<double>(2, 1) * gravProjection.at<double>(2, 0);
        tmaxCurr.at<double>(2, 2) = _tmax.at<double>(2, 2) * gravProjection.at<double>(2, 0);
    }
    else
    {
        fgmaxCurr.at<double>(2, 0) = -_fgmaxNeg.at<double>(2, 0) * gravProjection.at<double>(2, 0);
        fgmaxCurr.at<double>(2, 1) = -_fgmaxNeg.at<double>(2, 1) * gravProjection.at<double>(2, 0);
        fgmaxCurr.at<double>(2, 2) = -_fgmaxNeg.at<double>(2, 2) * gravProjection.at<double>(2, 0);

        tmaxCurr.at<double>(2, 0) = -_tmaxNeg.at<double>(2, 0) * gravProjection.at<double>(2, 0);
        tmaxCurr.at<double>(2, 1) = -_tmaxNeg.at<double>(2, 1) * gravProjection.at<double>(2, 0);
        tmaxCurr.at<double>(2, 2) = -_tmaxNeg.at<double>(2, 2) * gravProjection.at<double>(2, 0);
    }

    newData[0] = rawData[0] - _forcesBias[0] - fgmaxCurr.at<double>(0, 0) - fgmaxCurr.at<double>(1, 0) - fgmaxCurr.at<double>(2, 0);
    newData[1] = rawData[1] - _forcesBias[1] - fgmaxCurr.at<double>(0, 1) - fgmaxCurr.at<double>(1, 1) - fgmaxCurr.at<double>(2, 1);
    newData[2] = rawData[2] - _forcesBias[2] - fgmaxCurr.at<double>(0, 2) - fgmaxCurr.at<double>(1, 2) - fgmaxCurr.at<double>(2, 2);
    newData[3] = rawData[3] - _torquesBias[0] - tmaxCurr.at<double>(0, 0) - tmaxCurr.at<double>(1, 0) - tmaxCurr.at<double>(2, 0);
    newData[4] = rawData[4] - _torquesBias[1] - tmaxCurr.at<double>(0, 1) - tmaxCurr.at<double>(1, 1) - tmaxCurr.at<double>(2, 1);
    newData[5] = rawData[5] - _torquesBias[2] - tmaxCurr.at<double>(0, 2) - tmaxCurr.at<double>(1, 2) - tmaxCurr.at<double>(2, 2);

    return newData;
}

void TenzoMath::loadCalibData()
{
    std::ifstream input("calibData.txt");
    if (!input)
    {
        std::cout << "file is not found\n";
    }
        
    input >> _forcesBias[0] >> _forcesBias[1] >> _forcesBias[2] >> _torquesBias[0] >> _torquesBias[1] >> _torquesBias[2];
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            input >> _fgmax.at<double>(i, j);
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            input >> _fgmaxNeg.at<double>(i, j);
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            input >> _tmax.at<double>(i, j);
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            input >> _tmaxNeg.at<double>(i, j);
    input.close();
}

void TenzoMath::calculatePos(std::array<int, 6>& curPos)
{
    cv::Mat currRot = FanucModel::rotMatrix(curPos[3] / 180'000.0 * FanucModel::PI, curPos[4] / 180'000.0 * FanucModel::PI,
        curPos[5] / 180'000.0 * FanucModel::PI);

    constexpr double coefForces = 5.0;
    constexpr double coefTorques = 1.0;
    constexpr double threshold = 150.0;

    cv::Mat forces(1, 3, cv::DataType<double>::type);
    cv::Mat torques(1, 3, cv::DataType<double>::type);

    std::array<double, 6> ftReadings = swapData(_tenzoData.readComStrain());
    std::array<double, 6> newData = gravCompensation(currRot, ftReadings);
    forces.at<double>(0, 0) = (abs(newData[0]) < threshold ? 0 : newData[0] * coefForces);
    forces.at<double>(0, 1) = (abs(newData[1]) < threshold ? 0 : newData[1] * coefForces);
    forces.at<double>(0, 2) = (abs(newData[2]) < threshold ? 0 : newData[2] * coefForces);
    torques.at<double>(0, 0) = (abs(newData[3]) < threshold ? 0 : newData[3] * coefTorques);
    torques.at<double>(0, 1) = (abs(newData[4]) < threshold ? 0 : newData[4] * coefTorques);
    torques.at<double>(0, 2) = (abs(newData[5]) < threshold ? 0 : newData[5] * coefTorques * 5.0);

    forces *= currRot.t();
    torques *= currRot.t();

    curPos[0] += static_cast<int>(forces.at<double>(0, 0));
    curPos[1] += static_cast<int>(forces.at<double>(0, 1));
    curPos[2] += static_cast<int>(forces.at<double>(0, 2));
    curPos[3] += static_cast<int>(torques.at<double>(0, 0));
    curPos[4] -= static_cast<int>(torques.at<double>(0, 1));
    curPos[5] -= static_cast<int>(torques.at<double>(0, 2));
}

std::array<double, 6> TenzoMath::jointsToWorld(const std::array<double, 6>& joints)
{
    cv::Mat tmp = _model.fanucForwardTask(joints);
    std::array<double, 3> tmptmp = FanucModel::anglesFromMat(tmp);
    return std::array<double, 6>{
        tmp.at<double>(0, 3), tmp.at<double>(1, 3), tmp.at<double>(2, 3), tmptmp[0] * 180. / FanucModel::PI,
            tmptmp[1] * 180. / FanucModel::PI, tmptmp[2] * 180. / FanucModel::PI
    };
}

std::string TenzoMath::getCoordToMove() const
{
    return _coordToMove;
}

} //namespace nikita