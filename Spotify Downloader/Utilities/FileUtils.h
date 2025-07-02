#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <filesystem>

#include <QString>
#include <QRegularExpression>

#include <taglib/tfile.h>

class FileUtils {
	public:
		enum class FilePathError {
			None,
			StartsWithDirectory,
			EndsWithDirectory,
			ContainsDoubleSlashes,
			InvalidSlashes
		};

		static QString ValidateFileName(QString fileName);
		static QString ValidateFolderName(QString folderName);

		static FilePathError CheckInputtedFilePathErrors(QString filePath);

		static QString AnonymizeFilePath(QString filePath);
		static QString EncodeFilePath(QString filePath);

		static std::filesystem::path ToNativeFilePath(QString filePath);
		static TagLib::FileName ToNativeFilePathTagLib(QString filePath);

		static QString AddDirectoryBackspaces(QString filePath);
};

#endif