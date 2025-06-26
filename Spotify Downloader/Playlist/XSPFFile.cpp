#include "XSPFFile.h"

QString XSPFFile::WriteHeader() {
	return
		R"(<?xml version="1.0" encoding="UTF-8"?>)" "\n"
		R"(<playlist xmlns="http://xspf.org/ns/0/" version="1">)" "\n"
		"\t<trackList>\n";
}

QString XSPFFile::WriteFile(PlaylistFileTrack track) {
	// Start file tags
	QString fileOutput = "\t\t<track>\n";

	// Write file details
	fileOutput += QString("\t\t\t<location>%1</location>\n").arg(track.EncodedPath);
	fileOutput += QString("\t\t\t<title>%1</title>\n").arg(track.Title);
	fileOutput += QString("\t\t\t<creator>%1</creator>\n").arg(track.Artist);
	fileOutput += QString("\t\t\t<album>%1</album>\n").arg(track.Album);
	fileOutput += QString("\t\t\t<annotation>%1</annotation>\n").arg(track.Comment);
	if(track.TrackNumber != 0) fileOutput += QString("\t\t\t<trackNum>%1</trackNum>\n").arg(track.TrackNumber);
	fileOutput += QString("\t\t\t<duration>%1</duration>\n").arg(track.DurationMilliseconds);

	// End file tags
	fileOutput += "\t\t</track>\n";

	return fileOutput;
}

QString XSPFFile::WriteFooter() {
	return
		"\t</trackList>\n"
		"</playlist>";
}