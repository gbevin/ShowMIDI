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
    Desktop::getInstance().setDefaultLookAndFeel(&lookAndFeel_);
}

const String ShowMidiApplication::getApplicationName()
{
    return ProjectInfo::projectName;
}

const String ShowMidiApplication::getApplicationVersion()
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

void ShowMidiApplication::initialise(const String&)
{
    mainWindow_.reset(new MainWindow(getApplicationName()));
}

void ShowMidiApplication::shutdown()
{
    mainWindow_ = nullptr;
}

void ShowMidiApplication::systemRequestedQuit()
{
    quit();
}

void ShowMidiApplication::anotherInstanceStarted(const String&)
{
}

void ShowMidiApplication::setWindowTitle(const String& title)
{
    if (mainWindow_ != nullptr)
    {
        mainWindow_->setName(title);
    }
}

void ShowMidiApplication::setWindowSize(int width, int height)
{
    if (mainWindow_ != nullptr)
    {
        width += mainWindow_->getSizebarWidth();
#if JUCE_IOS
        mainWindow_->setBounds(0, 0, width, height);
#else
        mainWindow_->setSize(width, height);
#endif
    }
}

int ShowMidiApplication::getWindowHeight()
{
    if (mainWindow_ != nullptr)
    {
        return mainWindow_->getHeight();
    }
    
    return 0;
}

StoredSettings& ShowMidiApplication::getSettings()
{
    return settings_;
}

void ShowMidiApplication::storeSettings()
{
    settings_.storeTheme(theme_);
    settings_.flush();
    
    mainWindow_->repaint();
}

Theme& ShowMidiApplication::getTheme()
{
    return theme_;
}
