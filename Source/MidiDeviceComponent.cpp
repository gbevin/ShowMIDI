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
        Pimpl(MidiDeviceComponent* owner, SettingsManager& manager, const String& name) :
            owner_(owner),
            settingsManager_(manager),
            theme_(manager.getSettings().getTheme()),
            deviceInfo_({ name, ""})
        {
        }
        
        Pimpl(MidiDeviceComponent* owner, SettingsManager& manager, const MidiDeviceInfo info) :
            owner_(owner),
            settingsManager_(manager),
            theme_(manager.getSettings().getTheme()),
            deviceInfo_(info)
        {
            auto midi_input = MidiInput::openDevice(info.identifier, this);
            if (midi_input != nullptr)
            {
                midi_input->start();
                midiIn_.swap(midi_input);
            }
            
//            showTestData();
        }
        
        void showTestData()
        {
            const auto t = Time::getCurrentTime();
            
            deviceInfo_ = {"MIDI Instrument Name", deviceInfo_.identifier};
            
            auto& channel1 = channels_.channel_[0];
            channel1.time_ = t;
            channel1.programChange_.value_ = 0;
            channel1.programChange_.time_ = t;
            channel1.pitchBend_.value_ = 9256;
            channel1.pitchBend_.time_ = t;
            channel1.notes_.time_ = t;
            channel1.notes_.note_[61].on_ = true;
            channel1.notes_.note_[61].value_ = 127;
            channel1.notes_.note_[61].polyPressure_ = 0;
            channel1.notes_.note_[61].time_ = t;
            channel1.notes_.note_[79].on_ = true;
            channel1.notes_.note_[79].value_ = 38;
            channel1.notes_.note_[79].polyPressure_ = 0;
            channel1.notes_.note_[79].time_ = t;
            channel1.channelPressure_.value_ = 76;
            channel1.channelPressure_.time_ = t;
            channel1.controlChanges_.time_ = t;
            channel1.controlChanges_.controlChange_[74].value_ = 127;
            channel1.controlChanges_.controlChange_[74].time_ = t;
            
            auto& channel16 = channels_.channel_[15];
            channel16.time_ = t;
            channel16.programChange_.value_ = 127;
            channel16.programChange_.time_ = t;
            channel16.pitchBend_.value_ = 0;
            channel16.pitchBend_.time_ = t;
            channel16.notes_.time_ = t;
            channel16.notes_.note_[61].on_ = true;
            channel16.notes_.note_[61].value_ = 127;
            channel16.notes_.note_[61].polyPressure_ = 0;
            channel16.notes_.note_[61].time_ = t;
            channel16.notes_.note_[79].on_ = false;
            channel16.notes_.note_[79].value_ = 127;
            channel16.notes_.note_[79].polyPressure_ = 0;
            channel16.notes_.note_[79].time_ = t;
            channel16.channelPressure_.value_ = 76;
            channel16.channelPressure_.time_ = t;
            channel16.controlChanges_.time_ = t;
            channel16.controlChanges_.controlChange_[127].value_ = 100;
            channel16.controlChanges_.controlChange_[127].time_ = t;
            
            setPaused(true);
        }
        
        ~Pimpl()
        {
            midiIn_ = nullptr;
        }
        
        void handleIncomingMidiMessage(MidiInput*, const MidiMessage& msg)
        {
            if (msg.getChannel() <= 0)
            {
                return;
            }
            
            const auto t = Time::getCurrentTime();
            
            ChannelMessage* channel_message = nullptr;
            
            auto& channel = channels_.channel_[msg.getChannel() - 1];
            if (msg.isNoteOnOrOff())
            {
                auto& notes = channel.notes_;
                notes.time_ = t;
                
                auto& note = notes.note_[msg.getNoteNumber()];
                note.on_ = msg.isNoteOn();
                note.value_ = msg.getVelocity();
                channel_message = &note;
            }
            else if (msg.isAftertouch())
            {
                auto& notes = channel.notes_;
                notes.time_ = t;
                
                auto& note = notes.note_[msg.getNoteNumber()];
                note.polyPressure_ = msg.getAfterTouchValue();
                note.polyPressureTime_ = t;
                channel_message = &note;
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
        
        static constexpr int STANDARD_WIDTH = 252;
        
        static constexpr int X_PORT = 48;
        static constexpr int Y_PORT = 12;
        
        static constexpr int X_CHANNEL = 23;
        static constexpr int Y_CHANNEL = 12;
        static constexpr int Y_CHANNEL_MARGIN = 8;
        
        static constexpr int X_SEPERATOR = 1;
        static constexpr int Y_SEPERATOR = 2;
        static constexpr int WIDTH_SEPERATOR = 204;
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
        static constexpr int X_NOTE_DATA = 108;
        
        static constexpr int X_PP = 84;
        static constexpr int Y_PP = 7;
        static constexpr int X_PP_DATA = 108;
        
        static constexpr int X_CC = 156;
        static constexpr int Y_CC = 7;
        static constexpr int X_CC_DATA = 24;
        
        struct ChannelPaintState
        {
            Time time_;
            int offset_ { 0 };
            bool header_ { false };
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
            ChannelPaintState state = { t, 0, false };
            
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
            
            for (int channel_index = 0; channel_index < 16; ++channel_index)
            {
                auto& channel_messages = channels->channel_[channel_index];
                if (!isExpired(t, channel_messages.time_))
                {
                    state.header_ = false;
                    
                    paintProgramChange(g, state, channel_messages);
                    state.offset_ = paintPitchBend(g, state, channel_messages);
                    state.offset_ = paintParameters(g, state, "RPN", channel_messages, channel_messages.rpns_);
                    state.offset_ = paintParameters(g, state, "NRPN", channel_messages, channel_messages.nrpns_);
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
        
        void ensurePaintedChannelHeader(Graphics& g, ChannelPaintState& state, ActiveChannel& channel)
        {
            if (!state.header_)
            {
                paintChannelHeader(g, state, channel);
            }
        }
        
        void paintChannelHeader(Graphics& g, ChannelPaintState& state, ActiveChannel& channel)
        {
            state.header_ = true;
            
            g.setColour(theme_.colorData);
            g.setFont(theme_.fontLabel());
            state.offset_ += Y_CHANNEL;
            g.drawText(String("CH ") + output7Bit(channel.number_ + 1),
                       X_CHANNEL, state.offset_,
                       getStandardWidth() - X_CHANNEL, theme_.labelHeight(),
                       Justification::centredLeft);
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
                ensurePaintedChannelHeader(g, state, channel);
                
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
                ensurePaintedChannelHeader(g, state, channel);
                
                y_offset = state.offset_;
                
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
        
        int paintParameters(Graphics& g, ChannelPaintState& state, const String& name, ActiveChannel& channel, Parameters& parameters)
        {
            int y_offset = -1;
            
            if (!isExpired(state.time_, parameters.time_))
            {
                for (auto& [number, param] : parameters.param_)
                {
                    if (!isExpired(state.time_, param.time_))
                    {
                        ensurePaintedChannelHeader(g, state, channel);
                        
                        if (y_offset == -1)
                        {
                            y_offset = state.offset_;
                        }
                        
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
            
            if (y_offset == -1)
            {
                y_offset = state.offset_;
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
                    auto& note = notes.note_[i];
                    if (!isExpired(state.time_, note.time_))
                    {
                        if (note.on_)
                        {
                            channel.time_ = state.time_;
                            note.time_ = state.time_;
                            notes.time_ = state.time_;
                        }
                        
                        ensurePaintedChannelHeader(g, state, channel);
                        
                        if (y_offset == -1)
                        {
                            y_offset = state.offset_;
                        }
                        
                        y_offset += Y_NOTE;
                        
                        // draw note text
                        
                        auto note_color = note.on_ ? theme_.colorPositive : theme_.colorNegative;
                        g.setColour(note_color);
                        g.setFont(theme_.fontLabel());
                        g.drawText(outputNote(i),
                                   X_NOTE, y_offset,
                                   getStandardWidth() - X_NOTE - X_NOTE_DATA, theme_.labelHeight(),
                                   Justification::centredLeft);
                        
                        int note_width = getStandardWidth() - X_ON_OFF - X_NOTE_DATA;
                        g.setColour(theme_.colorLabel);
                        g.setFont(theme_.fontLabel());
                        g.drawText(note.on_ ? "ON" : "OFF",
                                   X_ON_OFF, y_offset,
                                   note_width, theme_.labelHeight(),
                                   Justification::centredLeft);
                        
                        g.setColour(theme_.colorData);
                        g.setFont(theme_.fontData());
                        g.drawText(output7Bit(note.value_),
                                   X_ON_OFF, y_offset,
                                   note_width, theme_.dataHeight(),
                                   Justification::centredRight);
                        
                        y_offset += theme_.labelHeight();
                        
                        // draw velocity indicator
                        
                        g.setColour(theme_.colorTrack);
                        g.fillRect(X_ON_OFF, y_offset,
                                   note_width, HEIGHT_INDICATOR);
                        
                        g.setColour(note_color);
                        g.fillRect(X_ON_OFF, y_offset,
                                   (note_width * note.value_) / 127, HEIGHT_INDICATOR);
                        
                        y_offset += HEIGHT_INDICATOR;
                        
                        if (!isExpired(state.time_, note.polyPressureTime_))
                        {
                            // draw polypressure text
                            
                            y_offset += Y_PP;
                            
                            int pp_width = getStandardWidth() - X_PP - X_PP_DATA;
                            g.setColour(theme_.colorLabel);
                            g.setFont(theme_.fontLabel());
                            g.drawText("PP",
                                       X_PP, y_offset,
                                       pp_width, theme_.labelHeight(),
                                       Justification::centredLeft);
                            
                            g.setColour(theme_.colorData);
                            g.setFont(theme_.fontData());
                            g.drawText(output7Bit(note.polyPressure_),
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
                                       (pp_width * note.polyPressure_) / 127, HEIGHT_INDICATOR);
                            
                            y_offset += HEIGHT_INDICATOR;
                        }
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
                paintControlChangeEntry(g, state, channel, y_offset, String("CP"), channel.channelPressure_.value_);
            }
            
            auto& control_changes = channel.controlChanges_;
            if (!isExpired(state.time_, control_changes.time_))
            {
                for (int i = 0; i < 128; ++i)
                {
                    auto& cc = control_changes.controlChange_[i];
                    if (!isExpired(state.time_, cc.time_))
                    {
                        paintControlChangeEntry(g, state, channel, y_offset, String("CC ") + output7Bit(cc.number_), cc.value_);
                    }
                }
            }
            
            return y_offset;
        }
        
        void paintControlChangeEntry(Graphics& g, ChannelPaintState& state, ActiveChannel& channel, int& yOffset, const String& label, int value)
        {
            ensurePaintedChannelHeader(g, state, channel);
            
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
            auto delay = settingsManager_.getSettings().getTimeoutDelay();
            return (currentTime - messageTime).inSeconds() > delay;
        }
        
        String output7BitAsHex(int v)
        {
            return String::toHexString(v).paddedLeft('0', 2).toUpperCase() + "H";
        }
        
        String output7Bit(int v)
        {
            if (settingsManager_.getSettings().getNumberFormat() == NumberFormat::formatHexadecimal)
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
            if (settingsManager_.getSettings().getNumberFormat() == NumberFormat::formatHexadecimal)
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
            if (settingsManager_.getSettings().getNoteFormat() == NoteFormat::formatNumber)
            {
                return output7Bit(noteNumber);
            }
            else
            {
                auto octave = settingsManager_.getSettings().getOctaveMiddleC();
                return MidiMessage::getMidiNoteName(noteNumber, true, true, octave);
            }
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
            
            settingsManager_.storeSettings();
        }

        MidiDeviceComponent* const owner_;
        
        SettingsManager& settingsManager_;
        Theme& theme_;
        MidiDeviceInfo deviceInfo_;
        std::unique_ptr<MidiInput> midiIn_ { nullptr };
        std::atomic_bool dirty_ { true };
        Time lastRender_;
        bool paused_ { false };
        
        ActiveChannels channels_;
        
        Time pausedTime_;
        ActiveChannels pausedChannels_;
        
        int lastHeight_ { 0 };
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pimpl)
    };
    
    MidiDeviceComponent::MidiDeviceComponent(SettingsManager& manager, const String& name) : pimpl_(new Pimpl(this, manager, name)) {}
    MidiDeviceComponent::MidiDeviceComponent(SettingsManager& manager, const MidiDeviceInfo& info) : pimpl_(new Pimpl(this, manager, info)) {}
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
