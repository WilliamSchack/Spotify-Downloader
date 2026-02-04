cmake_minimum_required(VERSION 3.18)

# Setup project
set(PROJECT_NAME CMakeTesting CACHE INTERNAL "")
set(DISPLAY_NAME "CMake Testing" CACHE INTERNAL "")
set(APP_VERSION "1.0.0" CACHE INTERNAL "")
set(APP_DESCRIPTION "Testing project for cmake with dependencies used for Spotify Downloader" CACHE INTERNAL "")
set(ORGANISATION_NAME "Organisation" CACHE INTERNAL "")
set(APP_ID "com.example.CMakeTesting" CACHE INTERNAL "")

project(
	${PROJECT_NAME}
	VERSION ${APP_VERSION}
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Setup Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS
	Core
	Widgets
	Qml
)

# Additional dependencies
set(ADDITIONAL_INCLUDE_DIRS CACHE INTERNAL "Additional include directories")
set(ADDITIONAL_LIBS CACHE INTERNAL "Additional libraries")
set(ADDITIONAL_DEPENDENCIES CACHE INTERNAL "Additional dependencies")
set(POST_BUILD_COPY_FILES CACHE INTERNAL "Files that will be copied to the final build")

set(BINARIES_DIR ${CMAKE_BINARY_DIR}/bin CACHE INTERNAL "Directory for external dependency files")
file(MAKE_DIRECTORY ${BINARIES_DIR})

include(cmake/GetTagLib.cmake)
include(cmake/GetYtdlp.cmake)
include(cmake/GetFFmpeg.cmake)

# Add the executable
add_executable(${PROJECT_NAME})

# Include files
add_subdirectory(include)
add_subdirectory(gui)
add_subdirectory(resources)
add_subdirectory(src)

# Add dependencies
add_dependencies(${PROJECT_NAME} ${ADDITIONAL_DEPENDENCIES})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
	Qt6::Core
	Qt6::Widgets
	Qt6::Qml
	${ADDITIONAL_LIBS}
)

# Include Directories
target_include_directories(${PROJECT_NAME} PRIVATE
	${ADDITIONAL_INCLUDE_DIRS}
)

# Enable debug logging on RELWITHDEBINFO configuration
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:RELWITHDEBINFO>:QT_MESSAGELOGCONTEXT>
)

# Extra setup
if (WIN32)
	# Hide console
	set_target_properties(${PROJECT_NAME} PROPERTIES
		WIN32_EXECUTABLE TRUE
	)

	# Add qt libraries
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND  ${WINDEPLOYQT_EXECUTABLE}
			"$<TARGET_FILE:${PROJECT_NAME}>"
			--verbose 1
			--no-translations
			--compiler-runtime
	)
elseif(APPLE)
	# Mac setup
	set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE_GUI_IDENTIFIER ${APP_ID}
    )
	
	# Add qt libraries
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND  ${MACDEPLOYQT_EXECUTABLE}
			"$<TARGET_BUNDLE_DIR:${PROJECT_NAME}>"
			-verbose=1
	)
endif()

# Copy files to final build
foreach(file ${POST_BUILD_COPY_FILES})
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${file}"
			$<TARGET_FILE_DIR:${PROJECT_NAME}>
	)
endforeach()