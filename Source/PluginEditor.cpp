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
#include "PluginEditor.h"

#include "MainLayoutComponent.h"
#include "MidiDeviceComponent.h"
#include "PluginProcessor.h"
#include "PluginSettings.h"
#include "SettingsManager.h"
#include "SidebarComponent.h"
#include "UwynLookAndFeel.h"

namespace showmidi
{
    struct ShowMIDIPluginAudioProcessorEditor::Pimpl : public MultiTimer, public KeyListener, public SettingsManager
    {
        static constexpr int DEFAULT_EDITOR_HEIGHT = 600;
        
        enum Timers
        {
            RenderDevices = 1,
            GrabKeyboardFocus
        };
        
        Pimpl(ShowMIDIPluginAudioProcessorEditor* owner, ShowMIDIPluginAudioProcessor& p) :
            owner_(owner),
            audioProcessor_(p),
            midiDevice_(*this, "ShowMIDI")
        {
            Desktop::getInstance().setDefaultLookAndFeel(&lookAndFeel_);
            
            owner_->setResizable(true, true);
            owner_->getConstrainer()->setMinimumWidth(SidebarComponent::SIDEBAR_WIDTH + MidiDeviceComponent::getStandardWidth());
            owner_->getConstrainer()->setMaximumWidth(SidebarComponent::SIDEBAR_WIDTH + MidiDeviceComponent::getStandardWidth());
            owner_->getConstrainer()->setMinimumHeight(120);
            
            sidebar_.setBounds(0, 0, SidebarComponent::SIDEBAR_WIDTH, DEFAULT_EDITOR_HEIGHT);
            layout_.addAndMakeVisible(sidebar_);

            midiDevice_.setBounds(0, 0, MidiDeviceComponent::getStandardWidth(), DEFAULT_EDITOR_HEIGHT);

            viewport_.setScrollOnDragMode(Viewport::ScrollOnDragMode::all);
            viewport_.setScrollBarsShown(true, false);
            viewport_.setScrollBarThickness(4);
            viewport_.setViewedComponent(&midiDevice_, false);
            viewport_.setBounds(sidebar_.getWidth(), 0, MidiDeviceComponent::getStandardWidth(), DEFAULT_EDITOR_HEIGHT);
            layout_.addAndMakeVisible(viewport_);

            layout_.setSize(sidebar_.getWidth() + viewport_.getWidth(), DEFAULT_EDITOR_HEIGHT);

            owner_->addAndMakeVisible(layout_);

            owner_->setSize(layout_.getWidth(), DEFAULT_EDITOR_HEIGHT);
            owner_->setWantsKeyboardFocus(true);
            owner_->addKeyListener(this);
            
            // 30Hz
            startTimer(RenderDevices, 1000 / 30);
            
            startTimer(GrabKeyboardFocus, 100);
        }
        
        ~Pimpl()
        {
            owner_->removeKeyListener(this);
            
            stopTimer(RenderDevices);
        }

        void handleIncomingMidiMessage(const MidiMessage& msg)
        {
            midiDevice_.handleIncomingMidiMessage(msg);
        }
        
        bool keyPressed(const KeyPress& key, Component*) override
        {
            if (key.getKeyCode() == KeyPress::spaceKey)
            {
                setPaused(!paused_);
                return true;
            }
            
            return false;
        }
        
        void setPaused(bool paused)
        {
            paused_ = paused;
            
            midiDevice_.setPaused(paused);
        }
        
        void timerCallback(int timerID) override
        {
            switch (timerID)
            {
                case RenderDevices:
                {
                    renderDevices();
                    break;
                }
                    
                case GrabKeyboardFocus:
                {
                    if (owner_->isVisible())
                    {
                        owner_->grabKeyboardFocus();
                        stopTimer(GrabKeyboardFocus);
                    }
                    break;
                }
            }
        }
        
        void renderDevices()
        {
            auto height = owner_->getParentHeight();
            midiDevice_.render();
            height = std::max(height, midiDevice_.getVisibleHeight());
            midiDevice_.setSize(MidiDeviceComponent::getStandardWidth(), height);
        }
        
        void paint(Graphics& g)
        {
            g.fillAll(audioProcessor_.getSettings().getTheme().colorSidebar);
        }
        
        void resized(int height)
        {
            layout_.setBounds(0, 0, layout_.getWidth(), height);
            sidebar_.setBounds(0, 0, sidebar_.getWidth(), height);
            viewport_.setBounds(sidebar_.getWidth(), 0, layout_.getWidth() - sidebar_.getWidth(), height);
        }
        
        Settings& getSettings() override
        {
            return audioProcessor_.getSettings();
        }
        
        void storeSettings() override
        {
            audioProcessor_.getSettings().storeTheme();
            owner_->repaint();
        }

        UwynLookAndFeel lookAndFeel_;
        
        ShowMIDIPluginAudioProcessorEditor* const owner_;
        ShowMIDIPluginAudioProcessor& audioProcessor_;
        
        MainLayoutComponent layout_ { *this };
        SidebarComponent sidebar_ { *this };
        Viewport viewport_;
        MidiDeviceComponent midiDevice_;
        
        bool paused_ { false };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pimpl)
    };
    
    ShowMIDIPluginAudioProcessorEditor::ShowMIDIPluginAudioProcessorEditor(ShowMIDIPluginAudioProcessor& p) : AudioProcessorEditor(&p), pimpl_(new Pimpl(this, p)) {
    }

    ShowMIDIPluginAudioProcessorEditor::~ShowMIDIPluginAudioProcessorEditor() = default;
    
    void ShowMIDIPluginAudioProcessorEditor::handleIncomingMidiMessage(const MidiMessage& m)   { pimpl_->handleIncomingMidiMessage(m); };

    void ShowMIDIPluginAudioProcessorEditor::paint(Graphics& g)
    {
        if (pimpl_.get())
        {
            pimpl_->paint(g);
        }
    }
    
    void ShowMIDIPluginAudioProcessorEditor::resized()
    {
        if (pimpl_.get() && isVisible() && getHeight() > 0)
        {
            pimpl_->resized(getHeight());
        }
    }
}
