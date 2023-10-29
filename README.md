<img src="https://github.com/ChazzBurger/Spotify-Downloader/assets/54973797/0998c0ca-bc59-4cb4-a9f5-76956d3bbe27" align="right" width="12%" height="12%">

# Spotify Downloader
[![Latest Release](https://img.shields.io/github/v/release/WilliamSchack/Spotify-Downloader?label=Latest%20Release&color=007ec6)](https://github.com/ChazzBurger/Spotify-Downloader/releases)
[![Downloads](https://img.shields.io/github/downloads/WilliamSchack/Spotify-Downloader/total?label=Downloads&color=007ec6)](https://github.com/ChazzBurger/Spotify-Downloader/releases)
[![Stars](https://img.shields.io/github/stars/WilliamSchack/Spotify-Downloader?label=Stars&color=007ec6)](https://github.com/ChazzBurger/Spotify-Downloader/stargazers)
[![Open Issues](https://img.shields.io/github/issues/WilliamSchack/Spotify-Downloader?label=Issues)](https://github.com/ChazzBurger/Spotify-Downloader/issues?q=is%3Aissue+is%3Aopen)
[![Closed Issues](https://img.shields.io/github/issues-closed/WilliamSchack/Spotify-Downloader?label=Issues)](https://github.com/ChazzBurger/Spotify-Downloader/issues?q=is%3Aissue+is%3Aclosed)

Spotify Downloader is an application that can download songs from spotify with ease and for free unlike spotify requiring spotify requiring a subscription for the same services.

The program has an easy to navigate GUI that will make downloading songs easier than ever!

## Contents
- [Installation](#installation)
- [Usage](#usage)
- [Features](#features)
- [Credits](#credits)

## Installation
***Only currently supports windows. Other platforms may be coming soon***

The latest release can be found [Here](https://github.com/WilliamSchack/Spotify-Downloader/releases/latest). **Installer Recommended**

Follow the instructions below depending on your chosen install type.

<details>
<summary><b> Installer </b></summary>

---
    
*Note: Installer requires administrator privileges otherwise install will fail*

**IF A WINDOW COMES UP SHOWING "Windows protected your PC" click:**
- More Info
- Run Anyway

<details>
<summary><b> Reasoning Behind This </b></summary>

This popup generally comes up when windows defender does not know much about a program being installed without a publisher. Since my program is not verified by microsoft, this will show up on first launch for almost everyone.

If we want this popup to go away, the only thing we can really do here is just to wait and hope it eventually stops.

</details>

---

**Choose your install location. Default is "C:/Program Files/Spotify Downloader" but you can change this to anywhere you like.**

*Note: If the program is already installed in the chosen location, it will overwrite and update the old install*
> <img src="https://github.com/WilliamSchack/Spotify-Downloader/assets/54973797/ed720dd2-1862-4247-8714-b1ba9f8b3c4c" width="50%" height="50%">

**Select the Spotify Downloader component (selected by default)**
> <img src="https://github.com/WilliamSchack/Spotify-Downloader/assets/54973797/8d64c8a9-1453-4b2d-b2ee-cf40742f31d3" width="50%" height="50%">

**The program will add a shortcut to the start menu and the name can be changed here**
> <img src="https://github.com/WilliamSchack/Spotify-Downloader/assets/54973797/0a8b4ec7-8fd1-41ea-a522-0182c0f0ebca" width="50%" height="50%">

**Click install and wait for the installer to finish**
> <img src="https://github.com/WilliamSchack/Spotify-Downloader/assets/54973797/d4e1628a-031a-4735-b454-ecb5b2cea886" width="50%" height="50%">

**Now click Finish and the program is ready to be ran**
> <img src="https://github.com/WilliamSchack/Spotify-Downloader/assets/54973797/563d6a7a-2c8c-47ab-b45e-00d4c8939ab9" width="50%" height="50%">

You can open the program through the start menu or from the installed location through "Spotify Downloader.exe"

---

</details>

<details>
<summary><b> ZIP </b></summary>

---

**Unzip the file to any location through your chosen unzipping software.**
> <img src="https://github.com/WilliamSchack/Spotify-Downloader/assets/54973797/acdfb267-7f91-49be-8c93-aca623b8749f" width="50%" height="50%">

**The program can now be ran through "Spotify Downloader.exe"**
> <img src="https://github.com/WilliamSchack/Spotify-Downloader/assets/54973797/2e7771fe-1ab3-4e7f-a040-befb0fc6f8da" width="50%" height="50%">

---

</details>

## Usage

<details>
<summary><b> Setup </b></summary>

---

<img src="https://github.com/WilliamSchack/Spotify-Downloader/assets/54973797/84a4debe-4565-4154-84a7-413186f477c7" width="50%" height="50%">

**Enter Song/Playlist URL**
- The URL to your songs you would like to download
- Accepts:
    - Singlular Songs
    - Playlists
    - Albums

*Note: Your playlist or album must be public or else it will not work*

<details>
<summary><b> How To Find Your URL </b></summary>

**In App**
- Right click Your chosen song or playlist
- Share
- Copy link to playlist
> <img src="https://github.com/WilliamSchack/Spotify-Downloader/assets/54973797/7114f20e-9176-4e5b-990b-421f59ff8343" width="50%" height="50%">

**In Browser**
- Click on your playlist or song
- Copy the URL
> <img src="https://github.com/WilliamSchack/Spotify-Downloader/assets/54973797/409a2b15-1c9a-48d7-a9a1-3b505b699b5a" width="50%" height="50%">

</details>

**Select A Save Location**
- The path to your download location. You can click the file icon directly to the right of the input box to browse to a location.

---
    
</details>

<details>
<summary><b> Settings </b></summary>
    
---

<img src="https://github.com/WilliamSchack/Spotify-Downloader/assets/54973797/bc4ce06c-898e-4b23-b9ef-4e4f60b82e01" width="50%" height="50%">

**Overwrite Existing Files - Recommended: OFF**
- If enabled, downloading will overwrite previously downloaded files or not

**Show Status Notifications - Recommended: ON**
- If enabled, notifications will show when notable events happen during downloading to notify you incase it is in the background

**Downloader Threads - Recommended: 3**
- The amount of threads used while downloading songs. This will control how many songs download simultaneously but large values can negatively effect CPU usage. Cannot be changed during download.

**Download Speed Limit - Recommended: 0MB/s**
- The speed limit to downloading songs in MB/s. A value of 0 is uncapped

**Normalize Volume - Recommended: ON, -14dB**
- If enabled, changes the value of all songs to the same depending on its average volume.
    - Quite: -17dB
    - Normal: -14dB
    - Loud: -11dB

---
    
</details>

<details>
<summary><b> Downloading </b></summary>
    
---

<img src="https://github.com/WilliamSchack/Spotify-Downloader/assets/54973797/0b2a885f-09a7-4f37-8da9-1dd68b73345f" width="50%" height="50%">

Each track downloading will show its:
- Cover image
- Downloading index on that thread
- Title
- Artist(s)
- Progress Percent
- Downloading Status

You can see your total download progress on the counter at the bottom left.

The **Pause Button** will pause all current downloads

The **Settings Button** will open the settings menu allowing you to change most of the settings

---
    
</details>

<details>
<summary><b> Failed Downloads </b></summary>
    
---

<img src="https://github.com/ChazzBurger/Spotify-Downloader/assets/54973797/764b6460-fa02-47d6-96b3-ee259f23b4af" width="50%" height="50%">

Shows all of the songs that failed to download due to various reasons including:
- Song not available on youtube
- Song on youtube not close enough to spotify version

---
    
</details>

## Features

- **GUI**
    - Much easier to navigate and more user friendly than a command line
- **Easy Downloading**
    - Downloading starts with only two inputs and a button click making it easy to start
    - While downloading the program shows exactly what is happening
- **Multithreading**
    - Can download multiple songs at once
    - Splits playlist into multiple smaller lists that download seperately to speed up the download time significantly
- **Automatic Metadata**
    - All downloads include metadata to keep all the details and easily differentiate songs from one another
- **Many Settings**
    - Allows you to have full control over how the application works
- **Automatic Cleanup**
    - When quitting the application it will automatically quit and cleanup any currently downloading and temp files that will not be needed later

## Credits
Thanks to sigma67 for his [Python YT Music API](https://github.com/sigma67/ytmusicapi) that has been translated to C++ for this project

**Packages Used**
- [Qt5](https://www.qt.io/)
- [ffmpeg](https://www.ffmpeg.org/)
- [yt-dlp](https://github.com/yt-dlp/yt-dlp)
- [Taglib](https://github.com/taglib/taglib)
- [Difflib C++](https://github.com/duckie/difflib)

Thanks to you for using my program :)
