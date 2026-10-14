target_sources(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/NetworkRequest.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/CookieFile.cpp
)

add_subdirectory(spotify)
add_subdirectory(youtube)