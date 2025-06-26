#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <QString>

#include <QRegularExpression>

class StringUtils {
	public:
		static QString ValidateFileName(QString string);
		static QString ValidateFolderName(QString string);

		static QString AnonymizeFilePath(QString string);
		static QString EncodeFilePath(QString string);

		static int StringNumberToInt(QString string);

		static float LevenshteinDistanceSimilarity(QString s1, QString s2);
};

#endif
