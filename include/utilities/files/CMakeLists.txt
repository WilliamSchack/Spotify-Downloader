target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/FileUtils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/FilePathReserver.h
	${CMAKE_CURRENT_SOURCE_DIR}/TemporaryPaths.h
)