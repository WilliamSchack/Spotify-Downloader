#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <QString>

#include <QRegularExpression>

class StringUtils {
	public:
		static QString ValidateString(QString string);

		static QString Anonymize(QString string);

		static int StringNumberToInt(QString string);

		static float LevenshteinDistanceSimilarity(QString s1, QString s2);
};

#endif
