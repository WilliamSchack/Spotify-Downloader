#ifndef IPLAYLISTFILE_H
#define IPLAYLISTFILE_H

#include "PlaylistFileTrack.h"
#include "MetadataManager.h"
#include "EExtension.h"
#include "FileUtils.h"
#include "StringUtils.h"
#include "VectorUtils.h"

#include <vector>
#include <iostream>
#include <fstream>

class IPlaylistFile {
	public:
		// Creates a playlist file using all the valid songs within the parent folder
		void CreatePlaylistFileFromTracks(const std::vector<std::filesystem::path>& trackFilePaths, const std::string& outputPathWithoutExtension);
		void CreatePlaylistFileFromDirectory(const std::filesystem::path& tracksFolder, const std::string& outputPathWithoutExtension);
	protected:
		virtual std::string GetExtension() = 0;
		virtual std::string WriteHeader() = 0;
		virtual std::string WriteFile(const PlaylistFileTrack& track) = 0;
		virtual std::string WriteFooter() = 0;
	private:
		inline static std::vector<std::string> _filtersCache;
	private:
		static std::vector<std::string> GetFilters();
};

#endif