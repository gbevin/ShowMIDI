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
#include "MainWindow.h"

#include "MainComponent.h"
#include "MainComponent.h"
#include "MainLayoutComponent.h"
#include "MidiDeviceComponent.h"
#include "SidebarComponent.h"
#include "ShowMidiApplication.h"

namespace showmidi
{
    struct MainWindow::Pimpl
    {
        static constexpr int DEFAULT_WINDOW_HEIGHT = 600;
        
        Pimpl(MainWindow* owner) : owner_(owner)
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
        
        MainWindow* const owner_;
        MainComponent main_;
        MainLayoutComponent layout_ { SMApp, MainLayoutType::layoutStandalone, &main_ };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pimpl)
    };
    
    MainWindow::MainWindow(String name) :
        DocumentWindow(name, SMApp.getSettings().getTheme().colorBackground, DocumentWindow::allButtons),
        pimpl_(new Pimpl(this))
    {
        setVisible(true);
    }
    
    MainWindow::~MainWindow() = default;
    
    void MainWindow::resized()
    {
        if (pimpl_.get() && isVisible())
        {
            pimpl_->layout_.setBounds(0, 0, getWidth(), getHeight());
        }
    };
    
    void MainWindow::closeButtonPressed() { pimpl_->closeButtonPressed(); }
    int MainWindow::getSidebarWidth()     { return pimpl_->layout_.getSidebarWidth(); }
}
