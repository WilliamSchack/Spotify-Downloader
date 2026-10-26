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

# Options
option(INCLUDE_DEPS "Copies dependencies to the build" OFF)

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

set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/out/$<CONFIG>")
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}"
)

add_custom_command(TARGET ${PROJECT_NAME} PRE_LINK
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
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
    if (INCLUDE_DEPS)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND  ${WINDEPLOYQT_EXECUTABLE}
                "$<TARGET_FILE:${PROJECT_NAME}>"
                --verbose 1
                --no-translations
                --compiler-runtime
                --qmldir "${CMAKE_SOURCE_DIR}/gui"
            VERBATIM
        )
    endif()
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
    if (INCLUDE_DEPS)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND  ${MACDEPLOYQT_EXECUTABLE}
                "$<TARGET_BUNDLE_DIR:${PROJECT_NAME}>"
                -verbose=1
                -qmldir="${CMAKE_SOURCE_DIR}/gui"
            VERBATIM
        )
    endif()
elseif (UNIX AND INCLUDE_DEPS)
    option(CREATE_APPIMAGE "Creates an appimage after building" OFF)
    option(DISABLE_STRIP_QT "Disable Qt library stripping when running linuxdeploy" OFF)

    include(cmake/GetLinuxDeploy.cmake)

    # Outputs as AppDir directory
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}/usr/bin"
    )

    # Get resources
    set(DESKTOP_FILE "${CMAKE_BINARY_DIR}/${APP_ID}.desktop")
    configure_file(
        "${CMAKE_SOURCE_DIR}/resources/linux/template.desktop"
        "${DESKTOP_FILE}"
        @ONLY
    )

    set(APP_ICON_LINUX "${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/Icon.png")

    # Get wayland details
    execute_process(
        COMMAND ${QMAKE_EXECUTABLE} -query QT_INSTALL_PLUGINS
        OUTPUT_VARIABLE QT_PLUGINS_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    set(WAYLAND_CLIENT_DEPS_DIR "${QT_PLUGINS_DIR}/wayland-graphics-integration-client")
    set(WAYLAND_CLIENT_DEPS_OUT_DIR "${OUTPUT_DIR}/usr/plugins/wayland-graphics-integration-client")

    # Setup linuxdeploy
    find_program(QMAKE_EXECUTABLE NAMES qmake6 REQUIRED)

    set(LINUXDEPLOY_EXTRA_ENV
        QMAKE=${QMAKE_EXECUTABLE}
        QML_SOURCES_PATHS=${CMAKE_SOURCE_DIR}/gui
    )

    if (EXISTS "${WAYLAND_CLIENT_DEPS_DIR}")
        # Wayland libraries were unified in 6.12.0
        if (Qt6_VERSION VERSION_GREATER_EQUAL 6.12.0)
            list(APPEND LINUXDEPLOY_EXTRA_ENV EXTRA_PLATFORM_PLUGINS=libqwayland.so)
        else()
            list(APPEND LINUXDEPLOY_EXTRA_ENV EXTRA_PLATFORM_PLUGINS=libqwayland-egl.so$<SEMICOLON>libqwayland-generic.so)
        endif()
    endif()

    if (DISABLE_STRIP_QT)
        list(APPEND LINUXDEPLOY_EXTRA_ENV NO_STRIP=1)
    endif()

    # Run linuxdeploy
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E env
            APPIMAGE_EXTRACT_AND_RUN=1
            ${LINUXDEPLOY_EXTRA_ENV}
            ${LINUXDEPLOY_EXE}
                --appdir "${OUTPUT_DIR}"
                --executable "$<TARGET_FILE:${PROJECT_NAME}>"
                --desktop-file "${DESKTOP_FILE}"
                --icon-file "${APP_ICON_LINUX}"
                --icon-filename "${APP_ID}"
                --plugin qt
        VERBATIM
    )

    # Copy wayland-graphics-integration-client since linuxdeploy qt doesnt do that for some reason
    if (EXISTS "${WAYLAND_CLIENT_DEPS_DIR}")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${WAYLAND_CLIENT_DEPS_DIR}
                ${WAYLAND_CLIENT_DEPS_OUT_DIR}
            COMMAND ${CMAKE_COMMAND} -E env
                APPIMAGE_EXTRACT_AND_RUN=1
                ${LINUXDEPLOY_EXE}
                    --appdir "${OUTPUT_DIR}"
                    --deploy-deps-only "${WAYLAND_CLIENT_DEPS_OUT_DIR}"
            VERBATIM
        )
    else()
        message(WARNING "The wayland qt module was not found, wayland will not work.")
    endif()

    if (CREATE_APPIMAGE)
        include(cmake/GetAppImageTool.cmake)

        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E env
                APPIMAGE_EXTRACT_AND_RUN=1
                ARCH=${APPIMAGE_ARCH}
                ${APPIMAGETOOL_EXE}
                    "${OUTPUT_DIR}"
                    "${CMAKE_BINARY_DIR}/out/${PROJECT_NAME}-${APPIMAGE_ARCH}.AppImage"
            VERBATIM
        )
    endif()
endif()