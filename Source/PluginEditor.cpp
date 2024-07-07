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
    struct ShowMIDIPluginAudioProcessorEditor::Pimpl : public MultiTimer, public SettingsManager, public DeviceManager
    {
        static constexpr int DEFAULT_EDITOR_HEIGHT = 600;
        
        enum Timers
        {
            RenderDevices = 1,
            GrabKeyboardFocus
        };
        
        Pimpl(ShowMIDIPluginAudioProcessorEditor* owner, ShowMIDIPluginAudioProcessor* p) :
            owner_(owner),
            audioProcessor_(p)
        {
            Desktop::getInstance().setDefaultLookAndFeel(&lookAndFeel_);
            
            midiDevice_ = std::make_unique<MidiDeviceComponent>(this, "ShowMIDI");
            layout_ = std::make_unique<MainLayoutComponent>(this, this, MainLayoutType::layoutPlugin, midiDevice_.get());
            
            owner_->setResizable(true, true);
            owner_->getConstrainer()->setMinimumWidth(layout_->getSidebarWidth() + MidiDeviceComponent::getStandardWidth() + Theme::SCROLLBAR_THICKNESS);
            owner_->getConstrainer()->setMaximumWidth(owner_->getConstrainer()->getMaximumWidth());
            owner_->getConstrainer()->setMinimumHeight(120);
            
            midiDevice_->setBounds(0, 0, MidiDeviceComponent::getStandardWidth(), DEFAULT_EDITOR_HEIGHT);

            owner_->addAndMakeVisible(layout_.get());

            owner_->setSize(layout_->getWidth(), DEFAULT_EDITOR_HEIGHT);
            owner_->setWantsKeyboardFocus(true);
            
            // 30Hz
            startTimer(RenderDevices, 1000 / 30);
            
            startTimer(GrabKeyboardFocus, 100);
#if SHOW_TEST_DATA
            togglePaused();
#endif
        }
        
        ~Pimpl()
        {
            stopTimer(RenderDevices);
        }

        void handleIncomingMidiMessage(const MidiMessage& msg)
        {
            midiDevice_->handleIncomingMidiMessage(msg);
        }
        
        bool isPaused() override
        {
            return paused_;
        }
        
        void togglePaused() override
        {
            setPaused(!paused_);
            deviceListeners_.broadcastPauseChange(paused_);
        }
        
        void resetChannelData() override
        {
            midiDevice_->resetChannelData();
        }
        
        DeviceListeners& getDeviceListeners() override
        {
            return deviceListeners_;
        }

        void setPaused(bool paused)
        {
            paused_ = paused;
            
            midiDevice_->setPaused(paused);
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
            int height;
            if (owner_->getParentComponent())
            {
                height = owner_->getParentHeight();
            }
            else
            {
                height = owner_->getHeight();
            }
            midiDevice_->render();
            height = std::max(height, midiDevice_->getVisibleHeight());
            midiDevice_->setSize(MidiDeviceComponent::getStandardWidth(), height);
        }
        
        void paint(Graphics& g)
        {
            g.fillAll(audioProcessor_->getSettings().getTheme().colorSidebar);
        }
        
        void resized(int height)
        {
            layout_->setBounds(0, 0, layout_->getWidth(), height);
        }
        
        bool isPlugin() override
        {
            return true;
        }
        
        Component* getTopLevelComponent() override
        {
#if JUCE_IOS
            return owner_;
#else
            return nullptr;
#endif
        }

        Settings& getSettings() override
        {
            return audioProcessor_->getSettings();
        }
        
        void applySettings() override
        {
            owner_->repaint();
        }

        void storeSettings() override
        {
            audioProcessor_->getSettings().storeTheme();
            owner_->repaint();
        }
        
        MidiDevicesListeners& getMidiDevicesListeners() override
        {
            return midiDevicesListeners_;
        }

        UwynLookAndFeel lookAndFeel_;
        
        ShowMIDIPluginAudioProcessorEditor* const owner_;
        ShowMIDIPluginAudioProcessor* const audioProcessor_;
        
        std::unique_ptr<MidiDeviceComponent> midiDevice_;
        std::unique_ptr<MainLayoutComponent> layout_;
        
        MidiDevicesListeners midiDevicesListeners_;

        bool paused_ { false };
        DeviceListeners deviceListeners_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pimpl)
    };
    
    ShowMIDIPluginAudioProcessorEditor::ShowMIDIPluginAudioProcessorEditor(ShowMIDIPluginAudioProcessor* p) : AudioProcessorEditor(p), pimpl_(new Pimpl(this, p)) {
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
