# ShowMIDI

ShowMIDI is a multi-platform GUI application to effortlessly visualize MIDI activity.

All the heavy lifting is done by the wonderful JUCE library.

The project website is https://github.com/gbevin/ShowMIDI

## Community

Join the Forums: https://forum.uwyn.com

Chat on Discord: https://discord.gg/TgsCnwqWTf

## Purpose

This application fills a void in the available MIDI monitoring solutions.

Instead of wading through logs of MIDI messages to correlate relevant ones and identify what is happening, ShowMIDI visualizes the current activity and hides what you don't care about anymore. It provides you with a real-time glanceable view of all MIDI activity on your computer.

Here's a tutorial video that shows you a quick overview of why I created ShowMID:

<a href="https://www.youtube.com/watch?v=TliuBpUY8pw" target="_blank"><img src="https://i.ytimg.com/vi/TliuBpUY8pw/maxresdefault.jpg" alt="Tutorial Video" width="640" height="360" border="0" /></a>

## Download

You can download pre-built binaries from the release section:
https://github.com/gbevin/ShowMIDI/releases

Since ShowMIDI is free and open-source, you can also easily build it yourself. Just take a look into the Builds directory when you download the sources.

## Usage

To use it, simply double-click the application to launch it.

To pause the MIDI data stream, press the spacebar, to un-pause, press it again.

## Building on Linux

To build ShowMIDI on Linux you need a minimal set of packages installed beforehand, on Ubuntu this can be done with:

```
sudo apt install build-essential pkg-config libasound2-dev \
  libfreetype6-dev libcurl4-openssl-dev libx11-dev \
  libxrandr-dev libxinerama-dev libxcursor-dev
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
