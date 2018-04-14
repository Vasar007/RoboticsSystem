#include "TrajectoryManager.h"


namespace danila
{

std::vector<vasily::RobotData> TrajectoryManager::positionalMovement(
    const vasily::ParsedResult& parsedResult) const noexcept
{
    assert(parsedResult.isCorrect);
    return parsedResult.points;
}

std::vector<vasily::RobotData> TrajectoryManager::partialMovement(
    const vasily::ParsedResult& parsedResult) const
{
    assert(parsedResult.isCorrect);
    std::vector<vasily::RobotData> ans;
    ans.reserve(parsedResult.numberOfIterations + 1);

    double len = 0.0;
    for(std::size_t i = 1; i < parsedResult.points.size(); ++i)
    {
        len += (parsedResult.points[i] - parsedResult.points[i - 1]).length() /
            parsedResult.numberOfIterations;
    }

    assert(len > 0.0);

    double prevlen = 0;

    vasily::RobotData prev = parsedResult.points[0];

    // TODO(Vasily): Think about replacing with push_back + move.
    ans.push_back(*parsedResult.points.begin());

    for (std::size_t i = 1; i < parsedResult.points.size(); ++i)
    {
        double curlen = (parsedResult.points[i] - prev).length();
        while (prevlen + curlen >= len)
        {
            prev += (parsedResult.points[i] - prev) * ((len - prevlen) / curlen);
            ans.push_back(prev);
            curlen = (parsedResult.points[i] - prev).length();
            prevlen = 0.0;
        }

        prev = parsedResult.points[i];
        prevlen += curlen;
    }

    return ans;
}

std::vector<vasily::RobotData> TrajectoryManager::circlicMovement(
    const vasily::ParsedResult& parsedResult) const
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
