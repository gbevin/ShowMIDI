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
#include "PaintedButton.h"
#include "PopupColourSelector.h"
#include "ShowMidiApplication.h"
#include "SidebarComponent.h"

namespace showmidi
{
    struct SettingsComponent::Pimpl : public Button::Listener, public Value::Listener
    {
        Pimpl(SettingsComponent* owner, SettingsManager* manager) : owner_(owner), manager_(manager)
        {
            auto& theme = manager_->getSettings().getTheme();
            
            middleCOct2Button_ = std::make_unique<PaintedButton>("oct 2");
            middleCOct3Button_ = std::make_unique<PaintedButton>("oct 3");
            middleCOct4Button_ = std::make_unique<PaintedButton>("oct 4");
            notesNameButton_ = std::make_unique<PaintedButton>("name");
            notesNumberButton_ = std::make_unique<PaintedButton>("number");
            numbersDecButton_ = std::make_unique<PaintedButton>("decimal");
            numbersHexButton_ = std::make_unique<PaintedButton>("hexadecimal");
            timeout2SecButton_ = std::make_unique<PaintedButton>("2 sec");
            timeout5SecButton_ = std::make_unique<PaintedButton>("5 sec");
            timeout10SecButton_ = std::make_unique<PaintedButton>("10 sec");
            windowRegularButton_ = std::make_unique<PaintedButton>("regular");
            windowAlwaysOnTopButton_ = std::make_unique<PaintedButton>("always on top");
            loadThemeButton_ = std::make_unique<PaintedButton>("load");
            saveThemeButton_ = std::make_unique<PaintedButton>("save");
            randomThemeButton_ = std::make_unique<PaintedButton>("random");
            colorBackgroundButton_ = std::make_unique<PaintedButton>();
            colorSidebarButton_ = std::make_unique<PaintedButton>();
            colorSeperatorButton_ = std::make_unique<PaintedButton>();
            colorTrackButton_ = std::make_unique<PaintedButton>();
            colorLabelButton_ = std::make_unique<PaintedButton>();
            colorDataButton_ = std::make_unique<PaintedButton>();
            colorPositiveButton_ = std::make_unique<PaintedButton>();
            colorNegativeButton_ = std::make_unique<PaintedButton>();
            colorControllerButton_ = std::make_unique<PaintedButton>();
            closeButton_ = std::make_unique<PaintedButton>("close");

            
            int height;
            if (manager_->isPlugin())
            {
#if JUCE_IOS
                height = theme.linePosition(14.5);
#else
                height = theme.linePosition(21.5);
#endif
            }
            else
            {
                height = theme.linePosition(24.5);
            }
            owner_->setSize(MidiDeviceComponent::getStandardWidth() - SidebarComponent::X_SETTINGS * 2, height);
            
            middleCOct2Button_->addListener(this);
            middleCOct3Button_->addListener(this);
            middleCOct4Button_->addListener(this);
            notesNameButton_->addListener(this);
            notesNumberButton_->addListener(this);
            numbersDecButton_->addListener(this);
            numbersHexButton_->addListener(this);
            timeout2SecButton_->addListener(this);
            timeout5SecButton_->addListener(this);
            timeout10SecButton_->addListener(this);
            windowRegularButton_->addListener(this);
            windowAlwaysOnTopButton_->addListener(this);
            loadThemeButton_->addListener(this);
            saveThemeButton_->addListener(this);
            randomThemeButton_->addListener(this);
            colorBackgroundButton_->addListener(this);
            colorSidebarButton_->addListener(this);
            colorSeperatorButton_->addListener(this);
            colorTrackButton_->addListener(this);
            colorLabelButton_->addListener(this);
            colorDataButton_->addListener(this);
            colorPositiveButton_->addListener(this);
            colorNegativeButton_->addListener(this);
            colorControllerButton_->addListener(this);
            closeButton_->addListener(this);
            
            colorValueBackground_.addListener(this);
            colorValueSidebar_.addListener(this);
            colorValueSeperator_.addListener(this);
            colorValueTrack_.addListener(this);
            colorValueLabel_.addListener(this);
            colorValueData_.addListener(this);
            colorValuePositive_.addListener(this);
            colorValueNegative_.addListener(this);
            colorValueController_.addListener(this);

            auto left_margin = 23;
            auto button_spacing = 70;
            auto y_offset = theme.linePosition(2);
            
            // middle c
            
            middleCOct2Button_->setBoundsForTouch(left_margin, y_offset,
                                                 getWidth(), theme.labelHeight());
            middleCOct3Button_->setBoundsForTouch(left_margin + button_spacing, y_offset,
                                                 getWidth(), theme.labelHeight());
            middleCOct4Button_->setBoundsForTouch(left_margin + button_spacing * 2, y_offset,
                                                 getWidth(), theme.labelHeight());
            
            // note format
            
            y_offset += theme.linePosition(3);
            
            notesNameButton_->setBoundsForTouch(left_margin, y_offset,
                                               getWidth(), theme.labelHeight());
            notesNumberButton_->setBoundsForTouch(left_margin + button_spacing, y_offset,
                                                 getWidth(), theme.labelHeight());
            
            // number format
            
            y_offset += theme.linePosition(3);
            
            numbersDecButton_->setBoundsForTouch(left_margin, y_offset,
                                                getWidth(), theme.labelHeight());
            numbersHexButton_->setBoundsForTouch(left_margin + button_spacing, y_offset,
                                                getWidth(), theme.labelHeight());
            
            // timeout delay
            
            y_offset += theme.linePosition(3);
            
            timeout2SecButton_->setBoundsForTouch(left_margin, y_offset,
                                                 getWidth(), theme.labelHeight());
            timeout5SecButton_->setBoundsForTouch(left_margin + button_spacing, y_offset,
                                                 getWidth(), theme.labelHeight());
            timeout10SecButton_->setBoundsForTouch(left_margin + button_spacing * 2, y_offset,
                                                  getWidth(), theme.labelHeight());
            
            if (!manager_->isPlugin())
            {
                // window position
                
                y_offset += theme.linePosition(3);
                
                windowRegularButton_->setBoundsForTouch(left_margin, y_offset,
                                                 getWidth(), theme.labelHeight());
                windowAlwaysOnTopButton_->setBoundsForTouch(left_margin + button_spacing, y_offset,
                                                     getWidth(), theme.labelHeight());
            }

            // active theme
            
            y_offset += theme.linePosition(3);
            
            loadThemeButton_->setBoundsForTouch(left_margin, y_offset,
                                               getWidth(), theme.labelHeight());
            saveThemeButton_->setBoundsForTouch(left_margin + button_spacing, y_offset,
                                               getWidth(), theme.labelHeight());
            randomThemeButton_->setBoundsForTouch(left_margin + button_spacing * 2, y_offset,
                                                 getWidth(), theme.labelHeight());

            // theme colours
            
            y_offset += theme.linePosition(2);

            int color_x = 23;
            int color_x_offset = (int)(Theme::DIALOG_LINE_HEIGHT * 1.5);
            int color_y = y_offset;
            int color_y_offset = (int)(Theme::DIALOG_LINE_HEIGHT * 1.5);
            int color_w = Theme::DIALOG_LINE_HEIGHT;
            int color_h = Theme::DIALOG_LINE_HEIGHT;
            
            colorBackgroundButton_->setBoundsForTouch(color_x, color_y, color_w, color_h);
            color_x += color_x_offset;
            colorSidebarButton_->setBoundsForTouch(color_x, color_y, color_w, color_h);
            color_x += color_x_offset;
            colorSeperatorButton_->setBoundsForTouch(color_x, color_y, color_w, color_h);
            color_x += color_x_offset;
            colorTrackButton_->setBoundsForTouch(color_x, color_y, color_w, color_h);
            color_x += color_x_offset;
            colorLabelButton_->setBoundsForTouch(color_x, color_y, color_w, color_h);
            color_x = 23;
            color_y += color_y_offset;
            colorDataButton_->setBoundsForTouch(color_x, color_y, color_w, color_h);
            color_x += color_x_offset;
            colorPositiveButton_->setBoundsForTouch(color_x, color_y, color_w, color_h);
            color_x += color_x_offset;
            colorNegativeButton_->setBoundsForTouch(color_x, color_y, color_w, color_h);
            color_x += color_x_offset;
            colorControllerButton_->setBoundsForTouch(color_x, color_y, color_w, color_h);
            
            // close button
            
            closeButton_->setBoundsForTouch(90, getHeight() - theme.linePosition(1) - theme.labelHeight(),
                                           getWidth() - 180, theme.labelHeight());
            
            owner_->addAndMakeVisible(middleCOct2Button_.get());
            owner_->addAndMakeVisible(middleCOct3Button_.get());
            owner_->addAndMakeVisible(middleCOct4Button_.get());
            owner_->addAndMakeVisible(notesNameButton_.get());
            owner_->addAndMakeVisible(notesNumberButton_.get());
            owner_->addAndMakeVisible(numbersDecButton_.get());
            owner_->addAndMakeVisible(numbersHexButton_.get());
            owner_->addAndMakeVisible(timeout2SecButton_.get());
            owner_->addAndMakeVisible(timeout5SecButton_.get());
            owner_->addAndMakeVisible(timeout10SecButton_.get());
            if (!manager_->isPlugin())
            {
                owner_->addAndMakeVisible(windowRegularButton_.get());
                owner_->addAndMakeVisible(windowAlwaysOnTopButton_.get());
            }
            if (SystemStats::getOperatingSystemType() != SystemStats::iOS || !manager_->isPlugin())
            {
                owner_->addAndMakeVisible(loadThemeButton_.get());
                owner_->addAndMakeVisible(saveThemeButton_.get());
                owner_->addAndMakeVisible(randomThemeButton_.get());
                owner_->addAndMakeVisible(colorBackgroundButton_.get());
                owner_->addAndMakeVisible(colorSidebarButton_.get());
                owner_->addAndMakeVisible(colorSeperatorButton_.get());
                owner_->addAndMakeVisible(colorTrackButton_.get());
                owner_->addAndMakeVisible(colorLabelButton_.get());
                owner_->addAndMakeVisible(colorDataButton_.get());
                owner_->addAndMakeVisible(colorPositiveButton_.get());
                owner_->addAndMakeVisible(colorNegativeButton_.get());
                owner_->addAndMakeVisible(colorControllerButton_.get());
            }
            owner_->addAndMakeVisible(closeButton_.get());
            
            loadThemeChooser_ = std::make_unique<FileChooser>("Please choose which theme to load...", File::getSpecialLocation(File::userHomeDirectory), "*.svg");
            saveThemeChooser_ = std::make_unique<FileChooser>("Please choose where to save the theme...", File::getSpecialLocation(File::userHomeDirectory), "*.svg");
        }

        int getWidth()
        {
            return owner_->getWidth();
        }
        
        int getHeight()
        {
            return owner_->getHeight();
        }
        
        void repaint()
        {
            owner_->repaint();
        }
        
        void setSettingOptionFont(Graphics& g, std::function<bool()> condition)
        {
            g.setFont(manager_->getSettings().getTheme().fontData().withStyle(condition() ? Font::underlined : Font::plain));
        }
        
        void paint(Graphics& g)
        {
            auto& settings = manager_->getSettings();
            auto& theme = settings.getTheme();
            
            g.fillAll(theme.colorBackground);
            
            g.setColour(theme.colorData);
            g.drawRect(Rectangle<int>{0, 0, getWidth(), getHeight()});
            
            auto y_offset = theme.linePosition(1);

            // middle c
            
            g.setColour(theme.colorData);
            g.setFont(theme.fontLabel());
            g.drawText("Middle C",
                       23, y_offset,
                       getWidth(), theme.labelHeight(),
                       Justification::centredLeft, true);
            
            g.setColour(theme.colorData.withAlpha(0.7f));
            setSettingOptionFont(g, [&settings] () { return settings.getOctaveMiddleC() == 2; });
            middleCOct2Button_->drawName(g, Justification::centredLeft);
            setSettingOptionFont(g, [&settings] () { return settings.getOctaveMiddleC() == 3; });
            middleCOct3Button_->drawName(g, Justification::centredLeft);
            setSettingOptionFont(g, [&settings] () { return settings.getOctaveMiddleC() == 4; });
            middleCOct4Button_->drawName(g, Justification::centredLeft);
            
            // note format
            
            y_offset += theme.linePosition(3);
            
            g.setColour(theme.colorData);
            g.setFont(theme.fontLabel());
            g.drawText("Note Format",
                       23, y_offset,
                       getWidth(), theme.labelHeight(),
                       Justification::centredLeft, true);
            
            g.setColour(theme.colorData.withAlpha(0.7f));
            setSettingOptionFont(g, [&settings] () { return settings.getNoteFormat() == NoteFormat::formatName; });
            notesNameButton_->drawName(g, Justification::centredLeft);
            setSettingOptionFont(g, [&settings] () { return settings.getNoteFormat() == NoteFormat::formatNumber; });
            notesNumberButton_->drawName(g, Justification::centredLeft);
            
            // number format
            
            y_offset += theme.linePosition(3);
            
            g.setColour(theme.colorData);
            g.setFont(theme.fontLabel());
            g.drawText("Number Format",
                       23, y_offset,
                       getWidth(), theme.labelHeight(),
                       Justification::centredLeft, true);
            
            g.setColour(theme.colorData.withAlpha(0.7f));
            setSettingOptionFont(g, [&settings] () { return settings.getNumberFormat() == NumberFormat::formatDecimal; });
            numbersDecButton_->drawName(g, Justification::centredLeft);
            setSettingOptionFont(g, [&settings] () { return settings.getNumberFormat() == NumberFormat::formatHexadecimal; });
            numbersHexButton_->drawName(g, Justification::centredLeft);
            
            // timeout delay
            
            y_offset += theme.linePosition(3);
            
            g.setColour(theme.colorData);
            g.setFont(theme.fontLabel());
            g.drawText("Timeout Delay",
                       23, y_offset,
                       getWidth(), theme.labelHeight(),
                       Justification::centredLeft, true);
            
            g.setColour(theme.colorData.withAlpha(0.7f));
            setSettingOptionFont(g, [&settings] () { return settings.getTimeoutDelay() == 2; });
            timeout2SecButton_->drawName(g, Justification::centredLeft);
            setSettingOptionFont(g, [&settings] () { return settings.getTimeoutDelay() == 5; });
            timeout5SecButton_->drawName(g, Justification::centredLeft);
            setSettingOptionFont(g, [&settings] () { return settings.getTimeoutDelay() == 10; });
            timeout10SecButton_->drawName(g, Justification::centredLeft);
            
            if (!manager_->isPlugin())
            {
                // window position
                
                y_offset += theme.linePosition(3);
                
                g.setColour(theme.colorData);
                g.setFont(theme.fontLabel());
                g.drawText("Window Position",
                           23, y_offset,
                           getWidth(), theme.labelHeight(),
                           Justification::centredLeft, true);
                
                g.setColour(theme.colorData.withAlpha(0.7f));
                setSettingOptionFont(g, [&settings] () { return settings.getWindowPosition() == WindowPosition::windowRegular; });
                windowRegularButton_->drawName(g, Justification::centredLeft);
                setSettingOptionFont(g, [&settings] () { return settings.getWindowPosition() == WindowPosition::windowAlwaysOnTop; });
                windowAlwaysOnTopButton_->drawName(g, Justification::centredLeft);
            }

            if (SystemStats::getOperatingSystemType() != SystemStats::iOS || !manager_->isPlugin())
            {
                // active theme
                
                y_offset += theme.linePosition(3);
                
                g.setColour(theme.colorData);
                g.setFont(theme.fontLabel());
                g.drawText("Active Theme",
                           23, y_offset,
                           getWidth(), theme.labelHeight(),
                           Justification::centredLeft, true);
                
                g.setColour(theme.colorData.withAlpha(0.7f));
                g.setFont(theme.fontData());
                loadThemeButton_->drawName(g, Justification::centredLeft);
                
                g.setColour(theme.colorData.withAlpha(0.7f));
                g.setFont(theme.fontData());
                saveThemeButton_->drawName(g, Justification::centredLeft);
                
                g.setColour(theme.colorData.withAlpha(0.7f));
                g.setFont(theme.fontData());
                randomThemeButton_->drawName(g, Justification::centredLeft);
                
                // theme colours
                
                g.setColour(theme.colorBackground);
                g.fillEllipse(colorBackgroundButton_->getBoundsForDrawing());
                g.setColour(theme.colorBackground.contrasting());
                g.drawEllipse(colorBackgroundButton_->getBoundsForDrawing(), 1);
                
                g.setColour(theme.colorSidebar);
                g.fillEllipse(colorSidebarButton_->getBoundsForDrawing());
                g.setColour(theme.colorSidebar.contrasting());
                g.drawEllipse(colorSidebarButton_->getBoundsForDrawing(), 1);
                
                g.setColour(theme.colorSeperator);
                g.fillEllipse(colorSeperatorButton_->getBoundsForDrawing());
                g.setColour(theme.colorSeperator.contrasting());
                g.drawEllipse(colorSeperatorButton_->getBoundsForDrawing(), 1);
                
                g.setColour(theme.colorTrack);
                g.fillEllipse(colorTrackButton_->getBoundsForDrawing());
                g.setColour(theme.colorTrack.contrasting());
                g.drawEllipse(colorTrackButton_->getBoundsForDrawing(), 1);
                
                g.setColour(theme.colorLabel);
                g.fillEllipse(colorLabelButton_->getBoundsForDrawing());
                g.setColour(theme.colorLabel.contrasting());
                g.drawEllipse(colorLabelButton_->getBoundsForDrawing(), 1);
                
                g.setColour(theme.colorData);
                g.fillEllipse(colorDataButton_->getBoundsForDrawing());
                g.setColour(theme.colorData.contrasting());
                g.drawEllipse(colorDataButton_->getBoundsForDrawing(), 1);
                
                g.setColour(theme.colorPositive);
                g.fillEllipse(colorPositiveButton_->getBoundsForDrawing());
                g.setColour(theme.colorPositive.contrasting());
                g.drawEllipse(colorPositiveButton_->getBoundsForDrawing(), 1);
                
                g.setColour(theme.colorNegative);
                g.fillEllipse(colorNegativeButton_->getBoundsForDrawing());
                g.setColour(theme.colorNegative.contrasting());
                g.drawEllipse(colorNegativeButton_->getBoundsForDrawing(), 1);
                
                g.setColour(theme.colorController);
                g.fillEllipse(colorControllerButton_->getBoundsForDrawing());
                g.setColour(theme.colorController.contrasting());
                g.drawEllipse(colorControllerButton_->getBoundsForDrawing(), 1);
            }
            
            // close button
            
            g.setColour(theme.colorController);
            g.setFont(theme.fontLabel());
            closeButton_->drawName(g, Justification::centred);
        }
        
        void buttonClicked(Button* buttonThatWasClicked)
        {
            auto& settings = manager_->getSettings();
            auto& theme = settings.getTheme();
            
            if (buttonThatWasClicked == middleCOct2Button_.get())
            {
                settings.setOctaveMiddleC(2);
                repaint();
            }
            else if (buttonThatWasClicked == middleCOct3Button_.get())
            {
                settings.setOctaveMiddleC(3);
                repaint();
            }
            else if (buttonThatWasClicked == middleCOct4Button_.get())
            {
                settings.setOctaveMiddleC(4);
                repaint();
            }
            else if (buttonThatWasClicked == notesNameButton_.get())
            {
                settings.setNoteFormat(NoteFormat::formatName);
                repaint();
            }
            else if (buttonThatWasClicked == notesNumberButton_.get())
            {
                settings.setNoteFormat(NoteFormat::formatNumber);
                repaint();
            }
            else if (buttonThatWasClicked == numbersDecButton_.get())
            {
                settings.setNumberFormat(NumberFormat::formatDecimal);
                repaint();
            }
            else if (buttonThatWasClicked == numbersHexButton_.get())
            {
                settings.setNumberFormat(NumberFormat::formatHexadecimal);
                repaint();
            }
            else if (buttonThatWasClicked == timeout2SecButton_.get())
            {
                settings.setTimeoutDelay(2);
                repaint();
            }
            else if (buttonThatWasClicked == timeout5SecButton_.get())
            {
                settings.setTimeoutDelay(5);
                repaint();
            }
            else if (buttonThatWasClicked == timeout10SecButton_.get())
            {
                settings.setTimeoutDelay(10);
                repaint();
            }
            else if (buttonThatWasClicked == windowRegularButton_.get())
            {
                settings.setWindowPosition(WindowPosition::windowRegular);
                repaint();
            }
            else if (buttonThatWasClicked == windowAlwaysOnTopButton_.get())
            {
                settings.setWindowPosition(WindowPosition::windowAlwaysOnTop);
                repaint();
            }
            else if (buttonThatWasClicked == loadThemeButton_.get())
            {
                loadThemeChooser_->launchAsync(FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles, [this] (const FileChooser& chooser)
                {
                    File file(chooser.getResult());
                    if (file.getFullPathName().isEmpty())
                    {
                        return;
                    }

                    manager_->getSettings().getTheme().parseXml(file.loadFileAsString());
                    manager_->storeSettings();
                });
            }
            else if (buttonThatWasClicked == saveThemeButton_.get())
            {
                saveThemeChooser_->launchAsync(FileBrowserComponent::saveMode | FileBrowserComponent::canSelectFiles, [this] (const FileChooser& chooser)
                {
                    File file(chooser.getResult());
                    if (file.getFullPathName().isEmpty())
                    {
                        return;
                    }
                    
                    TemporaryFile temp_file(file, File::createTempFile("svg"));
                    temp_file.getFile().appendText(manager_->getSettings().getTheme().generateXml(), false, false, "\r\n");
                    temp_file.overwriteTargetFileWithTemporary();
                });
            }
            else if (buttonThatWasClicked == randomThemeButton_.get())
            {
                manager_->getSettings().getTheme().randomize();
                manager_->storeSettings();
            }
            else if (buttonThatWasClicked == colorBackgroundButton_.get())
            {
                popupColorSelector(colorValueBackground_, buttonThatWasClicked, theme.colorBackground);
            }
            else if (buttonThatWasClicked == colorSidebarButton_.get())
            {
                popupColorSelector(colorValueSidebar_, buttonThatWasClicked, theme.colorSidebar);
            }
            else if (buttonThatWasClicked == colorSeperatorButton_.get())
            {
                popupColorSelector(colorValueSeperator_, buttonThatWasClicked, theme.colorSeperator);
            }
            else if (buttonThatWasClicked == colorTrackButton_.get())
            {
                popupColorSelector(colorValueTrack_, buttonThatWasClicked, theme.colorTrack);
            }
            else if (buttonThatWasClicked == colorLabelButton_.get())
            {
                popupColorSelector(colorValueLabel_, buttonThatWasClicked, theme.colorLabel);
            }
            else if (buttonThatWasClicked == colorDataButton_.get())
            {
                popupColorSelector(colorValueData_, buttonThatWasClicked, theme.colorData);
            }
            else if (buttonThatWasClicked == colorPositiveButton_.get())
            {
                popupColorSelector(colorValuePositive_, buttonThatWasClicked, theme.colorPositive);
            }
            else if (buttonThatWasClicked == colorNegativeButton_.get())
            {
                popupColorSelector(colorValueNegative_, buttonThatWasClicked, theme.colorNegative);
            }
            else if (buttonThatWasClicked == colorControllerButton_.get())
            {
                popupColorSelector(colorValueController_, buttonThatWasClicked, theme.colorController);
            }
            else if (buttonThatWasClicked == colorSidebarButton_.get())
            {
            }
            else if (buttonThatWasClicked == closeButton_.get())
            {
                owner_->setVisible(false);
            }
            
            manager_->applySettings();
        }
        
        void popupColorSelector(Value& colorValue, Button* colorButton, Colour& color)
        {
            colorValue.setValue(color.toDisplayString(true));
            CallOutBox::launchAsynchronously(std::make_unique<PopupColourSelector>(colorValue),
                                             colorButton->getScreenBounds(), nullptr);

        }
        
        void valueChanged(Value& value)
        {
            auto& theme = manager_->getSettings().getTheme();
            
            if (value.refersToSameSourceAs(colorValueBackground_))
            {
                theme.colorBackground = Colour::fromString(value.toString());
            }
            else if (value.refersToSameSourceAs(colorValueSidebar_))
            {
                theme.colorSidebar = Colour::fromString(value.toString());
            }
            else if (value.refersToSameSourceAs(colorValueSeperator_))
            {
                theme.colorSeperator = Colour::fromString(value.toString());
            }
            else if (value.refersToSameSourceAs(colorValueTrack_))
            {
                theme.colorTrack = Colour::fromString(value.toString());
            }
            else if (value.refersToSameSourceAs(colorValueLabel_))
            {
                theme.colorLabel = Colour::fromString(value.toString());
            }
            else if (value.refersToSameSourceAs(colorValueData_))
            {
                theme.colorData = Colour::fromString(value.toString());
            }
            else if (value.refersToSameSourceAs(colorValuePositive_))
            {
                theme.colorPositive = Colour::fromString(value.toString());
            }
            else if (value.refersToSameSourceAs(colorValueNegative_))
            {
                theme.colorNegative = Colour::fromString(value.toString());
            }
            else if (value.refersToSameSourceAs(colorValueController_))
            {
                theme.colorController = Colour::fromString(value.toString());
            }
            
            manager_->applySettings();
        }
        
        SettingsComponent* const owner_;
        SettingsManager* const manager_;
        
        std::unique_ptr<PaintedButton> middleCOct2Button_;
        std::unique_ptr<PaintedButton> middleCOct3Button_;
        std::unique_ptr<PaintedButton> middleCOct4Button_;
        std::unique_ptr<PaintedButton> notesNameButton_;
        std::unique_ptr<PaintedButton> notesNumberButton_;
        std::unique_ptr<PaintedButton> numbersDecButton_;
        std::unique_ptr<PaintedButton> numbersHexButton_;
        std::unique_ptr<PaintedButton> timeout2SecButton_;
        std::unique_ptr<PaintedButton> timeout5SecButton_;
        std::unique_ptr<PaintedButton> timeout10SecButton_;
        std::unique_ptr<PaintedButton> windowRegularButton_;
        std::unique_ptr<PaintedButton> windowAlwaysOnTopButton_;
        std::unique_ptr<PaintedButton> loadThemeButton_;
        std::unique_ptr<PaintedButton> saveThemeButton_;
        std::unique_ptr<PaintedButton> randomThemeButton_;
        std::unique_ptr<PaintedButton> colorBackgroundButton_;
        std::unique_ptr<PaintedButton> colorSidebarButton_;
        std::unique_ptr<PaintedButton> colorSeperatorButton_;
        std::unique_ptr<PaintedButton> colorTrackButton_;
        std::unique_ptr<PaintedButton> colorLabelButton_;
        std::unique_ptr<PaintedButton> colorDataButton_;
        std::unique_ptr<PaintedButton> colorPositiveButton_;
        std::unique_ptr<PaintedButton> colorNegativeButton_;
        std::unique_ptr<PaintedButton> colorControllerButton_;
        std::unique_ptr<PaintedButton> closeButton_;
        
        Value colorValueBackground_;
        Value colorValueSidebar_;
        Value colorValueSeperator_;
        Value colorValueTrack_;
        Value colorValueLabel_;
        Value colorValueData_;
        Value colorValuePositive_;
        Value colorValueNegative_;
        Value colorValueController_;
        
        std::unique_ptr<FileChooser> loadThemeChooser_;
        std::unique_ptr<FileChooser> saveThemeChooser_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pimpl)
    };
    
    SettingsComponent::SettingsComponent(SettingsManager* m) : pimpl_(new Pimpl(this, m)) {}
    SettingsComponent::~SettingsComponent() = default;
    
    void SettingsComponent::paint(Graphics& g) { pimpl_->paint(g); }
}
