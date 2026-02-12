set(taglib_INSTALL_DIR "${CMAKE_BINARY_DIR}/taglib-install")

# Download taglib
# Using ExternalProject as FetchContent has too many issues, namely includes do not work properly
include(ExternalProject)

ExternalProject_Add(
	taglib
	GIT_REPOSITORY https://github.com/taglib/taglib.git
	GIT_TAG v2.1.1
	GIT_SHALLOW ON
	PREFIX "${taglib_INSTALL_DIR}"
	CMAKE_ARGS
		-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
		-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
		-DWITH_ZLIB=OFF
		-DBUILD_SHARED_LIBS=ON
		-DBUILD_EXAMPLES=OFF
		-DBUILD_TESTING=OFF
		-DBUILD_BINDINGS=OFF
)

# Setup taglib
add_dependencies(${PROJECT_NAME} taglib)

target_include_directories(${PROJECT_NAME} PRIVATE
	${taglib_INSTALL_DIR}/include
)

if(WIN32)
	target_link_libraries(${PROJECT_NAME} PRIVATE
		${taglib_INSTALL_DIR}/lib/tag.lib
	)
elseif(APPLE)
	target_link_libraries(${PROJECT_NAME} PRIVATE
		${taglib_INSTALL_DIR}/lib/libtag.dylib
	)
elseif(UNIX)
	target_link_libraries(${PROJECT_NAME} PRIVATE
		${taglib_INSTALL_DIR}/lib/libtag.so
	)
endif()

# Copy shared library to folder after build
if(WIN32)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${taglib_INSTALL_DIR}/bin/tag.dll"
			$<TARGET_FILE_DIR:${PROJECT_NAME}>
	)
endif()