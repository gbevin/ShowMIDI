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
#include "MidiDeviceComponent.h"

#include "ChannelState.h"

namespace showmidi
{
    struct MidiDeviceComponent::Pimpl : public MidiInputCallback
    {
        Pimpl(MidiDeviceComponent* owner, SettingsManager* manager, const String& name) :
            owner_(owner),
            settingsManager_(manager),
            theme_(manager->getSettings().getTheme()),
            deviceInfo_({ name, ""})
        {
        }
        
        Pimpl(MidiDeviceComponent* owner, SettingsManager* manager, const MidiDeviceInfo info) :
            owner_(owner),
            settingsManager_(manager),
            theme_(manager->getSettings().getTheme()),
            deviceInfo_(info)
        {
            auto midi_input = MidiInput::openDevice(info.identifier, this);
            if (midi_input != nullptr)
            {
                midi_input->start();
                midiIn_.swap(midi_input);
            }
#if SHOW_TEST_DATA
            showTestData();
#endif
        }
        
        void showTestData()
        {
            const auto t = Time::getCurrentTime();
            
            deviceInfo_ = {"MIDI Instrument Name", deviceInfo_.identifier};
            
            auto& sysex = channels_.sysex_;
            sysex.time_ = t;
            sysex.length_ = 0xc;
            uint8_t data[Sysex::MAX_SYSEX_DATA] = {
                0x1, 0x2, 0xa, 0x7f, 0x0, 0xfe, 0x0, 0xaa, 0x1, 0x7e,
                0x1, 0x2, 0xa, 0x7f, 0x0, 0xfe, 0x0, 0xaa, 0x1, 0x7e};
            memcpy(sysex.data_, data, Sysex::MAX_SYSEX_DATA);

            auto& clock = channels_.clock_;
            clock.bpm_ = 111.1;
            clock.timeBpm_ = t;
            clock.timeStart_ = t;
            clock.timeContinue_ = t;
            clock.timeStop_ = t;

            auto& channel1 = channels_.channel_[0];
            channel1.mpeManager_ = true;
            channel1.mpeMember_ = MpeMember::mpeLower;
            channel1.time_ = t;
            channel1.programChange_.value_ = 0;
            channel1.programChange_.time_ = t;
            channel1.pitchBend_.value_ = 9256;
            channel1.pitchBend_.time_ = t;
            channel1.notes_.time_ = t;
            channel1.notes_.noteOn_[61].value_ = 127;
            channel1.notes_.noteOn_[61].polyPressure_ = 0;
            channel1.notes_.noteOn_[61].time_ = t;
            channel1.notes_.noteOn_[79].value_ = 38;
            channel1.notes_.noteOn_[79].polyPressure_ = 120;
            channel1.notes_.noteOn_[79].polyPressureTime_ = t;
            channel1.notes_.noteOn_[79].time_ = t;
            channel1.channelPressure_.value_ = 76;
            channel1.channelPressure_.time_ = t;
            channel1.controlChanges_.time_ = t;
            channel1.controlChanges_.controlChange_[74].value_ = 127;
            channel1.controlChanges_.controlChange_[74].time_ = t;
            channel1.rpns_.time_ = t;
            channel1.rpns_.param_[0] = { t, 5000 };
            channel1.rpns_.param_[6] = { t, 10 };
            
            auto& channel16 = channels_.channel_[15];
            channel16.time_ = t;
            channel16.programChange_.value_ = 127;
            channel16.programChange_.time_ = t;
            channel16.pitchBend_.value_ = 0;
            channel16.pitchBend_.time_ = t;
            channel16.notes_.time_ = t;
            channel16.notes_.noteOn_[61].value_ = 127;
            channel16.notes_.noteOn_[61].polyPressure_ = 73;
            channel16.notes_.noteOn_[61].polyPressureTime_ = t;
            channel16.notes_.noteOn_[61].time_ = t;
            channel16.notes_.noteOn_[79].value_ = 127;
            channel16.notes_.noteOn_[79].polyPressure_ = 0;
            channel16.notes_.noteOn_[79].time_ = t;
            channel16.notes_.noteOff_[79].value_ = 127;
            channel16.notes_.noteOff_[79].time_ = t;
            channel16.channelPressure_.value_ = 76;
            channel16.channelPressure_.time_ = t;
            channel16.controlChanges_.time_ = t;
            channel16.controlChanges_.controlChange_[1].value_ = 124;
            channel16.controlChanges_.controlChange_[1].time_ = t;
            channel16.controlChanges_.controlChange_[45].value_ = 89;
            channel16.controlChanges_.controlChange_[45].time_ = t;
            channel16.controlChanges_.controlChange_[127].value_ = 100;
            channel16.controlChanges_.controlChange_[127].time_ = t;
        }
        
        ~Pimpl()
        {
            midiIn_ = nullptr;
        }
        
        void handleIncomingMidiMessage(MidiInput*, const MidiMessage& msg)
        {
            const auto t = Time::getCurrentTime();
            
            if (msg.isSysEx())
            {
                auto& sysex = channels_.sysex_;
                sysex.time_ = t;
                sysex.length_ = msg.getSysExDataSize();
                memset(sysex.data_, 0, Sysex::MAX_SYSEX_DATA);
                memcpy(sysex.data_, msg.getSysExData(), std::min(msg.getSysExDataSize(), Sysex::MAX_SYSEX_DATA));
                dirty_ = true;
                return;
            }
            
            if (msg.isMidiClock())
            {
                auto ts_secs = msg.getTimeStamp();
                
                // keep a queue of MIDI clock timestamps, never exceeding TIMESTAMP_QUEUE_SIZE
                midiTimeStamps_.push_front(ts_secs);
                while (midiTimeStamps_.size() > TIMESTAMP_QUEUE_SIZE)
                {
                    midiTimeStamps_.pop_back();
                }
                
                // calculate the average across all the queued timestamps
                // this will be used to filter out outliers
                auto avg_ts = 0.0;
                for (auto i = 0; i < midiTimeStamps_.size(); i++)
                {
                    avg_ts += midiTimeStamps_[i];
                }
                avg_ts /= midiTimeStamps_.size();
                
                // only keep timestamps that are within 1% deviation of the average
                std::vector<double> keep;
                for (auto i = 0; i < midiTimeStamps_.size(); i++)
                {
                    if (fabs(avg_ts - midiTimeStamps_[i]) < avg_ts * 0.01)
                    {
                        keep.push_back(midiTimeStamps_[i]);
                    }
                }
                
                // if we have at least four valid timestamps, calculate the bpm
                if (keep.size() > 4)
                {
                    auto sum = 0.0;
                    auto size = (int) keep.size() - 1;
                    for (auto i = 0; i < size; i++)
                    {
                        sum += fabs(keep[i] - keep[i + 1]);
                    }
                    sum /= size;
                    
                    auto bpm = int((600.0 / sum / 24.0) + 0.5) / 10.0;
                    bpm = std::min(std::max(bpm, BPM_MIN), BPM_MAX);
                    
                    if (keep.size() > TIMESTAMP_QUEUE_SIZE / 2)
                    {
                        auto& clock = channels_.clock_;
                        if ((t - clock.timeBpm_).inSeconds() > 0.5)
                        {
                            clock.timeBpm_ = t;
                            if (fabs(clock.bpm_ - bpm) >= 0.1)
                            {
                                clock.bpm_ = bpm;
                                dirty_ = true;
                            }
                        }
                    }
                }
                return;
            }
            else if (msg.isMidiStart())
            {
                channels_.clock_.timeStart_ = t;
                midiTimeStamps_.clear();
                return;
            }
            else if (msg.isMidiContinue())
            {
                channels_.clock_.timeContinue_ = t;
                midiTimeStamps_.clear();
                return;
            }
            else if (msg.isMidiStop())
            {
                channels_.clock_.timeStop_ = t;
                midiTimeStamps_.clear();
                return;
            }

            if (msg.getChannel() <= 0)
            {
                return;
            }
            
            ChannelMessage* channel_message = nullptr;
            
            auto& channel = channels_.channel_[msg.getChannel() - 1];
            if (msg.isNoteOn())
            {
                auto& notes = channel.notes_;
                notes.time_ = t;
                
                auto& note_off = notes.noteOff_[msg.getNoteNumber()];
                note_off.time_ = Time();

                auto& note_on = notes.noteOn_[msg.getNoteNumber()];
                note_on.value_ = msg.getVelocity();
                channel_message = &note_on;
            }
            else if (msg.isNoteOff())
            {
                auto& notes = channel.notes_;
                notes.time_ = t;
                
                auto& note_off = notes.noteOff_[msg.getNoteNumber()];
                note_off.value_ = msg.getVelocity();
                channel_message = &note_off;
            }
            else if (msg.isAftertouch())
            {
                auto& notes = channel.notes_;
                notes.time_ = t;
                
                auto& note_on = notes.noteOn_[msg.getNoteNumber()];
                note_on.polyPressure_ = msg.getAfterTouchValue();
                note_on.polyPressureTime_ = t;
                channel_message = &note_on;
            }
            else if (msg.isController())
            {
                auto& control_changes = channel.controlChanges_;
                control_changes.time_ = t;
                
                auto number = msg.getControllerNumber();
                auto value = msg.getControllerValue();
                channel_message = &control_changes.controlChange_[number];
                channel_message->value_ = value;
                
                switch (number)
                {
                    case 6:
                        // if an NRPN or RPN parameter was selected, start constituting the data
                        if ((channel.lastRpnMsb_ != 127 || channel.lastRpnLsb_ != 127) ||
                            (channel.lastNrpnMsb_ != 127 || channel.lastNrpnLsb_ != 127))
                        {
                            channel.lastDataMsb_ = value;
                        }
                        break;
                    case 38:
                        if (channel.lastRpnMsb_ != 127 || channel.lastRpnLsb_ != 127)
                        {
                            channel.lastDataLsb_ = value;
                            
                            int rpn_number = (channel.lastRpnMsb_ << 7) + channel.lastRpnLsb_;
                            int rpn_value = (channel.lastDataMsb_ << 7) + channel.lastDataLsb_;
                            auto& rpns = channel.rpns_;
                            rpns.time_ = t;
                            rpns.param_[rpn_number].time_ = t;
                            rpns.param_[rpn_number].value_ = rpn_value;
                            
                            // handle MPE activation message
                            if (rpn_number == 6 && channel.lastDataMsb_ <= 0xf)
                            {
                                channels_.handleMpeActivation(t, channel, channel.lastDataMsb_);
                            }
                        }
                        else if (channel.lastNrpnMsb_ != 127 || channel.lastNrpnLsb_ != 127)
                        {
                            channel.lastDataLsb_ = value;
                            
                            int nrpn_number = (channel.lastNrpnMsb_ << 7) + channel.lastNrpnLsb_;
                            int nrpn_value = (channel.lastDataMsb_ << 7) + channel.lastDataLsb_;
                            auto& nrpns = channel.nrpns_;
                            nrpns.time_ = t;
                            nrpns.param_[nrpn_number].time_ = t;
                            nrpns.param_[nrpn_number].value_ = nrpn_value;
                        }
                        break;
                    case 98:
                        channel.lastNrpnLsb_ = value;
                        break;
                    case 99:
                        channel.lastNrpnMsb_ = value;
                        break;
                    case 100:
                        channel.lastRpnLsb_ = value;
                        // resetting RPN numbers also resets NRPN numbers
                        if (channel.lastRpnLsb_ == 127 && channel.lastRpnMsb_ == 127)
                        {
                            channel.lastNrpnLsb_ = 127;
                            channel.lastNrpnMsb_ = 127;
                        }
                        break;
                    case 101:
                        channel.lastRpnMsb_ = value;
                        break;
                }
            }
            else if (msg.isProgramChange())
            {
                channel_message = &channel.programChange_;
                channel_message->value_ = msg.getProgramChangeNumber();
            }
            else if (msg.isChannelPressure())
            {
                channel_message = &channel.channelPressure_;
                channel_message->value_ = msg.getChannelPressureValue();
            }
            else if (msg.isPitchWheel())
            {
                channel_message = &channel.pitchBend_;
                channel_message->value_ = msg.getPitchWheelValue();
            }
            
            if (channel_message != nullptr)
            {
                channel_message->time_ = t;
                channel.time_ = t;
                dirty_ = true;
            }
        }
        
        void render()
        {
            const auto t = Time::getCurrentTime();
            
            bool expected = true;
            if (dirty_.compare_exchange_strong(expected, false) || (t - lastRender_).inMilliseconds() > 200)
            {
                lastRender_ = t;
                owner_->repaint();
            }
        }
        
        static constexpr int TIMESTAMP_QUEUE_SIZE = 48;
        static constexpr double BPM_MIN = 20.0;
        static constexpr double BPM_MAX = 360.0;

        static constexpr int STANDARD_WIDTH = 254;
        static constexpr int X_MID = 151;
        
        static constexpr int X_PORT = 48;
        static constexpr int Y_PORT = 12;
        
        static constexpr int X_CLOCK = 23;
        static constexpr int Y_CLOCK = 12;
        static constexpr int X_CLOCK_BPM = 24;
        static constexpr int Y_CLOCK_PADDING = 8;

        static constexpr int X_SYSEX = 23;
        static constexpr int Y_SYSEX = 12;
        static constexpr int X_SYSEX_LENGTH = 24;
        static constexpr int Y_SYSEX_PADDING = 8;
        static constexpr int X_SYSEX_DATA = 75;
        static constexpr int X_SYSEX_DATA_WIDTH = 31;
        static constexpr int SYSEX_DATA_PER_ROW = 5;

        static constexpr int X_CHANNEL = 23;
        static constexpr int X_CHANNEL_MPE = 84;
        static constexpr int X_CHANNEL_MPE_TYPE = 110;
        static constexpr int Y_CHANNEL = 12;
        static constexpr int Y_CHANNEL_MARGIN = 8;
        
        static constexpr int X_SEPERATOR = 1;
        static constexpr int Y_SEPERATOR = 2;
        static constexpr int WIDTH_SEPERATOR = STANDARD_WIDTH - 48;
        static constexpr int HEIGHT_SEPERATOR = 1;
        
        static constexpr int X_PRGM = 24;
        static constexpr int Y_CHANNEL_PADDING = 8;
        
        static constexpr int HEIGHT_INDICATOR = 1;
        
        static constexpr int X_PB = 84;
        static constexpr int Y_PB = 7;
        static constexpr int X_PB_DATA = 24;
        
        static constexpr int X_PARAM = 84;
        static constexpr int Y_PARAM = 7;
        static constexpr int X_PARAM_DATA = 24;

        static constexpr int X_NOTE = 48;
        static constexpr int Y_NOTE = 7;
        static constexpr int X_ON_OFF = 84;
        static constexpr int X_NOTE_DATA = X_MID - 6;
        
        static constexpr int X_PP = 84;
        static constexpr int Y_PP = 7;
        static constexpr int X_PP_DATA = X_MID - 6;
        
        static constexpr int X_CC = X_MID + 6;
        static constexpr int Y_CC = 7;
        static constexpr int X_CC_DATA = 24;
        
        struct ChannelPaintState
        {
            Time time_;
            int offset_ { 0 };
        };
        
        void paint(Graphics& g)
        {
            g.fillAll(theme_.colorBackground);
            
            auto t = Time::getCurrentTime();
            auto channels = &channels_;
            if (paused_)
            {
                t = pausedTime_;
                channels = &pausedChannels_;
            }
            
            // draw the data for each channel
            ChannelPaintState state = { t, 0 };
            
            // draw MIDI port name
            auto port_name = deviceInfo_.name;
            if (midiIn_.get() == nullptr)
            {
                port_name = port_name + String(paused_ ? " (paused)": "");
            }
            g.setFont(theme_.fontLabel());
            g.setColour(theme_.colorData);
            g.drawText(port_name,
                       X_PORT, Y_PORT,
                       owner_->getWidth(), theme_.labelHeight(),
                       Justification::centredLeft);
            
            state.offset_ = Y_PORT + theme_.labelHeight();
            
            paintClock(g, state, t, channels->clock_);

            if (!isExpired(t, channels->sysex_.time_)) {
                paintSysex(g, state, channels->sysex_);
            }
            
            for (int channel_index = 0; channel_index < 16; ++channel_index)
            {
                auto& channel_messages = channels->channel_[channel_index];
                if (!isExpired(t, channel_messages.time_))
                {
                    paintChannelHeader(g, state, channel_messages);
                    
                    paintProgramChange(g, state, channel_messages);
                    state.offset_ = paintPitchBend(g, state, channel_messages);
                    state.offset_ = paintParameters(g, state, "RPN", channel_messages.rpns_);
                    state.offset_ = paintParameters(g, state, "NRPN", channel_messages.nrpns_);
                    int notes_bottom = paintNotes(g, state, channel_messages);
                    int control_changes_bottom = paintControlChanges(g, state, channel_messages);
                    
                    state.offset_ = std::max(state.offset_, std::max(notes_bottom, control_changes_bottom));
                    
                    state.offset_ += Y_CHANNEL_MARGIN;
                }
                
                pruneParameters(t, channel_messages.rpns_);
                pruneParameters(t, channel_messages.nrpns_);
            }
                        
            lastHeight_ = state.offset_;
        }
        
        void pruneParameters(Time t, Parameters& params)
        {
            auto it_param = params.param_.begin();
            while (it_param != params.param_.end())
            {
                if (isExpired(t, it_param->second.time_))
                {
                    it_param = params.param_.erase(it_param);
                }
                else
                {
                    ++it_param;
                }
            }
        }
        
        static int getStandardWidth()
        {
            return STANDARD_WIDTH;
        }

        int getVisibleHeight() const
        {
            return lastHeight_;
        }
        
        void paintClock(Graphics& g, ChannelPaintState& state, Time& t, Clock& clock)
        {
            auto show_bpm = !isExpired(t, clock.timeBpm_);
            auto show_start = !isExpired(t, clock.timeStart_);
            auto show_continue = !isExpired(t, clock.timeContinue_);
            auto show_stop = !isExpired(t, clock.timeStop_);
            auto show_transport = show_start || show_continue || show_stop;
            auto show_clock = show_bpm || show_transport;
            if (!show_clock)
            {
                return;
            }
            
            state.offset_ += Y_CLOCK;
            
            int clock_width = getStandardWidth() - X_PARAM - X_CLOCK_BPM;

            // draw clock header
            
            g.setColour(theme_.colorData);
            g.setFont(theme_.fontLabel());
            g.drawText(String("CLOCK"),
                       X_CLOCK, state.offset_,
                       getStandardWidth() - X_CLOCK, theme_.labelHeight(),
                       Justification::centredLeft);
            
            // draw BPM
            if (show_bpm)
            {
                g.setColour(theme_.colorController);
                g.setFont(theme_.fontLabel());
                g.drawText("BPM",
                           X_PARAM, state.offset_,
                           clock_width, theme_.labelHeight(),
                           Justification::centredLeft);
                
                g.setColour(theme_.colorData);
                g.setFont(theme_.fontData());
                g.drawText(outputBpm(clock.bpm_),
                           X_PARAM, state.offset_,
                           clock_width, theme_.dataHeight(),
                           Justification::centredRight);
                
                state.offset_ += theme_.labelHeight();
            }

            // draw transport
            
            if (show_transport)
            {
                if (show_start)
                {
                    g.setColour(theme_.colorPositive);
                    g.setFont(theme_.fontLabel());
                    g.drawText("START",
                               X_PARAM, state.offset_,
                               clock_width, theme_.labelHeight(),
                               Justification::centredLeft);
                }
                
                if (show_continue)
                {
                    g.setColour(theme_.colorPositive);
                    g.setFont(theme_.fontLabel());
                    g.drawText("CONT",
                               X_PARAM, state.offset_,
                               clock_width, theme_.labelHeight(),
                               Justification::centred);
                }
                
                if (show_stop)
                {
                    g.setColour(theme_.colorNegative);
                    g.setFont(theme_.fontLabel());
                    g.drawText("STOP",
                               X_PARAM, state.offset_,
                               clock_width, theme_.dataHeight(),
                               Justification::centredRight);
                }
                
                state.offset_ += theme_.labelHeight();
            }
            
            // draw seperator
            
            g.setColour(theme_.colorSeperator);
            state.offset_ += Y_SEPERATOR;
            g.drawRect(X_CHANNEL + X_SEPERATOR, state.offset_,
                       WIDTH_SEPERATOR, HEIGHT_SEPERATOR);
            state.offset_ += HEIGHT_SEPERATOR + Y_CLOCK_PADDING;
        }

        void paintSysex(Graphics& g, ChannelPaintState& state, Sysex& sysex)
        {
            state.offset_ += Y_SYSEX;
            
            int sysex_width = getStandardWidth() - X_SYSEX - X_SYSEX_LENGTH;

            // draw syxex header and length
            g.setColour(theme_.colorData);
            g.setFont(theme_.fontLabel());
            g.drawText(String("SYSEX"),
                       X_SYSEX, state.offset_,
                       getStandardWidth() - X_SYSEX, theme_.labelHeight(),
                       Justification::centredLeft);
            
            g.setColour(theme_.colorLabel);
            g.setFont(theme_.fontLabel());
            g.drawText(output14Bit(sysex.length_),
                       X_SYSEX, state.offset_,
                       sysex_width, theme_.dataHeight(),
                       Justification::centredRight);
            
            state.offset_ += theme_.labelHeight();
            
            // draw sysex data

            g.setColour(theme_.colorData);
            g.setFont(theme_.fontLabel());
            
            int data_x = 0;
            
            for (int i = 0; i < Sysex::MAX_SYSEX_DATA && i < sysex.length_;) {
                int i_row = i + SYSEX_DATA_PER_ROW;
                
                data_x = X_SYSEX_DATA;
                for (; i < i_row && i < sysex.length_; ++i) {
                    g.drawText(output7Bit(sysex.data_[i]),
                               data_x, state.offset_,
                               X_SYSEX_DATA_WIDTH, theme_.dataHeight(),
                               Justification::centredRight);
                    data_x += X_SYSEX_DATA_WIDTH;
                }
                
                state.offset_ += theme_.labelHeight();
            }

            // draw seperator
            
            g.setColour(theme_.colorSeperator);
            state.offset_ += Y_SEPERATOR;
            g.drawRect(X_CHANNEL + X_SEPERATOR, state.offset_,
                       WIDTH_SEPERATOR, HEIGHT_SEPERATOR);
            state.offset_ += HEIGHT_SEPERATOR + Y_SYSEX_PADDING;
        }
        
        void paintChannelHeader(Graphics& g, ChannelPaintState& state, ActiveChannel& channel)
        {
            g.setColour(theme_.colorData);
            g.setFont(theme_.fontLabel());
            state.offset_ += Y_CHANNEL;
            g.drawText(String("CH ") + output7Bit(channel.number_ + 1),
                       X_CHANNEL, state.offset_,
                       getStandardWidth() - X_CHANNEL, theme_.labelHeight(),
                       Justification::centredLeft);
            
            if (channel.mpeMember_ != MpeMember::mpeNone)
            {
                g.setColour(theme_.colorLabel);
                g.drawText("MPE",
                           X_CHANNEL_MPE, state.offset_,
                           getStandardWidth() - X_CHANNEL_MPE, theme_.labelHeight(),
                           Justification::centredLeft);
                auto mpe_label = String("");
                
                if (channel.mpeManager_)
                {
                    mpe_label = "MGR";
                }
                else if (channel.mpeMember_ == MpeMember::mpeLower)
                {
                    mpe_label = "LZ";
                }
                else if (channel.mpeMember_ == MpeMember::mpeUpper)
                {
                    mpe_label = "UZ";
                }
                g.setColour(theme_.colorLabel);
                g.drawText(mpe_label,
                           X_CHANNEL_MPE_TYPE, state.offset_,
                           getStandardWidth() - X_CHANNEL_MPE, theme_.labelHeight(),
                           Justification::centredLeft);
            }
            state.offset_ += theme_.labelHeight();

            g.setColour(theme_.colorSeperator);
            state.offset_ += Y_SEPERATOR;
            g.drawRect(X_CHANNEL + X_SEPERATOR, state.offset_,
                       WIDTH_SEPERATOR, HEIGHT_SEPERATOR);
            state.offset_ += HEIGHT_SEPERATOR + Y_CHANNEL_PADDING;
        }
        
        void paintProgramChange(Graphics& g, ChannelPaintState& state, ActiveChannel& channel)
        {
            auto& program_change = channel.programChange_;
            if (!isExpired(state.time_, program_change.time_))
            {
                // write the texts
                
                g.setColour(theme_.colorLabel);
                g.setFont(theme_.fontLabel());
                g.drawText(String("PRGM ") + output7Bit(program_change.value_),
                           0, state.offset_ - Y_CHANNEL_PADDING - Y_SEPERATOR - HEIGHT_SEPERATOR - theme_.labelHeight(),
                           getStandardWidth() - X_PRGM, theme_.labelHeight(),
                           Justification::centredRight);
            }
        }
        
        int paintPitchBend(Graphics& g, ChannelPaintState& state, ActiveChannel& channel)
        {
            int y_offset = state.offset_;
            
            auto& pitch_bend = channel.pitchBend_;
            if (!isExpired(state.time_, pitch_bend.time_))
            {
                y_offset += Y_PB;
                
                Colour pb_color = theme_.colorLabel;
                
                int pb_width = getStandardWidth() - X_PB - X_PB_DATA;
                int pb_range = pb_width / 2;
                
                int indicator_x = X_PB + pb_range + 1;
                int indicator_width = (pb_range * (pitch_bend.value_ - 0x2000)) / 0x2000;
                if (pitch_bend.value_ > 0x2000)
                {
                    pb_color = theme_.colorPositive;
                }
                else if (pitch_bend.value_ < 0x2000)
                {
                    pb_color = theme_.colorNegative;
                    indicator_x = X_PB + pb_range + indicator_width;
                    indicator_width = abs(indicator_width);
                }
                
                // draw the pitch bend text
                
                g.setColour(pb_color);
                g.setFont(theme_.fontLabel());
                g.drawText("PB",
                           X_PB, y_offset,
                           pb_width, theme_.labelHeight(),
                           Justification::centredLeft);
                
                g.setColour(theme_.colorData);
                g.setFont(theme_.fontData());
                g.drawText(output14Bit(pitch_bend.value_),
                           X_PB, y_offset,
                           pb_width, theme_.dataHeight(),
                           Justification::centredRight);
                
                y_offset += theme_.labelHeight();
                
                // draw pitchbend indicator
                
                g.setColour(theme_.colorTrack);
                g.fillRect(X_PB, y_offset,
                           pb_width, HEIGHT_INDICATOR);
                
                g.setColour(pb_color);
                g.fillRect(indicator_x, y_offset,
                           indicator_width, HEIGHT_INDICATOR);
                
                y_offset += HEIGHT_INDICATOR;
            }
            
            return y_offset;
        }
        
        int paintParameters(Graphics& g, ChannelPaintState& state, const String& name, Parameters& parameters)
        {
            int y_offset = state.offset_;

            if (!isExpired(state.time_, parameters.time_))
            {
                for (auto& [number, param] : parameters.param_)
                {
                    if (!isExpired(state.time_, param.time_))
                    {
                        y_offset += Y_PARAM;
                        
                        Colour param_color = theme_.colorController;
                        
                        int param_width = getStandardWidth() - X_PARAM - X_PARAM_DATA;
                        
                        // draw the parameter text
                        
                        g.setColour(param_color);
                        g.setFont(theme_.fontLabel());
                        g.drawText(name + String(" ") + output14Bit(number),
                                   X_PARAM, y_offset,
                                   param_width, theme_.labelHeight(),
                                   Justification::centredLeft);
                        
                        g.setColour(theme_.colorData);
                        g.setFont(theme_.fontData());
                        g.drawText(output14Bit(param.value_),
                                   X_PARAM, y_offset,
                                   param_width, theme_.dataHeight(),
                                   Justification::centredRight);
                        
                        y_offset += theme_.labelHeight();
                        
                        // draw value indicator

                        g.setColour(theme_.colorTrack);
                        g.fillRect(X_PARAM, y_offset,
                                   param_width, HEIGHT_INDICATOR);
                        
                        g.setColour(param_color);
                        g.fillRect(X_PARAM, y_offset,
                                   (param_width * param.value_) / 0x3FFF, HEIGHT_INDICATOR);
                        
                        y_offset += HEIGHT_INDICATOR;
                    }
                }
            }
            
            return y_offset;
        }

        int paintNotes(Graphics& g, ChannelPaintState& state, ActiveChannel& channel)
        {
            int y_offset = -1;
            
            auto& notes = channel.notes_;
            if (!isExpired(state.time_, notes.time_))
            {
                for (int i = 0; i < 128; ++i)
                {
                    auto& note_on = notes.noteOn_[i];
                    auto& note_off = notes.noteOff_[i];
                    
                    if (!isExpired(state.time_, note_on.time_))
                    {
                        if (isExpired(state.time_, note_off.time_))
                        {
                            channel.time_ = state.time_;
                            note_on.time_ = state.time_;
                            notes.time_ = state.time_;
                        }
                        
                        if (y_offset == -1)
                        {
                            y_offset = state.offset_;
                        }
                        
                        y_offset += Y_NOTE;
                        
                        // draw note text
                        
                        auto note_color = !isExpired(state.time_, note_off.time_) ? theme_.colorNegative : theme_.colorPositive;
                        g.setColour(note_color);
                        g.setFont(theme_.fontLabel());
                        g.drawText(outputNote(i),
                                   X_NOTE, y_offset,
                                   X_NOTE_DATA - X_NOTE, theme_.labelHeight(),
                                   Justification::centredLeft);
                        
                        int note_width = X_NOTE_DATA - X_ON_OFF;
                        g.setColour(theme_.colorLabel);
                        g.setFont(theme_.fontLabel());
                        g.drawText("ON",
                                   X_ON_OFF, y_offset,
                                   note_width, theme_.labelHeight(),
                                   Justification::centredLeft);
                        
                        g.setColour(theme_.colorData);
                        g.setFont(theme_.fontData());
                        g.drawText(output7Bit(note_on.value_),
                                   X_ON_OFF, y_offset,
                                   note_width, theme_.dataHeight(),
                                   Justification::centredRight);
                        
                        y_offset += theme_.labelHeight();
                        
                        // draw velocity indicator
                        
                        g.setColour(theme_.colorTrack);
                        g.fillRect(X_ON_OFF, y_offset,
                                   note_width, HEIGHT_INDICATOR);
                        
                        auto velocity_color = theme_.colorPositive;

                        g.setColour(velocity_color);
                        g.fillRect(X_ON_OFF, y_offset,
                                   (note_width * note_on.value_) / 127, HEIGHT_INDICATOR);
                        
                        y_offset += HEIGHT_INDICATOR;
                        
                        if (!isExpired(state.time_, note_on.polyPressureTime_))
                        {
                            // draw polypressure text
                            
                            y_offset += Y_PP;
                            
                            int pp_width = X_PP_DATA - X_PP;
                            g.setColour(theme_.colorLabel);
                            g.setFont(theme_.fontLabel());
                            g.drawText("PP",
                                       X_PP, y_offset,
                                       pp_width, theme_.labelHeight(),
                                       Justification::centredLeft);
                            
                            g.setColour(theme_.colorData);
                            g.setFont(theme_.fontData());
                            g.drawText(output7Bit(note_on.polyPressure_),
                                       X_PP, y_offset,
                                       pp_width, theme_.dataHeight(),
                                       Justification::centredRight);
                            
                            y_offset += theme_.labelHeight();
                            
                            // draw velocity indicator
                            
                            g.setColour(theme_.colorTrack);
                            g.fillRect(X_PP, y_offset,
                                       pp_width, HEIGHT_INDICATOR);
                            
                            g.setColour(note_color);
                            g.fillRect(X_PP, y_offset,
                                       (pp_width * note_on.polyPressure_) / 127, HEIGHT_INDICATOR);
                            
                            y_offset += HEIGHT_INDICATOR;
                        }
                    }
                    
                    if (!isExpired(state.time_, note_off.time_))
                    {
                        if (y_offset == -1)
                        {
                            y_offset = state.offset_;
                        }
                        
                        y_offset += Y_NOTE;
                        
                        // draw note text
                        
                        auto note_color = theme_.colorNegative;

                        if (isExpired(state.time_, note_on.time_))
                        {
                            g.setColour(note_color);
                            g.setFont(theme_.fontLabel());
                            g.drawText(outputNote(i),
                                       X_NOTE, y_offset,
                                       X_NOTE_DATA - X_NOTE, theme_.labelHeight(),
                                       Justification::centredLeft);
                        }
                        
                        int note_width = X_NOTE_DATA - X_ON_OFF;
                        g.setColour(theme_.colorLabel);
                        g.setFont(theme_.fontLabel());
                        g.drawText("OFF",
                                   X_ON_OFF, y_offset,
                                   note_width, theme_.labelHeight(),
                                   Justification::centredLeft);
                        
                        g.setColour(theme_.colorData);
                        g.setFont(theme_.fontData());
                        g.drawText(output7Bit(note_off.value_),
                                   X_ON_OFF, y_offset,
                                   note_width, theme_.dataHeight(),
                                   Justification::centredRight);
                        
                        y_offset += theme_.labelHeight();
                        
                        // draw velocity indicator
                        
                        g.setColour(theme_.colorTrack);
                        g.fillRect(X_ON_OFF, y_offset,
                                   note_width, HEIGHT_INDICATOR);
                        
                        auto velocity_color = theme_.colorNegative;

                        g.setColour(velocity_color);
                        g.fillRect(X_ON_OFF, y_offset,
                                   (note_width * note_off.value_) / 127, HEIGHT_INDICATOR);
                        
                        y_offset += HEIGHT_INDICATOR;
                    }
                }
            }
            
            return y_offset;
        }
        
        int paintControlChanges(Graphics& g, ChannelPaintState& state, ActiveChannel& channel)
        {
            int y_offset = -1;
            
            if (!isExpired(state.time_, channel.channelPressure_.time_))
            {
                paintControlChangeEntry(g, state, y_offset, String("CP"), channel.channelPressure_.value_);
            }
            
            auto& control_changes = channel.controlChanges_;
            if (!isExpired(state.time_, control_changes.time_))
            {
                for (int i = 0; i < 128; ++i)
                {
                    auto& cc = control_changes.controlChange_[i];
                    if (!isExpired(state.time_, cc.time_))
                    {
                        paintControlChangeEntry(g, state, y_offset, String("CC ") + output7Bit(cc.number_), cc.value_);
                    }
                }
            }
            
            return y_offset;
        }
        
        void paintControlChangeEntry(Graphics& g, ChannelPaintState& state, int& yOffset, const String& label, int value)
        {
            if (yOffset == -1)
            {
                yOffset = state.offset_;
            }
            
            yOffset += Y_CC;
            
            // write label text
            
            int cc_width = getStandardWidth() - X_CC - X_CC_DATA;
            g.setColour(theme_.colorController);
            g.setFont(theme_.fontLabel());
            g.drawText(label,
                       X_CC, yOffset,
                       cc_width, theme_.labelHeight(),
                       Justification::centredLeft);
            
            g.setColour(theme_.colorData);
            g.setFont(theme_.fontData());
            g.drawText(output7Bit(value),
                       X_CC, yOffset,
                       cc_width, theme_.dataHeight(),
                       Justification::centredRight);
            
            yOffset += theme_.labelHeight();
            
            // draw value indicator
            
            g.setColour(theme_.colorTrack);
            g.fillRect(X_CC, yOffset,
                       cc_width, HEIGHT_INDICATOR);
            
            g.setColour(theme_.colorController);
            g.fillRect(X_CC, yOffset,
                       (cc_width * value) / 127, HEIGHT_INDICATOR);
            
            yOffset += HEIGHT_INDICATOR;
        }
        
        bool isExpired(const Time& currentTime, Time& messageTime)
        {
            auto delay = settingsManager_->getSettings().getTimeoutDelay();
            return (currentTime - messageTime).inSeconds() > delay;
        }
        
        String output7BitAsHex(int v)
        {
            return String::toHexString(v).paddedLeft('0', 2).toUpperCase() + "H";
        }
        
        String output7Bit(int v)
        {
            if (settingsManager_->getSettings().getNumberFormat() == NumberFormat::formatHexadecimal)
            {
                return output7BitAsHex(v);
            }
            else
            {
                return String(v);
            }
        }
        
        String output14BitAsHex(int v)
        {
            return String::toHexString(v).paddedLeft('0', 4).toUpperCase() + "H";
        }
        
        String output14Bit(int v)
        {
            if (settingsManager_->getSettings().getNumberFormat() == NumberFormat::formatHexadecimal)
            {
                return output14BitAsHex(v);
            }
            else
            {
                return String(v);
            }
        }
        
        String outputNote(int noteNumber)
        {
            if (settingsManager_->getSettings().getNoteFormat() == NoteFormat::formatNumber)
            {
                return output7Bit(noteNumber);
            }
            else
            {
                auto octave = settingsManager_->getSettings().getOctaveMiddleC();
                return MidiMessage::getMidiNoteName(noteNumber, true, true, octave);
            }
        }
        
        String outputBpm(double bpm)
        {
            return String(bpm, 1);
        }

        void resized()
        {
            dirty_ = true;
        }
        
        void setPaused(bool paused)
        {
            if (paused)
            {
                pausedTime_ = Time::getCurrentTime();
                pausedChannels_ = channels_;
            }
            
            dirty_ = true;
            paused_ = paused;
        }
        
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
                theme_.parseXml(File(file).loadFileAsString());
            }
            
            owner_->getParentComponent()->repaint();
            
            settingsManager_->storeSettings();
        }

        MidiDeviceComponent* const owner_;
        
        SettingsManager* const settingsManager_;
        Theme& theme_;
        MidiDeviceInfo deviceInfo_;
        std::unique_ptr<MidiInput> midiIn_;
        std::atomic_bool dirty_ { true };
        Time lastRender_;
        bool paused_ { false };
        
        ActiveChannels channels_;
        std::deque<double> midiTimeStamps_;
        
        Time pausedTime_;
        ActiveChannels pausedChannels_;
        
        int lastHeight_ { 0 };
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pimpl)
    };
    
    MidiDeviceComponent::MidiDeviceComponent(SettingsManager* manager, const String& name) : pimpl_(new Pimpl(this, manager, name)) {}
    MidiDeviceComponent::MidiDeviceComponent(SettingsManager* manager, const MidiDeviceInfo& info) : pimpl_(new Pimpl(this, manager, info)) {}
    MidiDeviceComponent::~MidiDeviceComponent() = default;

    int MidiDeviceComponent::getStandardWidth()         { return Pimpl::getStandardWidth(); }
    int MidiDeviceComponent::getVisibleHeight() const   { return pimpl_->getVisibleHeight(); }

    void MidiDeviceComponent::render()            { pimpl_->render(); }
    void MidiDeviceComponent::paint(Graphics& g)  { pimpl_->paint(g); }
    void MidiDeviceComponent::resized()           { pimpl_->resized(); }
    void MidiDeviceComponent::setPaused(bool p)   { pimpl_->setPaused(p); }

    void MidiDeviceComponent::handleIncomingMidiMessage(const MidiMessage& m)   { pimpl_->handleIncomingMidiMessage(nullptr, m); };
    bool MidiDeviceComponent::isInterestedInFileDrag(const StringArray& f)      { return pimpl_->isInterestedInFileDrag(f); }
    void MidiDeviceComponent::filesDropped(const StringArray& f, int x, int y)  { pimpl_->filesDropped(f, x, y); }
}
