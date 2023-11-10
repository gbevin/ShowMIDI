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

#include "MidiDeviceComponent.h"
#include "PluginProcessor.h"
#include "UwynLookAndFeel.h"

namespace showmidi
{
    struct ShowMIDIPluginAudioProcessorEditor::Pimpl : public MultiTimer, public KeyListener
    {
        static constexpr int DEFAULT_EDITOR_HEIGHT = 600;
        static constexpr int MIDI_DEVICE_SPACING = 4;
        
        enum Timers
        {
            RenderDevices = 1,
            GrabKeyboardFocus
        };
        
        Pimpl(ShowMIDIPluginAudioProcessorEditor* owner, ShowMIDIPluginAudioProcessor& p) : owner_(owner), audioProcessor_(p), midiDevice_("ShowMIDI")
        {
            juce::Desktop::getInstance().setDefaultLookAndFeel(&lookAndFeel_);
            
#if JUCE_MAC
            owner_->setResizable(true, true);
            owner_->getConstrainer()->setMinimumWidth(MidiDeviceComponent::getStandardWidth());
            owner_->getConstrainer()->setMaximumWidth(MidiDeviceComponent::getStandardWidth());
            owner_->getConstrainer()->setMinimumHeight(120);
#endif
            
            midiDevice_.setBounds(0, 0, MidiDeviceComponent::getStandardWidth(), DEFAULT_EDITOR_HEIGHT);

            viewPort_.setScrollOnDragMode(Viewport::ScrollOnDragMode::all);
            viewPort_.setScrollBarsShown(true, false);
            viewPort_.setScrollBarThickness(4);
            viewPort_.setViewedComponent(&midiDevice_, false);
            viewPort_.setSize(MidiDeviceComponent::getStandardWidth(), DEFAULT_EDITOR_HEIGHT);
            
            owner_->addAndMakeVisible(viewPort_);

            owner_->setSize(MidiDeviceComponent::getStandardWidth(), DEFAULT_EDITOR_HEIGHT);
            owner_->setWantsKeyboardFocus(true);
            owner_->addKeyListener(this);
            
            // 30Hz
            startTimer(RenderDevices, 1000 / 30);
            
            startTimer(GrabKeyboardFocus, 100);
            
            initialized_ = true;
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
            midiDevice_.render();
            midiDevice_.setSize(MidiDeviceComponent::getStandardWidth(), midiDevice_.getVisibleHeight());
        }
        
        void paint(juce::Graphics& g)
        {
            g.fillAll(lookAndFeel_.findColour(juce::ResizableWindow::backgroundColourId));
        }
        
        void resized()
        {
#if JUCE_MAC
            if (owner_ != nullptr && initialized_ && owner_->getWidth() > 0 && owner_->getHeight() > 0)
            {
                viewPort_.setBounds(0, 0, owner_->getWidth(), owner_->getHeight());
            }
#endif
        }
        
        UwynLookAndFeel lookAndFeel_;
        bool initialized_ { false };
        ShowMIDIPluginAudioProcessorEditor* const owner_;
        ShowMIDIPluginAudioProcessor& audioProcessor_;
        Viewport viewPort_;
        MidiDeviceComponent midiDevice_;
        bool paused_ { false };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pimpl)
    };
    
    ShowMIDIPluginAudioProcessorEditor::ShowMIDIPluginAudioProcessorEditor(ShowMIDIPluginAudioProcessor& p) : AudioProcessorEditor(&p), pimpl_(new Pimpl(this, p)) {}
    ShowMIDIPluginAudioProcessorEditor::~ShowMIDIPluginAudioProcessorEditor() = default;
    
    void ShowMIDIPluginAudioProcessorEditor::handleIncomingMidiMessage(const MidiMessage& m)   { pimpl_->handleIncomingMidiMessage(m); };

    void ShowMIDIPluginAudioProcessorEditor::paint(juce::Graphics& g)   { if (pimpl_.get()) pimpl_->paint(g); }
    void ShowMIDIPluginAudioProcessorEditor::resized()                  { if (pimpl_.get()) pimpl_->resized(); }
}
