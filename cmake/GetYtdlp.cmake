# Assumed variables:
# BINARIES_DIR_NAME     | Name for the directory of external binaries
# BINARIES_DIR          | Directory to external binaries, assumes folder is created

# Setup paths
set(YTDLP_URL "https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp")
set(YTDLP_FILE_NAME "yt-dlp")

if(WIN32)
	set(YTDLP_URL "${YTDLP_URL}.exe")
	set(YTDLP_FILE_NAME "${YTDLP_FILE_NAME}.exe")
elseif(APPLE)
	set(YTDLP_URL "${YTDLP_URL}_macos")
	set(YTDLP_FILE_NAME "${YTDLP_FILE_NAME}_macos")
elseif(UNIX)
	set(YTDLP_URL "${YTDLP_URL}_linux")
	set(YTDLP_FILE_NAME "${YTDLP_FILE_NAME}_linux")
endif()

set(YTDLP_PATH "${BINARIES_DIR}/${YTDLP_FILE_NAME}")

# Download
if(NOT EXISTS ${YTDLP_PATH})
	message(STATUS "Downloading ytdlp...")
	file(DOWNLOAD ${YTDLP_URL} ${YTDLP_PATH} SHOW_PROGRESS)
	message(STATUS "Downloaded ytdlp to ${YTDLP_PATH}")
	
	# Give executable permissions
	if(UNIX)
		file(CHMOD ${YTDLP_PATH} PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)
	endif()
endif()

set(YTDLP_PATH_RELATIVE "${BINARIES_DIR_NAME}/${YTDLP_FILE_NAME}")

# Copy to folder after build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory
		"$<TARGET_FILE_DIR:${PROJECT_NAME}>/${BINARIES_DIR_NAME}"

	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${YTDLP_PATH}"
		"$<TARGET_FILE_DIR:${PROJECT_NAME}>/${BINARIES_DIR_NAME}"
)

# Make path available in the code
add_compile_definitions(${PROJECT_NAME} PRIVATE
	YTDLP_PATH_RELATIVE="${YTDLP_PATH_RELATIVE}"
)