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
#include "DeviceListener.h"
#include "PortListComponent.h"
#include "SettingsComponent.h"

namespace showmidi
{
    SidebarListener::SidebarListener() {}
    SidebarListener::~SidebarListener() {}

    struct SidebarComponent::Pimpl : public Button::Listener, public DeviceListener
    {
        static constexpr int COLLAPSED_WIDTH = 36;
        static constexpr int Y_PORTLIST = 48;
        static constexpr int PORTLIST_BOTTOM_MARGIN = 36;

        Pimpl(SidebarComponent* owner, SettingsManager* settings, DeviceManager* deviceManager, SidebarType type, SidebarListener* listener) :
            owner_(owner),
            settingsManager_(settings),
            deviceManager_(deviceManager),
            sidebarType_(type),
            listener_(listener)
        {
            deviceManager_->getDeviceListeners().add(this);
            
            collapsedButton_ = std::make_unique<PaintedButton>();
            expandedButton_ = std::make_unique<PaintedButton>();
            helpButton_ = std::make_unique<PaintedButton>();
            settingsButton_ = std::make_unique<PaintedButton>();
            playButton_ = std::make_unique<PaintedButton>();
            pauseButton_ = std::make_unique<PaintedButton>();
            barButton_ = std::make_unique<PaintedButton>();
            graphButton_ = std::make_unique<PaintedButton>();
            resetButton_ = std::make_unique<PaintedButton>();
            
            collapsedButton_->setTooltip("Hide or Show the Sidebar");
            expandedButton_->setTooltip("Hide or Show the Sidebar");
            helpButton_->setTooltip("Show the Help Window");
            settingsButton_->setTooltip("Show the Settings Window");
            playButton_->setTooltip("Pause or Play the MIDI Data Stream [shortcut: spacebar]");
            pauseButton_->setTooltip("Pause or Play the MIDI Data Stream [shortcut: spacebar]");
            barButton_->setTooltip("Visualize Control Data as Line or Graph [shortcut: v]");
            graphButton_->setTooltip("Visualize Control Data as Line or Graph [shortcut: v]");
            resetButton_->setTooltip("Clears All Data [shortcut: backspace]");
            
            settings_ = std::make_unique<SettingsComponent>(settingsManager_);
            about_ = std::make_unique<AboutComponent>(settingsManager_->getSettings().getTheme());

            collapsedButton_->addListener(this);
            expandedButton_->addListener(this);
            helpButton_->addListener(this);
            settingsButton_->addListener(this);
            playButton_->addListener(this);
            pauseButton_->addListener(this);
            barButton_->addListener(this);
            graphButton_->addListener(this);
            resetButton_->addListener(this);

            owner_->addChildComponent(collapsedButton_.get());
            owner_->addChildComponent(expandedButton_.get());
            owner_->addAndMakeVisible(helpButton_.get());
            owner_->addChildComponent(settingsButton_.get());
            owner_->addAndMakeVisible(playButton_.get());
            owner_->addChildComponent(pauseButton_.get());
            owner_->addChildComponent(barButton_.get());
            owner_->addChildComponent(graphButton_.get());
            owner_->addAndMakeVisible(resetButton_.get());
            
            updateSettings();
                        
            if (sidebarType_ == SidebarType::sidebarExpandable)
            {
                collapsedButton_->setVisible(true);
                
                portList_ = std::make_unique<PortListComponent>(settingsManager_);
                
                portViewport_ = std::make_unique<Viewport>();
                portViewport_->setScrollOnDragMode(Viewport::ScrollOnDragMode::all);
                portViewport_->setScrollBarsShown(true, false);
                portViewport_->setScrollBarThickness(Theme::SCROLLBAR_THICKNESS);
                portViewport_->setViewedComponent(portList_.get(), false);
                owner_->addChildComponent(portViewport_.get());
            }
            else
            {
                settingsButton_->setVisible(true);
            }
        }
        
        ~Pimpl()
        {
            deviceManager_->getDeviceListeners().remove(this);
        }

        void setup()
        {
            owner_->getParentComponent()->addChildComponent(about_.get());
            owner_->getParentComponent()->addChildComponent(settings_.get());
        }
        
        void updateSettings()
        {
            if (settingsManager_->getSettings().getVisualization() == Visualization::visualizationBar)
            {
                barButton_->setVisible(true);
                graphButton_->setVisible(false);
            }
            else
            {
                barButton_->setVisible(false);
                graphButton_->setVisible(true);
            }
        }
        
        void pauseChanged(bool state)
        {
            playButton_->setVisible(!state);
            pauseButton_->setVisible(state);
            owner_->repaint();
        }

        void buttonClicked(Button* button)
        {
            if (button == collapsedButton_.get())
            {
                expanded_ = true;
                collapsedButton_->setVisible(false);
                expandedButton_->setVisible(true);
                settingsButton_->setVisible(true);
                if (portViewport_.get())
                {
                    portViewport_->setVisible(true);
                }
                
                settings_->setVisible(false);
                about_->setVisible(false);
                
                listener_->sidebarChangedWidth();
            }
            else if (button == expandedButton_.get())
            {
                expanded_ = false;
                collapsedButton_->setVisible(true);
                expandedButton_->setVisible(false);
                settingsButton_->setVisible(false);
                if (portViewport_.get())
                {
                    portViewport_->setVisible(false);
                }
                
                settings_->setVisible(false);
                about_->setVisible(false);
                
                listener_->sidebarChangedWidth();
            }
            else if (button == playButton_.get() || button == pauseButton_.get())
            {
                deviceManager_->togglePaused();
            }
            else if (button == barButton_.get())
            {
                settingsManager_->getSettings().setVisualization(Visualization::visualizationGraph);
                updateSettings();
            }
            else if (button == graphButton_.get())
            {
                settingsManager_->getSettings().setVisualization(Visualization::visualizationBar);
                updateSettings();
            }
            else if (button == resetButton_.get())
            {
                deviceManager_->resetChannelData();
            }
            else if (button == helpButton_.get())
            {
                about_->setVisible(!about_->isVisible());
                settings_->setVisible(false);
            }
            else if (button == settingsButton_.get())
            {
                settings_->setVisible(!settings_->isVisible());
                about_->setVisible(false);
            }
        }

        void paint(Graphics& g)
        {
            auto& theme = settingsManager_->getSettings().getTheme();
            
            g.fillAll(theme.colorSidebar);

            if (collapsedButton_->isVisible())
            {
                auto collapsed_svg = collapsedSvg_->createCopy();
                collapsed_svg->replaceColour(Colours::black, theme.colorData);
                collapsedButton_->drawDrawable(g, *collapsed_svg);
            }

            if (expandedButton_->isVisible())
            {
                auto expanded_svg = expandedSvg_->createCopy();
                expanded_svg->replaceColour(Colours::black, theme.colorData);
                expandedButton_->drawDrawable(g, *expanded_svg);
            }

            if (playButton_->isVisible())
            {
                auto play_svg = playSvg_->createCopy();
                play_svg->replaceColour(Colours::black, theme.colorData);
                playButton_->drawDrawable(g, *play_svg);
            }

            if (pauseButton_->isVisible())
            {
                auto pause_svg = pauseSvg_->createCopy();
                pause_svg->replaceColour(Colours::black, theme.colorData);
                pauseButton_->drawDrawable(g, *pause_svg);
            }

            if (barButton_->isVisible())
            {
                auto bar_svg = barSvg_->createCopy();
                bar_svg->replaceColour(Colours::black, theme.colorData);
                barButton_->drawDrawable(g, *bar_svg);
            }

            if (graphButton_->isVisible())
            {
                auto graph_svg = graphSvg_->createCopy();
                graph_svg->replaceColour(Colours::black, theme.colorData);
                graphButton_->drawDrawable(g, *graph_svg);
            }

            if (resetButton_->isVisible())
            {
                auto reset_svg = resetSvg_->createCopy();
                reset_svg->replaceColour(Colours::black, theme.colorData);
                resetButton_->drawDrawable(g, *reset_svg);
            }

            auto help_svg = helpSvg_->createCopy();
            help_svg->replaceColour(Colours::black, theme.colorData);
            helpButton_->drawDrawable(g, *help_svg);

            if (settingsButton_->isVisible())
            {
                auto settings_svg = settingsSvg_->createCopy();
                settings_svg->replaceColour(Colours::black, theme.colorData);
                settingsButton_->drawDrawable(g, *settings_svg);
            }
        }
        
        void resized()
        {
            collapsedButton_->setBoundsForTouch(X_COLLAPSED, Y_COLLAPSED,
                                               collapsedSvg_->getWidth(), collapsedSvg_->getHeight());

            expandedButton_->setBoundsForTouch(X_EXPANDED, Y_EXPANDED,
                                              expandedSvg_->getWidth(), expandedSvg_->getHeight());

            if (expanded_)
            {
                playButton_->setBoundsForTouch(X_PLAY_EXPANDED, Y_PLAY_EXPANDED,
                                               playSvg_->getWidth(), playSvg_->getHeight());
                
                pauseButton_->setBoundsForTouch(X_PLAY_EXPANDED, Y_PLAY_EXPANDED,
                                                pauseSvg_->getWidth(), pauseSvg_->getHeight());
                
                barButton_->setBoundsForTouch(X_VISUALIZATION_EXPANDED, Y_VISUALIZATION_EXPANDED,
                                               barSvg_->getWidth(), barSvg_->getHeight());
                
                graphButton_->setBoundsForTouch(X_VISUALIZATION_EXPANDED, Y_VISUALIZATION_EXPANDED,
                                                graphSvg_->getWidth(), graphSvg_->getHeight());
                
                resetButton_->setBoundsForTouch(owner_->getWidth() - resetSvg_->getWidth() - X_RESET_EXPANDED, Y_RESET_EXPANDED,
                                                resetSvg_->getWidth(), resetSvg_->getHeight());
            }
            else
            {
                playButton_->setBoundsForTouch(X_PLAY_COLLAPSED, Y_PLAY_COLLAPSED,
                                               playSvg_->getWidth(), playSvg_->getHeight());
                
                pauseButton_->setBoundsForTouch(X_PLAY_COLLAPSED, Y_PLAY_COLLAPSED,
                                                pauseSvg_->getWidth(), pauseSvg_->getHeight());
                
                barButton_->setBoundsForTouch(X_VISUALIZATION_COLLAPSED, Y_VISUALIZATION_COLLAPSED,
                                               barSvg_->getWidth(), barSvg_->getHeight());
                
                graphButton_->setBoundsForTouch(X_VISUALIZATION_COLLAPSED, Y_VISUALIZATION_COLLAPSED,
                                                graphSvg_->getWidth(), graphSvg_->getHeight());
                
                resetButton_->setBoundsForTouch(X_RESET_COLLAPSED, owner_->getHeight() - resetSvg_->getHeight() - Y_RESET_COLLAPSED,
                                                resetSvg_->getWidth(), resetSvg_->getHeight());
            }

            helpButton_->setBoundsForTouch(X_HELP, owner_->getHeight() - helpSvg_->getHeight() - Y_HELP,
                                          helpSvg_->getWidth(), helpSvg_->getHeight());
            
            settingsButton_->setBoundsForTouch(owner_->getWidth() - expandedSvg_->getWidth() - X_SETTINGS, Y_SETTINGS,
                                              settingsSvg_->getWidth(), settingsSvg_->getHeight());

            if (portViewport_.get())
            {
                portViewport_->setBounds(0, Y_PORTLIST, getActiveWidth(), owner_->getHeight() - Y_PORTLIST - PORTLIST_BOTTOM_MARGIN);
                portList_->setSize(getActiveWidth() - portViewport_->getScrollBarThickness(), std::max(portViewport_->getHeight(), portList_->getVisibleHeight()));
            }
            
            about_->updateDimensions();
            about_->setTopLeftPosition(owner_->getWidth() + X_SETTINGS, owner_->getHeight() - Y_SETTINGS - about_->getHeight());

            settings_->updateDimensions();
            settings_->setTopLeftPosition(owner_->getWidth() + X_SETTINGS, Y_SETTINGS);
        }
        
        int getActiveWidth()
        {
            if (expanded_)
            {
                auto& theme = settingsManager_->getSettings().getTheme();
                return theme.getSidebarExpandedWidth();
            }
            
            return COLLAPSED_WIDTH;
        }

        SidebarComponent* const owner_;
        SettingsManager* const settingsManager_;
        DeviceManager* const deviceManager_;

        const SidebarType sidebarType_;
        SidebarListener* const listener_;
        
        bool expanded_ = false;
        
        std::unique_ptr<Drawable> collapsedSvg_ = Drawable::createFromImageData(BinaryData::collapsed_svg, BinaryData::collapsed_svgSize);
        std::unique_ptr<Drawable> expandedSvg_ = Drawable::createFromImageData(BinaryData::expanded_svg, BinaryData::expanded_svgSize);
        std::unique_ptr<Drawable> helpSvg_ = Drawable::createFromImageData(BinaryData::help_svg, BinaryData::help_svgSize);
        std::unique_ptr<Drawable> settingsSvg_ = Drawable::createFromImageData(BinaryData::settings_svg, BinaryData::settings_svgSize);
        std::unique_ptr<Drawable> playSvg_ = Drawable::createFromImageData(BinaryData::play_svg, BinaryData::play_svgSize);
        std::unique_ptr<Drawable> pauseSvg_ = Drawable::createFromImageData(BinaryData::pause_svg, BinaryData::pause_svgSize);
        std::unique_ptr<Drawable> barSvg_ = Drawable::createFromImageData(BinaryData::bar_svg, BinaryData::bar_svgSize);
        std::unique_ptr<Drawable> graphSvg_ = Drawable::createFromImageData(BinaryData::graph_svg, BinaryData::graph_svgSize);
        std::unique_ptr<Drawable> resetSvg_ = Drawable::createFromImageData(BinaryData::reset_svg, BinaryData::reset_svgSize);

        std::unique_ptr<PaintedButton> collapsedButton_;
        std::unique_ptr<PaintedButton> expandedButton_;
        std::unique_ptr<PaintedButton> helpButton_;
        std::unique_ptr<PaintedButton> settingsButton_;
        std::unique_ptr<PaintedButton> playButton_;
        std::unique_ptr<PaintedButton> pauseButton_;
        std::unique_ptr<PaintedButton> barButton_;
        std::unique_ptr<PaintedButton> graphButton_;
        std::unique_ptr<PaintedButton> resetButton_;
        std::unique_ptr<Viewport> portViewport_;
        std::unique_ptr<PortListComponent> portList_;
        std::unique_ptr<SettingsComponent> settings_;
        std::unique_ptr<AboutComponent> about_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pimpl)
    };
    
    SidebarComponent::SidebarComponent(SettingsManager* s, DeviceManager* d, SidebarType t, SidebarListener* l) : pimpl_(new Pimpl(this, s, d, t, l)) {}
    SidebarComponent::~SidebarComponent() = default;

    void SidebarComponent::setup() { pimpl_->setup(); }

    void SidebarComponent::paint(Graphics& g) { pimpl_->paint(g); }
    void SidebarComponent::resized()          { pimpl_->resized(); }
    int SidebarComponent::getActiveWidth()    { return pimpl_->getActiveWidth(); }
    void SidebarComponent::updateSettings()   { pimpl_->updateSettings(); }
}
