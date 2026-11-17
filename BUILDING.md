# Building Echolite

## Requirements

- Git
- CMake
- A C++ compiler
- Curl
- Qt 6.9+
  - Qt Quick
  - QtWebEngine

Make sure these dependencies are in your PATH, otherwise you will need to specify them manually

## Compiling

1 - **Download/Clone this branch**

Either download it manually, or use the command

`git clone https://github.com/WilliamSchack/Echolite`

2 - **Configure & Build the app:**

Execute the following in the root folder:

```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

3 - **Download the dependencies:**

This step is only required if you do not have ffmpeg, yt-dlp, and quickjs installed on your system. All of those are required so if you are missing any do this step

Execute the following:

```
./build/out/Release/Echolite download-deps
```

Then wait for the dependencies to download

4 - **Run the app:**

The app and its dependencies will be output to `./build/out/Release`

## Options

These options can be used when configuring the project using `-D{ARG}={VALUE}`<br />
**Example:** `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DINCLUDE_DEPS=ON`

| Option           | Description                                                                                                                                                                                                                                                                                                                                                                                         | Valid Values |
| ---------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------ |
| INCLUDE_DEPS     | Includes the dependencies with the build. Use if you want to run the app on another device that does not have the dependencies installed.<br><br>If you are building for linux, for the output to work on machines using wayland you will need wayland qt module installed<br><br>**Default: OFF**                                                                                                  | ON, OFF      |
| DISABLE_STRIP_QT | **Linux Only, Requires INCLUDE_DEPS=ON**<br>Disable Qt library stripping when running linuxdeploy. Enable this if you have errors related to stripping when using INCLUDE_DEPS<br><br>**Default: OFF**                                                                                                                                                                                              | ON, OFF      |

