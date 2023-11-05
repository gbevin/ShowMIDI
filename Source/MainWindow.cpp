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

using namespace showmidi;

MainWindow::MainWindow (juce::String name)
    : DocumentWindow(name,
                      juce::Desktop::getInstance().getDefaultLookAndFeel()
                        .findColour (juce::ResizableWindow::backgroundColourId),
                      DocumentWindow::allButtons)
{
    setUsingNativeTitleBar(true);
    
    auto viewport = new Viewport();
    
    viewport->setScrollOnDragMode(Viewport::ScrollOnDragMode::all);
    viewport->setScrollBarsShown(true, true);
    viewport->setScrollBarThickness(4);
    viewport->setViewedComponent(new MainComponent(), true);
    viewport->setSize((MidiDeviceComponent::getStandardWidth() + 2) + 2, 600);
    
    setContentOwned(viewport, true);
    
#if JUCE_IOS || JUCE_ANDROID
    setFullScreen(true);
#else
    setResizable(true, true);
    centreWithSize(getWidth(), getHeight());
#endif
    
    setVisible(true);
}

void MainWindow::closeButtonPressed()
{
    JUCEApplication::getInstance()->systemRequestedQuit();
}
