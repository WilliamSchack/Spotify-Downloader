<img width="1280" height="640" alt="EcholiteBannerLarge" src="https://github.com/user-attachments/assets/76b1049c-9a10-4b8d-bd83-83c55247ed91" />

# Echolite

[![Latest Release](https://img.shields.io/github/v/release/WilliamSchack/Spotify-Downloader?label=Latest%20Release&color=007ec6)](https://github.com/ChazzBurger/Spotify-Downloader/releases)
[![Downloads](https://img.shields.io/github/downloads/WilliamSchack/Spotify-Downloader/total?label=Downloads&color=007ec6)](https://github.com/ChazzBurger/Spotify-Downloader/releases)
[![Open Issues](https://img.shields.io/github/issues/WilliamSchack/Spotify-Downloader?label=Issues)](https://github.com/ChazzBurger/Spotify-Downloader/issues?q=is%3Aissue+is%3Aopen)
[![Closed Issues](https://img.shields.io/github/issues-closed/WilliamSchack/Spotify-Downloader?label=Issues)](https://github.com/ChazzBurger/Spotify-Downloader/issues?q=is%3Aissue+is%3Aclosed)
[![Stars](https://img.shields.io/github/stars/WilliamSchack/Spotify-Downloader?label=Stars&color=007ec6)](https://github.com/ChazzBurger/Spotify-Downloader/stargazers)
[![Ko-Fi](https://img.shields.io/badge/Support%20Development-FF5a16?style=flat&logo=Ko-fi&logoColor=FF5a16&label=ko-fi)](https://ko-fi.com/williamschack)

Echolite is an app that allows you to easily download songs from multiple platforms via one easy to use GUI including many settings to customise the output however you like.

> [!CAUTION]
> Note that users are responsible for any potential legal consequenses that comes with downloading music. I do not condone unauthorized downloading of copyrighted material and do not take any responsibility for user actions. I highly encourage you to purchase the songs directly from the artists to support them and their work

## Download

**This will be updated when the first v2 release is out, for now just click the releases button on the right side of the page for v1 releases. If you are seeing this, v2 will be released in a few hours :)**

> [!NOTE]
> Linux & MacOS builds **do not come packaged with the dependencies**. The app will detect system installed dependencies and use those, but if you do not have any installed, you can go to the Info Screen, then the Updates tab and download any that are not installed

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

## Features

- **Easy to use GUI**
- **Muliple Platforms**
	- Supports multiple platforms currently including:
	- Spotify
	- YouTube / YouTube Music
		- Has support for YouTube Premium audio quality
- **Multithreading**
	- Multiple threads are used to download multiple songs separately to speed up download time significantly
- **Metadata Saving**
	- Downloaded files include all the metadata found from the source
	- Title, Artists, Album Name & Artists, Track Number, Disk Number, Release Date & Year, Lyrics, Cover Art
	- The Source ID, Searched ID, and Lyrics source are also inputted the Copyright tag
	- *These vary based on what the sources provide but most of the time everything is filled. Metadata providers may come in a later update to fill missing data*
- **Multiple Formats**
	- M4A, AAC, MP3, OGG, WAV, FLAC, WEBM
	- *Formats here that should be lossless (WAV, FLAC) are not. They are converted from the source format which is lossy*
- **Audio Normalising**
	- The volume of every song can be normalised to a level you set
	- *Currently uses the track averages but more options will come later*
- **Dynamic File & Output Folders Naming**
	- File names can be set to use any combination of metadata in the songs
	- Sub folders can be created using any combination of metadata in the songs
	- These can be combined for organisation exactly how you like
- **Lyrics & LRC Files**
	- Lyrics are automatically embedded into the song metadata
	- LRC Files can be created along with each song at a customisable location using the dynamic file naming mentioned above
	- *Currently retrieved from the source/searched platforms, dedicated lyrics providers may come in a later update*
- **Playlist Files**
	- M3U, XSPF, PLS
	- Can be created with each download at a customisable location using the dynamic file naming mentioned above
- **Easy Dependency Updating**
	- The GUI includes a screen to easily update and install the dependencies the app requires
- **CLI**
	- Only includes basic commands at the moment, but will be documented and expanded with a dedicated cli update later
	- Provides the same json output for every platform
	- Currently includes: download-track, get-info, search, get-metadata
	- You can view the details on these at [ArgumentParser.cpp](https://github.com/WilliamSchack/Echolite/blob/main/src/cli/ArgumentParser.cpp) with proper documentation coming in the later cli update

## Usage

<img width="1500" height="794" alt="UsagePreview" src="https://github.com/user-attachments/assets/13d9cbc3-03ed-4126-bc24-aa262b336669" />

Echolite has been designed to be as simple to use as possible, as starting a download only takes one step, and every setting has a tooltip you can view by hovering its name that explains what it does in depth. If you are unsure of what a setting does, or want to know the valid tags for a dynamic naming setting, hover the name of the setting and it will show all the valid tags.

View below for platform specific instructions

> [!WARNING]
> Please note that downloading too many songs in a short time span can lead to YouTube flagging your IP and preventing further downloads (Will not effect your YouTube experience). I have not experienced it personally and have downloaded a few thousand songs within a day on a few occasions when testing but there is a case where someone downloaded 5000 songs in one day which got their IP flagged so if you intend to download a large amount of songs it is safer to spread it out over a few days or limit your download speed in the settings.

### YouTube

> [!NOTE]
> YouTube Cookies and PO Tokens are valid for a very short time usually being for at least 8 hours. It is recommended that you refresh both each time you use the program. They are stored in the temp folder for this reason

<details>
<summary><b>Retrieving & Inputting YouTube Cookies</b></summary>

<br />

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

Now that you have the cookies in your clipboard, on the YouTube Cookies setting either click the paste button, or click the Open File button and paste the cookies into the file that opens. Once saved, the app will update and tell you if they are valid or not.

If you have instead exported the cookies to a file, you can replace the file that opens with that one, or transfer the contents

> <img alt="image" src="https://github.com/user-attachments/assets/4bed7492-e2fe-4012-8b83-7fa4c5065cf5" style="width: 70%; height: 70%;"/>

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

Now take the copied PO Token and paste it into the YouTube PO Token Settings by clicking the paste button or just inputting it into the text field

> <img alt="image" src="https://github.com/user-attachments/assets/90e99f56-d3b2-482b-83d3-fffcb7e482ac" style="width: 70%; height: 70%;"/>

---

</details>

## Roadmap

The main bulk of the work is now complete with v2! With this refactor it will be much easier to work on and add new features so they should come a lot quicker. In saying that, now that I have released v2, I have a much larger project I will be working on and will work on this in my spare time outside of that, so that will probably about even out the time between releases

**Post v2 (In Order)**

- Initial bug fixing
	- This is a massive remake so there are bound to be many bugs, I want to dedicate some time to fixing those as people discover them
- UI Accessibility & small improvements
	- I have shipped v2 without proper tab support or screen readers in mind for the sake of releasing it, but I make sure thats all properly implemented soon
- Better volume normalisation
- Metadata providers to fill in missing data
- Lyrics providers
- Better CLI Support
- More platforms

## FAQ

**Where are songs downloaded from?**

In the case where downloading from the source either is not viable or will negatively effect your account, metadata is retrieved from the source platform but the audio is retrieved from another platform by searching for it and matching the metadata. View below for each platform that does not download from its platform

- Spotify > YouTube

**How accurate are the downloads?**

If a song is downloaded from its own source, then it will download directly from the input url (any platforms not listed above). But for any platforms that need to search on another, it uses an algorithm to match tracks using the metadata of both. This has been refined over the apps lifespan and has gotten pretty damn good but in some cases where the search platform has a cover and not the actual song, it may download the cover instead

**What is the audio quality of the downloaded songs?**

The audio quality of each song can be configured in the settings ranging from 33-192kb/s with an mp3 file, and 33-128kb/s for everything else. If this is not manually set, the app will use the quality of the source which usually is ~128kb/s. If the audio is downloaded from a platform where a higher quality is available, the quality will increase to that and the settings will adjust accordingly (eg. YouTube Premium, downloads up to 256kb/s)

**What can I download?**

The currently supported links are:

- Spotify: Track, Album, Playlist, Episode
- YouTube: Track, Album, Playlist