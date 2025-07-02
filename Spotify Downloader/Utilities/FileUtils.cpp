#include "FileUtils.h"

QString FileUtils::ValidateFileName(QString string) {
    QString copiedString = QString(string);
    QString invalidChars = R"(<>:"/\|?*)";
    foreach(QChar c, invalidChars) {
        copiedString.remove(c);
    }
    return copiedString;
}

QString FileUtils::ValidateFolderName(QString string) {
    QString copiedString = QString(string);
    QString invalidChars = R"(<>:"|?*)";
    foreach(QChar c, invalidChars) {
        copiedString.remove(c);
    }
    return copiedString;
}

FileUtils::FilePathError FileUtils::CheckInputtedFilePathErrors(QString string) {
    if (string.startsWith("/") || string.startsWith("\\")) return FileUtils::FilePathError::StartsWithDirectory;
    if (string.endsWith("/") || string.endsWith("\\"))     return FileUtils::FilePathError::EndsWithDirectory;
    if (string.contains("//") || string.contains("\\\\"))  return FileUtils::FilePathError::ContainsDoubleSlashes;
    if (string.contains("/\\") || string.contains("\\/"))  return FileUtils::FilePathError::InvalidSlashes;
    return FileUtils::FilePathError::None;
}

// Remove user name from file path
QString FileUtils::AnonymizeFilePath(QString string) {
    return QString(string).replace(QRegularExpression("/Users/[^/]+/"), "/Users/Anonymous/");
}

// Use over QUrl::toPercentEncoding as that also encodes "/"
QString FileUtils::EncodeFilePath(QString string) {
    return QString(string).replace(QRegularExpression("[\\s]+"), "%20");
}

std::filesystem::path FileUtils::ToNativeFilePath(QString string) {
#if _WIN32
    // Return wstring on windows
    return string.toStdWString();
#else
    // Return string on unix
    return string.toUtf8().toStdString();
#endif
}

TagLib::FileName FileUtils::ToNativeFilePathTagLib(QString string) {
#if _WIN32
    // Return wstring on windows
    return string.toStdWString().c_str();
#else
    // Return string on unix
    return string.toUtf8().toStdString().c_str();
#endif
}