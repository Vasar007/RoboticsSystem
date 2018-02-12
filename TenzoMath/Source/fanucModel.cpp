#include "fanucModel.h"
#include <iostream>
#include <opencv2/core.hpp>
#include <vector>
#include "poly34.h"

FanucModel::FanucModel()
    : RoboModel(std::vector<std::array<double, 4>>{
          {0, 0, 150, PI / 2},
          {0, 0, 790, 0},
          {0, 0, 250, PI / 2},
          {835, 0, 0, -PI / 2},
          {0, 0, 0, PI / 2},
          {100, 0, 0, 0},
          {130, PI / 2, -90, 0},
          {-190, 0, 0, 0}
      }),
      _toCamera((cv::Mat_<double>(4, 4) << 0, -1, 0, -43, 1, 0, 0, -90, 0, 0, 1, 130, 0, 0, 0, 1)),
      _toSixth((cv::Mat_<double>(4, 4) << 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, -190, 0, 0, 0, 1)),
      _forMovingToCamera((cv::Mat_<double>(4, 4) << 0, -1, 0, 43, 1, 0, 0, 90, 0, 0, 1, 130, 0, 0, 0, 1))
{
}

std::vector<RoboModel::DhParameters> FanucModel::getDhParameters() const
{
    return _kinematicChain;
}

std::vector<double> FanucModel::jointsToQ(std::array<double, 6> j)
{
    //degrees to radians
    for (int i = 0; i < 6; ++i)
    {
        j[i] *= PI / 180.0;
    }
    std::vector<double> q;
    q.reserve(6);
    q.push_back(j.at(0));
    q.push_back(-j.at(1) + PI / 2);
    q.push_back(j.at(1) + j.at(2));
    q.push_back(-j.at(3));
    q.push_back(j.at(4));
    q.push_back(-j.at(5));
    return q;
}

cv::Mat FanucModel::fanucForwardTask(const std::array<double, 6> inputJoints)
{
    const std::vector<double> q = jointsToQ(inputJoints);
    return forwardTask(q);
}

std::array<double, 3> FanucModel::anglesFromMat(const cv::Mat p6)
{
    std::array<double, 3> angleVector;
    angleVector.at(0) = atan2(p6.at<double>(2, 1), p6.at<double>(2, 2));
    angleVector.at(1) = atan2(-p6.at<double>(2, 0),
                              sqrt(p6.at<double>(2, 1) * p6.at<double>(2, 1) + p6.at<double>(2, 2) * p6.at<double>(2, 2)));
    angleVector.at(2) = atan2(p6.at<double>(1, 0), p6.at<double>(0, 0));
    return angleVector;
}

std::array<double, 6> FanucModel::getCoordsFromMat(cv::Mat transformMatrix)
{
    std::array<double, 3> wprAngles = anglesFromMat(transformMatrix);

    std::array<double, 6> res;
    res.at(0) = transformMatrix.at<double>(0, 3);
    res.at(1) = transformMatrix.at<double>(1, 3);
    res.at(2) = transformMatrix.at<double>(2, 3);
    res.at(3) = wprAngles.at(0);
    res.at(4) = wprAngles.at(1);
    res.at(5) = wprAngles.at(2);
    return res;
}


cv::Mat FanucModel::qi(const double& alpha, const double& q) const
{
    cv::Mat tmp(3, 3, CV_64F);
    tmp.at<double>(0, 0) = cos(q);
    tmp.at<double>(0, 1) = -cos(alpha) * sin(q);
    tmp.at<double>(0, 2) = sin(alpha) * sin(q);

    tmp.at<double>(1, 0) = sin(q);
    tmp.at<double>(1, 1) = cos(alpha) * cos(q);
    tmp.at<double>(1, 2) = -sin(alpha) * cos(q);

    tmp.at<double>(2, 0) = 0;
    tmp.at<double>(2, 1) = sin(alpha);
    tmp.at<double>(2, 2) = cos(alpha);

    return tmp;
}

cv::Mat FanucModel::rotMatrix(const double& w, const double& p, const double& r)
{
    cv::Mat mx(3, 3, CV_64F), my(3, 3, CV_64F), mz(3, 3, CV_64F);
    mx.at<double>(0, 0) = 1;
    mx.at<double>(0, 1) = mx.at<double>(0, 2) = mx.at<double>(1, 0) = mx.at<double>(2, 0) = 0;
    mx.at<double>(1, 1) = mx.at<double>(2, 2) = cos(w);
    mx.at<double>(1, 2) = -sin(w);
    mx.at<double>(2, 1) = sin(w);

    my.at<double>(1, 1) = 1;
    my.at<double>(0, 1) = my.at<double>(1, 2) = my.at<double>(1, 0) = my.at<double>(2, 1) = 0;
    my.at<double>(0, 0) = my.at<double>(2, 2) = cos(p);
    my.at<double>(0, 2) = sin(p);
    my.at<double>(2, 0) = -sin(p);

    mz.at<double>(2, 2) = 1;
    mz.at<double>(0, 2) = mz.at<double>(1, 2) = mz.at<double>(2, 0) = mz.at<double>(2, 1) = 0;
    mz.at<double>(1, 1) = mz.at<double>(0, 0) = cos(r);
    mz.at<double>(0, 1) = -sin(r);
    mz.at<double>(1, 0) = sin(r);

    return mz * my * mx;
}

cv::Mat FanucModel::fanucInverseTask(const std::array<double, 6> coord) const
{
    std::vector<RoboModel::DhParameters> param = getDhParameters();

    const double a = 2 * param[0]._aParam * coord[0];
    const double b = 2 * param[0]._aParam * coord[1];
    const double c = 2 * param[1]._aParam * param[2]._aParam - 2 * param[1]._dParam * param[3]._dParam *
                     sin(param[1]._alphaParam) * sin(param[2]._alphaParam);
    const double d = 2 * param[2]._aParam * param[1]._dParam * sin(param[1]._alphaParam) + 2 * param[1]._aParam * param[3]._dParam
                     * sin(param[2]._alphaParam);
    const double e = param[1]._aParam * param[1]._aParam + param[2]._aParam * param[2]._aParam + param[1]._dParam *
                     param[1]._dParam + param[2]._dParam * param[2]._dParam + param[3]._dParam * param[3]._dParam -
                     param[0]._aParam * param[0]._aParam - coord[0] * coord[0] - coord[1] * coord[1] - 
                     (coord[2] - param[0]._dParam) * (coord[2] - param[0]._dParam) + 2 *
                     param[1]._dParam * param[2]._dParam * cos(param[1]._alphaParam) + 2 * param[1]._dParam * param[3]._dParam *
                     cos(param[1]._alphaParam) * cos(param[2]._alphaParam) + 2 * param[2]._dParam * param[3]._dParam * cos(param[2]._alphaParam);
    const double f = coord[1] * sin(param[0]._alphaParam);
    const double g = -coord[0] * sin(param[0]._alphaParam);
    const double h = -param[3]._dParam * sin(param[1]._alphaParam) * sin(param[2]._alphaParam);
    const double i = param[2]._aParam * sin(param[1]._alphaParam);
    const double j = param[1]._dParam + param[2]._dParam * cos(param[1]._alphaParam) + param[3]._dParam *
                     cos(param[1]._alphaParam) * cos(param[2]._alphaParam) - (coord[2] - param[0]._dParam) *
                     cos(param[0]._alphaParam);
    const double r = 4. * param[0]._aParam * param[0]._aParam * (j - h) * (j - h) + sin(param[0]._alphaParam) *
                     sin(param[0]._alphaParam) * (e - c) * (e - c)
                     - 4. * param[0]._aParam * param[0]._aParam * sin(param[0]._alphaParam) * sin(param[0]._alphaParam)
                     * (coord[0] * coord[0] + coord[1] * coord[1]);
    const double s = 4. * (4. * param[0]._aParam * param[0]._aParam * i * (j - h) + sin(param[0]._alphaParam) *
                           sin(param[0]._alphaParam) * d *
                           (e - c));
    const double t = 2. * (4. * param[0]._aParam * param[0]._aParam * (j * j - h * h + 2 * i * i) +
                           sin(param[0]._alphaParam) * sin(param[0]._alphaParam)
                           * (e * e - c * c + 2 * d * d) - 4. * param[0]._aParam * param[0]._aParam *
                           sin(param[0]._alphaParam) * sin(param[0]._alphaParam) *
                           (coord[0] * coord[0] + coord[1] * coord[1]));
    const double u = 4. * (4. * param[0]._aParam * param[0]._aParam * i * (j + h) +
                           sin(param[0]._alphaParam) * sin(param[0]._alphaParam) * d * (e + c));
    const double v = 4. * param[0]._aParam * param[0]._aParam * (h + j) * (h + j) + sin(param[0]._alphaParam) *
                     sin(param[0]._alphaParam) *
                     (e + c) * (e + c) - 4. * param[0]._aParam * param[0]._aParam * sin(param[0]._alphaParam) *
                     sin(param[0]._alphaParam) *
                     (coord[0] * coord[0] + coord[1] * coord[1]);

    double x[4];
    const int numberOfRoots = SolveP4(x, s / r, t / r, u / r, v / r);

    if (numberOfRoots != 2 && numberOfRoots != 4)
    {
        std::cout << "\nsomething is wrong with roots of equatation\n";
        return cv::Mat();
    }

    cv::Mat theta(numberOfRoots, 3, cv::DataType<double>::type);

    for (int it = 0; it < numberOfRoots; ++it)
    {
        theta.at<double>(it, 2) = 2. * atan(x[it]);
    }

    double costheta, sintheta;
    for (int it = 0; it < numberOfRoots; ++it)
    {
        costheta = (-g * (c * cos(theta.at<double>(it, 2)) + d * sin(theta.at<double>(it, 2)) + e) +
                    b * (h * cos(theta.at<double>(it, 2)) + i * sin(theta.at<double>(it, 2) + j))) / (a * g - f * b);
        sintheta = (f * (c * cos(theta.at<double>(it, 2)) + d * sin(theta.at<double>(it, 2)) + e) -
                    a * (h * cos(theta.at<double>(it, 2)) + i * sin(theta.at<double>(it, 2) + j))) / (a * g - f * b);
        if (sintheta >= 0)
        {
            theta.at<double>(it, 0) = acos(costheta);
        }
        else
        {
            theta.at<double>(it, 0) = -acos(costheta);
        }
    }

    for (int it = 0; it < numberOfRoots; ++it)
    {
        const double a11 = param[1]._aParam + param[2]._aParam * cos(theta.at<double>(it, 2)) +
                           param[3]._dParam * sin(param[2]._alphaParam) * sin(theta.at<double>(it, 2));
        const double a12 = -param[2]._aParam * cos(param[1]._alphaParam) *
                           sin(theta.at<double>(it, 2)) + param[2]._dParam * sin(param[1]._alphaParam) + param[3]._dParam *
                           sin(param[2]._alphaParam) * cos(param[1]._alphaParam) * cos(theta.at<double>(it, 2)) +
                           param[3]._dParam * sin(param[1]._alphaParam) * cos(param[2]._alphaParam);
        costheta = (a11 * (coord[0] * cos(theta.at<double>(it, 0)) + coord[1] * sin(theta.at<double>(it, 0)) - param[0]._aParam)
                    - a12 * (-coord[0] * cos(param[0]._alphaParam) * sin(theta.at<double>(it, 0)) + coord[1] *
                             cos(param[0]._alphaParam) * cos(theta.at<double>(it, 0)) + (coord[2] - param[0]._dParam) * sin(param[0]._alphaParam)))
                   / (a11 * a11 + a12 * a12);
        sintheta = (a12 * (coord[0] * cos(theta.at<double>(it, 0)) + coord[1] * sin(theta.at<double>(it, 0)) - param[0]._aParam) + a11 *
                    (-coord[0] * cos(param[0]._alphaParam) * sin(theta.at<double>(it, 0)) + coord[1] *
                     cos(param[0]._alphaParam) *
                     cos(theta.at<double>(it, 0)) + (coord[2] - param[0]._dParam) * sin(param[0]._alphaParam))) / (a11 * a11 + a12 * a12);
        if (sintheta >= 0)
        {
            theta.at<double>(it, 1) = acos(costheta);
        }
        else
        {
            theta.at<double>(it, 1) = -acos(costheta);
        }
    }
    for (int it = 0; it < numberOfRoots; ++it)
    {
        theta.at<double>(it, 1) = -theta.at<double>(it, 1) + PI / 2;
        theta.at<double>(it, 2) -= theta.at<double>(it, 1);
    }

    std::vector<int> ind;

    for (int it = 0; it < numberOfRoots; ++it)
    {
        bool isOk = true;

        if (abs(theta.at<double>(it, 0)) > 170. / 180. * PI)
        {
            isOk = false;
        }
        if (theta.at<double>(it, 1) > 90. / 180. * PI || theta.at<double>(it, 1) < -70 / 180. * PI)
        {
            isOk = false;
        }
        if (theta.at<double>(it, 2) > 200. / 180. * PI || theta.at<double>(it, 2) < -70 / 180. * PI)
        {
            isOk = false;
        }

        if (!std::isnan(theta.at<double>(it, 1)) && isOk)
        {
            ind.emplace_back(it);
        }
    }
    cv::Mat thetaRes(ind.size(), 6, cv::DataType<double>::type);
    for (int it = 0; it < ind.size(); ++it)
    {
        thetaRes.at<double>(it, 0) = theta.at<double>(ind[it], 0);
        thetaRes.at<double>(it, 1) = theta.at<double>(ind[it], 1);
        thetaRes.at<double>(it, 2) = theta.at<double>(ind[it], 2);
        thetaRes.at<double>(it, 3) = 0;
        thetaRes.at<double>(it, 4) = 0;
        thetaRes.at<double>(it, 5) = 0;
    }

    cv::Mat thetaPrefinal(thetaRes.rows * 2, 6, cv::DataType<double>::type);
    for (int it = 0; it < thetaRes.rows; ++it)
    {
        cv::Mat r36(3, 3, cv::DataType<double>::type), r03(3, 3, cv::DataType<double>::type);
        std::array<double, 6> q;
        q[0] = thetaRes.at<double>(it, 0);
        q[1] = -thetaRes.at<double>(it, 1) + PI / 2;
        q[2] = thetaRes.at<double>(it, 2) + thetaRes.at<double>(it, 1);

        r03 = qi(param[0]._alphaParam, q[0]) * qi(param[1]._alphaParam, q[1]) * qi(param[2]._alphaParam, q[2]);
        r36 = r03.inv() * rotMatrix(coord[3] / 180. * PI, coord[4] / 180. * PI, coord[5] / 180. * PI);

        for (int zt = 0; zt < 2; ++zt)
        {
            thetaPrefinal.at<double>(it * 2 + zt, 0) = thetaRes.at<double>(it, 0);
            thetaPrefinal.at<double>(it * 2 + zt, 1) = thetaRes.at<double>(it, 1);
            thetaPrefinal.at<double>(it * 2 + zt, 2) = thetaRes.at<double>(it, 2);
        }

        thetaPrefinal.at<double>(it * 2, 4) = acos(r36.at<double>(2, 2));
        thetaPrefinal.at<double>(it * 2 + 1, 4) = -acos(r36.at<double>(2, 2));
        thetaPrefinal.at<double>(it * 2, 3) = -(PI - asin(r36.at<double>(1, 2) / sin(thetaPrefinal.at<double>(it * 2, 4))));
        thetaPrefinal.at<double>(it * 2 + 1, 3) = -asin(r36.at<double>(1, 2) / sin(thetaPrefinal.at<double>(it * 2 + 1, 4)));
        thetaPrefinal.at<double>(it * 2, 5) = thetaPrefinal.at<double>(it * 2 + 1, 5) = atan2(r36.at<double>(2, 1), r36.at<double>(2, 0));
       /* thetaPrefinal.at<double>(it * 2, 5) = -(PI - asin(r36.at<double>(2, 1) / sin(thetaPrefinal.at<double>(it * 2, 4))));
        thetaPrefinal.at<double>(it * 2 + 1, 5) = -asin(r36.at<double>(2, 1) / sin(thetaPrefinal.at<double>(it * 2 + 1, 4)));*/
    }

    std::vector<int> indFinal;
    for (int it = 0; it < thetaPrefinal.rows; ++it)
    {
        bool isOk = true;

        if (abs(thetaPrefinal.at<double>(it, 3)) > 210. / 180. * PI)
        {
            isOk = false;
        }
        if (abs(thetaPrefinal.at<double>(it, 4)) > 140. / 180. * PI)
        {
            isOk = false;
        }
        if (abs(thetaPrefinal.at<double>(it, 5)) > 270. / 180. * PI)
        {
            isOk = false;
        }

        if (isOk)
        {
            indFinal.push_back(it);
        }
    }

    cv::Mat thetaFinal(indFinal.size(), 6, cv::DataType<double>::type);
    for (int it = 0; it < indFinal.size(); ++it)
    {
        thetaFinal.at<double>(it, 0) = thetaPrefinal.at<double>(indFinal[it], 0);
        thetaFinal.at<double>(it, 1) = thetaPrefinal.at<double>(indFinal[it], 1);
        thetaFinal.at<double>(it, 2) = thetaPrefinal.at<double>(indFinal[it], 2);
        thetaFinal.at<double>(it, 3) = thetaPrefinal.at<double>(indFinal[it], 3);
        thetaFinal.at<double>(it, 4) = thetaPrefinal.at<double>(indFinal[it], 4);
        thetaFinal.at<double>(it, 5) = thetaPrefinal.at<double>(indFinal[it], 5);
    }

    return thetaFinal * 180. / PI;
}

cv::Mat FanucModel::fanucInverseTaskNew(const std::array<double, 6> coord) const
{
    cv::Mat p6 = rotMatrix(coord[3] / 180. * PI, coord[4] / 180. * PI, coord[5] / 180. * PI);
    double xc = coord[0] - p6.at<double>(0, 2) * 100.;
    double yc = coord[1] - p6.at<double>(1, 2) * 100.;
    double zc = coord[2] - p6.at<double>(2, 2) * 100.;
    return fanucInverseTask({ xc, yc, zc, coord[3], coord[4], coord[5] });
}

cv::Mat FanucModel::getToCamera() const
{
    return _toCamera;
}

cv::Mat FanucModel::getToSixth() const
{
    return _toSixth;
}
