target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/SpotifyAPI.h
	${CMAKE_CURRENT_SOURCE_DIR}/SpotifyAuth.h
	${CMAKE_CURRENT_SOURCE_DIR}/SpotifyAuthRetriever.h
	${CMAKE_CURRENT_SOURCE_DIR}/SpotifyAuthInterceptor.h
)