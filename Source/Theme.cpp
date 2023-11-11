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
#include "Theme.h"

#include <JuceHeader.h>

namespace showmidi
{
    Font Theme::fontLabel()
    {
        return Font(16, Font::bold);
    }

    int Theme::labelHeight()
    {
        return (int)fontLabel().getHeight();
    }
    
    Font Theme::fontData()
    {
        return Font(16, Font::italic);
    }

    int Theme::dataHeight()
    {
        return (int)fontData().getHeight();
    }
    
    String Theme::generateXml()
    {
        String result = R"(<svg width="96px" height="64px" xmlns="http://www.w3.org/2000/svg" baseProfile="full" version="1.1">
          <rect width='96' height='64'  id='background' fill='#{background}'></rect>
          <circle cx='24' cy='24' r='8' id='sidebar'    fill='#{sidebar}'></circle>
          <circle cx='40' cy='24' r='8' id='seperator'  fill='#{seperator}'></circle>
          <circle cx='56' cy='24' r='8' id='track'      fill='#{track}'></circle>
          <circle cx='72' cy='24' r='8' id='label'      fill='#{label}'></circle>
          <circle cx='24' cy='40' r='8' id='data'       fill='#{data}'></circle>
          <circle cx='40' cy='40' r='8' id='positive'   fill='#{positive}'></circle>
          <circle cx='56' cy='40' r='8' id='negative'   fill='#{negative}'></circle>
          <circle cx='72' cy='40' r='8' id='controller' fill='#{controller}'></circle>
        </svg>)";
        result = result.replace("{background}",  colorBackground.toDisplayString(false));
        result = result.replace("{sidebar}",     colorSidebar.toDisplayString(false));
        result = result.replace("{seperator}",   colorSeperator.toDisplayString(false));
        result = result.replace("{track}",       colorTrack.toDisplayString(false));
        result = result.replace("{label}",       colorLabel.toDisplayString(false));
        result = result.replace("{data}",        colorData.toDisplayString(false));
        result = result.replace("{positive}",    colorPositive.toDisplayString(false));
        result = result.replace("{negative}",    colorNegative.toDisplayString(false));
        result = result.replace("{controller}",  colorController.toDisplayString(false));
        
        return result;
    }
    
    void Theme::parseXml(const String& content)
    {
        XmlDocument xml(content);
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
    
    String Theme::convertSvgColor(const String& colour)
    {
        if (colour.length() != 7 || !colour.startsWith("#"))
        {
            return String();
        }
        
        return String("ff") + colour.substring(1);
    }
}
