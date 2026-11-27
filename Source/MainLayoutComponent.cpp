/*
 * This file is part of ShowMIDI.
 * Copyright (c) 2023 Uwyn LLC.  https://www.uwyn.com
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
#include "MainLayoutComponent.h"

#include "MidiDeviceComponent.h"
#include "SettingsComponent.h"
#include "SidebarComponent.h"
#include "ShowMidiApplication.h"

namespace showmidi
{
    struct MainLayoutComponent::Pimpl : public SidebarListener, public KeyListener, public ComponentListener
    {
        static constexpr int DEFAULT_WINDOW_HEIGHT = 600;
        
        struct SettingsColumn : public Component
        {
            SettingsColumn(SettingsManager* settings, MainLayoutType type) : settingsManager_(settings), layoutType_(type) {}
            
            // the standalone device columns start with a gap and the plugin's doesn't,
            // so the gap sits on the side that keeps this column apart from both
            int getColumnX()
            {
                return layoutType_ == MainLayoutType::layoutStandalone ? Theme::MIDI_DEVICE_SPACING : 0;
            }
            
            void paint(Graphics& g) override
            {
                auto& theme = settingsManager_->getSettings().getTheme();
                g.fillAll(theme.colorSidebar);
                g.setColour(theme.colorBackground);
                g.fillRect(getColumnX(), 0, MidiDeviceComponent::getStandardWidth(), getHeight());
            }
            
            SettingsManager* const settingsManager_;
            const MainLayoutType layoutType_;
        };
        
        Pimpl(MainLayoutComponent* owner, SettingsManager* settings, DeviceManager* deviceManager, MainLayoutType type, Component* content) :
            owner_(owner),
            settingsManager_(settings),
            deviceManager_(deviceManager),
            layoutType_(type)
        {
            sidebar_ = std::make_unique<SidebarComponent>(
                settingsManager_, deviceManager_,
                layoutType_ == MainLayoutType::layoutStandalone ? SidebarType::sidebarExpandable : SidebarType::sidebarFixed,
                this);
            viewport_ = std::make_unique<Viewport>();
            
            settings_ = std::make_unique<SettingsComponent>(settingsManager_);
            settingsColumn_ = std::make_unique<SettingsColumn>(settingsManager_, layoutType_);
            settingsViewport_ = std::make_unique<Viewport>();
            settingsViewport_->setScrollOnDragMode(Viewport::ScrollOnDragMode::all);
            settingsViewport_->setScrollBarsShown(true, false);
            settingsViewport_->setScrollBarThickness(Theme::SCROLLBAR_THICKNESS);
            settingsViewport_->setViewedComponent(settings_.get(), false);
            settingsColumn_->addAndMakeVisible(settingsViewport_.get());
            owner_->addChildComponent(settingsColumn_.get());
            settings_->setVisible(false);
            // the panel's close button hides it, the column follows along
            settings_->addComponentListener(this);

            owner_->setWantsKeyboardFocus(true);
            owner_->addKeyListener(this);
            
            sidebar_->setBounds(0, 0, getSidebarWidth(), DEFAULT_WINDOW_HEIGHT);
            owner_->addAndMakeVisible(sidebar_.get());

            auto default_width = MidiDeviceComponent::getStandardWidth() + Theme::SCROLLBAR_THICKNESS;
            if (type == MainLayoutType::layoutStandalone)
            {
                default_width += Theme::MIDI_DEVICE_SPACING * 2;
            }
            viewport_->setScrollOnDragMode(Viewport::ScrollOnDragMode::all);
            viewport_->setScrollBarsShown(true, true);
            viewport_->setScrollBarThickness(Theme::SCROLLBAR_THICKNESS);
            viewport_->setViewedComponent(content, false);
            viewport_->setBounds(sidebar_->getWidth(), 0, default_width, DEFAULT_WINDOW_HEIGHT);
            owner_->addAndMakeVisible(viewport_.get());
        }
        
        ~Pimpl()
        {
            owner_->removeKeyListener(this);
            settings_->removeComponentListener(this);
        }

        bool keyPressed(const KeyPress& key, Component*) override
        {
            if (key.getKeyCode() == KeyPress::spaceKey)
            {
                deviceManager_->togglePaused();
                return true;
            }
            else if (key.getKeyCode() == KeyPress::deleteKey || key.getKeyCode() == KeyPress::backspaceKey)
            {
                deviceManager_->resetChannelData();
                return true;
            }
            else if (key.getKeyCode() == 'v' || key.getKeyCode() == 'V')
            {
                auto& settings = settingsManager_->getSettings();
                auto visualization = settings.getVisualization();
                if (visualization == Visualization::visualizationBar)
                {
                    settings.setVisualization(Visualization::visualizationGraph);
                }
                else
                {
                    settings.setVisualization(Visualization::visualizationBar);
                }
                sidebar_->updateSettings();
                return true;
            }
            
            return false;
        }

        bool isInterestedInFileDrag(const StringArray& files)
        {
            for (auto file : files)
            {
                if (file.endsWith(".svg"))
                {
                    return true;
                }
            }
            
            return false;
        }
        
        void filesDropped(const StringArray& files)
        {
            for (auto file : files)
            {
                settingsManager_->getSettings().getTheme().parseXml(File(file).loadFileAsString());
            }
            
            owner_->repaint();
            
            settingsManager_->storeSettings();
        }

        void paint(Graphics& g)
        {
            // shows behind the devices until they catch up with a new window width
            g.fillAll(settingsManager_->getSettings().getTheme().colorSidebar);
        }
        
        void resized()
        {
            sidebar_->setBounds(0, 0, getSidebarWidth(), owner_->getHeight());
            
            settingsColumn_->setBounds(getSidebarWidth(), 0, getSettingsWidth(), owner_->getHeight());
            settings_->updateDimensions();
            auto settings_height = std::min(settings_->getHeight(), owner_->getHeight() - SidebarComponent::Y_SETTINGS * 2);
            // the extra pixel leaves a gap between the panel's right edge and
            // the scrollbar, which only shows when the panel doesn't fit
            settingsViewport_->setBounds(settingsColumn_->getColumnX() + SidebarComponent::X_SETTINGS, SidebarComponent::Y_SETTINGS,
                                         settings_->getWidth() + 1 + Theme::SCROLLBAR_THICKNESS, settings_height);
            
            viewport_->setBounds(getDevicesX(), 0, std::max(0, owner_->getWidth() - getDevicesX()), owner_->getHeight());
        }
        
        int getSidebarWidth()
        {
            return sidebar_->getActiveWidth();
        }
        
        static int getSettingsColumnWidth()
        {
            return MidiDeviceComponent::getStandardWidth() + Theme::MIDI_DEVICE_SPACING;
        }
        
        int getSettingsWidth()
        {
            return settings_->isVisible() ? getSettingsColumnWidth() : 0;
        }
        
        int getDevicesX()
        {
            return getSidebarWidth() + getSettingsWidth();
        }
        
        void toggleSettings() override
        {
            settings_->setVisible(!settings_->isVisible());
        }
        
        void componentVisibilityChanged(Component& component) override
        {
            if (&component != settings_.get() || settingsColumn_->isVisible() == settings_->isVisible())
            {
                return;
            }
            
            settingsColumn_->setVisible(settings_->isVisible());
            
            if (layoutType_ == MainLayoutType::layoutStandalone)
            {
                // the window resizes first, laying out before that shows a frame of squeezed devices
                SMApp.setWindowWidthForMainLayout(viewport_->getWidth());
                resized();
            }
            else
            {
                auto delta = settings_->isVisible() ? getSettingsColumnWidth() : -getSettingsColumnWidth();
                owner_->setSize(owner_->getWidth() + delta, owner_->getHeight());
                // a host can refuse the new size, the device column is then out of view until the settings close
                if (auto editor = owner_->findParentComponentOfClass<AudioProcessorEditor>())
                {
                    if (auto constrainer = editor->getConstrainer())
                    {
                        constrainer->setMinimumWidth(constrainer->getMinimumWidth() + delta);
                    }
                    editor->setSize(editor->getWidth() + delta, editor->getHeight());
                }
            }
        }
        
        void sidebarChangedWidth() override
        {
            if (layoutType_ == MainLayoutType::layoutStandalone)
            {
                SMApp.setWindowWidthForMainLayout(viewport_->getWidth());
            }
        }

        MainLayoutComponent* const owner_;
        SettingsManager* const settingsManager_;
        DeviceManager* const deviceManager_;
        TooltipWindow const tooltipWindow_;
        
        const MainLayoutType layoutType_;
        
        std::unique_ptr<SidebarComponent> sidebar_;
        std::unique_ptr<Viewport> viewport_;
        std::unique_ptr<SettingsComponent> settings_;
        std::unique_ptr<SettingsColumn> settingsColumn_;
        std::unique_ptr<Viewport> settingsViewport_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pimpl)
    };
    
    MainLayoutComponent::MainLayoutComponent(SettingsManager* s, DeviceManager* d, MainLayoutType t, Component* c) : pimpl_(new Pimpl(this, s, d, t, c))
    {
        pimpl_->sidebar_->setup();
        
        setSize(pimpl_->sidebar_->getWidth() + pimpl_->viewport_->getWidth(), Pimpl::DEFAULT_WINDOW_HEIGHT);
    }
    
    MainLayoutComponent::~MainLayoutComponent() = default;
    
    bool MainLayoutComponent::isInterestedInFileDrag(const StringArray& f)  { return pimpl_->isInterestedInFileDrag(f); };
    void MainLayoutComponent::filesDropped(const StringArray& f, int, int)  { pimpl_->filesDropped(f); };

    void MainLayoutComponent::paint(Graphics& g)  { pimpl_->paint(g); }
    void MainLayoutComponent::resized()         { pimpl_->resized(); }
    int MainLayoutComponent::getSidebarWidth()  { return pimpl_->getSidebarWidth(); }
    int MainLayoutComponent::getDevicesX()      { return pimpl_->getDevicesX(); }
}
