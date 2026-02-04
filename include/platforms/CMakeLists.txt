target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/EPlatform.h
	${CMAKE_CURRENT_SOURCE_DIR}/IPlatform.h
	${CMAKE_CURRENT_SOURCE_DIR}/PlatformDetector.h
	${CMAKE_CURRENT_SOURCE_DIR}/PlatformFactory.h
	${CMAKE_CURRENT_SOURCE_DIR}/SpotifyPlatform.h
)