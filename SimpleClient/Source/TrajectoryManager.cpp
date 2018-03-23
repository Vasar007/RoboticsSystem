#include "TrajectoryManager.h"

std::vector<vasily::RobotData> danila::TrajectoryManager::PositionalMovement(
    const vasily::ParsedResult& parsedResult) const noexcept
{
    return parsedResult.points;
}

std::vector<vasily::RobotData> danila::TrajectoryManager::PartialMovement(
    const vasily::ParsedResult& parsedResult) const noexcept
{
    std::vector<vasily::RobotData> ans;
    ans.reserve(parsedResult.numberOfIterations + 1);

    double len = 0.;
    for(size_t i = 1; i<parsedResult.points.size();++i)
    {
        len += (parsedResult.points[i] - parsedResult.points[i - 1]).length();
    }

    assert(len > 0.);

    len /= parsedResult.numberOfIterations;

    double prevlen = 0;

    vasily::RobotData prev = parsedResult.points[0];

    for (size_t i = 1; i<parsedResult.points.size();++i)
    {
        const vasily::RobotData direction = parsedResult.points[i] - parsedResult.points[i - 1];
        double curlen = (parsedResult.points[i] - prev).length();
        while(prevlen + curlen > len)
        {
            prev += (parsedResult.points[i] - prev) / (curlen/len);
            ans.emplace_back(prev);
            curlen = (parsedResult.points[i] - prev).length();
            prevlen = 0.;
        }

        prevlen += curlen;
    }

    ans.emplace_back(*parsedResult.points.rbegin());

    return ans;
}

std::vector<vasily::RobotData> danila::TrajectoryManager::CirclicMovement(
    const vasily::ParsedResult& parsedResult) const noexcept
{
    std::vector<vasily::RobotData> ans;
    ans.reserve(parsedResult.numberOfIterations * parsedResult.points.size());
    for(int i=0;i<parsedResult.numberOfIterations;++i)
    {
        for(auto& it:parsedResult.points)
        {
            ans.emplace_back(it);
        }
    }
    return ans;
}
