#include "PLSFile.h"

QString PLSFile::WriteHeader() {
	_currentFile = 1;
	return "[playlist]\n\n";
}

QString PLSFile::WriteFile(PlaylistFileTrack track) {
	QString fileOutput = QString(
		"File%1=%2\n"
		"Title%1=%3\n"
		"Length%1=%4\n\n"
	).arg(_currentFile)
		.arg(track.EncodedPath)
		.arg(track.Title)
		.arg(track.DurationSeconds);

	_currentFile++;
	return fileOutput;
}

QString PLSFile::WriteFooter() {
	return QString(
		"NumberOfEntries=%1\n"
		"Version=2"
	).arg(_currentFile - 1);
}