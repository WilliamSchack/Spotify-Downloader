# Assumed variables:
# BINARIES_DIR_NAME     | Name for the directory of external binaries
# BINARIES_DIR          | Directory to external binaries, assumes folder is created
# 
# Gets the quickjs prebuilt binary
# (Required for ytdlp, using this over others as its only ~2mb)

set(QUICKJS_URL "https://github.com/quickjs-ng/quickjs/releases/latest/download/qjs")
set(QUICKJS_FILE_NAME "qjs")

if(WIN32)
	set(QUICKJS_URL "${QUICKJS_URL}-windows-x86_64")
	set(QUICKJS_FILE_NAME "${QUICKJS_FILE_NAME}-windows-x86_64")
elseif(APPLE)
	set(QUICKJS_URL "${QUICKJS_URL}-darwin")
	set(QUICKJS_FILE_NAME "${QUICKJS_FILE_NAME}-darwin")
elseif(UNIX)
	set(QUICKJS_URL "${QUICKJS_URL}-linux-x86_64")
	set(QUICKJS_FILE_NAME "${QUICKJS_FILE_NAME}-linux-x86_64")
endif()

set(QUICKJS_PATH "${BINARIES_DIR}/${QUICKJS_FILE_NAME}")

# Download
if(NOT EXISTS ${QUICKJS_PATH})
	message(STATUS "Downloading quickjs...")
	file(DOWNLOAD ${QUICKJS_URL} ${QUICKJS_PATH} SHOW_PROGRESS)
	message(STATUS "Downloaded ytdlp to ${QUICKJS_PATH}")
	
	# Give executable permissions
	if(UNIX)
		file(CHMOD ${QUICKJS_PATH} PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)
	endif()
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