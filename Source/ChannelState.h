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
    
    struct Note : public ChannelMessage
    {
        bool on_ { false };
        int number_ { -1 };
        int polyPressure_ { 0 };
        Time polyPressureTime_;
    };
    
    struct Notes
    {
        Notes()
        {
            for (int i = 0; i < 128; ++i)
            {
                note_[i].number_ = i;
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
                note_[i] = other.note_[i];
            }
        }

        Time time_;
        Note note_[128];
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
        
        int lastRpnMsb_ { 127 };
        int lastRpnLsb_ { 127 };
        int lastNrpnMsb_ { 127 };
        int lastNrpnLsb_ { 127 };
        int lastDataMsb_ { 0 };
        int lastDataLsb_ { 0 };
    };
    
    struct ActiveChannels
    {
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
            for (int i = 0; i < 16; ++i)
            {
                channel_[i] = other.channel_[i];
            }
        }

        ActiveChannel channel_[16];
    };
}
