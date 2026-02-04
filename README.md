<img src="https://github.com/ChazzBurger/Spotify-Downloader/assets/54973797/0998c0ca-bc59-4cb4-a9f5-76956d3bbe27" align="right" style="height: 6%; width: 6%;">

# Spotify Downloader
[![Latest Release](https://img.shields.io/github/v/release/WilliamSchack/Spotify-Downloader?label=Latest%20Release&color=007ec6)](https://github.com/ChazzBurger/Spotify-Downloader/releases)
[![Downloads](https://img.shields.io/github/downloads/WilliamSchack/Spotify-Downloader/total?label=Downloads&color=007ec6)](https://github.com/ChazzBurger/Spotify-Downloader/releases)
[![Open Issues](https://img.shields.io/github/issues/WilliamSchack/Spotify-Downloader?label=Issues)](https://github.com/ChazzBurger/Spotify-Downloader/issues?q=is%3Aissue+is%3Aopen)
[![Closed Issues](https://img.shields.io/github/issues-closed/WilliamSchack/Spotify-Downloader?label=Issues)](https://github.com/ChazzBurger/Spotify-Downloader/issues?q=is%3Aissue+is%3Aclosed)
[![Stars](https://img.shields.io/github/stars/WilliamSchack/Spotify-Downloader?label=Stars&color=007ec6)](https://github.com/ChazzBurger/Spotify-Downloader/stargazers)
[![Ko-Fi](https://img.shields.io/badge/Support%20Development-FF5a16?style=flat&logo=Ko-fi&logoColor=FF5a16&label=ko-fi)](https://ko-fi.com/williamschack)

> [!NOTE]
> **This is the development branch for v2**<br />
> This readme is partially from the main branch before v2 so most of the details here will not apply<br />
> For instructions on compiling the program, view below

## Compiling

Notes:
- This is my first app using cmake so it may have problems, if you find any please [submit an issue](https://github.com/WilliamSchack/Spotify-Downloader/issues/new/choose)
- I have not tested this on mac and probably wont until the end of 2.0 development since I cant be bothered to setup a vm for it
- This does not have arm support at the minute, I will add it near the end of 2.0 development

**Dependencies**
- Qt6
- Curl
- CMake

1 - **Download/Clone this branch**

2 - **In the root folder:**

```
mkdir build && cd build
cmake ..
make
```

3 - **If on linux, make the output executable:**

`chmod +x ./UnnamedSongApp`

4 - **Run the app:**

`./UnnamedSongApp`

--

# Pre-v2 Readme

Spotify Downloader is an application that allows you to easily download spotify playlists and songs through YouTube without the need of Spotify Premium through an easy to use GUI and many customisable settings to get the output that you desire.

> [!CAUTION]
> Note that users are responsible for any potential legal consequenses that comes with downloading music. I do not condone unauthorized downloading of copyrighted material and do not take any responsibility for user actions. I highly encourage you to purchase the songs directly from the artists to support them and their work

## Contents
- [Installation](#installation)
- [Usage](#usage)
- [Features](#features)
- [Roadmap](#roadmap)
- [License](#license)
- [FAQ](#faq)
- [Credits](#credits)

## Installation
***Only currently supports windows. Other platforms may be coming soon***

The latest release can be found [Here](https://github.com/WilliamSchack/Spotify-Downloader/releases/latest). **For multiple use Installer Recommended, for single use Portable Recommended**

Follow the instructions below depending on your chosen install type.

<details>
<summary><b> Installer </b></summary>

---
 
**Choose your install location. Default is "C:/Program Files/Spotify Downloader" but you can change this to anywhere you like.**

*Note: If the program is already installed in the chosen location, it will overwrite and update the old install*
> <img src="https://github.com/WilliamSchack/Spotify-Downloader/assets/54973797/ed720dd2-1862-4247-8714-b1ba9f8b3c4c" style="width: 50%; height: 50%;">

**Select the Spotify Downloader component (selected by default)**
> <img src="https://github.com/WilliamSchack/Spotify-Downloader/assets/54973797/8d64c8a9-1453-4b2d-b2ee-cf40742f31d3" style="width: 50%; height: 50%;">

**The program will add a shortcut to the start menu and the name can be changed here**
> <img src="https://github.com/WilliamSchack/Spotify-Downloader/assets/54973797/0a8b4ec7-8fd1-41ea-a522-0182c0f0ebca" style="width: 50%; height: 50%;">

**Click install and wait for the installer to finish**
> <img src="https://github.com/WilliamSchack/Spotify-Downloader/assets/54973797/d4e1628a-031a-4735-b454-ecb5b2cea886" style="width: 50%; height: 50%;">

**Now click Finish and the program is ready to be ran**
> <img src="https://github.com/WilliamSchack/Spotify-Downloader/assets/54973797/563d6a7a-2c8c-47ab-b45e-00d4c8939ab9" style="width: 50%; height: 50%;">

You can open the program through the start menu or from the installed location through "Spotify Downloader.exe"

---

</details>

<details>
<summary><b> Portable/ZIP </b></summary>

---

**Unzip the file to any location through your chosen unzipping software.**
> <img src="https://github.com/WilliamSchack/Spotify-Downloader/assets/54973797/acdfb267-7f91-49be-8c93-aca623b8749f" style="width: 50%; height: 50%;">

**The program can now be ran through "Spotify Downloader.exe"**
> <img src="https://github.com/WilliamSchack/Spotify-Downloader/assets/54973797/2e7771fe-1ab3-4e7f-a040-befb0fc6f8da" style="width: 50%; height: 50%;">

---

</details>

<details>
<summary><b> Windows protected your PC popup, Windows Defender false positive </b></summary>

---

Since I am not a known publisher to microsoft and the app is not signed, the "Windows protected your PC" popup will always happen on first open and on some machines Windows defender may block the app from running if you are using the portable version

The only way I would be able to fix this is to sign the app but certificates are very costly and not something I would consider doing

### Closing the "Windows protected your PC" popup

Click:
- More Info
- Run Anyway

The popup should not open again after the first time running the app. It most likely will happen after updating the app though

<img src="https://github.com/user-attachments/assets/1ea63731-380e-4d18-b2c7-c2c99fe95cc6" style="width: 40%; height: 40%;">
<img src="https://github.com/user-attachments/assets/55148d8c-ebd9-4828-8775-d765bf5f4246" style="width: 40%; height: 40%;">

### Windows Defender detecting the app

This has been fixed and the app has been excluded from windows defender scans, if the app is still getting blocked, updating your malware definitions should fix it:

1. Open command prompt as administrator and change directory to c:\Program Files\Windows Defender
2. Run “MpCmdRun.exe -removedefinitions -dynamicsignatures”
3. Run "MpCmdRun.exe -SignatureUpdate"

---

</details>

## Usage

> [!WARNING]
> Please note that downloading too many songs in a short time span can lead to YouTube flagging your IP and preventing further downloads (Will not effect your YouTube experience). I have not experienced it personally and have downloaded a few thousand songs within a day on a few occasions when testing but there is a case where someone downloaded 5000 songs in one day which got their IP flagged so if you intend to download a large amount of songs it is safer to spread it out over a few days or limit your download speed in the settings.

### User Interface

<details>
<summary><b> Side Panel</b></summary>

---

***Note: Icon colour can be toggled to white by changing the Sidebar Icons Colour setting***

<img src="https://github.com/user-attachments/assets/ad2e1342-e124-4308-9ca9-d3379f9ba4b4" style="width: 4%; height: 4%;">

<br/>

**Download Screen Button**
- Takes you to the setup or processing screen depending on if a download is started or not

**Error Screen Button**
- Takes you to the previous download's song errors
- ***Note: Button is non-interactable and faded if no previous download has errors***

**Settings Screen Button**
- Takes you to the settings screen

**Notices Screen Button**
- Takes you to the notices screen
- Can be hidden in the settings

**Donate Button**
- Click to donate and support the development of this program
- ***Note: Opens in browser***

**Update Button**
- Click to check for updates
- The icon will change state depending on the checking status, and if an update is available or not
- If an update is available it will redirect you to the newest github release
    - ***Note: Opens in browser***

**Submit Bug Button**
- Click to submit an issue to the github repo
- ***Note: Opens in browser***

**Help Button**
- Click to access the help documentation (What you are reading now)
- ***Note: Opens in browser***

---

</details>

<details>
<summary><b> Setup </b></summary>

---

<img src="https://github.com/user-attachments/assets/de184e10-d1e4-41bc-a41f-bcbc07af38d3" style="width: 50%; height: 50%;">

**Download Status**
- Shows the result of the last download

**Enter Song/Playlist URL**
- The URL to your songs you would like to download
- You can click the paste button directly to the right to input the copied text from your clipboard into the input
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
> <img src="https://github.com/WilliamSchack/Spotify-Downloader/assets/54973797/7114f20e-9176-4e5b-990b-421f59ff8343" style="width: 50%; height: 50%;">

**In Browser**
- Click on your playlist or song
- Copy the URL
> <img src="https://github.com/WilliamSchack/Spotify-Downloader/assets/54973797/409a2b15-1c9a-48d7-a9a1-3b505b699b5a" style="width: 50%; height: 50%;">

</details>

**Select A Save Location**
- The path to your download location
- You can click the browse button directly to the right to browse to a location
- ***Note: Saved between sessions***

---
    
</details>

<details>
<summary><b> Settings </b></summary>
    
---

***Note: All settings are saved between sessions***

### Output

<img src="https://github.com/user-attachments/assets/e8ac28a3-d7b8-416e-b7a8-1b94ed4c5acb" style="width: 50%; height: 50%;">
<img src="https://github.com/user-attachments/assets/fa9a30c5-c9b1-4429-a6c1-9217d3091eda" style="width: 50%; height: 50%;">
<img src="https://github.com/user-attachments/assets/379444fa-06e1-4a83-aa30-9b299adf8e1e" style="width: 50%; height: 50%;">

**Overwrite Existing Files - Recommended: OFF**
- If enabled, downloading will overwrite previously downloaded files or not

**Codec - Recommended: M4A**
- The format the each song will be output as
- ***Note: All formats will be converted from M4A and as a result WAV & FLAC are lossy and only implemented for convenience***

**Codec Details**
- Displays conversion details to remind that it is converted from an M4A file
- Shows what type of metadata is used for the selected format
- Shows warnings in red when something is different (No cover art for example)

**Normalize Volume - Recommended: ON, -14dB**
- If enabled, changes the value of all songs to the same depending on its average volume.  
    - Quite: -17dB
    - Normal: -14dB
    - Loud: -11dB

**Auto Best Quality - Recommended: ON**
- If enabled, the bitrate will not be modified and the download will use the bitrate from the youtube video
- Only disable if you want to manually set the bitrate yourself

**Audio Bitrate - Recommended: 128kb/s**
- Sets the output quality of each song downloaded
- The quality range will change depending on the codec and if you have YouTube Premium
- **Non-MP3 (No Premium / Premium)**
    - High Quality: 128kb/s, 256kb/s
    - Good Quality: 92kb/s, 192kb/s
    - Low Quality:  64kb/s, 128kb/s
- **MP3 (No Premium / Premium)**
	- High Quality: 192kb/s, 320kb/s
    - Good Quality: 160kb/s, 256kb/s
    - Low Quality:  128kb/s, 192kb/s
	
**Average File Size**
- Displays a estimated file size based on the chosen bitrate
- The file size is different for if you use a compressed or PCM file format
- Compressed File Size (MB) = (bitrate(kbs) * duration seconds) / 8) / 1024
- PCM File Size (MB) = (44.1 * (16 / 8) * 2 * duration seconds) / 1024
  - PCM files use a preset 44.1kHz, 16 bits, & 2 channels
  - Calculation without preset is (Sample Rate * (Bits Per Sample / 8) * Channels * Seconds) / 1024
- ***Does not take into account metadata, an extra ~0.9MB is added***

**Artist Separator - Recommended: ;**
- Sets the seperator between artists in the metadata

**Track Number**
- The track number that will be assigned to the metadata
  - **Playlist** - The playlist track number
  - **Album** - The album track number
  - **Disk** - The disk number

**File Name**
- Enclosing tag input sets the characters to open and close a tag
    - Left character sets the opening input
    - Right character sets the closing input
- File Name Input sets the output name of each song.
    - Each tag must be enclosed with the characters set in the left input field
- If there are multiple songs with the file name, the spotify id will be appended to the end.
- Can only include the currently implemented tags listed below

**Sub Folders**
- Enclosing tag input sets the characters to open and close a tag
    - Left character sets the opening input
    - Right character sets the closing input
- Sub Folders Input sets the sub folders within the chosen directory that each song will be saved to
    - Each tag must be enclosed with the characters set in the left input field
- Can only include the currently implemented tags listed below

**Create LRC File**
- Creates a LRC file for each song downloaded

**LRC File Type**
- Enclosing tag input sets the characters to open and close a tag
    - Left character sets the opening input
    - Right character sets the closing input
- Input specifies how the lrc file will be named. This must be an **ABSOLUTE FILE PATH** using &lt;Download Path&gt; will make it relative to the download path
    - "../" is also valid, it will go back a folder
    - **Save Location** is also a valid tag which will make the file relative to the download path

**File Name, Sub Folders, LRC File Tags**
- These are the tags that can be used by the File Name & Sub Folders settings
    - **Song Name** - Name of the song
    - **Album Name** - Name of the album
    - **Song Artist** - Name of the first artist of a song
    - **Song Artists** - Names of each artist of a song "Artist1, Artist2, Artist3, ..."
    - **Album Artist** - Name of the first artist of the songs album
    - **Album Artists** - Names of each artist of a songs album "Artist1, Artist2, Artist3, ..."
    - **Codec** - The codec of the downloaded song
    - **Track Number** - The currently assigned track number
    - **Playlist Track Number** - Track number in the playlist (Only works when downloading a playlist)
    - **Album Track Number** - Track number in the album
    - **Disk Number** - Track disk number
    - **Song Time Seconds** - Time of the song in seconds
    - **Song Time Minutes** - Time of the song in minutes "01.23"
    - **Song Time Hours** - Time of the song in hours "00.01.23"
    - **Year** - Release Year of the song
    - **Month** - Release month of the song
    - **Day** - Release day of the song
    - **Codec** - The codec of the downloaded song

**Playlist File Type - Recommended M3U if using, None if not**
- The type of playlist that will be outputted when the download is finished
   - **None** - Doesn't output a playlist file
   - **M3U, XSPF, PLS** The type of playlist file that will be outputted

**Playlist File Name**
- Enclosing tag input sets the characters to open and close a tag
    - Left character sets the opening input
    - Right character sets the closing input
- Input specifies the way the playlist file will be named. This must be an **ABSOLUTE FILE PATH** using &lt;Download Path&gt; will make it relative to the download path
- "../" is also valid, it will go back a folder

**Playlist File Name Tags**
- These are the tags that can be used by the Playlist File Name setting
   - **Save Location** - The save location set in the main screen
   - **Playlist Name** - The name of the playlist being downloaded from (When using an album, uses album name)
   - **Playlist Owner** - The owner of the playlist being downloaded from (When using an album, uses album artists)

### Downloading

<img src="https://github.com/user-attachments/assets/39296c6c-868e-43c2-8776-0b5250cee888" style="width: 50%; height: 50%;">
<img src="https://github.com/user-attachments/assets/b07072a1-7af7-4dc7-9a4f-2271b27ffadd" style="width: 50%; height: 50%;">

**Show Status Notifications - Default: ON**
- If enabled, notifications will show when notable events happen during downloading to notify you incase it is in the background

**Downloader Threads - Recommended: 6**
- The amount of threads used while downloading songs. This will control how many songs download simultaneously but large values can negatively effect CPU usage. Cannot be changed during download.

**Download Speed Limit - Recommended: 0MB/s**
- The speed limit to downloading songs in MB/s. A value of 0 is uncapped

**Download Timeout - Default: 30000**
- How long it takes for a download to time out if no progress is made in ms

**YouTube Cookies**
- The YouTube cookies used for downloading songs. Used to download songs with age restrictions and at a higher quality with YouTube Premium
- Has Clear, Paste, & Help buttons

**PO Token**
- The PO Token used to authenticate your cookies and reduce the risk of your account being flagged
- Has Clear, Paste, & Help buttons

***For help inputting your own YouTube cookies, view the lower section titled "Inputting your YouTube Cookies""***

**Spotify Client ID**
- Spotify API Client ID used for fetching songs
- Has Clear, Paste, & Help buttons
- **If empty uses the default Spotify API Client ID**

**Spotify Client Secret**
- Spotify API Client Secret used for fetching songs
- Has Clear, Paste, & Help buttons
- **If empty uses the default Spotify API Client Secret**

***For help creating your own spotify app and keys, view the lower section titled "Creating your own Spotify API keys"***

### Interface

<img src="https://github.com/user-attachments/assets/8d1e85f5-0c26-437a-97e8-a68f71f08f7c" style="width: 50%; height: 50%;">

**Downloader Thread UI - Recommended: Compact**
- Changes the style of each thread UI while downloading
- **Compact:** Newer compact and smaller design
- **Original:** Older larger design, spaces out details
- **Dynamic:** Switches between compact and original depending on threads used to fit the screen

**Auto Open Download Folder - Default: ON**
- Toggles if the application automatically opens the target folder after a finished download

**Sidebar Icons Colour - Default: ON**
- Enables colour for the sidebar icons

**Check For Updates - Default: ON**
- Toggles if the app checks for updates when launching

**Notices Screen - Default: ON**
- Toggles if the notices screen button is hidden

**Check For Notices - Default: ON**
- Toggles if the app checks for notices when launching
- ***Note: Hidden and disabled when Notices Screen is disabled***

---
    
</details>

<details>
<summary><b> Downloading </b></summary>
    
---

<img src="https://github.com/user-attachments/assets/6b419783-3d06-4457-b3c9-25b1331763ca" style="width: 50%; height: 50%;">

Each track downloading will show its:
- Cover image
- Downloading index on that thread
- Title
- Artist(s)
- Progress Percent
- Downloading Status

You can see your total download progress on the counter at the bottom left.

**Download Progress**
- Shows your total download progress
    - Located in the bottom left corner

**Pause Button**
- Pauses all current downloads

**Cancel Button**
- Cancels the current download and returns to the setup screen

**Settings Button**
- Opens the settings menu, download continues in the background while open

---
    
</details>

<details>
<summary><b> Download Errors </b></summary>
    
---

<img src="https://github.com/user-attachments/assets/d7b6c70f-6f5b-4797-8b66-bb4a8c4509b5" style="width: 50%; height: 50%;">

Shows all of the songs that failed to download due to various reasons that will be listed below each song

If a song could not be found on youtube, you can manually input a link in the input that appears below.
- Clicking the search button will take you to youtube with a prefilled search query
- Clicking the download button will re-download the song with the inputted youtube link

---
    
</details>

<details>
<summary><b> Notices </b></summary>
    
---

<img src="https://github.com/user-attachments/assets/e849b086-185d-40fc-b211-571232942d3d" style="width: 50%; height: 50%;">

Shows various notices including changelogs and progress updates to the app. Will be updated with more notices over time

---
    
</details>

### Assigning YouTube Cookies / Spotify API Keys

> [!CAUTION]
> By using your account with Spotify Downloader you risk your account being flagged or at worse banned. If using an account be cautious of the rate and amount of downloads you are doing. Adding an account is only used for downloading age restricted songs and using YouTube Premium higher quality audio so if you dont intend to use those features I advise you do not add your own cookies.

> [!NOTE]
> YouTube Cookies and PO Tokens are valid for a very short time usually being for at least 8 hours. It is recommended that you refresh both each time you use the program.

<details>
<summary><b> Inputting your YouTube Cookies</b></summary>

---

***YouTube Cookies are not required***

**The way of retrieving cookies and PO Tokens will vary from browser to browser but I will put the general steps with some browser specific instructions below. There are also images in each of the dropdowns if needed**

***It is recommended you do the following steps in a private/incognito window so the cookies are not effected by general use***

### Retrieving Your YouTube Cookies

First open [YouTube Music](https://music.youtube.com/) and sign in with the account you wish to use

If you are using premium and want higher quality downloads you will first need to go to your settings and change the Audio Quality to High

<details>
<summary><b> How To Change YT Music Audio Quality</b></summary>
	
> <img src="https://github.com/user-attachments/assets/0f8fa4a8-0f25-4726-993c-1944fe10dd22" style="width: 50%; height: 50%;">
> <img src="https://github.com/user-attachments/assets/489980fc-e043-427a-8c06-e87ccd3ab31c" style="width: 50%; height: 50%;">

</details>

Now retrieve your netscape formatted cookies from YouTube

There are many ways to do this but I use the extension **Cookie-Editor** ([Firefox](https://addons.mozilla.org/en-US/firefox/addon/cookie-editor/)) ([Chrome](https://chromewebstore.google.com/detail/cookie-editor/hlkenndednhfkekhgcdicdfddnkalmdm)) ([Edge](https://microsoftedge.microsoft.com/addons/detail/cookieeditor/neaplmfkghagebokkhpjpoebhdledlfi))

<details>
<summary><b> Exporting cookies using Cookie-Editor</b></summary>

Open the extension and export your cookies as netscape

> <img src="https://github.com/user-attachments/assets/61ffae4c-cd2d-4c8a-b52b-9c23174e5d8e" style="width: 50%; height: 50%;">
> <img src="https://github.com/user-attachments/assets/b719478e-490c-4956-88a0-ea06de6166c6" style="width: 50%; height: 50%;">

</details>

Now that you have got the cookies in your clipboard, paste them into the downloading settings in the Spotify Downloader
> <img src="https://github.com/user-attachments/assets/1fe7e9d2-99f6-4579-ac3e-a5efb49b3ad1" style="width: 50%; height: 50%;">

### Retrieving Your PO Token

Getting the PO Token is a bit more complicated but if you are having trouble view the browser specific images below

Open your browsers developer console (Usually F12) and go to the network tab. In the filter section enter "googlevideo.com" to only show results under the googlevideo.com domain

Now navigate to a song and start playing it. You will notice that some results now popup in the network tab. Click on any of them

In the headers tab look for the POST Url and find the argument "pot". Copy everything to the right of the "=" and to the left of the "&" before the next argument

<details>
<summary><b> Firefox</b></summary>
	
> <img src="https://github.com/user-attachments/assets/e957ee3f-9346-456d-b8b2-323d456299e1" style="width: 50%; height: 50%;">
> <img src="https://github.com/user-attachments/assets/c0e48308-719e-461d-8a48-c309ac2bb70f" style="width: 50%; height: 50%;">

</details>

<details>
<summary><b> Chrome/Edge</b></summary>

> <img src="https://github.com/user-attachments/assets/a571a6f5-0648-4f62-a4d7-bfd1742df6e5" style="width: 50%; height: 50%;">
> <img src="https://github.com/user-attachments/assets/871d9412-2ca5-4d67-99b9-b9e4bd869c2e" style="width: 50%; height: 50%;">

</details>

Now take the copied PO Token and paste it into the downloading settings in the Spotify Downloader

> <img src="https://github.com/user-attachments/assets/5b566303-6827-4604-bd8f-ed7de5e971d2" style="width: 50%; height: 50%;">

---

</details>

<details>
<summary><b> Creating your own Spotify API keys </b></summary>

---
 
***Spotify API Keys are not required***
 
Go to the [spotify developer dashboard](https://developer.spotify.com/dashboard) and sign in with your spotify account *(Can be any it doesnt matter)*

Click the "Create app" button and input the following fields:

<img src="https://github.com/user-attachments/assets/5d999095-874d-4929-a40a-9ebdce8501c0" style="width: 50%; height: 50%;">

- **App Name:** Anything you like
- **App Description:** Anything you like
- **Redirect URIs:** *(For each input them and click the "Add" button on the right)*
  - http://127.0.0.1:1337/callback
  - http://localhost:8888/callback
-  **Which API/SDKs are you planning to use?** Tick "Web API"

Once you have read and accepted Spotify's [Developer Terms of Service](https://developer.spotify.com/terms) and [Design Guidelines](https://developer.spotify.com/documentation/design) you can click the "Save" button at the bottom

Now in the app page, click the "Settings" button at the top right

In this menu click the "View client secret" button to access the secret and now you have both the Client ID and Client Secret. Copy these both into the respective fields in the Spotify Downloader and it will now use your keys

<img src="https://github.com/user-attachments/assets/8a46ffde-38cd-4486-bccd-303775746c89" style="width: 50%; height: 50%;">

---

</details>

## Features

- **Simple GUI**
- **Easy Downloading**
- **Multithreading**
    - Automatically splits playlists into smaller lists that download seperately speeding up download time significantly
- **Automatic Metadata Saving**
    - Automatically saves the following to each file:
    - Song Title
    - Artists
    - Album Cover
    - Album name & artists
    - Track Number
    - Year
    - Track Number
    - Lyrics
    - Spotify ID, Youtube ID, & Lyrics source used to download the song
- **Many Downloading Settings**
    - Many settings for downloading including:
    - Overwriting
    - Audio Codec
    - Volume Normalization
    - Audio Bitrate
    - Dynamic File Naming
    - Sub-Folders
    - Downloading Thread Count & Speed Limit
    - Multiple GUI Settings
- **Multiple File Formats**
    - M4A
    - AAC
    - MP3
    - OGG Vorbis
    - WAV
    - FLAC
- **Lyrics & LRC Files**
    - Lyrics are automatically embedded into the song metadata
    - LRC files can be created along with each song at a customisable location
- **Playlist Files**
    - Can create a playlist file with each download at a customisable location
    - Supports the following types:
    - M3U
    - XSPF
    - PLS
- **Support for YouTube Premium Audio Quality**
- **Automatic Cleanup**

## Roadmap

The next few updates will take a fair amount of work and time so figured I would put this here so you all know what is going on. No time frames because I dont know how long everything will take and im doing this project on the side so I dont have much time to put into it, but hopefully at least v2.0.0 will be out by the end of this year

- **Complete refactor**
   - Doing this because the codebase has become a mess, this will make it much easier to work on and add new features
   - Not going to release it as its own update as there will be no new features, will be released with v2.0.0
- **v2.0.0 UI Redesign, Rebrand, Linux/Mac Support, New Features**
   - Im completely redesigning and remaking the UI since the current one is becoming a bit hard to work with. It has also been planned for a while and the current one was only meant to be temporary, its been needed for a while now
   - YouTube downloading support
   - Multi-Platform playlists
   - Get platform links from local files
   - Metadata editing
   - Support for Linux & Mac
   - Because of the major changes the app will be going through a rebrand, I do not have a name at the moment but I'll get around to it
   - Since this update will be the first with the refactor as well, expect more issues then other updates but I will try to find and fix as many as I can before release
- **v2.0.0 - v3.0.0 Feature Reworks**
   - There are a few features that are poorly implemented and could be much better so I for those I want to go through a proper process to implement them
   - From v2.0.0 I will start properly planning out features and I will re-do some that are currently implemented to make them better to use

## License
***Spotify Downloader is distributed under the GNU General Public License v3.0 from 17/04/2024 and Release v1.1.3***

Spotify Downloader is a program that downloads songs from Spotify.
Copyright (C) 2024  William Schack

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

## FAQ
<details>
<summary><b>Where are songs downloaded from?</b></summary>
Despite what the title implies, songs are actually downloaded from youtube. The program retrieves the data of each song from spotify and uses a combination of the duration, title, artists, album, and views in special cases, to determine which songs on youtube correspond to those on spotify. You can find the algorithm behind this in <a href="https://github.com/search?q=repo%3AWilliamSchack%2FSpotify-Downloader+Song%3A%3ASearchForSong&type=code">Song.cpp under SearchForSong</a>.
</details>

<details>
<summary><b>How accurate are the downloads?</b></summary>
The downloads are almost always accurate and have only few times has downloaded the incorrect song being usually covers of a song. If the song is not on youtube at all, it will not download the song if there are no extremely similar songs in title, artists, duration, etc. After roughly 500 songs tested over many artists and genres I have only gotten around 5-10 incorrect which were all covers of songs that were not on youtube. You can find the algorithm behind this in <a href="https://github.com/search?q=repo%3AWilliamSchack%2FSpotify-Downloader+Song%3A%3ASearchForSong&type=code">Song.cpp under SearchForSong</a>.
</details>

<details>
<summary><b>What can I download from spotify?</b></summary>
You can download a playlist, album, episode, or individual songs. You also cannot download all songs from a given artist but if you need to get around this the only suggestion I have is to compile all of their songs into a playlist and input that into the program but I may have a look into supporting this in the future. If you need any help finding the link for your music, there is a guide <a href="https://github.com/WilliamSchack/Spotify-Downloader?tab=readme-ov-file#usage">Here</a> under Setup > How To Find Your URL.
</details>

<details>
<summary><b>What is the quality of the downloaded songs?</b></summary>
The audio quality of each song can be configured in the settings ranging from 33-256kb/s with an mp3 file, and 33-128kb/s for everything else in steps of 32 (excluding 33). The program downloads the highest quality stream from youtube usually being around a 128kb/s m4a file and converts it to the desired quality.
</details>

<details>
<summary><b>Why are the lossless audio formats lossy?</b></summary>
Each file is downloaded as an M4A format through youtube at approx ~128kb/s. This is then later converted to the codec chosen in the settings. Because of this formats like WAV that are usually lossless will still have the quality of that M4A file and the quality cannot be increased further. Lossless audio formats where only implemented as it may be easier for some people to download it with those formats if they require them rather than converting each file themselves.
</details>

<details>
<summary><b>Why would my IP be flagged and how can I fix it?</b></summary>
Downloading too many songs in a short time span can lead to YouTube flagging your IP and prevent further downloads (Will not effect your YouTube experience). I have not experienced it personally and have downloaded a few thousand songs within a day on a few occasions when testing but there is a case where someone downloaded 5000 songs in one day which got their IP flagged so it is only administered on overuse. I have no control over when YouTube flags your IP or have any clue what the threshold is but after it has been done there isn't much I can do. The only suggestion I have is to disable your VPN as its IP may have been previously flagged if you have one enabled or try using one if not. 
</details>

<details>
<summary><b>Why have there been months between updates?</b></summary>
When I first created this project I has a lot of free time on my hands and was able to work on it for as long as I wanted, but now I have other responsibilities that are taking up a good chunk of my time and there are other projects that I work on outside of this one so I just do not have the time to work on this constantly. This project is being made out of passion and I do work on it often enough to now release updates hopefully monthly and if not bi-monthly, but do not let that put you off as I enjoy working on this project and no matter the breaks that I have taken, this will continue to be developed. If anything does happen where I am unable to work on the project anymore, I will update the readme to reflect that.
</details>

## Credits
**Packages Used**
- [Qt](https://www.qt.io/) - [LGPL v3 License](https://doc.qt.io/qt-6/lgpl.html)
- [Python YT Music API](https://github.com/sigma67/ytmusicapi) - [MIT License](https://github.com/sigma67/ytmusicapi/blob/main/LICENSE) (Translated to C++)
- [Python Musixmatch API](https://github.com/Strvm/musicxmatch-api) - [MIT License](https://github.com/Strvm/musicxmatch-api?tab=readme-ov-file#license) (Translated to C++)
- [ffmpeg essentials-www.gyan.dev](https://www.ffmpeg.org/) - [LGPL v2.1 License](https://www.ffmpeg.org/legal.html)
- [yt-dlp](https://github.com/yt-dlp/yt-dlp) - [Unlicense](https://github.com/yt-dlp/yt-dlp/blob/master/LICENSE)
- [Taglib](https://github.com/taglib/taglib) - [LGPL v2.1 License](https://github.com/taglib/taglib/blob/master/COPYING.LGPL), [Mozilla Public License](https://github.com/taglib/taglib/blob/master/COPYING.MPL)

Thanks to you for using my program :)
