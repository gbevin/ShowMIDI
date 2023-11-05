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
#include "ShowMidiApplication.h"

#include "CommandIDs.h"
#include "MainWindow.h"

using namespace showmidi;

ShowMidiApplication::ShowMidiApplication()
{
    juce::Desktop::getInstance().setDefaultLookAndFeel(&lookAndFeel_);
}

const juce::String ShowMidiApplication::getApplicationName()
{
    return ProjectInfo::projectName;
}

const juce::String ShowMidiApplication::getApplicationVersion()
{
    return ProjectInfo::versionString;
}

bool ShowMidiApplication::moreThanOneInstanceAllowed()
{
    return true;
}

UwynLookAndFeel& ShowMidiApplication::getLookAndFeel()
{
    return lookAndFeel_;
}

void ShowMidiApplication::initialise(const juce::String&)
{
    commandManager = new ApplicationCommandManager();
    commandManager->addListener(this);
    commandManager->registerAllCommandsForTarget(this);

    mainWindow_.reset(new MainWindow(getApplicationName()));
}

void ShowMidiApplication::shutdown()
{
    commandManager->removeListener(this);
    
    mainWindow_ = nullptr;
    
    deleteAndZero(commandManager);
}

void ShowMidiApplication::systemRequestedQuit()
{
    quit();
}

void ShowMidiApplication::anotherInstanceStarted(const juce::String&)
{
}

ApplicationCommandTarget* ShowMidiApplication::getNextCommandTarget()
{
    return nullptr;
}

void ShowMidiApplication::getAllCommands(Array <CommandID> &commands)
{
    const CommandID ids[] = {
        CommandIDs::version,
        CommandIDs::uwyn,
        CommandIDs::quit
    };
    commands.addArray(ids, numElementsInArray(ids));
}

void ShowMidiApplication::getCommandInfo(const CommandID commandID, ApplicationCommandInfo &result)
{
    const int cmd = ModifierKeys::commandModifier;
    
    switch (commandID)
    {
        case CommandIDs::version:
            result.setInfo(getApplicationName() + " v" + String(ProjectInfo::versionString),
                           "The version of the " + getApplicationName() + ".",
                           CommandCategories::help, 0);
            result.setActive(true);
            break;
            
        case CommandIDs::uwyn:
            result.setInfo("About Uwyn",
                           "About Uwyn",
                           CommandCategories::help, 0);
            result.setActive(true);
            break;
            
        case CommandIDs::quit:
            result.setInfo("Exit",
                           "Exit " + getApplicationName() + ".",
                           CommandCategories::help, 0);
            result.setActive(true);
            result.defaultKeypresses.add(KeyPress('q', cmd, 0));
            break;
            
        default:
            break;
    }
}

bool ShowMidiApplication::perform(const InvocationInfo &info)
{
    switch (info.commandID)
    {
        case CommandIDs::version:
        {
            URL("https://github.com/gbevin/ShowMIDI").launchInDefaultBrowser();
            break;
        }
            
        case CommandIDs::uwyn:
        {
            URL("https://uwyn.com").launchInDefaultBrowser();
            break;
        }
            
        case CommandIDs::quit:
        {
            this->systemRequestedQuit();
            break;
        }
        
        default:
            return false;
    }
    
    return true;
}

void ShowMidiApplication::applicationCommandInvoked(const ApplicationCommandTarget::InvocationInfo &)
{
}

void ShowMidiApplication::applicationCommandListChanged()
{
}

void ShowMidiApplication::setWindowTitle(const String& title)
{
    if (mainWindow_ != nullptr)
    {
        mainWindow_->setName(title);
    }
}

void ShowMidiApplication::setWindowSize(int width, int height)
{
    if (mainWindow_ != nullptr)
    {
#if JUCE_IOS
        mainWindow_->setBounds(0, 0, width, height);
#else
        mainWindow_->setSize(width, height);
#endif
    }
}

int ShowMidiApplication::getWindowHeight()
{
    if (mainWindow_ != nullptr)
    {
        return mainWindow_->getHeight();
    }
    
    return 0;
}
