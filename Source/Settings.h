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

#include "Theme.h"

namespace showmidi
{
    enum NoteFormat
    {
        formatName = 1,
        formatNumber
    };
    
    enum NumberFormat
    {
        formatDecimal = 1,
        formatHexadecimal
    };
    
    enum WindowPosition
    {
        windowRegular = 1,
        windowAlwaysOnTop
    };
    
    class Settings
    {
    public:
        static constexpr int DEFAULT_OCTAVE_MIDDLE_C { 3 };
        static constexpr NoteFormat DEFAULT_NOTE_FORMAT { formatName };
        static constexpr NumberFormat DEFAULT_NUMBER_FORMAT { formatDecimal };
        static constexpr int DEFAULT_TIMEOUT_DELAY { 2 };
        static constexpr WindowPosition DEFAULT_WINDOW_POSITION { windowRegular };

        Settings() {};
        virtual ~Settings() {};
        
        virtual int getOctaveMiddleC() = 0;
        virtual void setOctaveMiddleC(int) = 0;
        
        virtual NoteFormat getNoteFormat() = 0;
        virtual void setNoteFormat(NoteFormat) = 0;
        
        virtual NumberFormat getNumberFormat() = 0;
        virtual void setNumberFormat(NumberFormat) = 0;
        
        virtual int getTimeoutDelay() = 0;
        virtual void setTimeoutDelay(int) = 0;
        
        virtual WindowPosition getWindowPosition() = 0;
        virtual void setWindowPosition(WindowPosition) = 0;

        virtual Theme& getTheme() = 0;
        virtual void storeTheme() = 0;
        
        virtual bool isMidiDeviceVisible(const String&) = 0;
        virtual void setMidiDeviceVisible(const String&, bool) = 0;
    };
}
