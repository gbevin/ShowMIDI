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
        websiteButton_ = std::make_unique<PaintedButton>("https://uwyn.com");
        closeButton_ = std::make_unique<PaintedButton>("close");

        websiteButton_->addListener(this);
        closeButton_->addListener(this);
        
        addAndMakeVisible(websiteButton_.get());
        addAndMakeVisible(closeButton_.get());

        updateDimensions();
        resized();
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
        websiteButton_->drawName(g, Justification::centred);
        
        // close button
        
        g.setColour(theme_.colorController);
        g.setFont(theme_.fontLabel());
        closeButton_->drawName(g, Justification::centred);
    }
    
    void AboutComponent::resized()
    {
        websiteButton_->setBoundsForTouch(0, theme_.linePosition(4),
                                         getWidth(), theme_.labelHeight());
        closeButton_->setBoundsForTouch(0, getHeight() - theme_.linePosition(1) - theme_.labelHeight(),
                                       getWidth(), theme_.labelHeight());
    }
    
    void AboutComponent::buttonClicked(Button* buttonThatWasClicked)
    {
        if (buttonThatWasClicked == websiteButton_.get())
        {
            URL(websiteButton_->getName()).launchInDefaultBrowser();
        }
        else if (buttonThatWasClicked == closeButton_.get())
        {
            setVisible(false);
        }
    }
    
    void AboutComponent::updateDimensions()
    {
        setSize(MidiDeviceComponent::getStandardWidth() - SidebarComponent::X_SETTINGS * 2, theme_.linePosition(8));
    }
}
