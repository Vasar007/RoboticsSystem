#include "tenzoMath.h"

#include <fstream>
#include <thread>
#include <chrono>
#include "poly34.h"
#include <cmath>
#include <string>
#include <vector>
#include <tchar.h>
#include <ctime>
#include <iostream>

#include "Tenzo.h"

void printCvMat(cv::Mat t, const std::string& str = "some matrix")
{
    std::cout << str << ":\n";
    for (int i = 0; i < t.rows; ++i)
    {
        for (int j = 0; j < t.cols; ++j)
            if (abs(t.at<double>(i, j)) < 0.001)
                std::cout << 0 << ' ';
            else
                std::cout << t.at<double>(i, j) << ' ';
        std::cout << '\n';
    }
    std::cout << "-------------------------" << '\n';
}



TenzoMath::TenzoMath() 
    : _xMin (680.),
      _yMin (-465.),
      _zMin ( 520.),
      _xMax ( 1380.),
      _yMax ( 465.),
      _zMax ( 1300.),
      _tenzoData(_T("COM13"))
{
    _positions = {
        {
            { 0, 0, 0, 0, -90, 0 },
            { 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 180 },
            { 0, 0, 0, 0, 90, 180 },
            { 0, 0, 0, 0, 0, 270 },
            { 0, 0, 0, 0, 0, 90 }
        }
    };

    _g = cv::Mat(3, 1, cv::DataType<double>::type);
    _g.at<double>(0, 0) = 0.;
    _g.at<double>(1, 0) = 0.;
    _g.at<double>(2, 0) = -1;

    _fgmax = cv::Mat(3, 3, cv::DataType<double>::type);

    _tmax = cv::Mat(3, 3, cv::DataType<double>::type);

    _tmaxNeg = cv::Mat(3, 3, cv::DataType<double>::type);

    _fgmaxNeg = cv::Mat(3, 3, cv::DataType<double>::type);

    _tmp = cv::Mat(6, 6, cv::DataType<double>::type);

    _isConnectedFanuc = false;
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

std::string TenzoMath::toString(const std::array<double, 6>& coord) const
{
    std::array<int, 6> convert = convertToInt(coord);

    std::stringstream stringStream;

    std::copy(convert.begin(), convert.end(),
        std::ostream_iterator<int>(stringStream, " "));

    return stringStream.str();
}

void TenzoMath::collectData(const std::size_t index)
{
    /*if (!_isConnectedFanuc)
    {
    _fanuc.startWorking();
    _fanuc.setJointFrame();
    }*/

        //Robot.goTo and wait
        //_fanuc.goToCoordinates(_positions[i][0], _positions[i][1], _positions[i][2],
        //                       _positions[i][3], _positions[i][4], _positions[i][5]);
        //_fanuc.getJointAngles();


    getchar();
    
    //readData and put it in arrays
    std::array<double, 6> tmp = swapData(_tenzoData.readData());
    _tmp.at<double>(index, 0) = tmp[0];
    _tmp.at<double>(index, 1) = tmp[1];
    _tmp.at<double>(index, 2) = tmp[2];
    _tmp.at<double>(index, 3) = tmp[3];
    _tmp.at<double>(index, 4) = tmp[4];
    _tmp.at<double>(index, 5) = tmp[5];
}

std::array<int, 6> TenzoMath::getPosition(const std::size_t index) const
{
    return convertToInt(_positions.at(index));
}

void TenzoMath::doCalibration()
{
    _forcesBias[0] = (_tmp.at<double>(1, 0) + _tmp.at<double>(2, 0)) / 2.0;
    _forcesBias[1] = (_tmp.at<double>(4, 1) + _tmp.at<double>(5, 1)) / 2.0;
    _forcesBias[2] = (_tmp.at<double>(0, 2) + _tmp.at<double>(3, 2)) / 2.0;

    _torquesBias[0] = (_tmp.at<double>(1, 3) + _tmp.at<double>(2, 3)) / 2.0;
    _torquesBias[1] = (_tmp.at<double>(4, 4) + _tmp.at<double>(5, 4)) / 2.0;
    _torquesBias[2] = (_tmp.at<double>(0, 5) + _tmp.at<double>(3, 5)) / 2.0;

    _fgmax.at<double>(0, 0) = _tmp.at<double>(2, 0) - _forcesBias[0];
    _fgmax.at<double>(0, 1) = _tmp.at<double>(2, 1) - _forcesBias[1];
    _fgmax.at<double>(0, 2) = _tmp.at<double>(2, 2) - _forcesBias[2];

    _fgmax.at<double>(1, 0) = _tmp.at<double>(4, 0) - _forcesBias[0];
    _fgmax.at<double>(1, 1) = _tmp.at<double>(4, 1) - _forcesBias[1];
    _fgmax.at<double>(1, 2) = _tmp.at<double>(4, 2) - _forcesBias[2];

    _fgmax.at<double>(2, 0) = _tmp.at<double>(0, 0) - _forcesBias[0];
    _fgmax.at<double>(2, 1) = _tmp.at<double>(0, 1) - _forcesBias[1];
    _fgmax.at<double>(2, 2) = _tmp.at<double>(0, 2) - _forcesBias[2];

    _tmax.at<double>(0, 0) = _tmp.at<double>(2, 3) - _torquesBias[0];
    _tmax.at<double>(0, 1) = _tmp.at<double>(2, 4) - _torquesBias[1];
    _tmax.at<double>(0, 2) = _tmp.at<double>(2, 5) - _torquesBias[2];

    _tmax.at<double>(1, 0) = _tmp.at<double>(4, 3) - _torquesBias[0];
    _tmax.at<double>(1, 1) = _tmp.at<double>(4, 4) - _torquesBias[1];
    _tmax.at<double>(1, 2) = _tmp.at<double>(4, 5) - _torquesBias[2];

    _tmax.at<double>(2, 0) = _tmp.at<double>(0, 3) - _torquesBias[0];
    _tmax.at<double>(2, 1) = _tmp.at<double>(0, 4) - _torquesBias[1];
    _tmax.at<double>(2, 2) = _tmp.at<double>(0, 5) - _torquesBias[2];

    _fgmaxNeg.at<double>(0, 0) = _tmp.at<double>(1, 0) - _forcesBias[0];
    _fgmaxNeg.at<double>(0, 1) = _tmp.at<double>(1, 1) - _forcesBias[1];
    _fgmaxNeg.at<double>(0, 2) = _tmp.at<double>(1, 2) - _forcesBias[2];

    _fgmaxNeg.at<double>(1, 0) = _tmp.at<double>(5, 0) - _forcesBias[0];
    _fgmaxNeg.at<double>(1, 1) = _tmp.at<double>(5, 1) - _forcesBias[1];
    _fgmaxNeg.at<double>(1, 2) = _tmp.at<double>(5, 2) - _forcesBias[2];

    _fgmaxNeg.at<double>(2, 0) = _tmp.at<double>(3, 0) - _forcesBias[0];
    _fgmaxNeg.at<double>(2, 1) = _tmp.at<double>(3, 1) - _forcesBias[1];
    _fgmaxNeg.at<double>(2, 2) = _tmp.at<double>(3, 2) - _forcesBias[2];

    _tmaxNeg.at<double>(0, 0) = _tmp.at<double>(1, 3) - _torquesBias[0];
    _tmaxNeg.at<double>(0, 1) = _tmp.at<double>(1, 4) - _torquesBias[1];
    _tmaxNeg.at<double>(0, 2) = _tmp.at<double>(1, 5) - _torquesBias[2];

    _tmaxNeg.at<double>(1, 0) = _tmp.at<double>(5, 3) - _torquesBias[0];
    _tmaxNeg.at<double>(1, 1) = _tmp.at<double>(5, 4) - _torquesBias[1];
    _tmaxNeg.at<double>(1, 2) = _tmp.at<double>(5, 5) - _torquesBias[2];

    _tmaxNeg.at<double>(2, 0) = _tmp.at<double>(3, 3) - _torquesBias[0];
    _tmaxNeg.at<double>(2, 1) = _tmp.at<double>(3, 4) - _torquesBias[1];
    _tmaxNeg.at<double>(2, 2) = _tmp.at<double>(3, 5) - _torquesBias[2];

    std::ofstream out("calibData.txt");

    out << _forcesBias[0] << ' ' << _forcesBias[1] << ' ' << _forcesBias[2] << ' ' << _torquesBias[0] << ' ' 
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

std::array<double, 6> TenzoMath::gravCompensation(cv::Mat p6, std::array<double, 6>& rawData)
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
        std::cout << "file is not found\n";
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

void TenzoMath::calculatePos(std::array<double, 6>& curPos)
{
    cv::Mat currRot = FanucModel::rotMatrix(curPos[3] / 180.0 * FanucModel::PI, curPos[4] / 180.0 * FanucModel::PI,
       curPos[5] / 180.0 * FanucModel::PI);

    constexpr double coefForces = 0.005;
    constexpr double coefTorques = 0.001;
    constexpr double threshold = 150;

    cv::Mat forces(1, 3, cv::DataType<double>::type);
    cv::Mat torques(1, 3, cv::DataType<double>::type);

    std::array<double, 6> ftReadings = swapData(_tenzoData.readData());
    std::array<double, 6> newData = gravCompensation(currRot, ftReadings);
    forces.at<double>(0, 0) = (abs(newData[0]) < threshold ? 0 : newData[0] * coefForces);
    forces.at<double>(0, 1) = (abs(newData[1]) < threshold ? 0 : newData[1] * coefForces);
    forces.at<double>(0, 2) = (abs(newData[2]) < threshold ? 0 : newData[2] * coefForces);
    torques.at<double>(0, 0) = (abs(newData[3]) < threshold ? 0 : newData[3] * coefTorques);
    torques.at<double>(0, 1) = (abs(newData[4]) < threshold ? 0 : newData[4] * coefTorques);
    torques.at<double>(0, 2) = (abs(newData[5]) < threshold ? 0 : newData[5] * coefTorques * 5);

    forces *= currRot.t();
    torques *= currRot.t();

    curPos[0] += forces.at<double>(0, 0);
    curPos[1] += forces.at<double>(0, 1);
    curPos[2] += forces.at<double>(0, 2);
    curPos[3] += torques.at<double>(0, 0);
    curPos[4] -= torques.at<double>(0, 1);
    curPos[5] -= torques.at<double>(0, 2);

    _coordToMove = toString(curPos);
}

void TenzoMath::ftControlCartesianCoord()
{
    std::array<double, 6> worldPos = {
        985.0, 0.0, 940.0, -180.0, 0.0, 0.0
    };
    cv::Mat forces(1, 3, cv::DataType<double>::type);
    cv::Mat torques(1, 3, cv::DataType<double>::type);
    cv::Mat currRot(3, 3, cv::DataType<double>::type);
    constexpr double coefForces = 0.005;
    constexpr double coefTorques = 0.001;
    constexpr double threshold = 150;
   // _fanuc.startWorking();
   // _fanuc.setWorldFrame();
    
    while (true)
    {
        std::array<double, 6> tmp = swapData(_tenzoData.readData());
        currRot = FanucModel::rotMatrix(worldPos[3] / 180.0 * FanucModel::PI, worldPos[4] / 180.0 * FanucModel::PI,
            worldPos[5] / 180.0 * FanucModel::PI);

        std::array<double, 6> newData = gravCompensation(currRot, tmp);
        forces.at<double>(0, 0) = (abs(newData[0]) < threshold ? 0 : newData[0] * coefForces);
        forces.at<double>(0, 1) = (abs(newData[1]) < threshold ? 0 : newData[1] * coefForces);
        forces.at<double>(0, 2) = (abs(newData[2]) < threshold ? 0 : newData[2] * coefForces);
        torques.at<double>(0, 0) = (abs(newData[3]) < threshold ? 0 : newData[3] * coefTorques);
        torques.at<double>(0, 1) = (abs(newData[4]) < threshold ? 0 : newData[4] * coefTorques);
        torques.at<double>(0, 2) = (abs(newData[5]) < threshold ? 0 : newData[5] * coefTorques * 5);

        forces *= currRot.t();
        torques *= currRot.t();

       /* std::cout << forces.at<double>(0, 0) << '\t' << forces.at<double>(0, 1) << '\t' << forces.at<double>(0, 2) << '\t'
        		<< torques.at<double>(0, 0) << '\t' << torques.at<double>(0, 1) << '\t' << torques.at<double>(0, 2) << '\n';*/

        worldPos[0] += forces.at<double>(0, 0);
        worldPos[1] += forces.at<double>(0, 1);
        worldPos[2] += forces.at<double>(0, 2);
        worldPos[3] += torques.at<double>(0, 0);
        worldPos[4] -= torques.at<double>(0, 1);
        worldPos[5] -= torques.at<double>(0, 2);

       for (int i = 3; i < 6; ++i)
        {
            if (worldPos[i] > 180.0f)
                worldPos[i] -= 360.0f;
            if (worldPos[i] < -180.0f)
                worldPos[i] += 360.0f;
        }
        /*for (int i = 0; i < 6; ++i)
        {
            std::cout << worldPos[i] << '\t';
        }
        std::cout << '\n';*/
        
       // _fanuc.goToCoordinates(worldPos[0], worldPos[1], worldPos[2], worldPos[3], worldPos[4],
         //                      worldPos[5]);
        
        //_fanuc.getJointAngles();

       _coordToMove = toString(worldPos);
       std::cout << getCoordToMove() << '\n';
    }
}

std::string TenzoMath::getCoordToMove() const
{
    return _coordToMove;
}

void TenzoMath::newJointsControl()
{
    std::array<double, 6> worldPos{ 985.0, 0.0, 1040.0, -180.0, 0.0, 0.0 };
    std::array<double, 6> jointPos{ 0.0, 0.0, 0.0, 0.0, -90.0, 0.0 };
    Tenzo tenzoData(_T("COM6"));
    cv::Mat forces(1, 3, cv::DataType<double>::type);
    cv::Mat torques(1, 3, cv::DataType<double>::type);
    cv::Mat currRot(3, 3, cv::DataType<double>::type);
    constexpr double coefForces = 0.005;
    constexpr double coefTorques = 0.001;
    constexpr double threshold = 150.0;
    cv::Mat p6 = _model.fanucForwardTask(jointPos);
    //_fanuc.startWorking();
    //_fanuc.setJointFrame();
    while (true)
    {   
        std::array<double, 6> tmp = swapData(tenzoData.readData());
        
        std::array<double, 6> newData = gravCompensation(p6, tmp);
        forces.at<double>(0, 0) = (abs(newData[0]) < threshold ? 0 : newData[0] * coefForces);
        forces.at<double>(0, 1) = (abs(newData[1]) < threshold ? 0 : newData[1] * coefForces);
        forces.at<double>(0, 2) = (abs(newData[2]) < threshold ? 0 : newData[2] * coefForces);
        torques.at<double>(0, 0) = (abs(newData[3]) < threshold ? 0 : newData[3] * coefTorques);
        torques.at<double>(0, 1) = (abs(newData[4]) < threshold ? 0 : newData[4] * coefTorques);
        torques.at<double>(0, 2) = (abs(newData[5]) < threshold ? 0 : newData[5] * coefTorques * 5);
       // std::cout << "Data: " << forces.at<double>(0, 0) << '\t' << forces.at<double>(0, 1) << '\t' << forces.at<double>(0, 2) << '\t' <<
        //    torques.at<double>(0, 0) << '\t' << torques.at<double>(0, 1) << '\t' << torques.at<double>(0, 2) << '\n';

        /*if (abs(forces.at<double>(0, 0)) > 0.0 || abs(forces.at<double>(0, 1)) > 0.0 || abs(forces.at<double>(0, 2)) > 0.0 ||
            abs(torques.at<double>(0, 0)) > 0.0 || abs(torques.at<double>(0, 1)) > 0.0 || abs(torques.at<double>(0, 2)) > 0.0)*/ // != 0
        if (forces.at<double>(0, 0) != 0.0 || forces.at<double>(0, 1) != 0.0 || forces.at<double>(0, 2) != 0.0 ||
            torques.at<double>(0, 0) != 0.0 || torques.at<double>(0, 1) != 0.0 || torques.at<double>(0, 2) != 0.0) 
        {
            //double t0 = torques.at<double>(0, 0);
            //double t1 = torques.at<double>(0, 1);
            //torques.at<double>(0, 0) = t0 * cos(worldPos[5] / 180. * PI) - t1 * sin(worldPos[5] / 180. * PI);
            //torques.at<double>(0, 1) = -t0 * sin(worldPos[5] / 180. * PI) + t1 * cos(worldPos[5] / 180. * PI);
            //torques.at<double>(0, 2) = (sin(worldPos[5] / 180. * PI) > 0 ? torques.at<double>(0, 2) : -torques.at<double>(0, 2));

            p6(cv::Rect(0, 0, 3, 3)).copyTo(currRot);
            forces *= currRot.t();
            torques *= currRot.t(); 

            worldPos[0] += forces.at<double>(0, 0);
            worldPos[1] += forces.at<double>(0, 1);
            worldPos[2] += forces.at<double>(0, 2);
            worldPos[3] += torques.at<double>(0, 0);
            worldPos[4] -= torques.at<double>(0, 1);
            worldPos[5] -= torques.at<double>(0, 2);

            for (int i = 3; i < 6; ++i)
            {
                if (worldPos[i] > 180.0)
                    worldPos[i] -= 360.0;
                if (worldPos[i] < -180.0)
                    worldPos[i] += 360.0;
            }

            for (int i = 0; i < 6; ++i)
            {
                std::cout << worldPos[i] << '\t';
            }
            std::cout << '\n';

            std::cout << _model.fanucInverseTask(worldPos) << '\n';

            jointPos = chooseNearestPose(_model.fanucInverseTask(worldPos), jointPos);
            p6 = _model.fanucForwardTask(jointPos);
           /* for (int i = 0; i < 6; ++i)
            {
                std::cout << jointPos[i] << '\t';
            }
            std::cout << '\n';*/

            //_fanuc.goToCoordinates(jointPos[0], jointPos[1], jointPos[2], jointPos[3], jointPos[4], jointPos[5]);
            //_fanuc.getJointAngles(); //????падает
        }
       // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

std::array<double, 6> TenzoMath::chooseNearestPose(cv::Mat res, std::array<double, 6> prevPos)
{
    if (!res.empty())
    {
        // std::cout << res << '\n' << '\n';
        std::vector<double> delta;
        for (int j = 0; j < res.rows; ++j)
        {
            double deltaTmp = 0;
            for (int t = 0; t < 6; ++t)
            {
                deltaTmp += abs(res.at<double>(j, t) - prevPos[t]);
            }
            delta.push_back(deltaTmp);
        }
        if (!delta.empty())
        {
            int num = 0;
            double min = delta[0];
            for (int j = 0; j < delta.size(); ++j)
            {
                if (delta[j] < min)
                {
                    min = delta[j];
                    num = j;
                }
            }
            if (abs(min) < 50.f) // обосновать или убрать
            {
                std::cout << "\nchosen\n";
                return std::array<double, 6>{res.at<double>(num, 0), res.at<double>(num, 1), res.at<double>(num, 2),  res.at<double>(num, 3), res.at<double>(num, 4), res.at<double>(num, 5) };
            }
        }
        return prevPos;
    }
    return prevPos;
}

