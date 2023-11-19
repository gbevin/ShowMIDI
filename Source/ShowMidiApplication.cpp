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

#include "StandaloneWindow.h"

namespace showmidi
{
    struct ShowMidiApplication::Pimpl : public Timer
    {
        Pimpl()
        {
            // poll for MIDI devices refresh
            startTimerHz(5);
        }
        
        ~Pimpl()
        {
            stopTimer();
        }
        
        void timerCallback()
        {
            midiDevicesListeners_.broadcast();
        }
        
        void setWindowTitle(const String& title)
        {
            if (mainWindow_ != nullptr)
            {
                mainWindow_->setName(title);
            }
        }
        
        void setWindowWidthForMainLayout(int width)
        {
            if (mainWindow_ != nullptr)
            {
#if JUCE_IOS
                mainWindow_->updatePosition();
#else
                width += mainWindow_->getSidebarWidth();
                mainWindow_->setSize(width, mainWindow_->getHeight());
#endif
            }
        }
        
        void storeSettings()
        {
            settings_.storeTheme();
            settings_.flush();
            
            mainWindow_->repaint();
        }

        UwynLookAndFeel lookAndFeel_;
        std::unique_ptr<StandaloneWindow> mainWindow_;
        PropertiesSettings settings_;
        MidiDevicesListeners midiDevicesListeners_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pimpl)
    };
    
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
        return pimpl_->lookAndFeel_;
    }
    
    void ShowMidiApplication::initialise(const String&)
    {
        pimpl_->mainWindow_.reset(new StandaloneWindow(getApplicationName()));
        
        applySettings();
    }
    
    void ShowMidiApplication::shutdown()
    {
        pimpl_->mainWindow_ = nullptr;
    }
    
    void ShowMidiApplication::systemRequestedQuit()
    {
        quit();
    }
    
    void ShowMidiApplication::anotherInstanceStarted(const String&)
    {
    }
    
    bool ShowMidiApplication::isPlugin()
    {
        return false;
    }
    
    Component* ShowMidiApplication::getTopLevelComponent()
    {
        return nullptr;
    }

    Settings& ShowMidiApplication::getSettings()
    {
        return pimpl_->settings_;
    }
    
    void ShowMidiApplication::applySettings()
    {
        pimpl_->mainWindow_->setAlwaysOnTop(pimpl_->settings_.getWindowPosition() == WindowPosition::windowAlwaysOnTop);
        pimpl_->mainWindow_->repaint();
    }
    
    MidiDevicesListeners& ShowMidiApplication::getMidiDevicesListeners()
    {
        return pimpl_->midiDevicesListeners_;
    }
    
    ShowMidiApplication::ShowMidiApplication() : pimpl_(new Pimpl())
    {
        Desktop::getInstance().setDefaultLookAndFeel(&pimpl_->lookAndFeel_);
    }

    void ShowMidiApplication::setWindowTitle(const String& title)    { pimpl_->setWindowTitle(title); }
    void ShowMidiApplication::setWindowWidthForMainLayout(int width) { pimpl_->setWindowWidthForMainLayout(width); }
    void ShowMidiApplication::storeSettings()                        { pimpl_->storeSettings(); }
}
