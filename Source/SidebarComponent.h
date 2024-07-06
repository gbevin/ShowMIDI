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

#include <JuceHeader.h>

#include "PauseManager.h"
#include "SettingsManager.h"

namespace showmidi
{
    enum SidebarType
    {
        sidebarExpandable = 1,
        sidebarFixed
    };
    
    class SidebarListener
    {
    public:
        SidebarListener();
        virtual ~SidebarListener();
        
        virtual void sidebarChangedWidth() = 0;
    };
    
    class SidebarComponent : public Component
    {
    public:        
        static constexpr int X_COLLAPSED = 14;
        static constexpr int Y_COLLAPSED = 13;

        static constexpr int X_EXPANDED = 12;
        static constexpr int Y_EXPANDED = 13;

        static constexpr int X_SETTINGS = 11;
        static constexpr int Y_SETTINGS = 13;

        static constexpr int X_PLAY_COLLAPSED = 12;
        static constexpr int Y_PLAY_COLLAPSED = 44;

        static constexpr int X_PLAY_EXPANDED = 40;
        static constexpr int Y_PLAY_EXPANDED = 11;

        static constexpr int X_VISUALIZATION_COLLAPSED = 12;
        static constexpr int Y_VISUALIZATION_COLLAPSED = 78;

        static constexpr int X_VISUALIZATION_EXPANDED = 68;
        static constexpr int Y_VISUALIZATION_EXPANDED = 11;

        static constexpr int X_HELP = 11;
        static constexpr int Y_HELP = 12;

        SidebarComponent(SettingsManager*, PauseManager*, SidebarType, SidebarListener*);
        ~SidebarComponent() override;
        
        void setup();

        void paint(Graphics&) override;
        
        void resized() override;
        
        int getActiveWidth();
        
        struct Pimpl;
    private:
        std::unique_ptr<Pimpl> pimpl_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SidebarComponent)
    };
}
