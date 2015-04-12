# M*Player
Multi-*channel*, multi-*track*, multi-*player* player for audio files.

## Features
* Supports basic audio file types like WAV and MP3
* Output to ASIO devices
* Freely configure output channels
* Play multi-track recordings consisting of one file per track

## Download
Precompiled binaries are available for download:
* [Windows](http://serviushack.space/MStarPlayer/MStarPlayer.zip)

Due to the restrictive license of the ASIO SDK I'm bulding this on my own buildsystem.

**Note for Avira users:** The executable does NOT contain any virus. You can verify this using [VirusTotal](https://www.virustotal.com). Unfortunately Avira is unable to fix their detection even after multiple reports.

## Screenshot
![Main window with players](http://serviushack.space/MStarPlayer/screenshots/MainWindow.png)
![Audio configuration dialog](http://serviushack.space/MStarPlayer/screenshots/Configure Audio.png)

## Development
Pull requests are always welcome to improve the current code or add new features.

### How To Compile with Visual Studio 2013

1. Clone this repository
2. Download the [ASIO SDK](https://www.steinberg.net/de/company/developer.html) and extract it into the repository root.
3. Download the JUCE framework from http://www.juce.com/download and compile the Introjucer in `extras/Introjucer`.
4. Start the Introjucer, open MStarPlayer.jucer and select File -> Save Project to generate the Visual Studio 2013 solution.
5. Open the solution at `Builds/VisualStudio2013/` and let it compile.

## Notes
Unnecessary binary data and the ASIO framework have been removed from the commit history. This means older revisions might not compile.