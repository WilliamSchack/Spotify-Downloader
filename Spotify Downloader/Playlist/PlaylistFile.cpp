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
	// ^ REMOVE

	// Get all the files in the given directory and add to a list
	QList<PlaylistFileTrack> tracks;
	QDirIterator iterator(parentFolder, GetFilters(), QDir::Files, QDirIterator::Subdirectories);
	while (iterator.hasNext()) {
		// Get the file path
		QString absolutePath = iterator.next();
		QString encodedPath = StringUtils::EncodeFilePath(QUrl::fromLocalFile(absolutePath).toString());

		// Get the file details
		TagLib::FileName tagFileName(reinterpret_cast<const wchar_t*>(absolutePath.constData()));
		TagLib::FileRef tagFileRef(tagFileName, true, TagLib::AudioProperties::Accurate);

		std::string title = tagFileRef.tag()->title().toCString();
		std::string artist = tagFileRef.tag()->artist().toCString();
		std::string album = tagFileRef.tag()->album().toCString();
		std::string comment = tagFileRef.tag()->comment().toCString();
		unsigned int trackNum = tagFileRef.tag()->track();
		int durationSeconds = tagFileRef.audioProperties()->lengthInSeconds();
		int durationMilliseconds = tagFileRef.audioProperties()->lengthInMilliseconds();
		
		// Create a PlaylistFileTrack
		PlaylistFileTrack track {
			absolutePath,
			encodedPath,
			QString::fromLocal8Bit(title.c_str()),
			QString::fromLocal8Bit(artist.c_str()),
			QString::fromLocal8Bit(album.c_str()),
			QString::fromLocal8Bit(comment.c_str()),
			trackNum,
			durationSeconds,
			durationMilliseconds
		};

		// Add file to the list
		tracks.append(track);

		// Write file to the playlist
		//QFileInfo fileInfo(iterator.next());
		//out << WriteFile(fileInfo);
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