#include "RoboModel.h"


RoboModel::DhParameters::
DhParameters(const double d, const double q, const double a, const double alpha)
    : dParam(d),
      qParam(q),
      aParam(a),
      alphaParam(alpha)
{
}

RoboModel::RoboModel(std::vector<std::array<double, 4>> input)
{
    _kinematicChain.reserve(input.size());
    for (auto& i : input)
    {
        _kinematicChain.emplace_back(i[0], i[1], i[2], i[3]);
    }
}

cv::Mat RoboModel::prevMatTransform(const std::size_t i)
{
    cv::Mat result(4, 4, CV_64F);
    result.at<double>(0, 0) = cos(_kinematicChain[i].qParam);
    result.at<double>(0, 1) = -cos(_kinematicChain[i].alphaParam) * sin(_kinematicChain[i].qParam);
    result.at<double>(0, 2) = sin(_kinematicChain[i].alphaParam) * sin(_kinematicChain[i].qParam);
    result.at<double>(0, 3) = _kinematicChain[i].aParam * cos(_kinematicChain[i].qParam);

    result.at<double>(1, 0) = sin(_kinematicChain[i].qParam);
    result.at<double>(1, 1) = cos(_kinematicChain[i].alphaParam) * cos(_kinematicChain[i].qParam);
    result.at<double>(1, 2) = -sin(_kinematicChain[i].alphaParam) * cos(_kinematicChain[i].qParam);
    result.at<double>(1, 3) = _kinematicChain[i].aParam * sin(_kinematicChain[i].qParam);

    result.at<double>(2, 0) = 0;
    result.at<double>(2, 1) = sin(_kinematicChain[i].alphaParam);
    result.at<double>(2, 2) = cos(_kinematicChain[i].alphaParam);
    result.at<double>(2, 3) = _kinematicChain[i].dParam;

    result.at<double>(3, 0) = result.at<double>(3, 1) = result.at<double>(3, 2) = 0;
    result.at<double>(3, 3) = 1;

    return result;
}

cv::Mat RoboModel::forwardTask(std::vector<double> inputq)
{
    _kinematicChain[0].qParam = inputq[0];
    cv::Mat transformMatrix = prevMatTransform(0);
    for (std::size_t i = 1; i < inputq.size(); ++i)
    {
        _kinematicChain[i].qParam = inputq[i];
        transformMatrix = transformMatrix * prevMatTransform(i);
    }

    return transformMatrix;
}
