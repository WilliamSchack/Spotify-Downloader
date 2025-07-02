#include "PlaylistFile.h"

void PlaylistFile::CreatePlaylistFileFromTracks(QStringList trackFilePaths, QString outputPathWithoutExtension) {
	// Create file
	QString fullOutputPath = QString("%1.%2").arg(outputPathWithoutExtension).arg(Extension());
	QFile outputFile(fullOutputPath);
	outputFile.open(QIODevice::WriteOnly | QIODevice::Text);

	// Create stream
	QTextStream out(&outputFile);

	// Write header to the playlist
	out << WriteHeader();

	// Get all the tracks from the given paths
	QList<PlaylistFileTrack> tracks;
	foreach(QString trackFilePath, trackFilePaths) {
		// Get the file path
		QString absolutePath = trackFilePath;
		QString encodedPath = StringUtils::EncodeFilePath(QUrl::fromLocalFile(absolutePath).toString());

		// Skip if the file doesnt exist
		QFileInfo trackFileInfo(absolutePath);
		if (!trackFileInfo.exists() || !trackFileInfo.isFile())
			continue;

		// Get the file details
		TagLib::FileRef tagFileRef(StringUtils::ToNativeFilePathTagLib(absolutePath), true, TagLib::AudioProperties::Accurate);

		std::string title = tagFileRef.tag()->title().to8Bit(true).c_str();
		std::string artist = tagFileRef.tag()->artist().to8Bit(true).c_str();
		std::string album = tagFileRef.tag()->album().to8Bit(true).c_str();
		std::string comment = tagFileRef.tag()->comment().to8Bit(true).c_str();
		unsigned int trackNum = tagFileRef.tag()->track();
		int durationSeconds = tagFileRef.audioProperties()->lengthInSeconds();
		int durationMilliseconds = tagFileRef.audioProperties()->lengthInMilliseconds();

		// Create a PlaylistFileTrack
		PlaylistFileTrack track{
			absolutePath,
			encodedPath,
			QString::fromUtf8(title),
			QString::fromUtf8(artist),
			QString::fromUtf8(album),
			QString::fromUtf8(comment),
			trackNum,
			durationSeconds,
			durationMilliseconds
		};

		// Add file to the list
		tracks.append(track);
	}

	// Sort files by their track number
	std::sort(tracks.begin(), tracks.end(), [](const PlaylistFileTrack t1, const PlaylistFileTrack t2) -> bool {
		return (t1.TrackNumber < t2.TrackNumber);
		});

	// Write sorted files
	foreach(PlaylistFileTrack track, tracks) {
		out << WriteFile(track);
	}

	// Write footer to the playlist
	out << WriteFooter();

	// Save and close file
	out.flush();
	outputFile.close();
}

void PlaylistFile::CreatePlaylistFileFromDirectory(QString tracksFolder, QString outputPathWithoutExtension) {
	// Get all the files in the given directory and add to a list
	QStringList filePaths;
	QDirIterator iterator(tracksFolder, GetFilters(), QDir::Files, QDirIterator::Subdirectories);
	while (iterator.hasNext()) {
		filePaths.append(iterator.next());
	}

	// Create the playlist file
	CreatePlaylistFileFromTracks(filePaths, outputPathWithoutExtension);
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