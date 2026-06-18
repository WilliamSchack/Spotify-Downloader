# macOS Development Notes

This document records the macOS porting state and the dependencies needed to build and run the app.

## Current Status

The app has two macOS build paths:

- Recommended: Qt 6 + CMake
- Temporary/local compatibility path: qmake from an Anaconda Qt 5.15 installation

Qt 6 is the preferred path because the Spotify auth interceptor relies on `QWebEngineUrlRequestInfo::httpHeaders()`, which is available in Qt 6.

## Dependencies

Preferred isolated CLI/tooling environment:

```sh
conda env create -f environment-macos.yml
conda activate spotify-downloader-macos
```

This installs:

- `python=3.11`
- `cmake`
- `ninja`
- `pkg-config`
- `ffmpeg`
- `yt-dlp`
- `quickjs`
- `pyinstaller`

`environment-macos.yml` intentionally avoids the Anaconda base environment and keeps these tools under the dedicated `spotify-downloader-macos` env.

Full Qt/TagLib development dependencies:

```sh
brew bundle
```

The included `Brewfile` installs the dependencies that are not fully covered by conda-forge on `osx-64`:

- `taglib`
- `ffmpeg`
- `yt-dlp`
- `quickjs`
- `cmake`
- `ninja`
- `qt`
- `pkg-config`

Current conda-forge `osx-64` limitations checked locally:

- No `taglib` package is available by that name.
- `qt6-main` is available, but Qt 6 WebEngine is not available as `qt6-webengine`.
- Qt 5 WebEngine is available as `qt-webengine`, but it does not provide the Qt 6 `QWebEngineUrlRequestInfo::httpHeaders()` API needed for full Spotify auth capture.

For full Qt 6 functionality, prefer the official Qt 6 installer or a CI/Homebrew build that includes Qt WebEngine. For isolated local development, install Homebrew dependencies in a dedicated prefix, VM, CI runner, or separate user account rather than relying on a daily-use machine.

## Build

Recommended CMake flow:

```sh
cmake -S . -B build -G Ninja \
  -DCMAKE_PREFIX_PATH="$(brew --prefix qt)" \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Packaging flow:

```sh
conda activate spotify-downloader-macos
QT_PREFIX="$HOME/Qt/6.x.x/macos" scripts/package-macos.sh
```

The packaging script:

- Builds the app with CMake/Ninja.
- Runs `macdeployqt`.
- Copies `ffmpeg` and `qjs` from the active conda env or `PATH` into `SpotifyDownloader.app/Contents/Resources/bin`.
- Builds `yt-dlp` into a standalone binary with PyInstaller when available, so the app bundle does not depend on the user's conda Python.
- Ad-hoc signs the local app bundle by default; set `CODESIGN_IDENTITY` for Developer ID signing.
- Optionally creates a dmg when `CREATE_DMG=1` is set.

Examples:

```sh
QT_PREFIX="$HOME/Qt/6.x.x/macos" CREATE_DMG=1 scripts/package-macos.sh
QT_PREFIX="$(brew --prefix qt)" CODESIGN_IDENTITY="Developer ID Application: Example" scripts/package-macos.sh
```

Temporary qmake flow:

```sh
qmake SpotifyDownloader.pro
make -j4
macdeployqt SpotifyDownloader.app
```

On newer Xcode/macOS SDKs, Qt 5.15 may emit warnings about unsupported SDK versions. The local Anaconda Qt 5.15 mkspec can also inject the removed `AGL.framework`; if this happens, remove `-framework AGL` from the generated `Makefile` before linking, or use the Qt 6 CMake path.

## Known Limitations

- The Qt 5 compatibility path cannot read Spotify auth request headers because `QWebEngineUrlRequestInfo::httpHeaders()` is a Qt 6 API. Use Qt 6 for full functionality.
- The app bundle produced by `macdeployqt` may need extra Qt WebEngine resources when using non-standard Qt layouts such as Anaconda Qt.
- Homebrew's TagLib bottle may target a newer macOS version than an older deployment target. This is fine for local development, but release builds should align the deployment target and dependency builds.

## Environment Hygiene

The conda environment keeps the CLI tools isolated under its own
`$CONDA_PREFIX`. Homebrew packages are installed in the active Homebrew
prefix; this is normal for Homebrew, but it is not isolated. For cleaner
development, prefer one of these approaches:

- Use GitHub Actions or a dedicated macOS VM for release builds.
- Use a dedicated Homebrew prefix, CI runner, or separate user account for local macOS dependency experiments.
- Use `environment-macos.yml` for the tools that conda can manage cleanly.
- Prefer the CMake build so dependency discovery is explicit and reproducible.

Do not commit generated qmake or build artifacts such as `Makefile`, `*.o`, `moc_*.cpp`, `qrc_*.cpp`, `ui_*.h`, or `SpotifyDownloader.app`.
