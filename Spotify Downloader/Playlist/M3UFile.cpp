#include "M3UFile.h"

QString M3UFile::WriteHeader() {
	return "#EXTM3U\n";
}

QString M3UFile::WriteFile(QFileInfo fileInfo) {
	// Get file path
	QString absolutePath = fileInfo.absoluteFilePath();
	QString encodedPath = StringUtils::EncodeFilePath(QUrl::fromLocalFile(absolutePath).toString());

	// Get duration in seconds
	TagLib::FileName tagFileName(reinterpret_cast<const wchar_t*>(absolutePath.constData()));
	TagLib::FileRef tagFileRef(tagFileName, true, TagLib::AudioProperties::Accurate);

	std::string title = tagFileRef.tag()->title().toCString();
	std::string artist = tagFileRef.tag()->artist().toCString();
	int durationSeconds = tagFileRef.audioProperties()->lengthInSeconds();

	// File Details
	QString fileOutput = QString("#EXTINF:%1,%2 - %3\n")
		.arg(QString::number(durationSeconds))
		.arg(QString::fromLocal8Bit(artist.c_str()))
		.arg(QString::fromLocal8Bit(title.c_str()));

	// File Path
	fileOutput.append(QString("%1\n").arg(encodedPath));

	return fileOutput;
}

QString M3UFile::WriteFooter() {
	// No footer for m3u file
	return "";
}