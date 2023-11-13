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
#include "PaintedButton.h"
#include "PortListComponent.h"
#include "SettingsComponent.h"

namespace showmidi
{
    SidebarListener::SidebarListener() {}
    SidebarListener::~SidebarListener() {}

    struct SidebarComponent::Pimpl : public Button::Listener
    {
        static constexpr int COLLAPSED_WIDTH = 36;
        static constexpr int EXPANDED_WIDTH = 200;
        static constexpr int Y_PORTLIST = 48;
        static constexpr int PORTLIST_BOTTOM_MARGIN = 36;

        Pimpl(SidebarComponent* owner, SettingsManager& manager, SidebarType type, SidebarListener& listener) :
            owner_(owner),
            manager_(manager),
            sidebarType_(type),
            listener_(listener),
            settings_(manager),
            about_(manager.getSettings().getTheme())
        {
            collapsedButton_.addListener(this);
            expandedButton_.addListener(this);
            helpButton_.addListener(this);
            settingsButton_.addListener(this);

            owner_->addChildComponent(collapsedButton_);
            owner_->addChildComponent(expandedButton_);
            owner_->addAndMakeVisible(helpButton_);
            owner_->addChildComponent(settingsButton_);
                        
            if (sidebarType_ == SidebarType::sidebarExpandable)
            {
                collapsedButton_.setVisible(true);
                
                portList_ = std::make_unique<PortListComponent>(manager_);
                
                portViewport_ = std::make_unique<Viewport>();
                portViewport_->setScrollOnDragMode(Viewport::ScrollOnDragMode::all);
                portViewport_->setScrollBarsShown(true, false);
                portViewport_->setScrollBarThickness(Theme::SCROLLBAR_THICKNESS);
                portViewport_->setViewedComponent(portList_.get(), false);
                owner_->addChildComponent(portViewport_.get());
            }
            else
            {
                settingsButton_.setVisible(true);
            }
        }
        
        void setup()
        {
            owner_->getParentComponent()->addChildComponent(about_);
            owner_->getParentComponent()->addChildComponent(settings_);
        }

        void buttonClicked(Button* button)
        {
            if (button == &collapsedButton_)
            {
                expanded_ = true;
                collapsedButton_.setVisible(false);
                expandedButton_.setVisible(true);
                settingsButton_.setVisible(true);
                if (portViewport_.get())
                {
                    portViewport_->setVisible(true);
                }
                
                settings_.setVisible(false);
                about_.setVisible(false);
                
                listener_.sidebarChangedWidth();
            }
            else if (button == &expandedButton_)
            {
                expanded_ = false;
                collapsedButton_.setVisible(true);
                expandedButton_.setVisible(false);
                settingsButton_.setVisible(false);
                if (portViewport_.get())
                {
                    portViewport_->setVisible(false);
                }
                
                settings_.setVisible(false);
                about_.setVisible(false);
                
                listener_.sidebarChangedWidth();
            }
            else if (button == &helpButton_)
            {
                about_.setVisible(!about_.isVisible());
                settings_.setVisible(false);
            }
            else if (button == &settingsButton_)
            {
                settings_.setVisible(!settings_.isVisible());
                about_.setVisible(false);
            }
        }

        void paint(Graphics& g)
        {
            auto& theme = manager_.getSettings().getTheme();
            
            g.fillAll(theme.colorSidebar);

            if (collapsedButton_.isVisible())
            {
                auto collapsed_svg = collapsedSvg_->createCopy();
                collapsed_svg->replaceColour(Colours::black, theme.colorData);
                collapsedButton_.drawDrawable(g, *collapsed_svg);
            }

            if (expandedButton_.isVisible())
            {
                auto expanded_svg = expandedSvg_->createCopy();
                expanded_svg->replaceColour(Colours::black, theme.colorData);
                expandedButton_.drawDrawable(g, *expanded_svg);
            }

            auto help_svg = helpSvg_->createCopy();
            help_svg->replaceColour(Colours::black, theme.colorData);
            helpButton_.drawDrawable(g, *help_svg);

            if (settingsButton_.isVisible())
            {
                auto settings_svg = settingsSvg_->createCopy();
                settings_svg->replaceColour(Colours::black, theme.colorData);
                settingsButton_.drawDrawable(g, *settings_svg);
            }
        }
        
        void resized()
        {
            collapsedButton_.setBoundsForTouch(X_COLLAPSED, Y_COLLAPSED,
                                               collapsedSvg_->getWidth(), collapsedSvg_->getHeight());

            expandedButton_.setBoundsForTouch(X_EXPANDED, Y_EXPANDED,
                                              expandedSvg_->getWidth(), expandedSvg_->getHeight());

            helpButton_.setBoundsForTouch(X_HELP, owner_->getHeight() - helpSvg_->getHeight() - Y_HELP,
                                          helpSvg_->getWidth(), helpSvg_->getHeight());
            
            settingsButton_.setBoundsForTouch(owner_->getWidth() - expandedSvg_->getWidth() - X_SETTINGS, Y_SETTINGS,
                                              settingsSvg_->getWidth(), settingsSvg_->getHeight());

            if (portViewport_.get())
            {
                portViewport_->setBounds(0, Y_PORTLIST, owner_->getWidth(), owner_->getHeight() - Y_PORTLIST - PORTLIST_BOTTOM_MARGIN);
                portList_->setSize(owner_->getWidth() - portViewport_->getScrollBarThickness(), std::max(portViewport_->getHeight(), portList_->getVisibleHeight()));
            }
            about_.setTopLeftPosition(owner_->getWidth() + X_SETTINGS, owner_->getHeight() - Y_SETTINGS - about_.getHeight());
            settings_.setTopLeftPosition(owner_->getWidth() + X_SETTINGS, Y_SETTINGS);
        }
        
        int getActiveWidth()
        {
            if (expanded_)
            {
                return EXPANDED_WIDTH;
            }
            
            return COLLAPSED_WIDTH;
        }

        SidebarComponent* const owner_;
        SettingsManager& manager_;
        const SidebarType sidebarType_;
        SidebarListener& listener_;
        
        bool expanded_ = false;
        
        std::unique_ptr<Drawable> collapsedSvg_ = Drawable::createFromImageData(BinaryData::collapsed_svg, BinaryData::collapsed_svgSize);
        std::unique_ptr<Drawable> expandedSvg_ = Drawable::createFromImageData(BinaryData::expanded_svg, BinaryData::expanded_svgSize);
        std::unique_ptr<Drawable> helpSvg_ = Drawable::createFromImageData(BinaryData::help_svg, BinaryData::help_svgSize);
        std::unique_ptr<Drawable> settingsSvg_ = Drawable::createFromImageData(BinaryData::settings_svg, BinaryData::settings_svgSize);

        PaintedButton collapsedButton_;
        PaintedButton expandedButton_;
        PaintedButton helpButton_;
        PaintedButton settingsButton_;
        std::unique_ptr<Viewport> portViewport_;
        std::unique_ptr<PortListComponent> portList_;
        SettingsComponent settings_;
        AboutComponent about_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pimpl)
    };
    
    SidebarComponent::SidebarComponent(SettingsManager& m, SidebarType t, SidebarListener& l) : pimpl_(new Pimpl(this, m, t, l)) {}
    SidebarComponent::~SidebarComponent() = default;

    void SidebarComponent::setup() { pimpl_->setup(); }

    void SidebarComponent::paint(Graphics& g) { pimpl_->paint(g); }
    void SidebarComponent::resized()          { pimpl_->resized(); }
    int SidebarComponent::getActiveWidth()    { return pimpl_->getActiveWidth(); }
}
