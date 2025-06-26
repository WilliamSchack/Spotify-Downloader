#ifndef PLAYLISTFILETRACK_H
#define PLAYLISTFILETRACK_H

#include <QString>

struct PlaylistFileTrack {
	public:
		// File Paths
		QString AbsolutePath;
		QString EncodedPath;
	
		// Track Details
		QString Title;
		QString Artist;
		QString Album;
		QString Comment;
		unsigned int TrackNumber;
		int DurationSeconds;
		int DurationMilliseconds;
};

#endif