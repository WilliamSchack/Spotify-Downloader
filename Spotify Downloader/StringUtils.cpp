#include "StringUtils.h"

QString StringUtils::ValidateString(QString string) {
	QString invalidChars = R"(<>:"/\|?*)";
	foreach(QChar c, invalidChars) {
		string.remove(c);
	}
	return string;
}

// Bit of a brute force but I doubt views will exceed the trillions 
int StringUtils::StringNumberToInt(QString string) {
	if (string.contains("K"))
		return string.replace("K", "").toInt() * 1000;
	if (string.contains("M"))
		return string.replace("M", "").toInt() * 1000000;
	if (string.contains("B"))
		return string.replace("B", "").toInt() * 1000000000;
	if (string.contains("T"))
		return string.replace("T", "").toInt() * 1000000000000;
	return string.toInt(); // Just incase not returned before
}