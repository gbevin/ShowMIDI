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

#include "ShowMidiApplication.h"

namespace showmidi
{
    AboutComponent::AboutComponent()
    {
        websiteButton_.addListener(this);
        closeButton_.addListener(this);
        
        setSize(240, 160);
        
        websiteButton_.setBounds(0, 80, getWidth(), SMApp.getTheme().labelHeight());
        addAndMakeVisible(websiteButton_);
        
        closeButton_.setBounds(0, 130, getWidth(), SMApp.getTheme().labelHeight());
        addAndMakeVisible(closeButton_);
    }
    
    void AboutComponent::paint(Graphics& g)
    {
        g.fillAll(SMApp.getTheme().colorBackground);
        
        g.setColour(SMApp.getTheme().colorData);
        g.drawRect(Rectangle<int>{0, 0, getWidth(), getHeight()});
        
        g.setFont(SMApp.getTheme().fontLabel());
        g.drawText(ProjectInfo::projectName,
                   0, 20,
                   getWidth(), SMApp.getTheme().labelHeight(),
                   Justification::centred, true);
        g.drawText(ProjectInfo::versionString,
                   0, 40,
                   getWidth(), SMApp.getTheme().labelHeight(),
                   Justification::centred, true);
        
        g.setFont(SMApp.getTheme().fontLabel());
        g.drawText("https://uwyn.com",
                   websiteButton_.getX(), websiteButton_.getY(),
                   websiteButton_.getWidth(), websiteButton_.getHeight(),
                   Justification::centred, true);
        
        g.setColour(SMApp.getTheme().colorController);
        g.setFont(SMApp.getTheme().fontLabel().withStyle(Font::underlined));
        g.drawText("close",
                   closeButton_.getX(), closeButton_.getY(),
                   closeButton_.getWidth(), closeButton_.getHeight(),
                   Justification::centred, true);
    }
    
    void AboutComponent::setVisible(bool shouldBeVisible)
    {
        if (shouldBeVisible)
        {
            centreWithSize(getWidth(), getHeight());
        }
        
        Component::setVisible(shouldBeVisible);
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
