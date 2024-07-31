#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <vector>

class MathUtils {
public:
	static float Lerp(float a, float b, float t);
	static double LerpInList(std::vector<double> list, int index);
};

#endif