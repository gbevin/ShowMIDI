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
#include "SettingsComponent.h"

#include "MidiDeviceComponent.h"
#include "ShowMidiApplication.h"
#include "SidebarComponent.h"

namespace showmidi
{
    SettingsComponent::SettingsComponent(Theme& theme) : theme_(theme)
    {
        setSize(MidiDeviceComponent::getStandardWidth() - SidebarComponent::X_SETTINGS * 2, theme_.linePosition(12));
        
        closeButton_.addListener(this);
        
        closeButton_.setBounds(0,  getHeight() - theme_.linePosition(1) - theme_.labelHeight(), getWidth(), theme_.labelHeight());
        addAndMakeVisible(closeButton_);
    }
    
    void SettingsComponent::paint(Graphics& g)
    {
        g.fillAll(theme_.colorBackground);
        
        g.setColour(theme_.colorData);
        g.drawRect(Rectangle<int>{0, 0, getWidth(), getHeight()});
        
        // middle c
        
        g.setColour(theme_.colorData);
        g.setFont(theme_.fontLabel());
        g.drawText("Middle C",
                   23, theme_.linePosition(1),
                   getWidth(), theme_.labelHeight(),
                   Justification::centredLeft, true);
        
        g.setColour(theme_.colorData.withAlpha(0.5f));
        g.setFont(theme_.fontData());
        g.drawText("oct 2",
                   23, theme_.linePosition(2),
                   getWidth(), theme_.labelHeight(),
                   Justification::centredLeft, true);
        g.setFont(theme_.fontData().withStyle(Font::underlined));
        g.drawText("oct 3",
                   93, theme_.linePosition(2),
                   getWidth(), theme_.labelHeight(),
                   Justification::centredLeft, true);
        g.setFont(theme_.fontData());
        g.drawText("oct 4",
                   163, theme_.linePosition(2),
                   getWidth(), theme_.labelHeight(),
                   Justification::centredLeft, true);
        
        // timeout delay
        
        g.setColour(theme_.colorData);
        g.setFont(theme_.fontLabel());
        g.drawText("Timeout Delay",
                   23, theme_.linePosition(4),
                   getWidth(), theme_.labelHeight(),
                   Justification::centredLeft, true);
        
        g.setColour(theme_.colorData.withAlpha(0.5f));
        g.setFont(theme_.fontData().withStyle(Font::underlined));
        g.drawText("2 sec",
                   23, theme_.linePosition(5),
                   getWidth(), theme_.labelHeight(),
                   Justification::centredLeft, true);
        g.setFont(theme_.fontData());
        g.drawText("3 sec",
                   93, theme_.linePosition(5),
                   getWidth(), theme_.labelHeight(),
                   Justification::centredLeft, true);
        g.setFont(theme_.fontData());
        g.drawText("5 sec",
                   163, theme_.linePosition(5),
                   getWidth(), theme_.labelHeight(),
                   Justification::centredLeft, true);
        
        // timeout delay
        
        g.setColour(theme_.colorData);
        g.setFont(theme_.fontLabel());
        g.drawText("Active Theme",
                   23, theme_.linePosition(7),
                   getWidth(), theme_.labelHeight(),
                   Justification::centredLeft, true);
        
        g.setColour(theme_.colorData.withAlpha(0.5f));
        g.setFont(theme_.fontData());
        g.drawText("load",
                   23, theme_.linePosition(8),
                   getWidth(), theme_.labelHeight(),
                   Justification::centredLeft, true);

        // close button
        
        g.setColour(theme_.colorController);
        g.setFont(theme_.fontLabel());
        g.drawText("close",
                   closeButton_.getX(), closeButton_.getY(),
                   closeButton_.getWidth(), closeButton_.getHeight(),
                   Justification::centred, true);
    }
    
    void SettingsComponent::buttonClicked(Button* buttonThatWasClicked)
    {
        if (buttonThatWasClicked == &closeButton_)
        {
            setVisible(false);
        }
    }
}
