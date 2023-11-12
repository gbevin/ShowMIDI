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
#include "StandaloneWindow.h"

#include "StandaloneDevicesComponent.h"
#include "StandaloneDevicesComponent.h"
#include "MainLayoutComponent.h"
#include "MidiDeviceComponent.h"
#include "SidebarComponent.h"
#include "ShowMidiApplication.h"

namespace showmidi
{
    struct StandaloneWindow::Pimpl
    {
        Pimpl(StandaloneWindow* owner) : owner_(owner)
        {
            owner_->setUsingNativeTitleBar(true);
            
            owner_->setContentNonOwned(&layout_, true);
#if JUCE_IOS
            owner_->setFullScreen(true);
#else
            owner_->setResizable(true, true);
            owner_->centreWithSize(owner_->getWidth(), owner_->getHeight());
#endif
        }
        
        ~Pimpl()
        {
            owner_->clearContentComponent();
        }
        
        void closeButtonPressed()
        {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }
        
        StandaloneWindow* const owner_;
        StandaloneDevicesComponent main_;
        MainLayoutComponent layout_ { SMApp, main_, MainLayoutType::layoutStandalone, &main_ };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pimpl)
    };
    
    StandaloneWindow::StandaloneWindow(String name) :
        DocumentWindow(name, SMApp.getSettings().getTheme().colorBackground, DocumentWindow::allButtons),
        pimpl_(new Pimpl(this))
    {
        setVisible(true);
    }
    
    StandaloneWindow::~StandaloneWindow() = default;
    
    void StandaloneWindow::resized()
    {
        if (pimpl_.get() && isVisible())
        {
            pimpl_->layout_.setBounds(0, 0, getWidth(), getHeight());
        }
    };
    
    void StandaloneWindow::closeButtonPressed() { pimpl_->closeButtonPressed(); }
    int StandaloneWindow::getSidebarWidth()     { return pimpl_->layout_.getSidebarWidth(); }
}
