#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <cmath>
#include <algorithm>
#include <limits>

class MathUtils
{
    public:
        static double Lerp(const double& a, const double& b, const double& t);
        static bool FloatsEqual(const float& a, const float& b);
        static bool DoublesEqual(const double& a, const double& b);
};

#endif