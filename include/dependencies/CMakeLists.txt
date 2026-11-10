target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/ExternalProcess.h
	${CMAKE_CURRENT_SOURCE_DIR}/ExternalProcessResult.h
	${CMAKE_CURRENT_SOURCE_DIR}/ArchiveHandler.h
	${CMAKE_CURRENT_SOURCE_DIR}/Dependency.h
	${CMAKE_CURRENT_SOURCE_DIR}/VersionManager.h
)

add_subdirectory(ffmpeg)
add_subdirectory(ytdlp)