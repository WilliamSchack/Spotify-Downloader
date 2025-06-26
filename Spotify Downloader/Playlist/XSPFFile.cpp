#include "XSPFFile.h"

QString XSPFFile::WriteHeader() {
	return
		R"(<?xml version="1.0" encoding="UTF-8"?>)" "\n"
		R"(<playlist xmlns="http://xspf.org/ns/0/" version="1">)" "\n"
		"\t<trackList>\n";
}

QString XSPFFile::WriteFile(QFileInfo fileInfo) {
	// Get file path
	QString absolutePath = fileInfo.absoluteFilePath();
	QString encodedPath = StringUtils::EncodeFilePath(QUrl::fromLocalFile(absolutePath).toString());

	// Get file details
	TagLib::FileName tagFileName(reinterpret_cast<const wchar_t*>(absolutePath.constData()));
	TagLib::FileRef tagFileRef(tagFileName, true, TagLib::AudioProperties::Accurate);

	std::string title = tagFileRef.tag()->title().toCString();
	std::string artist = tagFileRef.tag()->artist().toCString();
	std::string album = tagFileRef.tag()->album().toCString();
	std::string comment = tagFileRef.tag()->comment().toCString();
	unsigned int trackNum = tagFileRef.tag()->track();
	int durationSeconds = tagFileRef.audioProperties()->lengthInSeconds();

	// Start file tags
	QString fileOutput = "\t\t<track>\n";

	// Write file details
	fileOutput += QString("\t\t\t<location>%1</location>\n").arg(encodedPath);
	fileOutput += QString("\t\t\t<title>%1</title>\n").arg(QString::fromLocal8Bit(title.c_str()));
	fileOutput += QString("\t\t\t<creator>%1</creator>\n").arg(QString::fromLocal8Bit(artist.c_str()));
	fileOutput += QString("\t\t\t<album>%1</album>\n").arg(QString::fromLocal8Bit(album.c_str()));
	if(trackNum != 0) fileOutput += QString("\t\t\t<trackNum>%1</trackNum>\n").arg(trackNum);
	fileOutput += QString("\t\t\t<duration>%1</duration>\n").arg(durationSeconds);

	// End file tags
	fileOutput += "\t\t</track>\n";

	return fileOutput;
}

QString XSPFFile::WriteFooter() {
	return
		"\t</trackList>\n"
		"</playlist>";
}