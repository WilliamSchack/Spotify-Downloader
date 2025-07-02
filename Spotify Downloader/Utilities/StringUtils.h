#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <QString>

class StringUtils {
	public:
		static int StringNumberToInt(QString string);

		static float LevenshteinDistanceSimilarity(QString s1, QString s2);
};

#endif
