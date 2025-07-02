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

		static QString ValidateFileName(QString string);
		static QString ValidateFolderName(QString string);

		static FilePathError CheckInputtedFilePathErrors(QString string);

		static QString AnonymizeFilePath(QString string);
		static QString EncodeFilePath(QString string);

		static std::filesystem::path ToNativeFilePath(QString string);
		static TagLib::FileName ToNativeFilePathTagLib(QString string);
};

#endif