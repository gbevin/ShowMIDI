# ShowMIDI

ShowMIDI is a multi-platform GUI application to effortlessly visualize MIDI activity.

All the heavy lifting is done by the wonderful JUCE library.

The project website is https://github.com/gbevin/ShowMIDI

## Community

Join the Forums: https://forum.uwyn.com

Chat on Discord: https://discord.gg/TgsCnwqWTf

## Purpose

This application fills a void in the avaiable MIDI monitoring solutions.

Instead of wading through logs of MIDI messages to correlate relevant ones and identify what is happening, ShowMIDI visualizes the current activity and hides what you don't care about anymore. It provides you with a real-time glanceable view of all MIDI activatity on your computer.

## Download

You can download pre-built binaries from the release section:
https://github.com/gbevin/ShowMIDI/releases

Since ShowMIDI is free and open-source, you can also easily build it yourself. Just take a look into the Builds directory when you download the sources.

If you're using the macOS Homebrew package manager, you can install ShowMIDI with:
```
brew install gbevin/tools/showmidi
```

## Usage
To use it, simply double-click the application to launch it.

## Building on Linux

To build ShowMIDI on Linux you need a minimal set of packages installed beforehand, on Ubuntu this can be done with:

```
sudo apt install build-essential pkg-config libasound2-dev libfreetype6-dev libcurl4-openssl-dev libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev
```

After that, go to the `LinuxMakeFile` directory

```
cd Builds/LinuxMakeFile
```

and build the binary by typing `make`

```
make CONFIG=Release
```

The resulting binary will be in the `Build/LinuxMakeFile/build` directory and can be moved anywhere appropriate on your system, for instance into `/usr/local/bin`:

```
sudo mv build/showmidi /usr/local/bin
```
