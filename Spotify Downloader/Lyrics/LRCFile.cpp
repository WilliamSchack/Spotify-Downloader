#include "LRCFile.h"

void LRCFile::CreateLRCFile(const QString& outputPathWithoutExtension, const Song& song, Lyrics& lyrics) {
	// Create file
	QString fullOutputPath = QString("%1.lrc").arg(outputPathWithoutExtension);
	QFile outputFile(fullOutputPath);

	// Check the directory exists
	QString outputWithForwardSlashes = QString(outputPathWithoutExtension).replace("\\", "/");
	QString parentOutputDirectoryString = outputWithForwardSlashes.mid(0, outputWithForwardSlashes.lastIndexOf("/"));

	// If the directory doesnt exist, create it
	if (!QDir(parentOutputDirectoryString).exists()) {
		QDir().mkpath(parentOutputDirectoryString);
	}

	// Create the file
	if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		// If could not create the playlist file, log error
		qWarning() << "Failed to create LRC file at the path:" << FileUtils::AnonymizeFilePath(fullOutputPath);
		return;
	}

	// Create stream
	QTextStream out(&outputFile);

	// Write header
	out << CreateHeader(song);
	out << "\n";

	// Write lyrics
	out << QString::fromStdString(lyrics.GetString());

	// Save and close file
	out.flush();
	outputFile.close();
}

QString LRCFile::CreateHeader(const Song& song) {
	QString header = "";

	QString timeMMSS = QDateTime::fromSecsSinceEpoch(song.TimeSeconds, Qt::UTC).toString("mm:ss");

	header += QString("[ar:%1]\n").arg(song.ArtistNames);
	header += QString("[al:%1]\n").arg(song.AlbumName);
	header += QString("[ti:%1]\n").arg(song.Title);
	header += QString("[length: %1]\n").arg(timeMMSS);

	return header;
}