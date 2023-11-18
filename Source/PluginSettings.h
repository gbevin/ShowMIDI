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

#include "Settings.h"

namespace showmidi
{
    class PluginSettings : public Settings
    {
    public:
        PluginSettings();
        ~PluginSettings();
        
        int getOctaveMiddleC();
        void setOctaveMiddleC(int);
        
        NoteFormat getNoteFormat();
        void setNoteFormat(NoteFormat);

        NumberFormat getNumberFormat();
        void setNumberFormat(NumberFormat);
        
        int getTimeoutDelay();
        void setTimeoutDelay(int);
        
        WindowPosition getWindowPosition();
        void setWindowPosition(WindowPosition);

        Theme& getTheme();
        void storeTheme();
        
        bool isMidiDeviceVisible(const String&);
        void setMidiDeviceVisible(const String&, bool);

        ValueTree& getValueTree();
        void copyValueTree(ValueTree&);
        
    private:
        ValueTree settings_ { ProjectInfo::projectName };
        Theme theme_;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginSettings)
    };
}
