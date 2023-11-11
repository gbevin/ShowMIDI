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
#include "SidebarComponent.h"

#include "AboutComponent.h"
#include "SettingsComponent.h"

namespace showmidi
{
    struct SidebarComponent::Pimpl : public Button::Listener
    {
        Pimpl(SidebarComponent* owner, Theme& theme) : owner_(owner), theme_(theme), settings_(theme), about_(theme)
        {
            MessageManager::getInstance()->callAsync([this] { setup(); });
        }
        
        ~Pimpl()
        {
        }
        
        void setup()
        {
            helpButton_.addListener(this);
            settingsButton_.addListener(this);

            owner_->addAndMakeVisible(helpButton_);
            owner_->addAndMakeVisible(settingsButton_);

            owner_->getParentComponent()->addChildComponent(about_);
            owner_->getParentComponent()->addChildComponent(settings_);

            resized();
        }

        void buttonClicked(Button* button)
        {
            if (button == &settingsButton_)
            {
                settings_.setVisible(!settings_.isVisible());
            }
            else if (button == &helpButton_)
            {
                about_.setVisible(!about_.isVisible());
            }
        }

        void paint(Graphics& g)
        {
            g.fillAll(theme_.colorSidebar);
            
            auto settings_svg = settingsSvg_->createCopy();
            settings_svg->replaceColour(Colours::black, theme_.colorData);
            settings_svg->drawAt(g, settingsButton_.getX(), settingsButton_.getY(), 1.0);
            
            auto help_svg = helpSvg_->createCopy();
            help_svg->replaceColour(Colours::black, theme_.colorData);
            help_svg->drawAt(g, helpButton_.getX(), helpButton_.getY(), 1.0);
        }
        
        void resized()
        {
            settingsButton_.setBounds(X_SETTINGS, Y_SETTINGS,
                                      settingsSvg_->getWidth(), settingsSvg_->getHeight());

            helpButton_.setBounds(X_HELP, owner_->getHeight() - helpSvg_->getHeight() - Y_HELP,
                                  helpSvg_->getWidth(), helpSvg_->getHeight());
            
            settings_.setTopLeftPosition(owner_->getWidth() + X_SETTINGS, Y_SETTINGS);
            about_.setTopLeftPosition(owner_->getWidth() + X_SETTINGS, owner_->getHeight() - Y_SETTINGS - about_.getHeight());
        }

        SidebarComponent* const owner_;
        Theme& theme_;
        
        std::unique_ptr<Drawable> settingsSvg_ = Drawable::createFromImageData(BinaryData::settings_svg, BinaryData::settings_svgSize);
        std::unique_ptr<Drawable> helpSvg_ = Drawable::createFromImageData(BinaryData::help_svg, BinaryData::help_svgSize);

        TextButton settingsButton_;
        TextButton helpButton_;
        SettingsComponent settings_;
        AboutComponent about_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pimpl)
    };
    
    SidebarComponent::SidebarComponent(Theme& theme) : pimpl_(new Pimpl(this, theme)) {}
    SidebarComponent::~SidebarComponent() = default;
    
    void SidebarComponent::paint(Graphics& g) { pimpl_->paint(g); }
    void SidebarComponent::resized()          { pimpl_->resized(); }
}
