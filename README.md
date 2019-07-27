# M*Player
Multi-*channel*, multi-*track*, multi-*player* player for audio files.

## Features
* Supports basic audio file types like WAV and MP3
* Output to ASIO devices\*
* Freely configure output channels
* Play multi-track recordings consisting of one file per track

\* Requires a custom build, see [below](#how-to-compile).

## Download
Precompiled binaries are available for download:
* [Windows](http://serviushack.space/MStarPlayer/MStarPlayer.zip)

## Screenshot
![Main window with players](Screenshots/MainWindow.png)
![Audio configuration dialog](Screenshots/Configure%20Audio.png)

## Development
Pull requests are always welcome to improve the current code or add new features.

### How To Compile

1. Get [Visual Studio 2017](https://www.visualstudio.com/downloads/).
2. Clone this repository.
3. Download the [ASIO SDK](https://www.steinberg.net/de/company/developer.html) and extract it.
4. Download the [JUCE framework](https://shop.juce.com/get-juce).
5. Open MStarPlayer.jucer with Projucer from the JUCE framework.
6. Change the 'Header search paths' for the configurations to include the ASIO SDK.
7. Select File -> Save Project to generate the Visual Studio solution.
8. Open the solution at `Builds/VisualStudio2017/` and compile.

### Format Source Code

The format the source code according to the .clang-format file:

      powershell -executionpolicy bypass -file .\format-source.ps1

## License

M\*Player's own code is dual-licensed. This makes licensing a bit complicated but allows building it with ASIO.

Component      | License
---------------|--------
Icons          | LGPL
JUCE           | see https://github.com/WeAreROLI/JUCE
M\*Player code | GPL and ISC

The ISC version of the code can be used together with a JUCE license and an ASIO SDK license to build M\*Player with ASIO support.


### Why is there no ASIO support anymore?

The ASIO SDK and open source code are not compatible. To make it short this would have been the options:

M\*Player license | ASIO SDK | JUCE restriction
------------------|----------|-----------------
GPL               | no       | none
proprietary       | yes      | forced splashscreen

The dual-licensing still allows everyone to build M\*Player with ASIO support given they have a proper license for the ASIO SDK and JUCE.


## Notes
Unnecessary binary data and the ASIO framework have been removed from the commit history. This means older revisions might not compile.
