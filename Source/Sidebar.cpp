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
#include "Sidebar.h"

#include "AboutComponent.h"
#include "ShowMidiApplication.h"

namespace showmidi
{
    struct Sidebar::Pimpl : public Button::Listener
    {
        Pimpl(Sidebar* owner) : owner_(owner)
        {
            MessageManager::getInstance()->callAsync([this] { setup(); });
        }
        
        ~Pimpl()
        {
        }
        
        void setup()
        {
            helpButton_.setBounds(X_HELP, owner_->getHeight() - helpSvg_->getHeight() - Y_HELP,
                                  helpSvg_->getWidth(), helpSvg_->getHeight());
            helpButton_.addListener(this);
            owner_->addAndMakeVisible(helpButton_);
            
            owner_->getParentComponent()->addChildComponent(about_);
        }
        static constexpr int X_SETTINGS = 12;
        static constexpr int Y_SETTINGS = 13;

        static constexpr int X_HELP = 11;
        static constexpr int Y_HELP = 12;

        void buttonClicked(Button* button)
        {
            if (button == &helpButton_)
            {
                about_.setVisible(!about_.isVisible());
            }
        }

        void paint(juce::Graphics& g)
        {
            g.fillAll(SMApp.getTheme().colorSidebar);
            
            auto settings_svg = settingsSvg_->createCopy();
            settings_svg->replaceColour(Colours::black, SMApp.getTheme().colorData);
            settings_svg->drawAt(g, X_SETTINGS, Y_SETTINGS, 1.0);
            
            auto help_svg = helpSvg_->createCopy();
            help_svg->replaceColour(Colours::black, SMApp.getTheme().colorData);
            help_svg->drawAt(g, X_HELP, owner_->getHeight() - help_svg->getHeight() - Y_HELP, 1.0);
        }

        Sidebar* const owner_;
        
        std::unique_ptr<Drawable> settingsSvg_ = Drawable::createFromImageData(BinaryData::settings_svg, BinaryData::settings_svgSize);
        std::unique_ptr<Drawable> helpSvg_ = Drawable::createFromImageData(BinaryData::help_svg, BinaryData::help_svgSize);

        TextButton helpButton_;
        AboutComponent about_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pimpl)
    };
    
    Sidebar::Sidebar() : pimpl_(new Pimpl(this)) {}
    Sidebar::~Sidebar() = default;
    
    void Sidebar::paint(juce::Graphics& g) { pimpl_->paint(g); }
}
