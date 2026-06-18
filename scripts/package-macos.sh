#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-"$ROOT_DIR/build-macos"}"
BUILD_TYPE="${BUILD_TYPE:-Release}"
APP_PATH="$BUILD_DIR/SpotifyDownloader.app"
TOOLS_DIR="$APP_PATH/Contents/Resources/bin"
TOOLS_LIB_DIR="$APP_PATH/Contents/Resources/lib"
DMG_PATH="${DMG_PATH:-"$BUILD_DIR/SpotifyDownloader-macOS.dmg"}"

QT_PREFIX="${QT_PREFIX:-${CMAKE_PREFIX_PATH:-}}"
if [[ -z "$QT_PREFIX" ]] && command -v brew >/dev/null 2>&1; then
    QT_PREFIX="$(brew --prefix qt 2>/dev/null || true)"
fi

if [[ -z "$QT_PREFIX" || ! -d "$QT_PREFIX" ]]; then
    echo "QT_PREFIX or CMAKE_PREFIX_PATH must point to a Qt 6 installation with WebEngine." >&2
    echo "Example: QT_PREFIX=\"\$HOME/Qt/6.8.2/macos\" $0" >&2
    exit 1
fi

MACDEPLOYQT="${MACDEPLOYQT:-"$QT_PREFIX/bin/macdeployqt"}"
if [[ ! -x "$MACDEPLOYQT" && -x "/usr/local/opt/qtbase/bin/macdeployqt" ]]; then
    MACDEPLOYQT="/usr/local/opt/qtbase/bin/macdeployqt"
fi
if [[ ! -x "$MACDEPLOYQT" ]]; then
    MACDEPLOYQT="$(command -v macdeployqt || true)"
fi

if [[ -z "$MACDEPLOYQT" || ! -x "$MACDEPLOYQT" ]]; then
    echo "macdeployqt was not found. Check QT_PREFIX or set MACDEPLOYQT." >&2
    exit 1
fi

tool_path() {
    local name="$1"

    if [[ -n "${CONDA_PREFIX:-}" && -x "$CONDA_PREFIX/bin/$name" ]]; then
        printf '%s\n' "$CONDA_PREFIX/bin/$name"
        return
    fi

    command -v "$name" || true
}

copy_tool() {
    local name="$1"
    local source

    if [[ "$name" == "yt-dlp" ]] && command -v python >/dev/null 2>&1 && python -m PyInstaller --version >/dev/null 2>&1; then
        build_standalone_ytdlp
        return
    fi

    source="$(tool_path "$name")"
    if [[ -z "$source" || ! -x "$source" ]]; then
        echo "Required tool '$name' was not found. Activate the conda env or set PATH." >&2
        exit 1
    fi

    install -m 755 "$source" "$TOOLS_DIR/$name"
}

build_standalone_ytdlp() {
    local pyinstaller_dir="$BUILD_DIR/tools/pyinstaller"
    local dist_dir="$BUILD_DIR/tools/dist"
    local launcher="$BUILD_DIR/tools/yt_dlp_launcher.py"

    mkdir -p "$BUILD_DIR/tools" "$dist_dir" "$pyinstaller_dir"

    cat > "$launcher" <<'PY'
from yt_dlp import main

if __name__ == "__main__":
    main()
PY

    PYINSTALLER_CONFIG_DIR="$BUILD_DIR/tools/pyinstaller-config" python -m PyInstaller \
        --onedir \
        --clean \
        --name yt-dlp \
        --distpath "$dist_dir" \
        --workpath "$pyinstaller_dir" \
        --specpath "$BUILD_DIR/tools" \
        "$launcher"

    install -m 755 "$dist_dir/yt-dlp/yt-dlp" "$TOOLS_DIR/yt-dlp"
    ditto "$dist_dir/yt-dlp/_internal" "$TOOLS_DIR/_internal"
}

bundle_conda_dylibs_for() {
    local executable="$1"

    if [[ -z "${CONDA_PREFIX:-}" || ! -d "$CONDA_PREFIX/lib" ]]; then
        return
    fi

    mkdir -p "$TOOLS_LIB_DIR"

    local queue=("$executable")
    local seen=" "
    local current dep base source target

    while ((${#queue[@]})); do
        current="${queue[0]}"
        queue=("${queue[@]:1}")

        [[ -f "$current" ]] || continue

        while IFS= read -r dep; do
            base="$(basename "$dep")"
            source=""

            if [[ "$dep" == @rpath/* && -f "$CONDA_PREFIX/lib/$base" ]]; then
                source="$CONDA_PREFIX/lib/$base"
            elif [[ "$dep" == "$CONDA_PREFIX/lib/"* && -f "$dep" ]]; then
                source="$dep"
                install_name_tool -change "$dep" "@rpath/$base" "$current" 2>/dev/null || true
            fi

            [[ -n "$source" ]] || continue

            target="$TOOLS_LIB_DIR/$base"
            if [[ ! -f "$target" ]]; then
                install -m 755 "$source" "$target"
                install_name_tool -id "@rpath/$base" "$target" 2>/dev/null || true
            fi

            if [[ "$seen" != *" $target "* ]]; then
                seen="$seen$target "
                queue+=("$target")
            fi
        done < <(otool -L "$current" 2>/dev/null | awk 'NR > 1 { print $1 }')
    done
}

cmake_prefixes=("$QT_PREFIX")
macdeployqt_args=("$APP_PATH" "-verbose=1" "-always-overwrite")
if command -v brew >/dev/null 2>&1; then
    while IFS= read -r formula; do
        prefix="$(brew --prefix "$formula" 2>/dev/null || true)"
        if [[ -d "$prefix/lib/cmake" ]]; then
            cmake_prefixes+=("$prefix")
        fi
        if [[ -d "$prefix/lib" ]]; then
            macdeployqt_args+=("-libpath=$prefix/lib")
        fi
    done < <(brew list --formula | grep '^qt' || true)

    for formula in brotli webp zstd dbus graphite2 harfbuzz fontconfig taglib; do
        prefix="$(brew --prefix "$formula" 2>/dev/null || true)"
        if [[ -d "$prefix/lib" ]]; then
            macdeployqt_args+=("-libpath=$prefix/lib")
        fi
    done
fi

CMAKE_PREFIX_JOINED="$(IFS=';'; printf '%s' "${cmake_prefixes[*]}")"

cmake -S "$ROOT_DIR" -B "$BUILD_DIR" -G Ninja \
    -DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_JOINED" \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE"

cmake --build "$BUILD_DIR" --config "$BUILD_TYPE"

if [[ ! -d "$APP_PATH" ]]; then
    echo "Expected app bundle was not created: $APP_PATH" >&2
    exit 1
fi

mkdir -p "$TOOLS_DIR"
copy_tool ffmpeg
copy_tool yt-dlp
copy_tool qjs

if [[ -n "${CODESIGN_IDENTITY:-}" ]]; then
    macdeployqt_args+=("-codesign=$CODESIGN_IDENTITY")
else
    macdeployqt_args+=("-no-codesign")
fi

macdeployqt_args+=("-executable=$TOOLS_DIR/ffmpeg")
macdeployqt_args+=("-executable=$TOOLS_DIR/qjs")

if [[ -n "${CONDA_PREFIX:-}" && -d "$CONDA_PREFIX/lib" ]]; then
    macdeployqt_args+=("-libpath=$CONDA_PREFIX/lib")
fi

"$MACDEPLOYQT" "${macdeployqt_args[@]}"

bundle_conda_dylibs_for "$TOOLS_DIR/ffmpeg"
bundle_conda_dylibs_for "$TOOLS_DIR/qjs"

if [[ -n "${CODESIGN_IDENTITY:-}" ]]; then
    codesign --force --deep --options runtime --sign "$CODESIGN_IDENTITY" "$APP_PATH"
else
    codesign --force --deep --sign - "$APP_PATH"
fi

if [[ "${CREATE_DMG:-0}" == "1" ]]; then
    rm -f "$DMG_PATH"
    hdiutil create -volname "Spotify Downloader" -srcfolder "$APP_PATH" -ov -format UDZO "$DMG_PATH"
    echo "Created $DMG_PATH"
else
    echo "Created $APP_PATH"
fi
