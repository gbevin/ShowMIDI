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

namespace showmidi
{
    struct ChannelMessage
    {
        Time time_;
        int value_ { 0 };
    };
    
    struct NoteOn : public ChannelMessage
    {
        int number_ { -1 };
        int polyPressure_ { 0 };
        Time polyPressureTime_;
    };
    
    struct NoteOff : public ChannelMessage
    {
        int number_ { -1 };
    };
    
    struct Notes
    {
        Notes()
        {
            for (int i = 0; i < 128; ++i)
            {
                noteOn_[i].number_ = i;
                noteOff_[i].number_ = i;
            }
        }
        
        Notes(const Notes& other)
        {
            deepCopy(other);
        }
        
        Notes& operator=(Notes other)
        {
            deepCopy(other);
            return *this;
        }
        
        void deepCopy(const Notes& other)
        {
            time_ = other.time_;
            for (int i = 0; i < 128; ++i)
            {
                noteOn_[i] = other.noteOn_[i];
                noteOff_[i] = other.noteOff_[i];
            }
        }

        Time time_;
        NoteOn noteOn_[128];
        NoteOff noteOff_[128];
    };
    
    struct ControlChange : public ChannelMessage
    {
        int number_ { -1 };
    };
    
    struct ControlChanges
    {
        ControlChanges()
        {
            for (int i = 0; i < 128; ++i)
            {
                controlChange_[i].number_ = i;
            }
        }
        
        ControlChanges(const ControlChanges& other)
        {
            deepCopy(other);
        }
        
        ControlChanges& operator=(ControlChanges other)
        {
            deepCopy(other);
            return *this;
        }
        
        void deepCopy(const ControlChanges& other)
        {
            time_ = other.time_;
            for (int i = 0; i < 128; ++i)
            {
                controlChange_[i] = other.controlChange_[i];
            }
        }

        Time time_;
        ControlChange controlChange_[128];
    };
    
    struct ProgramChange : public ChannelMessage
    {
    };
    
    struct ChannelPressure : public ChannelMessage
    {
    };
    
    struct PitchBend : public ChannelMessage
    {
    };
    
    struct Parameter : public ChannelMessage
    {
    };
    
    struct Parameters
    {
        Parameters()
        {
        }
        
        Parameters(const Parameters& other)
        {
            deepCopy(other);
        }
        
        Parameters& operator=(Parameters other)
        {
            deepCopy(other);
            return *this;
        }
        
        void deepCopy(const Parameters& other)
        {
            time_ = other.time_;
            param_.clear();
            param_ = other.param_;
        }

        Time time_;
        std::map<int, Parameter> param_;
    };
    
    enum MpeMember
    {
        mpeNone,
        mpeLower,
        mpeUpper
    };
    
    struct ActiveChannel
    {
        int number_ { -1 };
        Time time_;
        Notes notes_;
        ControlChanges controlChanges_;
        ProgramChange programChange_;
        ChannelPressure channelPressure_;
        PitchBend pitchBend_;
        Parameters rpns_;
        Parameters nrpns_;
        bool mpeManager_ { false };
        MpeMember mpeMember_ { MpeMember::mpeNone };
        
        int lastRpnMsb_ { 127 };
        int lastRpnLsb_ { 127 };
        int lastNrpnMsb_ { 127 };
        int lastNrpnLsb_ { 127 };
        int lastDataMsb_ { 0 };
        int lastDataLsb_ { 0 };
    };
    
    struct Sysex
    {
        static constexpr int MAX_SYSEX_DATA = 20;
        
        Sysex()
        {
        }
        
        Sysex(const Sysex& other)
        {
            deepCopy(other);
        }
        
        Sysex& operator=(Sysex other)
        {
            deepCopy(other);
            return *this;
        }
        
        Time time_;
        
        uint8 data_[MAX_SYSEX_DATA] { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        unsigned int length_ { 0 };
        
        void deepCopy(const Sysex& other)
        {
            time_ = other.time_;
            length_ = other.length_;
            memcpy(data_, other.data_, MAX_SYSEX_DATA);
        }

    };
    
    struct ActiveChannels
    {
        Sysex sysex_;
        
        ActiveChannels()
        {
            for (int i = 0; i < 16; ++i)
            {
                channel_[i].number_ = i;
            }
        }
        
        ActiveChannels(const ActiveChannels& other)
        {
            deepCopy(other);
        }
        
        ActiveChannels& operator=(ActiveChannels other)
        {
            deepCopy(other);
            return *this;
        }
        
        void deepCopy(const ActiveChannels& other)
        {
            sysex_ = other.sysex_;
            
            for (int i = 0; i < 16; ++i)
            {
                channel_[i] = other.channel_[i];
            }
        }

        ActiveChannel channel_[16];
        
        void handleMpeActivation(Time t, ActiveChannel& channel, int range)
        {
            // handle lower zone
            if (channel.number_ == 0)
            {
                // disable MPE for the lower zone
                if (range == 0)
                {
                    // turn off all member channels that were assigned
                    // to the lower zone
                    for (int i = 1; i <= 14; ++i)
                    {
                        auto& ch = channel_[i];
                        if (ch.mpeMember_ == MpeMember::mpeLower)
                        {
                            ch.mpeMember_ = MpeMember::mpeNone;
                            ch.time_ = t;
                        }
                    }
                    channel.mpeManager_ = false;
                    channel.mpeMember_ = MpeMember::mpeNone;
                }
                // enable MPE for the lower zone
                else
                {
                    // based on the MPE zone range,
                    // assign channels to the lower zone
                    for (int i = 1; i <= range; ++i)
                    {
                        auto& ch = channel_[i];
                        ch.mpeMember_ = MpeMember::mpeLower;
                        ch.time_ = t;
                    }
                    channel.mpeManager_ = true;
                    channel.mpeMember_ = MpeMember::mpeLower;
                    
                    // if all member channels were assigned to the
                    // lower zone, disabled the manager channel of the
                    // upper zone, if it was enabled
                    if (range >= 14)
                    {
                        auto& ch = channel_[15];
                        if (ch.mpeManager_)
                        {
                            ch.mpeManager_ = false;
                            ch.mpeMember_ = MpeMember::mpeNone;
                            ch.time_ = t;
                        }
                    }
                }
            }
            // handle upper zone
            else if (channel.number_ == 15)
            {
                // disable MPE for the upper zone
                if (range == 0)
                {
                    // turn off all member channels that were assigned
                    // to the upper zone
                    for (int i = 14; i >= 1; --i)
                    {
                        auto& ch = channel_[i];
                        if (ch.mpeMember_ == MpeMember::mpeUpper)
                        {
                            ch.mpeMember_ = MpeMember::mpeNone;
                            ch.time_ = t;
                        }
                    }
                    channel.mpeManager_ = false;
                    channel.mpeMember_ = MpeMember::mpeNone;
                }
                // enable MPE for the upper zone
                else
                {
                    // based on the MPE zone range,
                    // assign channels to the upper zone
                    for (int i = 1; i <= range; ++i)
                    {
                        auto& ch = channel_[15 - i];
                        ch.mpeMember_ = MpeMember::mpeUpper;
                        ch.time_ = t;
                    }
                    channel.mpeManager_ = true;
                    channel.mpeMember_ = MpeMember::mpeUpper;
                    
                    // if all member channels were assigned to the
                    // upper zone, disabled the manager channel of the
                    // lower zone, if it was enabled
                    if (range >= 14)
                    {
                        auto& ch = channel_[0];
                        if (ch.mpeManager_)
                        {
                            ch.mpeManager_ = false;
                            ch.mpeMember_ = MpeMember::mpeNone;
                            ch.time_ = t;
                        }
                    }
                }
            }
        }
    };
}
