#include "PlaylistFile.h"

void PlaylistFile::CreatePlaylistFile(QString parentFolder, QString outputPathWithoutExtension) {
	// Create file
	QString fullOutputPath = QString("%1.%2").arg(outputPathWithoutExtension).arg(Extension());
	QFile outputFile(fullOutputPath);
	outputFile.open(QIODevice::WriteOnly | QIODevice::Text);
	
	// Create stream
	QTextStream out(&outputFile);

	// Write header to the playlist
	out << WriteHeader();

	// Iterate through the parent folder and look for valid songs
	QDirIterator iterator(parentFolder, GetFilters(), QDir::Files, QDirIterator::Subdirectories);
	while (iterator.hasNext()) {
		// Write file to the playlist
		QFileInfo fileInfo(iterator.next());
		out << WriteFile(fileInfo);
	}

	// Write footer to the playlist
	out << WriteFooter();

	// Save and close file
	out.flush();
	outputFile.close();
}

QStringList PlaylistFile::GetFilters() {
	// Return cache if calculated before
	if (!_filtersCache.isEmpty())
		return _filtersCache;

	// Iterate through codecs in Codec and add them to a list
	QStringList filters;
	foreach (Codec::ExtensionData extension, Codec::Data.values()) {
		filters << QString("*.%1").arg(extension.String);
	}

	_filtersCache = filters;
	return filters;
}