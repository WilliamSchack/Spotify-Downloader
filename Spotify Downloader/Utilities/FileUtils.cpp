#include "FileUtils.h"

QString FileUtils::ValidateFileName(QString fileName) {
    QString copiedString = QString(fileName);
    QString invalidChars = R"(<>:"/\|?*)";
    foreach(QChar c, invalidChars) {
        copiedString.remove(c);
    }
    return copiedString;
}

QString FileUtils::ValidateFolderName(QString folderName) {
    QString copiedString = QString(folderName);
    QString invalidChars = R"(<>:"|?*)";
    foreach(QChar c, invalidChars) {
        copiedString.remove(c);
    }
    return copiedString;
}

FileUtils::FilePathError FileUtils::CheckInputtedFilePathErrors(QString filePath) {
    if (filePath.startsWith("/") || filePath.startsWith("\\")) return FileUtils::FilePathError::StartsWithDirectory;
    if (filePath.endsWith("/") || filePath.endsWith("\\"))     return FileUtils::FilePathError::EndsWithDirectory;
    if (filePath.contains("//") || filePath.contains("\\\\"))  return FileUtils::FilePathError::ContainsDoubleSlashes;
    if (filePath.contains("/\\") || filePath.contains("\\/"))  return FileUtils::FilePathError::InvalidSlashes;
    return FileUtils::FilePathError::None;
}

// Remove user name from file path
QString FileUtils::AnonymizeFilePath(QString filePath) {
    return QString(filePath).replace(QRegularExpression("/Users/[^/]+/"), "/Users/Anonymous/");
}

// Use over QUrl::toPercentEncoding as that also encodes "/"
QString FileUtils::EncodeFilePath(QString filePath) {
    return QString(filePath).replace(QRegularExpression("[\\s]+"), "%20");
}

std::filesystem::path FileUtils::ToNativeFilePath(QString filePath) {
#if _WIN32
    // Return wstring on windows
    return filePath.toStdWString();
#else
    // Return string on unix
    return filePath.toUtf8().toStdString();
#endif
}

TagLib::FileName FileUtils::ToNativeFilePathTagLib(QString filePath) {
#if _WIN32
    // Return wstring on windows
    return filePath.toStdWString().c_str();
#else
    // Return string on unix
    return filePath.toUtf8().toStdString().c_str();
#endif
}

// Implements the "../" in file paths
QString FileUtils::AddDirectoryBackspaces(QString filePath) {
    QString copiedFilePath = QString(filePath);

    // For simplicity, replace all "\" with "/"
    copiedFilePath.replace("\\", "/");

    while (copiedFilePath.contains("..")) {
        // Get the index of the backspace
        int indexOfBackspace = copiedFilePath.indexOf("..");
        int endOfBackspaceIndex = indexOfBackspace + 2;

        // Find the previous two slashes
        int previousSlashIndex = copiedFilePath.mid(0, indexOfBackspace).lastIndexOf("/");
        int secondPreviousSlashIndex = copiedFilePath.mid(0, previousSlashIndex).lastIndexOf("/");

        // If either of the previous slashes are -1, remove this backspace and continue to the next
        if (previousSlashIndex == -1 || secondPreviousSlashIndex == -1) {
            copiedFilePath = copiedFilePath.mid(endOfBackspaceIndex + 1, copiedFilePath.length() - endOfBackspaceIndex);
            continue;
        }

        // Remove from the previous directory to the end of the backspace
        QString newPath = copiedFilePath.mid(0, secondPreviousSlashIndex);
        newPath += copiedFilePath.mid(endOfBackspaceIndex, copiedFilePath.length() - endOfBackspaceIndex);
        copiedFilePath = newPath;
    }

    return copiedFilePath;
}