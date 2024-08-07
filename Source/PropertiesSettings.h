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
    class PropertiesSettings : public Settings
    {
    public:
        static const String VISUALIZATION;
        static const String OCTAVE_MIDDLE_C;
        static const String NOTE_FORMAT;
        static const String NUMBER_FORMAT;
        static const String TIMEOUT_DELAY;
        static const String WINDOW_POSITION;
        static const String CONTROL_GRAPH_HEIGHT;
        static const String MIDI_DEVICE_VISIBLE_PREFIX;
        static const String THEME;
        
        PropertiesSettings();
        ~PropertiesSettings();
        
        Visualization getVisualization();
        void setVisualization(Visualization);
        
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
        
        int getControlGraphHeight();
        void setControlGraphHeight(int);

        Theme& getTheme();
        void storeTheme();
        
        bool isMidiDeviceVisible(const String&);
        void setMidiDeviceVisible(const String&, bool);

        void flush();
        
    private:
        std::unique_ptr<PropertiesFile> propertyFile_;
        
        PropertiesFile& getGlobalProperties();        
        void updateGlobalProps();
        void reload();

        Theme theme_;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PropertiesSettings)
    };
}
