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
#include "AboutComponent.h"

#include "MidiDeviceComponent.h"
#include "SidebarComponent.h"

namespace showmidi
{
    AboutComponent::AboutComponent(Theme& theme) : theme_(theme)
    {
        setSize(MidiDeviceComponent::getStandardWidth() - SidebarComponent::X_SETTINGS * 2, theme_.linePosition(8));

        websiteButton_.addListener(this);
        closeButton_.addListener(this);
        
        websiteButton_.setBounds(0, theme_.linePosition(4), getWidth(), theme_.labelHeight());
        addAndMakeVisible(websiteButton_);
        
        closeButton_.setBounds(0,  getHeight() - theme_.linePosition(1) - theme_.labelHeight(), getWidth(), theme_.labelHeight());
        addAndMakeVisible(closeButton_);
    }
    
    void AboutComponent::paint(Graphics& g)
    {
        g.fillAll(theme_.colorBackground);
        
        g.setColour(theme_.colorData);
        g.drawRect(Rectangle<int>{0, 0, getWidth(), getHeight()});
        
        g.setFont(theme_.fontLabel());
        g.drawText(ProjectInfo::projectName,
                   0, theme_.linePosition(1),
                   getWidth(), theme_.labelHeight(),
                   Justification::centred, true);
        g.drawText(ProjectInfo::versionString,
                   0, theme_.linePosition(2),
                   getWidth(), theme_.labelHeight(),
                   Justification::centred, true);
        
        g.setColour(theme_.colorData.withAlpha(0.7f));
        g.setFont(theme_.fontData());
        g.drawText("https://uwyn.com",
                   websiteButton_.getX(), websiteButton_.getY(),
                   websiteButton_.getWidth(), websiteButton_.getHeight(),
                   Justification::centred, true);
        
        // close button
        
        g.setColour(theme_.colorController);
        g.setFont(theme_.fontLabel());
        g.drawText("close",
                   closeButton_.getX(), closeButton_.getY(),
                   closeButton_.getWidth(), closeButton_.getHeight(),
                   Justification::centred, true);
    }
    
    void AboutComponent::buttonClicked(Button* buttonThatWasClicked)
    {
        if (buttonThatWasClicked == &websiteButton_)
        {
            URL("https://uwyn.com").launchInDefaultBrowser();
        }
        else if (buttonThatWasClicked == &closeButton_)
        {
            setVisible(false);
        }
    }
}
