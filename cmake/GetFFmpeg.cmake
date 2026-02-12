# Assumed variables:
# BINARIES_DIR_NAME     | Name for the directory of external binaries
# BINARIES_DIR          | Directory to external binaries, assumes folder is created

# Setup paths
set(FFMPEG_URL "")
set(FFMPEG_FILE_NAME "ffmpeg")
set(FFMPEG_ARCHIVE_PATH "${BINARIES_DIR}/ffmpeg_archive")
set(FFMPEG_ARCHIVE_EXTRACTED_PATH "${BINARIES_DIR}/ffmpeg_archive_extracted")

if(WIN32)
	# Get the gyan.dev mirror from github as it downloads much quicker
	# Release files have version numbers though, so get latest archive name from github first
	# Get name rather than url for accessing the folder later
	execute_process(
		COMMAND curl -s https://api.github.com/repos/GyanD/codexffmpeg/releases/latest
		OUTPUT_VARIABLE FFMPEG_GYANDEV_LATEST_RELEASE_JSON
	)
	
	string(REGEX MATCH "\"name\": *\"(ffmpeg-[^\"]+-essentials_build)\\.zip\"" _match "${FFMPEG_GYANDEV_LATEST_RELEASE_JSON}")
	set(FFMPEG_GYANDEV_LATEST_RELEASE_NAME ${CMAKE_MATCH_1})
	
	set(FFMPEG_URL "https://github.com/GyanD/codexffmpeg/releases/latest/download/${FFMPEG_GYANDEV_LATEST_RELEASE_NAME}.zip")
	set(FFMPEG_FILE_NAME "${FFMPEG_FILE_NAME}.exe")
elseif(APPLE)
	set(FFMPEG_URL "https://evermeet.cx/ffmpeg/get/zip")
elseif(UNIX)
	# Latest build with master has tag "latest"
	set(FFMPEG_URL "https://github.com/BtbN/FFmpeg-Builds/releases/download/latest/ffmpeg-master-latest-linux64-gpl.tar.xz")
endif()

set(FFMPEG_PATH "${BINARIES_DIR}/${FFMPEG_FILE_NAME}")

# Download ffmpeg
if(NOT EXISTS ${FFMPEG_PATH})
	# Download ffmpeg archive
	message(STATUS "Downloading ffmpeg archive...")
	file(DOWNLOAD ${FFMPEG_URL} ${FFMPEG_ARCHIVE_PATH} SHOW_PROGRESS)
	message(STATUS "Downloaded ffmpeg archive to ${FFMPEG_ARCHIVE_PATH}")
	
	# Extract archive
	message(STATUS "Extracting ffmpeg...")
	file(ARCHIVE_EXTRACT
		INPUT "${FFMPEG_ARCHIVE_PATH}"
		DESTINATION "${FFMPEG_ARCHIVE_EXTRACTED_PATH}"
	)
	message(STATUS "Extracted ffmpeg to ${FFMPEG_ARCHIVE_EXTRACTED_PATH}")
	
	# Copy ffmpeg from archive to binaries folder
	message(STATUS "Copying ffmpeg to binaries...")
	if(WIN32)
		# Use release name from earlier to access top folder
		file(COPY "${FFMPEG_ARCHIVE_EXTRACTED_PATH}/${FFMPEG_GYANDEV_LATEST_RELEASE_NAME}/bin/ffmpeg.exe" DESTINATION "${BINARIES_DIR}")
	elseif(APPLE)
		file(COPY "${FFMPEG_ARCHIVE_EXTRACTED_PATH}/ffmpeg" DESTINATION "${BINARIES_DIR}")
	elseif(UNIX)
		file(COPY "${FFMPEG_ARCHIVE_EXTRACTED_PATH}/ffmpeg-master-latest-linux64-gpl/bin/ffmpeg" DESTINATION "${BINARIES_DIR}")
		
		# Give executable permissions
		file(CHMOD ${FFMPEG_PATH} PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)
	endif()
	
	# Remove archive folders
	message(STATUS "Removing ffmpeg archives...")
	file(REMOVE_RECURSE "${FFMPEG_ARCHIVE_EXTRACTED_PATH}")
	file(REMOVE "${FFMPEG_ARCHIVE_PATH}")
endif()

set(FFMPEG_PATH_RELATIVE "${BINARIES_DIR_NAME}/${FFMPEG_FILE_NAME}")

# Copy to folder after build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory
		"$<TARGET_FILE_DIR:${PROJECT_NAME}>/${BINARIES_DIR_NAME}"

	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${FFMPEG_PATH}"
		"$<TARGET_FILE_DIR:${PROJECT_NAME}>/${BINARIES_DIR_NAME}"
)

# Make path available in the code
add_compile_definitions(${PROJECT_NAME} PRIVATE
	FFMPEG_PATH_RELATIVE="${FFMPEG_PATH_RELATIVE}"
)
