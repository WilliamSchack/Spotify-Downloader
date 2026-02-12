# Assumed variables:
# BINARIES_DIR_NAME     | Name for the directory of external binaries
# BINARIES_DIR          | Directory to external binaries, assumes folder is created
# 
# Gets the quickjs prebuilt binary
# (Required for ytdlp, using this over others as its only ~2mb)

# Get latest version, download the json file and parse version with regex
set(QUICKJS_VERSION_JSON_PATH "${BINARIES_DIR}/quickJsVersion.json")

if(NOT EXISTS ${QUICKJS_VERSION_JSON_PATH})
	file(DOWNLOAD
		"https://bellard.org/quickjs/binary_releases/LATEST.json"
		"${QUICKJS_VERSION_JSON_PATH}"
		STATUS latestJsonGetStatus
	)
endif()

file(READ "${QUICKJS_VERSION_JSON_PATH}" QUICKJS_VERSION_JSON)
string(REGEX MATCH "\"version\":\"([^\"]+)\"" match "${QUICKJS_VERSION_JSON}")
set(QUICKJS_VERSION ${CMAKE_MATCH_1})

# Get url
set(QUICKJS_URL "https://bellard.org/quickjs/binary_releases/quickjs")
set(QUICKJS_FILE_NAME "qjs")

if(WIN32)
	set(QUICKJS_URL "${QUICKJS_URL}-win-x86_64-${QUICKJS_VERSION}")
	set(QUICKJS_FILE_NAME "${QUICKJS_FILE_NAME}.exe")
elseif(APPLE)
	set(QUICKJS_URL "${QUICKJS_URL}-cosmo-${QUICKJS_VERSION}")
elseif(UNIX)
	set(QUICKJS_URL "${QUICKJS_URL}-linux-x86_64-${QUICKJS_VERSION}")
endif()

set(QUICKJS_URL "${QUICKJS_URL}.zip")
set(QUICKJS_PATH "${BINARIES_DIR}/${QUICKJS_FILE_NAME}")
set(QUICKJS_ARCHIVE_PATH "${BINARIES_DIR}/quickjs_archive")
set(QUICKJS_ARCHIVE_EXTRACTED_PATH "${BINARIES_DIR}/quickjs_archive_extracted")

# Download
if(NOT EXISTS ${QUICKJS_PATH})
	# Download archive
	message(STATUS "Downloading quickjs archive...")
	file(DOWNLOAD ${QUICKJS_URL} ${QUICKJS_ARCHIVE_PATH} SHOW_PROGRESS)

	# Extract archive
	message(STATUS "Extracting quickjs...")
	file(ARCHIVE_EXTRACT
		INPUT "${QUICKJS_ARCHIVE_PATH}"
		DESTINATION "${QUICKJS_ARCHIVE_EXTRACTED_PATH}"
	)

	# Copy from archive to binaries folder
	message(STATUS "Copying quickjs to binaries...")
	file(COPY "${QUICKJS_ARCHIVE_EXTRACTED_PATH}/${QUICKJS_FILE_NAME}" DESTINATION "${BINARIES_DIR}")
	
	# Give executable permissions
	if(UNIX)
		file(CHMOD ${QUICKJS_PATH} PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)
	endif()

	# Remove archive folders
	message(STATUS "Removing ffmpeg archive...")
	file(REMOVE_RECURSE "${QUICKJS_ARCHIVE_EXTRACTED_PATH}")
	file(REMOVE "${QUICKJS_ARCHIVE_PATH}")
endif()

set(QUICKJS_PATH_RELATIVE "${BINARIES_DIR_NAME}/${QUICKJS_FILE_NAME}")

# Copy to folder after build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory
		"$<TARGET_FILE_DIR:${PROJECT_NAME}>/${BINARIES_DIR_NAME}"

	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${QUICKJS_PATH}"
		"$<TARGET_FILE_DIR:${PROJECT_NAME}>/${BINARIES_DIR_NAME}"
)

# Make path available in the code
add_compile_definitions(${PROJECT_NAME} PRIVATE
	QUICKJS_PATH_RELATIVE="${QUICKJS_PATH_RELATIVE}"
)