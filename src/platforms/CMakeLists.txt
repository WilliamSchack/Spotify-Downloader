target_sources(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/IPlatformDownloader.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/IPlatformSearcher.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/PlatformDetector.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/PlatformFactory.cpp
)

add_subdirectory(spotify)