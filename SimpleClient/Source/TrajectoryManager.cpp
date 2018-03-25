#include "TrajectoryManager.h"

namespace danila
{

std::vector<vasily::RobotData> TrajectoryManager::PositionalMovement(
    const vasily::ParsedResult& parsedResult) const noexcept
{
    assert(parsedResult.isCorrect);
    return parsedResult.points;
}

std::vector<vasily::RobotData> TrajectoryManager::PartialMovement(
    const vasily::ParsedResult& parsedResult) const noexcept
{
    assert(parsedResult.isCorrect);
    std::vector<vasily::RobotData> ans;
    ans.reserve(parsedResult.numberOfIterations + 1);

    double len = 0.;
    for(size_t i = 1; i<parsedResult.points.size();++i)
    {
        len += (parsedResult.points[i] - parsedResult.points[i - 1]).length() /
            parsedResult.numberOfIterations;
    }

    assert(len > 0.);

    double prevlen = 0;

    vasily::RobotData prev = parsedResult.points[0];

    ans.emplace_back(*parsedResult.points.begin());

    for (std::size_t i = 1; i < parsedResult.points.size(); ++i)
    {
        double curlen = (parsedResult.points[i] - prev).length();
        while(prevlen + curlen >= len)
        {
            prev += (parsedResult.points[i] - prev) * ((len - prevlen) / curlen);
            ans.emplace_back(prev);
            curlen = (parsedResult.points[i] - prev).length();
            prevlen = 0.;
        }

        prev = parsedResult.points[i];
        prevlen += curlen;
    }

    return ans;
}

std::vector<vasily::RobotData> TrajectoryManager::CirclicMovement(
    const vasily::ParsedResult& parsedResult) const noexcept
{
    assert(parsedResult.isCorrect);
    std::vector<vasily::RobotData> ans;
    ans.reserve(parsedResult.numberOfIterations * parsedResult.points.size());
    for (int i = 0; i < parsedResult.numberOfIterations; ++i)
    {
        ans.insert(std::end(ans), std::begin(parsedResult.points), std::end(parsedResult.points));
    }
    return ans;
}

} // namespace danila
