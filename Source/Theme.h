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

namespace showmidi
{
    struct Theme
    {
        Font fontLabel()
        {
            return Font(16, Font::bold);
        }

        int labelHeight()
        {
            return (int)fontLabel().getHeight();
        }
        
        Font fontData()
        {
            return Font(16, Font::italic);
        }

        int dataHeight()
        {
            return (int)fontData().getHeight();
        }
        
        Colour colorSidebar;
        Colour colorBackground;
        Colour colorLabel;
        Colour colorData;
        Colour colorSeperator;
        Colour colorPositive;
        Colour colorNegative;
        Colour colorController;
        Colour colorTrack;
    };
    
    static const Theme THEME_DARK =
    {
        Colour(0xff201e21), // colorSidebar
        Colour(0xff29272b), // colorBackground
        Colour(0xff66606b), // colorLabel
        Colour(0xffffffff), // colorData
        Colour(0xff66606b), // colorSeperator
        Colour(0xff66adf3), // colorPositive
        Colour(0xffd8414e), // colorNegative
        Colour(0xffffab2b), // colorController
        Colour(0xff201e21)  // colorTrack
    };
    
    static const Theme THEME_LIGHT =
    {
        Colour(0xffffffff), // colorSidebar
        Colour(0xfff2f2f2), // colorBackground
        Colour(0xffa0a0a0), // colorLabel
        Colour(0xff222222), // colorData
        Colour(0xffa0a0a0), // colorSeperator
        Colour(0xff66adf3), // colorPositive
        Colour(0xffd8414e), // colorNegative
        Colour(0xffffab2b), // colorController
        Colour(0xffffffff)  // colorTrack
    };
}
