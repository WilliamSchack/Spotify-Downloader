target_sources(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/ExternalProcess.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ArchiveHandler.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/VersionManager.cpp
)

add_subdirectory(ffmpeg)
add_subdirectory(ytdlp)