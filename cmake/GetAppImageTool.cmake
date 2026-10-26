# Assumes an OS that supports the AppImage format
# Creates variables:
# APPIMAGETOOL_EXE | Path to appimagetool
# APPIMAGE_ARCH    | Value to use with ARCH= when using the exe
# Assumed variables:
# IS_ARM_CPU       | If the cpu architecture is arm or not
# BINARIES_DIR     | Directory to external binaries, assumes folder is created

set(AIT_URL "https://github.com/AppImage/appimagetool/releases/download/continuous/appimagetool")

if(IS_ARM_CPU)
    set(AIT_URL "${AIT_URL}-aarch64.AppImage")
else()
    set(AIT_URL "${AIT_URL}-x86_64.AppImage")
endif()

set(LD_DIR "${BINARIES_DIR}/linuxdeploy")
set(AIT_PATH "${LD_DIR}/appimagetool.AppImage")

file(MAKE_DIRECTORY ${LD_DIR})

if(NOT EXISTS ${AIT_PATH})
	message(STATUS "Downloading appimagetool...")
	file(DOWNLOAD ${AIT_URL} ${AIT_PATH} SHOW_PROGRESS)
	message(STATUS "Downloaded appimagetool to ${AIT_PATH}")

	# Give executable permissions
	file(CHMOD ${AIT_PATH} PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)
endif()

# Create variables
set(APPIMAGETOOL_EXE ${AIT_PATH} CACHE INTERNAL "Path to appimagetool")

if(IS_ARM_CPU)
    set(APPIMAGE_ARCH aarch64 CACHE INTERNAL "Architecture to use with appimagetool")
else()
    set(APPIMAGE_ARCH x86_64 CACHE INTERNAL "Architecture to use with appimagetool")
endif()