#include "MathUtils.h"

double MathUtils::Lerp(const double& a, const double& b, const double& t)
{
    return a * (1.0 - t) + (b * t);
}