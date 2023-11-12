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
#include "PopupColourSelector.h"

namespace showmidi
{
    PopupColourSelector::PopupColourSelector(const Value& colour) : colourValue_(colour)
    {
        addAndMakeVisible(&selector_);
        selector_.setName("Colour");
        selector_.setCurrentColour(getColour());
        selector_.addChangeListener(this);
        
        colourValue_.addListener(this);
        setSize(300, 400);
    }
    
    void PopupColourSelector::resized()
    {
        selector_.setBounds (getLocalBounds());
    }
    
    Colour PopupColourSelector::getColour() const
    {
        return Colour::fromString(colourValue_.toString());
    }
    
    void PopupColourSelector::setColour(const Colour &newColour)
    {
        if (getColour() != newColour)
        {
            colourValue_ = newColour.toDisplayString(true);
        }
    }
    
    void PopupColourSelector::changeListenerCallback(ChangeBroadcaster *)
    {
        if (selector_.getCurrentColour() != getColour())
        {
            setColour(selector_.getCurrentColour());
        }
    }
    
    void PopupColourSelector::valueChanged(Value &)
    {
        selector_.setCurrentColour(getColour());
    }
}
