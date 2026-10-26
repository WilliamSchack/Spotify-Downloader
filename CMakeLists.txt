cmake_minimum_required(VERSION 3.18)

# Setup project
set(PROJECT_NAME Echolite CACHE INTERNAL "")
set(DISPLAY_NAME "Echolite" CACHE INTERNAL "")
set(APP_VERSION "2.0.0" CACHE INTERNAL "")
set(APP_DESCRIPTION "Can download songs & lyrics, modify metadata, and probably other stuff im forgetting atm" CACHE INTERNAL "")
set(ORGANISATION_NAME "WilliamSchack" CACHE INTERNAL "")
set(APP_ID "com.williamschack.echolite" CACHE INTERNAL "")

add_compile_definitions(${PROJECT_NAME} PRIVATE
	APP_NAME="${PROJECT_NAME}"
	APP_DISPLAY_NAME="${DISPLAY_NAME}"
)

project(
	${PROJECT_NAME}
	VERSION ${APP_VERSION}
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Wont work for apple, but that isnt needed
if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm|aarch64|ARM|ARM64")
	set(IS_ARM_CPU TRUE)
else()
	set(IS_ARM_CPU FALSE)
endif()

# Setup Curl
find_package(CURL REQUIRED)

include_directories(${CURL_INCLUDE_DIR})

# Setup Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS
	Core
	Widgets
	Qml
	Quick
    QuickControls2
	WebEngineCore
)

# Add the executable
add_executable(${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/$<CONFIG>"
)

# Additional dependencies
set(BINARIES_DIR_NAME "bin" CACHE INTERNAL "The name for the binaries directory")
set(BINARIES_DIR "${CMAKE_BINARY_DIR}/${BINARIES_DIR_NAME}" CACHE INTERNAL "Directory for external dependency files")
file(MAKE_DIRECTORY ${BINARIES_DIR})

include(cmake/GetTagLib.cmake)
include(cmake/GetYtdlp.cmake)
include(cmake/GetFFmpeg.cmake)
include(cmake/GetQuickJs.cmake)
include(cmake/GetJson.cmake)
include(cmake/GetStb.cmake)
include(cmake/GetCppcodec.cmake)
include(cmake/GetLexbor.cmake)

# Include files
add_subdirectory(include)
add_subdirectory(gui)
add_subdirectory(resources)
add_subdirectory(src)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
	Qt6::Core
	Qt6::Widgets
	Qt6::Qml
	Qt6::Quick
	Qt6::QuickControls2
	Qt6::WebEngineCore
	CURL::libcurl
)

target_compile_definitions(${PROJECT_NAME} PRIVATE QT_QML_DEBUG)

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

	# Add curl dll
	get_target_property(CURL_DLL CURL::libcurl IMPORTED_LOCATION)
	file(TO_CMAKE_PATH "${CURL_DLL}" CURL_DLL)
	if (CURL_DLL MATCHES "/lib/libcurl\\.lib$")
		# If the path is the .lib file, change to the dll file
		string(REPLACE "/lib/libcurl.lib" "/bin/libcurl.dll" CURL_DLL "${CURL_DLL}")
	endif()
	
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CURL_DLL}
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )

	# Add qt libraries
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND  ${WINDEPLOYQT_EXECUTABLE}
			"$<TARGET_FILE:${PROJECT_NAME}>"
			--verbose 1
			--no-translations
			--compiler-runtime
			--qmldir "${CMAKE_SOURCE_DIR}/gui"
	)
elseif (APPLE)
	# Mac setup
    set(APP_ICON_MACOS_NAME "Icon.icns")
    set(APP_ICON_MACOS "${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/${APP_ICON_MACOS_NAME}")
    set_source_files_properties(${APP_ICON_MACOS} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

	set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE_GUI_IDENTIFIER ${APP_ID}
        MACOSC_BUNDLE_ICON_FILE ${APP_ICON_MACOS_NAME}
    )

	# Add qt libraries
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND  ${MACDEPLOYQT_EXECUTABLE}
			"$<TARGET_BUNDLE_DIR:${PROJECT_NAME}>"
			-verbose=1
			-qmldir="${CMAKE_SOURCE_DIR}/gui"
	)
endif()

# TODO: Setup linux app icon