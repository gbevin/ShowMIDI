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

#include "JuceHeader.h"

#include "PaintedButton.h"
#include "SettingsManager.h"

namespace showmidi
{
    class SettingsComponent : public Component, public Button::Listener
    {
    public:
        SettingsComponent(SettingsManager&);
        
        void paint(Graphics&) override;
        
        void buttonClicked(Button*) override;
        
    private:
        void setSettingOptionFont(Graphics&, std::function<bool()>);
        
        SettingsManager& manager_;
        
        PaintedButton middleCOct2Button_;
        PaintedButton middleCOct3Button_;
        PaintedButton middleCOct4Button_;
        PaintedButton notesNameButton_;
        PaintedButton notesNumberButton_;
        PaintedButton numbersDecButton_;
        PaintedButton numbersHexButton_;
        PaintedButton timeout2SecButton_;
        PaintedButton timeout5SecButton_;
        PaintedButton timeout10SecButton_;
        PaintedButton loadThemeButton_;
        PaintedButton randomThemeButton_;
        PaintedButton saveThemeButton_;
        PaintedButton closeButton_;
        
        std::unique_ptr<FileChooser> themeChooser_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsComponent)
    };
}
