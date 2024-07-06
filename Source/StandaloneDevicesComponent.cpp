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
#include "StandaloneDevicesComponent.h"

#include "MidiDeviceComponent.h"
#include "MidiDeviceInfoComparator.h"
#include "MidiDevicesListener.h"
#include "ShowMidiApplication.h"

namespace showmidi
{
    struct StandaloneDevicesComponent::Pimpl : public MultiTimer, public MidiDevicesListener
    {
        static constexpr int MIN_MIDI_DEVICES_AUTO_SHOWN = 1;
        static constexpr int MAX_MIDI_DEVICES_AUTO_SHOWN = 6;
        
        enum Timers
        {
            RenderDevices = 1,
            GrabKeyboardFocus
        };
        
        Pimpl(StandaloneDevicesComponent* owner) : owner_(owner)
        {
            SMApp.getMidiDevicesListeners().add(this);
            
            refreshMidiDevices();
            
            // 30Hz
            startTimer(RenderDevices, 1000 / 30);
            
            startTimer(GrabKeyboardFocus, 100);
#if SHOW_TEST_DATA
            togglePaused();
#endif
        }
        
        ~Pimpl()
        {
            SMApp.getMidiDevicesListeners().remove(this);
            
            stopTimer(RenderDevices);
            
            {
                ScopedLock g(midiDevicesLock_);
                for (HashMap<const String, MidiDeviceComponent*>::Iterator i(midiDevices_); i.next();)
                {
                    owner_->removeChildComponent(i.getValue());
                    delete i.getValue();
                }
            }
        }
        
        void paint(Graphics& g)
        {
            auto& theme = SMApp.getSettings().getTheme();
            g.fillAll(theme.colorSidebar);
            
            {
                ScopedLock guard(midiDevicesLock_);
                if (midiDevices_.size() == 0)
                {
                    g.setColour(theme.colorBackground);
                    g.fillRect(Rectangle<int>(Theme::MIDI_DEVICE_SPACING, 0, MidiDeviceComponent::getStandardWidth(), owner_->getHeight()));
                    
                    g.setFont(theme.fontLabel());
                    g.setColour(theme.colorData);
                    
                    g.drawMultiLineText("No MIDI devices are visible.\n\n"
                                        "Either no devices are connected, or all connected devices are hidden in the settings.", Theme::MIDI_DEVICE_SPACING + 23, 24, MidiDeviceComponent::getStandardWidth() - 46);
                }
            }
        }
        
        bool isPaused()
        {
            return paused_;
        }
        
        void togglePaused()
        {
            setPaused(!paused_);
            pauseListeners_.broadcast(paused_);
        }
        
        void resetChannelData()
        {
            ScopedLock g(midiDevicesLock_);
            for (HashMap<const String, MidiDeviceComponent*>::Iterator i(midiDevices_); i.next();)
            {
                i.getValue()->resetChannelData();
            }
        }
        
        PauseListeners& getPauseListeners()
        {
            return pauseListeners_;
        }

        void setPaused(bool paused)
        {
            SMApp.setWindowTitle(String("ShowMIDI") + (paused ? " (paused)" : ""));
            paused_ = paused;
            
            ScopedLock g(midiDevicesLock_);
            for (HashMap<const String, MidiDeviceComponent*>::Iterator i(midiDevices_); i.next();)
            {
                i.getValue()->setPaused(paused);
            }
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
            ScopedLock g(midiDevicesLock_);

            auto height = owner_->getParentHeight();
            for (HashMap<const String, MidiDeviceComponent*>::Iterator i(midiDevices_); i.next();)
            {
                auto c = i.getValue();
                c->render();
                height = std::max(height, c->getVisibleHeight());
            }
            
            auto width = std::max(owner_->getParentWidth(), midiDevices_.size() * (MidiDeviceComponent::getStandardWidth() + Theme::MIDI_DEVICE_SPACING) - Theme::MIDI_DEVICE_SPACING);
            owner_->setSize(width, height);

            for (HashMap<const String, MidiDeviceComponent*>::Iterator i(midiDevices_); i.next();)
            {
                i.getValue()->setSize(MidiDeviceComponent::getStandardWidth(), height);
            }
        }
        
        void refreshMidiDevices() override
        {
            auto& settings = SMApp.getSettings();

            ScopedLock g(midiDevicesLock_);
            
            auto devices = MidiInput::getAvailableDevices();
            
            MidiDeviceInfoComparator comparator;
            devices.sort(comparator);
            
            // detect the previous devices that have now disappeared
            Array<String> devices_to_remove;
            for (HashMap<const String, MidiDeviceComponent*>::Iterator it(midiDevices_); it.next();)
            {
                auto identifier = it.getKey();
                bool found = false;
                for (int i = 0; i < devices.size(); ++i)
                {
                    if (devices[i].identifier == identifier && settings.isMidiDeviceVisible(identifier))
                    {
                        found = true;
                        break;
                    }
                }
                
                if (!found)
                {
                    devices_to_remove.add(identifier);
                }
            }
            
            // check if all the devices are already present
            bool new_devices_preset = false;
            for (int i = 0; i < devices.size(); ++i)
            {
                auto identifier = devices[i].identifier;
                if (!midiDevices_.contains(identifier) && settings.isMidiDeviceVisible(identifier))
                {
                    new_devices_preset = true;
                    break;
                }
            }
            
            if (devices_to_remove.size() > 0 || new_devices_preset)
            {
                owner_->removeAllChildren();
                
                // remove the devices that disappeared
                for (int i = 0; i < devices_to_remove.size(); ++i)
                {
                    auto identifier = devices_to_remove[i];
                    auto component = midiDevices_.getReference(identifier);
                    delete component;
                    
                    midiDevices_.remove(identifier);
                }
                
                // create the new devices and reuse the existing ones
                // lay them out in alpabetical order
                int position = 0;
                for (int i = 0; i < devices.size(); ++i)
                {
                    auto info = devices[i];
                    if (!devices_to_remove.contains(info.identifier) && settings.isMidiDeviceVisible(info.identifier))
                    {
                        MidiDeviceComponent* component = midiDevices_.getReference(info.identifier);
                        if (component == nullptr)
                        {
                            component = new MidiDeviceComponent(&SMApp, info);
                            component->setPaused(paused_);
                            midiDevices_.set(info.identifier, component);
                        }
                        
                        component->setBounds(Theme::MIDI_DEVICE_SPACING + position++ * (MidiDeviceComponent::getStandardWidth() + Theme::MIDI_DEVICE_SPACING), 0,
                                             component->getStandardWidth(), owner_->getParentHeight());
                        
                        owner_->addAndMakeVisible(component);
                    }
                }
                
                updateWindowSize();
            }
        }
        
        void updateWindowSize()
        {
            MessageManager::callAsync([this] () {
                // resize the window in order to display the MIDI devices
                auto devices_width = (MidiDeviceComponent::getStandardWidth() + Theme::MIDI_DEVICE_SPACING) * std::max(MIN_MIDI_DEVICES_AUTO_SHOWN, std::min(MAX_MIDI_DEVICES_AUTO_SHOWN, midiDevices_.size())) + Theme::MIDI_DEVICE_SPACING;
                SMApp.setWindowWidthForMainLayout(devices_width + Theme::SCROLLBAR_THICKNESS);
            });
        }

        StandaloneDevicesComponent* const owner_;
        
        HashMap<const String, MidiDeviceComponent*> midiDevices_;
        CriticalSection midiDevicesLock_;
        
        bool paused_ { false };
        PauseListeners pauseListeners_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pimpl)
    };
    
    StandaloneDevicesComponent::StandaloneDevicesComponent() : pimpl_(new Pimpl(this)) {}
    StandaloneDevicesComponent::~StandaloneDevicesComponent() = default;
    
    void StandaloneDevicesComponent::paint(Graphics& g)             { pimpl_->paint(g); }
    bool StandaloneDevicesComponent::isPaused()                     { return pimpl_->isPaused(); }
    void StandaloneDevicesComponent::togglePaused()                 { pimpl_->togglePaused(); }
    PauseListeners& StandaloneDevicesComponent::getPauseListeners() { return pimpl_->getPauseListeners(); }
    void StandaloneDevicesComponent::resetChannelData()             { pimpl_->resetChannelData(); }
}
