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
#include "MainLayoutComponent.h"

#include "MidiDeviceComponent.h"
#include "SidebarComponent.h"
#include "ShowMidiApplication.h"

namespace showmidi
{
    struct MainLayoutComponent::Pimpl : public SidebarListener, public KeyListener
    {
        static constexpr int DEFAULT_WINDOW_HEIGHT = 600;
        
        Pimpl(MainLayoutComponent* owner, SettingsManager* settings, PauseManager* pause, MainLayoutType type, Component* content) :
            owner_(owner),
            settingsManager_(settings),
            pauseManager_(pause),
            layoutType_(type)
        {
            sidebar_ = std::make_unique<SidebarComponent>(
                settingsManager_, pauseManager_,
                layoutType_ == MainLayoutType::layoutStandalone ? SidebarType::sidebarExpandable : SidebarType::sidebarFixed,
                this);
            viewport_ = std::make_unique<Viewport>();

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
        }

        bool keyPressed(const KeyPress& key, Component*) override
        {
            if (key.getKeyCode() == KeyPress::spaceKey)
            {
                pauseManager_->togglePaused();
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

        void resized()
        {
            sidebar_->setBounds(0, 0, getSidebarWidth(), owner_->getHeight());
            viewport_->setBounds(getSidebarWidth(), 0, owner_->getWidth() - getSidebarWidth(), owner_->getHeight());
        }
        
        int getSidebarWidth()
        {
            return sidebar_->getActiveWidth();
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
        PauseManager* const pauseManager_;
        
        const MainLayoutType layoutType_;
        
        std::unique_ptr<SidebarComponent> sidebar_;
        std::unique_ptr<Viewport> viewport_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pimpl)
    };
    
    MainLayoutComponent::MainLayoutComponent(SettingsManager* s, PauseManager* p, MainLayoutType t, Component* c) : pimpl_(new Pimpl(this, s, p, t, c))
    {
        pimpl_->sidebar_->setup();
        
        setSize(pimpl_->sidebar_->getWidth() + pimpl_->viewport_->getWidth(), Pimpl::DEFAULT_WINDOW_HEIGHT);
    }
    
    MainLayoutComponent::~MainLayoutComponent() = default;
    
    bool MainLayoutComponent::isInterestedInFileDrag(const StringArray& f)  { return pimpl_->isInterestedInFileDrag(f); };
    void MainLayoutComponent::filesDropped(const StringArray& f, int, int)  { pimpl_->filesDropped(f); };

    void MainLayoutComponent::resized()         { pimpl_->resized(); }
    int MainLayoutComponent::getSidebarWidth()  { return pimpl_->getSidebarWidth(); }
}
