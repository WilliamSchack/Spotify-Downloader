#include "M3UFile.h"

QString M3UFile::WriteHeader() {
	return "#EXTM3U\n";
}

QString M3UFile::WriteFile(PlaylistFileTrack track) {
	// File Details
	QString fileOutput = QString("#EXTINF:%1,%2 - %3\n")
		.arg(track.DurationSeconds)
		.arg(track.Artist)
		.arg(track.Title);

	// File Path
	fileOutput.append(QString("%1\n").arg(track.EncodedPath));

	return fileOutput;
}

QString M3UFile::WriteFooter() {
	// No footer for m3u file
	return "";
}