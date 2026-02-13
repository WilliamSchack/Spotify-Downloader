target_sources(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/Process.cpp
)

add_subdirectory(ffmpeg)
add_subdirectory(ytdlp)
add_subdirectory(taglib)