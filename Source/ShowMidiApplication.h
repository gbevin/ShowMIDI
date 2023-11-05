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

#include "UwynLookAndFeel.h"

#define SMApp ShowMidiApplication::getInstance()

extern ApplicationCommandManager* commandManager;

namespace showmidi
{
    class MainWindow;
    
    class ShowMidiApplication : public juce::JUCEApplication, public juce::ApplicationCommandManagerListener
    {
    public:
        ShowMidiApplication();
        
        inline static ShowMidiApplication& getInstance()
        {
            ShowMidiApplication* const app = dynamic_cast<ShowMidiApplication*>(JUCEApplication::getInstance());
            jassert (app != nullptr);
            return *app;
        }
        
        const juce::String getApplicationName() override;
        const juce::String getApplicationVersion() override;
        bool moreThanOneInstanceAllowed() override;
        UwynLookAndFeel& getLookAndFeel();
        
        void initialise(const juce::String& commandLine) override;
        void shutdown() override;
        void systemRequestedQuit() override;
        void anotherInstanceStarted(const juce::String& commandLine) override;

        ApplicationCommandTarget* getNextCommandTarget() override;
        void getAllCommands(Array <CommandID> &) override;
        void getCommandInfo(CommandID, ApplicationCommandInfo &) override;
        bool perform(const InvocationInfo &) override;
        void applicationCommandInvoked(const ApplicationCommandTarget::InvocationInfo &) override;
        void applicationCommandListChanged() override;

        void setWindowTitle(const String&);
        void setWindowSize(int, int);
        int getWindowHeight();

    private:
        UwynLookAndFeel lookAndFeel_;
        std::unique_ptr<MainWindow> mainWindow_;
    };
}
