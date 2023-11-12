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

#include "MainComponent.h"
#include "MidiDeviceComponent.h"
#include "SidebarComponent.h"

namespace showmidi
{
    struct MainLayoutComponent::Pimpl
    {
        static constexpr int DEFAULT_WINDOW_HEIGHT = 600;
        
        Pimpl(MainLayoutComponent* owner, SettingsManager& manager, MainLayoutType type, Component* content) :
            owner_(owner),
            settingsManager_(manager),
            layoutType_(type)
        {
            sidebar_.setBounds(0, 0, getSidebarWidth(), DEFAULT_WINDOW_HEIGHT);
            owner_->addAndMakeVisible(sidebar_);

            viewport_.setScrollOnDragMode(Viewport::ScrollOnDragMode::all);
            viewport_.setScrollBarsShown(true, true);
            viewport_.setScrollBarThickness(4);
            viewport_.setViewedComponent(content, false);
            auto viewport_width = MidiDeviceComponent::getStandardWidth();
            if (layoutType_ == MainLayoutType::layoutStandalone)
            {
                viewport_width += 4;
            }
            viewport_.setBounds(sidebar_.getWidth(), 0, viewport_width, DEFAULT_WINDOW_HEIGHT);
            owner_->addAndMakeVisible(viewport_);
        }
        
        ~Pimpl()
        {
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
                settingsManager_.getSettings().getTheme().parseXml(File(file).loadFileAsString());
            }
            
            owner_->repaint();
            
            settingsManager_.storeSettings();
        }

        void resized()
        {
            sidebar_.setBounds(0, 0, getSidebarWidth(), owner_->getHeight());
            viewport_.setBounds(getSidebarWidth(), 0, owner_->getWidth() - getSidebarWidth(), owner_->getHeight());
        }
        
        int getSidebarWidth()
        {
            return sidebar_.getActiveWidth();
        }

        MainLayoutComponent* const owner_;
        SettingsManager& settingsManager_;
        const MainLayoutType layoutType_;
        SidebarComponent sidebar_ { settingsManager_, layoutType_ == MainLayoutType::layoutStandalone };
        Viewport viewport_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pimpl)
    };
    
    MainLayoutComponent::MainLayoutComponent(SettingsManager& m, MainLayoutType t, Component* c) : pimpl_(new Pimpl(this, m, t, c))
    {
        setSize(pimpl_->sidebar_.getWidth() + pimpl_->viewport_.getWidth(), Pimpl::DEFAULT_WINDOW_HEIGHT);
    }
    
    MainLayoutComponent::~MainLayoutComponent() = default;
    
    bool MainLayoutComponent::isInterestedInFileDrag(const StringArray& f)  { return pimpl_->isInterestedInFileDrag(f); };
    void MainLayoutComponent::filesDropped(const StringArray& f, int, int)  { pimpl_->filesDropped(f); };

    void MainLayoutComponent::resized()         { pimpl_->resized(); }
    int MainLayoutComponent::getSidebarWidth()  { return pimpl_->getSidebarWidth(); }
}
