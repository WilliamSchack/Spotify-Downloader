#include "StringUtils.h"

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