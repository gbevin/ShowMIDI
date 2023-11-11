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
    SettingsComponent::SettingsComponent(SettingsManager& manager) : manager_(manager),
        middleCOct2Button_("oct 2"),
        middleCOct3Button_("oct 3"),
        middleCOct4Button_("oct 4"),
        timeout2SecButton_("2 sec"),
        timeout5SecButton_("5 sec"),
        timeout10SecButton_("10 sec"),
        loadThemeButton_("load"),
        closeButton_{ "close" }
    {
        auto& theme = manager_.getSettings().getTheme();
        
        setSize(MidiDeviceComponent::getStandardWidth() - SidebarComponent::X_SETTINGS * 2, theme.linePosition(12));
        
        middleCOct2Button_.addListener(this);
        middleCOct3Button_.addListener(this);
        middleCOct4Button_.addListener(this);
        timeout2SecButton_.addListener(this);
        timeout5SecButton_.addListener(this);
        timeout10SecButton_.addListener(this);
        loadThemeButton_.addListener(this);
        closeButton_.addListener(this);

        auto left_margin = 23;
        auto button_spacing = 70;
        middleCOct2Button_.setBoundsForTouch(left_margin, theme.linePosition(2),
                                             getWidth(), theme.labelHeight());
        middleCOct3Button_.setBoundsForTouch(left_margin + button_spacing, theme.linePosition(2),
                                             getWidth(), theme.labelHeight());
        middleCOct4Button_.setBoundsForTouch(left_margin + button_spacing * 2, theme.linePosition(2),
                                             getWidth(), theme.labelHeight());
        
        timeout2SecButton_.setBoundsForTouch(left_margin, theme.linePosition(5),
                                             getWidth(), theme.labelHeight());
        timeout5SecButton_.setBoundsForTouch(left_margin + button_spacing, theme.linePosition(5),
                                             getWidth(), theme.labelHeight());
        timeout10SecButton_.setBoundsForTouch(left_margin + button_spacing * 2, theme.linePosition(5),
                                             getWidth(), theme.labelHeight());

        loadThemeButton_.setBoundsForTouch(left_margin, theme.linePosition(8),
                                           getWidth(), theme.labelHeight());
        
        closeButton_.setBoundsForTouch(0, getHeight() - theme.linePosition(1) - theme.labelHeight(),
                                       getWidth(), theme.labelHeight());
        
        addAndMakeVisible(middleCOct2Button_);
        addAndMakeVisible(middleCOct3Button_);
        addAndMakeVisible(middleCOct4Button_);
        addAndMakeVisible(timeout2SecButton_);
        addAndMakeVisible(timeout5SecButton_);
        addAndMakeVisible(timeout10SecButton_);
        addAndMakeVisible(loadThemeButton_);
        addAndMakeVisible(closeButton_);
        
        themeChooser_ = std::make_unique<FileChooser>("Please select the theme you want to load...", File::getSpecialLocation(File::userHomeDirectory), "*.svg");
    }
    
    void SettingsComponent::setSettingOptionFont(Graphics& g, std::function<bool()> condition)
    {
        g.setFont(manager_.getSettings().getTheme().fontData().withStyle(condition() ? Font::underlined : Font::plain));
    }
    
    void SettingsComponent::paint(Graphics& g)
    {
        auto& settings = manager_.getSettings();
        auto& theme = settings.getTheme();
        
        g.fillAll(theme.colorBackground);
        
        g.setColour(theme.colorData);
        g.drawRect(Rectangle<int>{0, 0, getWidth(), getHeight()});
        
        // middle c
        
        g.setColour(theme.colorData);
        g.setFont(theme.fontLabel());
        g.drawText("Middle C",
                   23, theme.linePosition(1),
                   getWidth(), theme.labelHeight(),
                   Justification::centredLeft, true);
        
        g.setColour(theme.colorData.withAlpha(0.7f));
        setSettingOptionFont(g, [&settings] () { return settings.getOctaveMiddleC() == 2; });
        middleCOct2Button_.drawName(g, Justification::centredLeft);
        setSettingOptionFont(g, [&settings] () { return settings.getOctaveMiddleC() == 3; });
        middleCOct3Button_.drawName(g, Justification::centredLeft);
        setSettingOptionFont(g, [&settings] () { return settings.getOctaveMiddleC() == 4; });
        middleCOct4Button_.drawName(g, Justification::centredLeft);
        
        // timeout delay
        
        g.setColour(theme.colorData);
        g.setFont(theme.fontLabel());
        g.drawText("Timeout Delay",
                   23, theme.linePosition(4),
                   getWidth(), theme.labelHeight(),
                   Justification::centredLeft, true);
        
        g.setColour(theme.colorData.withAlpha(0.7f));
        setSettingOptionFont(g, [&settings] () { return settings.getTimeoutDelay() == 2; });
        timeout2SecButton_.drawName(g, Justification::centredLeft);
        setSettingOptionFont(g, [&settings] () { return settings.getTimeoutDelay() == 5; });
        timeout5SecButton_.drawName(g, Justification::centredLeft);
        setSettingOptionFont(g, [&settings] () { return settings.getTimeoutDelay() == 10; });
        timeout10SecButton_.drawName(g, Justification::centredLeft);
        
        // timeout delay
        
        g.setColour(theme.colorData);
        g.setFont(theme.fontLabel());
        g.drawText("Active Theme",
                   23, theme.linePosition(7),
                   getWidth(), theme.labelHeight(),
                   Justification::centredLeft, true);
        
        g.setColour(theme.colorData.withAlpha(0.7f));
        g.setFont(theme.fontData());
        loadThemeButton_.drawName(g, Justification::centredLeft);

        // close button
        
        g.setColour(theme.colorController);
        g.setFont(theme.fontLabel());
        closeButton_.drawName(g, Justification::centred);
    }
    
    void SettingsComponent::buttonClicked(Button* buttonThatWasClicked)
    {
        auto& settings = manager_.getSettings();
        
        if (buttonThatWasClicked == &middleCOct2Button_)
        {
            settings.setOctaveMiddleC(2);
            repaint();
        }
        else if (buttonThatWasClicked == &middleCOct3Button_)
        {
            settings.setOctaveMiddleC(3);
            repaint();
        }
        else if (buttonThatWasClicked == &middleCOct4Button_)
        {
            settings.setOctaveMiddleC(4);
            repaint();
        }
        else if (buttonThatWasClicked == &timeout2SecButton_)
        {
            settings.setTimeoutDelay(2);
            repaint();
        }
        else if (buttonThatWasClicked == &timeout5SecButton_)
        {
            settings.setTimeoutDelay(5);
            repaint();
        }
        else if (buttonThatWasClicked == &timeout10SecButton_)
        {
            settings.setTimeoutDelay(10);
            repaint();
        }
        else if (buttonThatWasClicked == &loadThemeButton_)
        {
            themeChooser_->launchAsync(FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles, [this] (const FileChooser& chooser)
            {
                File file(chooser.getResult());
         
                manager_.getSettings().getTheme().parseXml(file.loadFileAsString());
                manager_.storeSettings();
            });
        }
        else if (buttonThatWasClicked == &closeButton_)
        {
            setVisible(false);
        }
    }
}
