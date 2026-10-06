#include "MathUtils.h"

double MathUtils::Lerp(const double& a, const double& b, const double& t)
{
    return a * (1.0 - t) + (b * t);
}

bool MathUtils::FloatsEqual(const float& a, const float& b)
{
    return std::fabs(a - b) <= std::numeric_limits<float>::epsilon() * std::max(std::fabs(a), std::fabs(b));
}

bool MathUtils::DoublesEqual(const double& a, const double& b)
{
    return std::abs(a - b) <= std::numeric_limits<double>::epsilon() * std::max(std::abs(a), std::abs(b));
}