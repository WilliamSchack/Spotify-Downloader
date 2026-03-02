target_sources(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/PlatformDetector.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/PlatformFactory.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/PlatformUtils.cpp
)

add_subdirectory(spotify)
add_subdirectory(youtube)