# Assumed variables:
# BINARIES_DIR          | Directory to external binaries, assumes folder is created
# POST_BUILD_COPY_FILES | List of files to copy to final build

# Setup paths
set(YTDLP_URL "https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp")
set(YTDLP_PATH "${BINARIES_DIR}/yt-dlp")

if(WIN32)
	set(YTDLP_URL "${YTDLP_URL}.exe")
	set(YTDLP_PATH "${YTDLP_PATH}.exe")
elseif(APPLE)
	set(YTDLP_URL "${YTDLP_URL}_macos")
	set(YTDLP_PATH "${YTDLP_PATH}_macos")
elseif(UNIX)
	set(YTDLP_URL "${YTDLP_URL}_linux")
	set(YTDLP_PATH "${YTDLP_PATH}_linux")
endif()

# Download ytdlp
if(NOT EXISTS ${YTDLP_PATH})
	message(STATUS "Downloading ytdlp...")
	file(DOWNLOAD ${YTDLP_URL} ${YTDLP_PATH} SHOW_PROGRESS)
	message(STATUS "Downloaded ytdlp to ${YTDLP_PATH}")
	
	# Give executable permissions
	if(UNIX)
		file(CHMOD ${YTDLP_PATH} PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)
	endif()
endif()

# Copy to folder after build
list(APPEND POST_BUILD_COPY_FILES
	${YTDLP_PATH}
)

# Make path available in the code
add_compile_definitions(${PROJECT_NAME} PRIVATE
	YTDLP_PATH="${YTDLP_PATH}"
)
