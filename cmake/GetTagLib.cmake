# Assumed variables:
# ADDITIONAL_INCLUDE_DIRS | List of additional include directories
# ADDITIONAL_LIBS         | List of additional libraries
# ADDITIONAL_DEPENDENCIES | List of additional dependencies
# POST_BUILD_COPY_FILES   | List of files to copy to final build

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
list(APPEND ADDITIONAL_DEPENDENCIES
	taglib
)

list(APPEND ADDITIONAL_INCLUDE_DIRS
	${taglib_INSTALL_DIR}/include
)

if(WIN32)
	list(APPEND ADDITIONAL_LIBS
		${taglib_INSTALL_DIR}/lib/tag.lib
	)
elseif(APPLE)
	list(APPEND ADDITIONAL_LIBS
		${taglib_INSTALL_DIR}/lib/libtag.dylib
	)
elseif(UNIX)
	list(APPEND ADDITIONAL_LIBS
		${taglib_INSTALL_DIR}/lib/libtag.so
	)
endif()

# Copy shared library to folder after build
if(WIN32)
	set(taglib_LIBRARY_PATH "${taglib_INSTALL_DIR}/bin/tag.dll")
endif()

list(APPEND POST_BUILD_COPY_FILES
	${taglib_LIBRARY_PATH}
)