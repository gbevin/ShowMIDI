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
        
        void parseXmlFile(const String& path)
        {
            XmlDocument xml(File(path).loadFileAsString());
            std::unique_ptr<XmlElement> svg = xml.getDocumentElement();
            if (svg && svg->getTagName().toLowerCase() == "svg")
            {
                for (auto* element : svg->getChildIterator())
                {
                    auto id = element->getStringAttribute("id", "").toLowerCase();
                    auto fill = convertSvgColor(element->getStringAttribute("fill", "").toLowerCase());
                    if (id.isNotEmpty() && fill.isNotEmpty())
                    {
                        auto colour = Colour::fromString(fill);
                        
                        // support the native theme SVG format
                        if      (id == "background") colorBackground = colour;
                        else if (id == "sidebar")    colorSidebar = colour;
                        else if (id == "seperator")  colorSeperator = colour;
                        else if (id == "track")      colorTrack = colour;
                        else if (id == "label")      colorLabel = colour;
                        else if (id == "data")       colorData = colour;
                        else if (id == "positive")   colorPositive = colour;
                        else if (id == "negative")   colorNegative = colour;
                        else if (id == "controller") colorController = colour;
                        
                        // support the Hundred Rabbits theme SVG format
                        else if (id == "b_low")     colorSidebar = colour;
                        else if (id == "b_med")     colorSeperator = colour;
                        else if (id == "b_high")    colorTrack = colour;
                        else if (id == "f_med")     colorLabel = colour;
                        else if (id == "f_high")    colorData = colour;
                        else if (id == "f_inv")     colorPositive = colour;
                        else if (id == "b_inv")     colorNegative = colour;
                        else if (id == "f_low")     colorController = colour;
                    }
                }
            }
        }
        
        static String convertSvgColor(const String& colour)
        {
            if (colour.length() != 7 || !colour.startsWith("#"))
            {
                return String();
            }
            
            return String("ff") + colour.substring(1);
        }

        Colour colorBackground;
        Colour colorSidebar;
        Colour colorSeperator;
        Colour colorTrack;
        Colour colorLabel;
        Colour colorData;
        Colour colorPositive;
        Colour colorNegative;
        Colour colorController;
    };
    
    static const Theme THEME_DARK =
    {
        Colour(0xFF29272B), // colorBackground
        Colour(0xFF201E21), // colorSidebar
        Colour(0xFF66606B), // colorSeperator
        Colour(0xFF201E21), // colorTrack
        Colour(0xFF66606B), // colorLabel
        Colour(0xFFFFFFFF), // colorData
        Colour(0xFF66ADF3), // colorPositive
        Colour(0xFFD8414E), // colorNegative
        Colour(0xFFFFAB2B)  // colorController
    };
    
    static const Theme THEME_LIGHT =
    {
        Colour(0xFFF2F2F2), // colorBackground
        Colour(0xFFFFFFFF), // colorSidebar
        Colour(0xFFA0A0A0), // colorSeperator
        Colour(0xFFFFFFFF), // colorTrack
        Colour(0xFFA0A0A0), // colorLabel
        Colour(0xFF222222), // colorData
        Colour(0xFF66ADF3), // colorPositive
        Colour(0xFFD8414E), // colorNegative
        Colour(0xFFFFAB2B)  // colorController
    };
}
