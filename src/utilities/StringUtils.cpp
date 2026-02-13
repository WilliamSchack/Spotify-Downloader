#include "StringUtils.h"

bool StringUtils::StartsWith(const std::string& string, const std::string& value)
{
    return string.rfind(value, 0) == 0;
}

bool StringUtils::Contains(const std::string& string, const std::string& value)
{
    return string.find(value) != std::string::npos;
}

void StringUtils::ToLower(std::string& string)
{
    std::transform(string.begin(), string.end(), string.begin(),
        [](unsigned char c){ return std::tolower(c); });
}

std::string StringUtils::ReplaceAll(std::string string, const std::string& from, const std::string& to)
{
    size_t start_pos = 0;
    while((start_pos = string.find(from, start_pos)) != std::string::npos) {
        string.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }

    return string;
}

void StringUtils::RemoveChar(std::string& string, const char& c)
{
    string.erase(std::remove(string.begin(), string.end(), c), string.end());
}

std::string StringUtils::RemoveLast(const std::string& string)
{
    return string.substr(0, string.size() - 1);
}

std::vector<std::string> StringUtils::Split(const std::string& string, const std::string& delimiter)
{
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = string.find(delimiter);
    
    while (end != std::string::npos) {
        tokens.push_back(string.substr(start, end - start));
        start = end + delimiter.size();
        end = string.find(delimiter, start);
    }
    
    tokens.push_back(string.substr(start));
    return tokens;
}

// Levenshtein Distance Algorithim From Geeks For Geeks
// https://www.geeksforgeeks.org/introduction-to-levenshtein-distance/
double StringUtils::LevenshteinDistanceSimilarity(const std::string& s1, const std::string& s2)
{
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
                            prevRow[j - 1]
                        )
                    );
            }
        }

        prevRow = currRow;
    }

    // Normalize distance to similarity
    double maxLength = std::max(m, n);
    double dist = (maxLength - currRow[n]) / maxLength;

    return dist;
}

// Changes time formatted like x:x:... to seconds
unsigned int StringUtils::TimeToSeconds(const std::string& string)
{
    std::vector<std::string> seperated = Split(string, ":");
	int seconds = 0;

    // Make sure the strings are numbers
    for(std::string string : seperated) {
        bool hasOnlyDigits = string.find_first_not_of("0123456789") == std::string::npos;
        if (!hasOnlyDigits)
            return 0;
    }

	switch (seperated.size()) {
		case 1:
			seconds = std::stoi(seperated[0]);
			break;
		case 2:
			seconds = std::stoi(seperated[0]) * 60 + std::stoi(seperated[1]);
			break;
		case 3:
			seconds = std::stoi(seperated[0]) * 3600 + std::stoi(seperated[1]) * 60 + std::stoi(seperated[2]);
			break;
		case 4:
			seconds = std::stoi(seperated[0]) * 86400 + std::stoi(seperated[1]) * 3600 + std::stoi(seperated[2]) * 60 + std::stoi(seperated[3]);
			break;
        default:
            return 0;
	}

	return seconds;
}

unsigned int StringUtils::TimeToMilliseconds(const std::string& string)
{
    std::vector<std::string> seperated = Split(string, ".");
    if (seperated.size() != 2) return 0;
    
    unsigned int seconds = TimeToSeconds(seperated[0]);
    unsigned int milliseconds = seconds * 1000;
    milliseconds += std::stoi(seperated[1]) * 10;

    return milliseconds;
};