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
#pragma once

#include <JuceHeader.h>

#include "MidiDevicesListener.h"
#include "PropertiesSettings.h"
#include "SettingsManager.h"
#include "UwynLookAndFeel.h"

#define SMApp ShowMidiApplication::getInstance()

namespace showmidi
{
    class StandaloneWindow;
    
    class ShowMidiApplication : public JUCEApplication, public SettingsManager
    {
    public:
        ShowMidiApplication();
        
        inline static bool hasInstance()
        {
            return JUCEApplication::getInstance() != nullptr;
        }
        
        inline static ShowMidiApplication& getInstance()
        {
            ShowMidiApplication* const app = dynamic_cast<ShowMidiApplication*>(JUCEApplication::getInstance());
            jassert (app != nullptr);
            return *app;
        }
        
        const String getApplicationName() override;
        const String getApplicationVersion() override;
        bool moreThanOneInstanceAllowed() override;
        UwynLookAndFeel& getLookAndFeel();
        
        void initialise(const String& commandLine) override;
        void shutdown() override;
        void systemRequestedQuit() override;
        void anotherInstanceStarted(const String& commandLine) override;

        void setWindowTitle(const String&);
        void setWindowWidthForMainLayout(int);
        
        Settings& getSettings() override;
        void applySettings() override;
        void storeSettings() override;
        
        MidiDevicesListeners& getMidiDevicesListeners() override;
        
        struct Pimpl;
    private:
        std::unique_ptr<Pimpl> pimpl_;
    };
}
