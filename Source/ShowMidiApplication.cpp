/*
 * This file is part of ShowMIDI.
 * Copyright (command) 2023 Uwyn LLC.  https://www.uwyn.com
 *
 * ShowMIDI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ShowMIDI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "ShowMidiApplication.h"

#include "MainWindow.h"

using namespace showmidi;

ShowMidiApplication::ShowMidiApplication()
{
    juce::Desktop::getInstance().setDefaultLookAndFeel(&lookAndFeel_);
}

const juce::String ShowMidiApplication::getApplicationName()
{
    return ProjectInfo::projectName;
}

const juce::String ShowMidiApplication::getApplicationVersion()
{
    return ProjectInfo::versionString;
}

bool ShowMidiApplication::moreThanOneInstanceAllowed()
{
    return true;
}

UwynLookAndFeel& ShowMidiApplication::getLookAndFeel()
{
    return lookAndFeel_;
}

void ShowMidiApplication::initialise(const juce::String& commandLine)
{
    mainWindow.reset(new MainWindow(getApplicationName()));
}

void ShowMidiApplication::shutdown()
{
    mainWindow = nullptr;
}

void ShowMidiApplication::systemRequestedQuit()
{
    quit();
}

void ShowMidiApplication::anotherInstanceStarted(const juce::String& commandLine)
{
}
