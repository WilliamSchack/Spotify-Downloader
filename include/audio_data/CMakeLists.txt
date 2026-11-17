target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/TrackData.h
	${CMAKE_CURRENT_SOURCE_DIR}/PlaylistData.h 
	${CMAKE_CURRENT_SOURCE_DIR}/PlaylistTracks.h 
	${CMAKE_CURRENT_SOURCE_DIR}/AlbumData.h 
	${CMAKE_CURRENT_SOURCE_DIR}/AlbumTracks.h 
	${CMAKE_CURRENT_SOURCE_DIR}/ArtistData.h
	${CMAKE_CURRENT_SOURCE_DIR}/EAlbumType.h
)

add_subdirectory(codecs)