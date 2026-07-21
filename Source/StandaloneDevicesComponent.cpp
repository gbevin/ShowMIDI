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
                if (shownDevices_.isEmpty())
                {
                    g.setColour(theme.colorBackground);
                    g.fillRect(Rectangle<int>(Theme::MIDI_DEVICE_SPACING, 0, MidiDeviceComponent::getStandardWidth(), owner_->getHeight()));
                    
                    g.setFont(theme.fontLabel());
                    g.setColour(theme.colorData);
                    
                    String message("No MIDI devices are visible.\n\n"
                                   "Either no devices are connected, or all connected devices are hidden in the settings.");
                    if (SMApp.getSettings().isAutoHideInactiveDevices() && midiDevices_.size() > 0)
                    {
                        message = "No MIDI devices are active.\n\n"
                                  "Devices without recent MIDI activity are hidden automatically, they reappear as soon as MIDI arrives.";
                    }
                    g.drawMultiLineText(message, Theme::MIDI_DEVICE_SPACING + 23, 24, MidiDeviceComponent::getStandardWidth() - 46);
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
            deviceListeners_.broadcastPauseChange(paused_);
        }
        
        void resetChannelData()
        {
            ScopedLock g(midiDevicesLock_);
            for (HashMap<const String, MidiDeviceComponent*>::Iterator i(midiDevices_); i.next();)
            {
                i.getValue()->resetChannelData();
            }
        }
        
        DeviceListeners& getDeviceListeners()
        {
            return deviceListeners_;
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

            // activity can change which devices are shown, so the layout is
            // reconciled whenever that set changes
            if (computeShownDevices() != shownDevices_)
            {
                layoutDevices();
            }

            auto height = owner_->getParentHeight();
            for (auto&& identifier : shownDevices_)
            {
                auto c = midiDevices_[identifier];
                if (c == nullptr) continue;
                c->render();
                height = std::max(height, c->getVisibleHeight());
            }
            
            auto width = std::max(owner_->getParentWidth(), shownDevices_.size() * (MidiDeviceComponent::getStandardWidth() + Theme::MIDI_DEVICE_SPACING) - Theme::MIDI_DEVICE_SPACING);
            owner_->setSize(width, height);

            for (auto&& identifier : shownDevices_)
            {
                auto c = midiDevices_[identifier];
                if (c != nullptr)
                {
                    c->setSize(MidiDeviceComponent::getStandardWidth(), height);
                }
            }
        }
        
        // a device column is shown when the user hasn't hidden it, and, if
        // idle devices hide automatically, when it's pinned or had activity
        // recently (twice the message timeout, so a column lingers a bit
        // after its last message faded)
        bool isDeviceShown(const String& identifier, MidiDeviceComponent* component)
        {
            auto& settings = SMApp.getSettings();
            if (!settings.isMidiDeviceVisible(identifier))
            {
                return false;
            }
            if (!settings.isAutoHideInactiveDevices() || settings.isMidiDevicePinned(identifier))
            {
                return true;
            }
            auto timeout = settings.getTimeoutDelay();
            if (timeout == 0)
            {
                return true;
            }
            return (Time::getCurrentTime() - component->getLastActivityTime()).inSeconds() <= timeout * 2;
        }
        
        StringArray computeShownDevices()
        {
            StringArray shown;
            MidiDeviceInfoComparator comparator;
            auto devices = knownDevices_;
            devices.sort(comparator);
            for (auto&& info : devices)
            {
                auto component = midiDevices_[info.identifier];
                if (component != nullptr && isDeviceShown(info.identifier, component))
                {
                    shown.add(info.identifier);
                }
            }
            return shown;
        }
        
        void layoutDevices()
        {
            shownDevices_ = computeShownDevices();
            
            owner_->removeAllChildren();
            int position = 0;
            for (auto&& identifier : shownDevices_)
            {
                auto component = midiDevices_[identifier];
                if (component == nullptr) continue;
                component->setBounds(Theme::MIDI_DEVICE_SPACING + position++ * (MidiDeviceComponent::getStandardWidth() + Theme::MIDI_DEVICE_SPACING), 0,
                                     component->getStandardWidth(), owner_->getParentHeight());
                owner_->addAndMakeVisible(component);
            }
            
            owner_->repaint();
            updateWindowSize();
        }
        
        void refreshMidiDevices() override
        {
            ScopedLock g(midiDevicesLock_);
            
            auto devices = MidiInput::getAvailableDevices();
            
            MidiDeviceInfoComparator comparator;
            devices.sort(comparator);
            
            // every connected device gets a component, hidden ones included:
            // they keep listening so the sidebar activity indicators work and
            // auto-hidden devices can reappear on activity; visibility only
            // decides which components are laid out
            
            // remove the devices that disappeared from the system
            Array<String> devices_to_remove;
            for (HashMap<const String, MidiDeviceComponent*>::Iterator it(midiDevices_); it.next();)
            {
                auto identifier = it.getKey();
                bool found = false;
                for (int i = 0; i < devices.size(); ++i)
                {
                    if (devices[i].identifier == identifier)
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
            
            for (int i = 0; i < devices_to_remove.size(); ++i)
            {
                auto identifier = devices_to_remove[i];
                auto component = midiDevices_[identifier];
                owner_->removeChildComponent(component);
                delete component;
                
                midiDevices_.remove(identifier);
            }
            
            // create components for the devices that appeared
            for (int i = 0; i < devices.size(); ++i)
            {
                auto info = devices[i];
                if (!midiDevices_.contains(info.identifier))
                {
                    auto component = new MidiDeviceComponent(&SMApp, info);
                    component->setPaused(paused_);
                    midiDevices_.set(info.identifier, component);
                }
            }
            
            knownDevices_ = devices;
            
            layoutDevices();
        }
        
        void updateWindowSize()
        {
            MessageManager::callAsync([this] () {
                // resize the window in order to display the MIDI devices
                auto devices_width = (MidiDeviceComponent::getStandardWidth() + Theme::MIDI_DEVICE_SPACING) * std::max(MIN_MIDI_DEVICES_AUTO_SHOWN, std::min(MAX_MIDI_DEVICES_AUTO_SHOWN, shownDevices_.size())) + Theme::MIDI_DEVICE_SPACING;
                SMApp.setWindowWidthForMainLayout(devices_width + Theme::SCROLLBAR_THICKNESS);
            });
        }

        StandaloneDevicesComponent* const owner_;
        
        Time getLastMidiActivity()
        {
            ScopedLock g(midiDevicesLock_);
            Time last;
            for (HashMap<const String, MidiDeviceComponent*>::Iterator i(midiDevices_); i.next();)
            {
                if (i.getValue() != nullptr)
                {
                    last = std::max(last, i.getValue()->getLastActivityTime());
                }
            }
            return last;
        }
        
        Time getLastMidiActivity(const String& identifier)
        {
            ScopedLock g(midiDevicesLock_);
            // plain lookup: getReference would insert a null entry for a
            // device that has no component yet
            auto component = midiDevices_[identifier];
            return component != nullptr ? component->getLastActivityTime() : Time();
        }

        HashMap<const String, MidiDeviceComponent*> midiDevices_;
        Array<MidiDeviceInfo> knownDevices_;
        StringArray shownDevices_;
        CriticalSection midiDevicesLock_;
        
        bool paused_ { false };
        DeviceListeners deviceListeners_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pimpl)
    };
    
    StandaloneDevicesComponent::StandaloneDevicesComponent() : pimpl_(new Pimpl(this)) {}
    StandaloneDevicesComponent::~StandaloneDevicesComponent() = default;
    
    void StandaloneDevicesComponent::paint(Graphics& g)                 { pimpl_->paint(g); }
    bool StandaloneDevicesComponent::isPaused()                         { return pimpl_->isPaused(); }
    void StandaloneDevicesComponent::togglePaused()                     { pimpl_->togglePaused(); }
    DeviceListeners& StandaloneDevicesComponent::getDeviceListeners()   { return pimpl_->getDeviceListeners(); }
    void StandaloneDevicesComponent::resetChannelData()                 { pimpl_->resetChannelData(); }
    Time StandaloneDevicesComponent::getLastMidiActivity()              { return pimpl_->getLastMidiActivity(); }
    Time StandaloneDevicesComponent::getLastMidiActivity(const String& deviceIdentifier) { return pimpl_->getLastMidiActivity(deviceIdentifier); }
}
