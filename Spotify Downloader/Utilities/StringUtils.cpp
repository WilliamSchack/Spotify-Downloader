#include "StringUtils.h"

QString StringUtils::ValidateFileName(QString string) {
    QString copiedString = QString(string);
	QString invalidChars = R"(<>:"/\|?*)";
	foreach(QChar c, invalidChars) {
        copiedString.remove(c);
	}
	return copiedString;
}

QString StringUtils::ValidateFolderName(QString string) {
    QString copiedString = QString(string);
    QString invalidChars = R"(<>:"|?*)";
    foreach(QChar c, invalidChars) {
        copiedString.remove(c);
    }
    return copiedString;
}

StringUtils::FilePathError StringUtils::CheckInputtedFilePathErrors(QString string) {
    if (string.startsWith("/") || string.startsWith("\\")) return StringUtils::FilePathError::StartsWithDirectory;
    if (string.endsWith("/") || string.endsWith("\\"))     return StringUtils::FilePathError::EndsWithDirectory;
    if (string.contains("//") || string.contains("\\\\"))  return StringUtils::FilePathError::ContainsDoubleSlashes;
    if (string.contains("/\\") || string.contains("\\/"))  return StringUtils::FilePathError::InvalidSlashes;
    return StringUtils::FilePathError::None;
}

// Remove user name from file path
QString StringUtils::AnonymizeFilePath(QString string) {
    return QString(string).replace(QRegularExpression("/Users/[^/]+/"), "/Users/Anonymous/");
}

// Use over QUrl::toPercentEncoding as that also encodes "/"
QString StringUtils::EncodeFilePath(QString string) {
    return QString(string).replace(QRegularExpression("[\\s]+"), "%20");
}

std::filesystem::path StringUtils::ToNativeFilePath(QString string) {
#if _WIN32
    // Return wstring on windows
    return string.toStdWString();
#else
    // Return string on unix
    return string.toUtf8().toStdString();
#endif
}

TagLib::FileName StringUtils::ToNativeFilePathTagLib(QString string) {
#if _WIN32
    // Return wstring on windows
    return string.toStdWString().c_str();
#else
    // Return string on unix
    return string.toUtf8().toStdString().c_str();
#endif
}

// Bit of a brute force but I doubt views will exceed the trillions 
int StringUtils::StringNumberToInt(QString string) {
	if (string.contains("K"))
		return QString(string).replace("K", "").toInt() * 1000;
	if (string.contains("M"))
		return QString(string).replace("M", "").toInt() * 1000000;
	if (string.contains("B"))
		return QString(string).replace("B", "").toInt() * 1000000000;
	if (string.contains("T"))
		return QString(string).replace("T", "").toInt() * 1000000000000;
	return QString(string).toInt(); // Just incase not returned before
}

// Levenshtein Distance Algorithim From Geeks For Geeks
// https://www.geeksforgeeks.org/introduction-to-levenshtein-distance/

float StringUtils::LevenshteinDistanceSimilarity(QString s1, QString s2) {
    int m = s1.length();
    int n = s2.length();

    std::vector<int> prevRow(n + 1, 0);
    std::vector<int> currRow(n + 1, 0);

    for (int j = 0; j <= n; j++) {
        prevRow[j] = j;
    }

    for (int i = 1; i <= m; i++) {
        currRow[0] = i;

        for (int j = 1; j <= n; j++) {
            if (s1[i - 1] == s2[j - 1]) {
                currRow[j] = prevRow[j - 1];
            }
            else {
                currRow[j] = 1
                    + std::min(

                        // Insert
                        currRow[j - 1],
                        std::min(

                            // Remove
                            prevRow[j],

                            // Replace
                            prevRow[j - 1]));
            }
        }

        prevRow = currRow;
    }

    // Normalize distance to similarity
    float maxLength = std::max(m, n);
    float dist = (maxLength - currRow[n]) / maxLength;

    return dist;
}