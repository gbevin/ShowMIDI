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
    static constexpr int DEFAULT_OCTAVE_MIDDLE_C = 3;
    
    struct MidiDeviceComponent::Pimpl : public juce::MidiInputCallback
    {
        Pimpl(MidiDeviceComponent* owner, const String& name) : owner_(owner), deviceInfo_({ name, ""})
        {
            initialize();
        }
        
        Pimpl(MidiDeviceComponent* owner, const MidiDeviceInfo info) : owner_(owner), deviceInfo_(info)
        {
            auto midi_input = MidiInput::openDevice(info.identifier, this);
            if (midi_input != nullptr)
            {
                midi_input->start();
                midiIn_.swap(midi_input);
            }
            
            initialize();
        }

        static Font getHeaderFont()
        {
            return Font(16, Font::plain);
        }
        
        static Font getDataTitleFont()
        {
            return Font(16, Font::bold);
        }
        
        static Font getDataFont()
        {
            return Font(16, Font::plain);
        }
        
        void initialize()
        {
            nameLabel_.setText(deviceInfo_.name, dontSendNotification);
            nameLabel_.setFont(getHeaderFont());
            nameLabel_.setColour(Label::backgroundColourId, juce::Colours::black);
            nameLabel_.setColour(Label::textColourId, juce::Colours::white);
            nameLabel_.setJustificationType(Justification::centred);
            
            owner_->addAndMakeVisible(nameLabel_);
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
                Notes& notes = channel.notes_;
                notes.time_ = t;
                
                Note& note = notes.note_[msg.getNoteNumber()];
                note.on_ = msg.isNoteOn();
                note.value_ = msg.getVelocity();
                channel_message = &note;
            }
            else if (msg.isAftertouch())
            {
                Notes& notes = channel.notes_;
                notes.time_ = t;
                
                Note& note = notes.note_[msg.getNoteNumber()];
                note.polyPressure_ = msg.getAfterTouchValue();
                channel_message = &note;
            }
            else if (msg.isController())
            {
                ControlChanges& control_changes = channel.controlChanges_;
                control_changes.time_ = t;
                
                channel_message = &control_changes.controlChange_[msg.getControllerNumber()];
                channel_message->value_ = msg.getControllerValue();
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
        
        static constexpr int HEADER_HEIGHT = 30;
        static constexpr int ROW_SPACING = 1;
        static constexpr int ROW_HEIGHT = 30;
        static constexpr int COLUMN_MARGIN = 4;
        static constexpr int COLUMN_SPACING = 2;
        static constexpr int NOTE_WIDTH = 36;
        static constexpr int VALUE_WIDTH = 30;
        static constexpr int INDICATOR_HEIGHT = 4;
        static constexpr int NOTE_COLUMN_WIDTH =
        COLUMN_MARGIN + NOTE_WIDTH + COLUMN_MARGIN + COLUMN_SPACING +
        COLUMN_MARGIN + VALUE_WIDTH + COLUMN_MARGIN + VALUE_WIDTH + COLUMN_MARGIN;
        static constexpr int CC_COLUMN_WIDTH =
        COLUMN_MARGIN + NOTE_WIDTH + COLUMN_MARGIN + COLUMN_SPACING +
        COLUMN_MARGIN + VALUE_WIDTH + COLUMN_MARGIN;
        static constexpr int STANDARD_WIDTH = NOTE_COLUMN_WIDTH + COLUMN_MARGIN + CC_COLUMN_WIDTH;
        
        static int getStandardWidth()
        {
            return STANDARD_WIDTH;
        }
        
        struct ChannelPaintState
        {
            Time time_;
            int offset_ { 0 };
            bool header_ { false };
        };
        
        void paint(juce::Graphics& g)
        {
            auto t = Time::getCurrentTime();
            ActiveChannels* channels = &channels_;
            if (paused_)
            {
                t = pausedTime_;
                channels = &pausedChannels_;
            }

            g.setColour(Colours::white);
            
            ChannelPaintState state = { t, HEADER_HEIGHT, false };
            
            for (int channel_index = 0; channel_index < 16; ++channel_index)
            {
                auto& channel_messages = channels->channel_[channel_index];
                if (!isExpired(t, channel_messages.time_))
                {
                    state.header_ = false;
                    
                    paintProgramChange(g, state, channel_messages);
                    state.offset_ = paintPitchBend(g, state, channel_messages);
                    int notes_bottom = paintNotes(g, state, channel_messages);
                    int control_changes_bottom = paintControlChanges(g, state, channel_messages);
                    
                    state.offset_ = std::max(state.offset_, std::max(notes_bottom, control_changes_bottom));
                }
            }
            
            lastHeight_ = state.offset_;
        }
        
        int getVisibleHeight() const
        {
            return lastHeight_;
        }
        
        void ensurePaintedChannelHeader(juce::Graphics& g, ChannelPaintState& state, ActiveChannel& channel)
        {
            if (!state.header_)
            {
                paintChannelHeader(g, state, channel);
            }
        }
        
        void paintChannelHeader(juce::Graphics& g, ChannelPaintState& state, ActiveChannel& channel)
        {
            state.header_ = true;
            
            g.setColour(Colours::white);
            g.setFont(getDataTitleFont());
            g.drawText(String("CH ") + String(channel.number_ + 1), COLUMN_MARGIN, state.offset_, NOTE_COLUMN_WIDTH, ROW_HEIGHT, Justification::centredLeft);
            state.offset_ += ROW_HEIGHT;
        }
        
        void paintProgramChange(juce::Graphics& g, ChannelPaintState& state, ActiveChannel& channel)
        {
            int y_offset = -1;
            
            auto& program_change = channel.programChange_;
            if (!isExpired(state.time_, program_change.time_))
            {
                ensurePaintedChannelHeader(g, state, channel);
                
                // write the texts
                
                g.setColour(Colours::white);
                g.setFont(getDataFont());
                int pc_x = COLUMN_MARGIN + NOTE_WIDTH + COLUMN_MARGIN;
                int pc_width = getStandardWidth() - pc_x;
                g.drawText(String("PC ") + String(program_change.value_), pc_x, state.offset_ - ROW_HEIGHT, pc_width, ROW_HEIGHT, Justification::centredRight);
                
                y_offset += ROW_HEIGHT;
            }
        }
        
        int paintPitchBend(juce::Graphics& g, ChannelPaintState& state, ActiveChannel& channel)
        {
            int y_offset = state.offset_;
            
            auto& pitch_bend = channel.pitchBend_;
            if (!isExpired(state.time_, pitch_bend.time_))
            {
                ensurePaintedChannelHeader(g, state, channel);
                
                y_offset = state.offset_;
                
                // draw the backgrounds
                
                Colour fill;
                if (pitch_bend.value_ >= 0x2000)
                {
                    fill = Colours::green.brighter().withAlpha(0.5f);
                }
                else
                {
                    fill = Colours::red.brighter().withAlpha(0.4f);
                }
                
                g.setColour(fill.withAlpha(0.2f));
                int pb_bg_width = STANDARD_WIDTH;
                int pb_bg_height = ROW_HEIGHT - ROW_SPACING * 2 - INDICATOR_HEIGHT;
                g.fillRect(0, y_offset + ROW_SPACING, pb_bg_width, pb_bg_height);
                
                // draw the pitch bend indicator
                
                g.setColour(fill);
                int value_indicator_range = STANDARD_WIDTH / 2;
                int value_indicator_x = value_indicator_range + 1;
                int value_indicator_y = y_offset + ROW_SPACING + pb_bg_height;
                int value_indicator_width = (value_indicator_range * (pitch_bend.value_ - 0x2000)) / 0x2000;
                if (value_indicator_width < 0)
                {
                    value_indicator_x = value_indicator_range + value_indicator_width;
                }
                g.fillRect(value_indicator_x, value_indicator_y, abs(value_indicator_width), INDICATOR_HEIGHT);
                
                g.setColour(fill.withAlpha(0.2f));
                if (value_indicator_width < 0)
                {
                    g.fillRect(0, value_indicator_y, value_indicator_x, INDICATOR_HEIGHT);
                    g.fillRect(value_indicator_range, value_indicator_y, value_indicator_range + 1, INDICATOR_HEIGHT);
                }
                else
                {
                    g.fillRect(0, value_indicator_y, value_indicator_range + 1, INDICATOR_HEIGHT);
                    int indicator_fill_x = value_indicator_x + abs(value_indicator_width);
                    g.fillRect(indicator_fill_x, value_indicator_y, STANDARD_WIDTH - indicator_fill_x, INDICATOR_HEIGHT);
                }
                
                // write the texts
                
                g.setColour(Colours::white.withAlpha(0.8f));
                g.setFont(getDataFont());
                g.drawText(String(pitch_bend.value_), 0, y_offset, STANDARD_WIDTH, ROW_HEIGHT, Justification::centred);
                
                y_offset += ROW_HEIGHT;
            }
            
            return y_offset;
        }
        
        int paintNotes(juce::Graphics& g, ChannelPaintState& state, ActiveChannel& channel)
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
                        ensurePaintedChannelHeader(g, state, channel);
                        
                        if (y_offset == -1)
                        {
                            y_offset = state.offset_;
                        }
                        
                        // draw the backgrounds
                        
                        Colour fill;
                        if (note.on_)
                        {
                            fill = Colours::green.brighter().withAlpha(0.5f);
                            channel.time_ = state.time_;
                            note.time_ = state.time_;
                            notes.time_ = state.time_;
                        }
                        else
                        {
                            fill = Colours::red.brighter().withAlpha(0.4f);
                        }
                        
                        g.setColour(fill);
                        int note_bg_width = COLUMN_MARGIN + NOTE_WIDTH + COLUMN_MARGIN;
                        int note_bg_height = ROW_HEIGHT - ROW_SPACING * 2;
                        g.fillRect(0, y_offset + ROW_SPACING, note_bg_width, note_bg_height);
                        
                        int value_bg_x = note_bg_width + COLUMN_SPACING;
                        int value_bg_width = COLUMN_MARGIN + VALUE_WIDTH + COLUMN_MARGIN + VALUE_WIDTH + COLUMN_MARGIN;
                        int value_bg_height = note_bg_height - INDICATOR_HEIGHT;
                        g.setColour(fill.withAlpha(0.2f));
                        g.fillRect(value_bg_x, y_offset + ROW_SPACING, value_bg_width, value_bg_height);
                        
                        // draw the velocity indicator
                        
                        int vel_bg_width = COLUMN_MARGIN + VALUE_WIDTH;
                        
                        g.setColour(fill);
                        int value_indicator_y = y_offset + ROW_SPACING + value_bg_height;
                        int vel_indicator_width = (vel_bg_width * note.value_) / 127;
                        g.fillRect(value_bg_x, value_indicator_y, vel_indicator_width, INDICATOR_HEIGHT);
                        
                        g.setColour(fill.withAlpha(0.2f));
                        g.fillRect(value_bg_x + vel_indicator_width, value_indicator_y, vel_bg_width - vel_indicator_width, INDICATOR_HEIGHT);
                        
                        // draw the poly pressure indicator
                        
                        int pp_bg_width = COLUMN_MARGIN + VALUE_WIDTH;
                        int pp_bg_x = value_bg_x + COLUMN_MARGIN + VALUE_WIDTH + COLUMN_MARGIN;
                        
                        g.fillRect(pp_bg_x - COLUMN_MARGIN, value_indicator_y, COLUMN_MARGIN, INDICATOR_HEIGHT);
                        
                        g.setColour(fill);
                        int pp_indicator_width = (pp_bg_width * note.polyPressure_) / 127;
                        g.fillRect(pp_bg_x, value_indicator_y, pp_indicator_width, INDICATOR_HEIGHT);
                        
                        g.setColour(fill.withAlpha(0.2f));
                        g.fillRect(pp_bg_x + pp_indicator_width, value_indicator_y, pp_bg_width - pp_indicator_width, INDICATOR_HEIGHT);
                        
                        // write the texts
                        
                        g.setColour(Colours::white);
                        g.setFont(getDataTitleFont());
                        g.drawText(outputNote(i), COLUMN_MARGIN, y_offset, NOTE_WIDTH, ROW_HEIGHT, Justification::centred);
                        
                        g.setColour(Colours::white.withAlpha(0.8f));
                        g.setFont(getDataFont());
                        int value_x = COLUMN_MARGIN + NOTE_WIDTH + COLUMN_MARGIN + COLUMN_SPACING + COLUMN_MARGIN;
                        g.drawText(String(note.value_), value_x, y_offset, VALUE_WIDTH, ROW_HEIGHT, Justification::centred);
                        int polypressure_x = value_x + VALUE_WIDTH + COLUMN_MARGIN;
                        g.drawText(String(note.polyPressure_), polypressure_x, y_offset, VALUE_WIDTH, ROW_HEIGHT, Justification::centred);
                        
                        y_offset += ROW_HEIGHT;
                    }
                }
            }
            
            return y_offset;
        }
        
        int paintControlChanges(juce::Graphics& g, ChannelPaintState& state, ActiveChannel& channel)
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
                        paintControlChangeEntry(g, state, channel, y_offset, String(cc.number_), cc.value_);
                    }
                }
            }
            
            return y_offset;
        }
        
        void paintControlChangeEntry(juce::Graphics& g, ChannelPaintState& state, ActiveChannel& channel, int& yOffset, const String& label, int value)
        {
            ensurePaintedChannelHeader(g, state, channel);
            
            if (yOffset == -1)
            {
                yOffset = state.offset_;
            }
            
            Colour fill = Colours::blue.brighter().withAlpha(0.5f);
            
            int x_offset = NOTE_COLUMN_WIDTH + COLUMN_MARGIN;
            
            // draw the backgrounds
            
            int cc_bg_width = COLUMN_MARGIN + NOTE_WIDTH + COLUMN_MARGIN;
            int cc_bg_height = ROW_HEIGHT - ROW_SPACING * 2;
            g.setColour(fill);
            g.fillRect(x_offset + 0, yOffset + ROW_SPACING, cc_bg_width, cc_bg_height);
            
            int value_bg_x = x_offset + cc_bg_width + COLUMN_SPACING;
            int value_bg_width = COLUMN_MARGIN + VALUE_WIDTH + COLUMN_MARGIN;
            int value_bg_height = cc_bg_height - INDICATOR_HEIGHT;
            g.setColour(fill.withAlpha(0.2f));
            g.fillRect(value_bg_x, yOffset + ROW_SPACING, value_bg_width, value_bg_height);
            
            // draw the value indicator
            
            g.setColour(fill);
            int value_indicator_y = yOffset + ROW_SPACING + value_bg_height;
            int value_indicator_width = (value_bg_width * value) / 127;
            g.fillRect(value_bg_x, value_indicator_y, value_indicator_width, INDICATOR_HEIGHT);
            
            g.setColour(fill.withAlpha(0.2f));
            g.fillRect(value_bg_x + value_indicator_width, value_indicator_y, value_bg_width - value_indicator_width, INDICATOR_HEIGHT);
            
            // write the texts
            
            g.setColour(Colours::white);
            g.setFont(getDataTitleFont());
            g.drawText(label, x_offset + COLUMN_MARGIN, yOffset, NOTE_WIDTH, ROW_HEIGHT, Justification::centred);
            
            g.setColour(Colours::white.withAlpha(0.8f));
            g.setFont(getDataFont());
            int value_x = COLUMN_MARGIN + NOTE_WIDTH + COLUMN_MARGIN + COLUMN_SPACING + COLUMN_MARGIN;
            g.drawText(String(value), x_offset + value_x, yOffset, VALUE_WIDTH, ROW_HEIGHT, Justification::centred);
            
            yOffset += ROW_HEIGHT;
        }
        
        static bool isExpired(const Time& currentTime, Time& messageTime)
        {
            return (currentTime - messageTime).inSeconds() > 2.0;
        }
        
        String outputNote(int noteNumber)
        {
            return MidiMessage::getMidiNoteName(noteNumber, true, true, DEFAULT_OCTAVE_MIDDLE_C);
        }
        
        void resized()
        {
            dirty_ = true;
            
            auto area = owner_->getLocalBounds();
            nameLabel_.setBounds(area.removeFromTop(HEADER_HEIGHT));
        }
        
        void setPaused(bool paused)
        {
            if (paused)
            {
                pausedTime_ = Time::getCurrentTime();
                pausedChannels_ = channels_;
            }
            
            if (midiIn_.get() == nullptr)
            {
                nameLabel_.setText(deviceInfo_.name + (paused ? " (paused)": ""), dontSendNotification);
            }
            
            paused_ = paused;
        }
        
        MidiDeviceComponent* const owner_;
        const MidiDeviceInfo deviceInfo_;
        std::unique_ptr<juce::MidiInput> midiIn_ { nullptr };
        std::atomic_bool dirty_ { true };
        Time lastRender_;
        bool paused_ { false };
        
        juce::Label nameLabel_;
        
        ActiveChannels channels_;
        
        Time pausedTime_;
        ActiveChannels pausedChannels_;
        
        int lastHeight_ { 0 };
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pimpl)
    };
    
    MidiDeviceComponent::MidiDeviceComponent(const String& name) : pimpl_(new Pimpl(this, name)) {}
    MidiDeviceComponent::MidiDeviceComponent(const MidiDeviceInfo& info) : pimpl_(new Pimpl(this, info)) {}
    MidiDeviceComponent::~MidiDeviceComponent() = default;
    
    int MidiDeviceComponent::getStandardWidth()         { return Pimpl::getStandardWidth(); }
    
    void MidiDeviceComponent::render()                  { pimpl_->render(); }
    void MidiDeviceComponent::paint(juce::Graphics& g)  { pimpl_->paint(g); }
    void MidiDeviceComponent::resized()                 { pimpl_->resized(); }
    
    void MidiDeviceComponent::handleIncomingMidiMessage(const MidiMessage& m)   { pimpl_->handleIncomingMidiMessage(nullptr, m); };
    
    void MidiDeviceComponent::setPaused(bool p)         { pimpl_->setPaused(p); }

    int MidiDeviceComponent::getVisibleHeight() const   { return pimpl_->getVisibleHeight(); }
}
