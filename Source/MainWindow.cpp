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
#include "MidiDeviceComponent.h"
#include "SidebarComponent.h"
#include "ShowMidiApplication.h"

namespace showmidi
{
    class LayoutComponent : public Component, public FileDragAndDropTarget
    {
    public:
        LayoutComponent() {}
        virtual ~LayoutComponent() = default;
        
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
        
        void filesDropped(const StringArray& files, int, int)
        {
            for (auto file : files)
            {
                SMApp.getSettings().getTheme().parseXml(File(file).loadFileAsString());
            }
            
            repaint();
            
            SMApp.storeSettings();
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LayoutComponent)
    };
    
    struct MainWindow::Pimpl
    {
        static constexpr int SIDEBAR_WIDTH = 36;

        Pimpl(MainWindow* owner) : owner_(owner)
        {
            owner_->setUsingNativeTitleBar(true);
            
            auto layout = new LayoutComponent();
            
            sidebar_.setBounds(0, 0, SIDEBAR_WIDTH, 600);
            layout->addAndMakeVisible(sidebar_);

            viewport_.setScrollOnDragMode(Viewport::ScrollOnDragMode::all);
            viewport_.setScrollBarsShown(true, true);
            viewport_.setScrollBarThickness(4);
            viewport_.setViewedComponent(new MainComponent(), true);
            viewport_.setBounds(sidebar_.getWidth(), 0, (MidiDeviceComponent::getStandardWidth() + 2) + 2, 600);
            layout->addAndMakeVisible(viewport_);

            layout->setSize(sidebar_.getWidth() + viewport_.getWidth(), 600);
            
            owner_->setContentOwned(layout, true);
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
        
        int getSidebarWidth()
        {
            return SIDEBAR_WIDTH;
        }
        
        MainWindow* const owner_;
        SidebarComponent sidebar_ { SMApp };
        Viewport viewport_;

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
            getContentComponent()->setBounds(0, 0, getWidth(), getHeight());
            pimpl_->sidebar_.setBounds(0, 0, pimpl_->getSidebarWidth(), getHeight());
            pimpl_->viewport_.setBounds(pimpl_->getSidebarWidth(), 0, getWidth() - pimpl_->getSidebarWidth(), getHeight());
        }
    };
    
    void MainWindow::closeButtonPressed() { pimpl_->closeButtonPressed(); }
    int MainWindow::getSizebarWidth()     { return pimpl_->getSidebarWidth(); }
}
