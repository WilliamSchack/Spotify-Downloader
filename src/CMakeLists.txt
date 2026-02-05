target_sources(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

add_subdirectory(downloading)
add_subdirectory(gui)
add_subdirectory(platforms)
add_subdirectory(network)
add_subdirectory(dependencies)
add_subdirectory(utilities)