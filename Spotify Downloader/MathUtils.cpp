#include "MathUtils.h"

float MathUtils::Lerp(float a, float b, float t) {
	return a * (1.0 - t) + (b * t);
}

double MathUtils::LerpInList(std::vector<double> list, int index) {
	double maxVal = 0;
	for (int i = 0; i < list.size(); i++) {
		double val = list[i];
		if (val > maxVal) maxVal = val;
	}

	return list[0] / maxVal;
}