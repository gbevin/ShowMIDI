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
#include "UwynLookAndFeel.h"

using namespace showmidi;

UwynLookAndFeel::UwynLookAndFeel()
{
    setColourScheme(getDarkColourScheme());
    
    setColour(TextEditor::focusedOutlineColourId, juce::Colour(0x00000000));
    setColour(TextEditor::outlineColourId, juce::Colour(0x00000000));
    setColour(ComboBox::outlineColourId, juce::Colour(0x00000000));
    setColour(ComboBox::buttonColourId, juce::Colour(0xff2e2e2e));
    setColour(TextButton::buttonColourId, juce::Colour(0xff2e2e2e));
    setColour(ScrollBar::thumbColourId, juce::Colour(0xff999999));
    setColour(ScrollBar::trackColourId, juce::Colour(0xff666666));
    setColour(AlertWindow::backgroundColourId, juce::Colour(0xff1e1e1e));
    setColour(AlertWindow::outlineColourId, juce::Colour(0xff2e2e2e));
}

UwynLookAndFeel::~UwynLookAndFeel()
{
}

LookAndFeel_V4::ColourScheme UwynLookAndFeel::getDarkColourScheme()
{
    return {
        0xff1e1e1e, 0xff323232, 0xff1e1e1e,
        0xff989898, 0xffffffff, 0xffa2a2a2,
        0xffffffff, 0xff4f4f4f, 0xffffffff };
}

void UwynLookAndFeel::drawPopupMenuBackground(Graphics& g, int, int)
{
    const Colour background(findColour(PopupMenu::backgroundColourId));
    g.fillAll(background);
}

void UwynLookAndFeel::drawButtonBackground(Graphics &g, Button &button, const Colour &backgroundColour, bool isMouseOverButton, bool isButtonDown)
{
    if (!button.getClickingTogglesState())
    {
        LookAndFeel_V4::drawButtonBackground(g, button, backgroundColour, isMouseOverButton, isButtonDown);
        return;
    }
    
    const float outlineThickness = 1.5f;
    const float halfThickness = outlineThickness*0.5f;
    
    const float indentL = halfThickness;
    const float indentR = halfThickness;
    const float indentT = halfThickness;
    const float indentB = halfThickness;
    
    const float x = indentL;
    const float y = indentT;
    const float width = button.getWidth() - indentL - indentR;
    const float height = button.getHeight() - indentT - indentB;
    
    const Colour colour(button.getToggleState() ? juce::Colour(0xaaffffff) : juce::Colour(0x88ffffff));
    
    if (width <= outlineThickness || height <= outlineThickness)
    {
        return;
    }
    
    if (button.getToggleState())
    {
        Path outline;
        const float cornerSize = jmin(width*0.25f, height*0.25f);
        outline.addRoundedRectangle(x, y, width, height, cornerSize, cornerSize);
        
        g.setColour(colour.withAlpha(0.3f));
        g.fillPath(outline);
        
        if (button.isEnabled())
        {
            g.setColour(colour);
            g.strokePath(outline, PathStrokeType(outlineThickness));
        }
    }
}

void UwynLookAndFeel::drawButtonText(Graphics &g, TextButton &button, bool isMouseOverButton, bool isButtonDown)
{
    if (!button.getClickingTogglesState())
    {
        LookAndFeel_V4::drawButtonText(g, button, isMouseOverButton, isButtonDown);
        return;
    }
    
    if (button.isEnabled())
    {
        g.setColour(button.getToggleState() ? juce::Colour(0x88ffffff) : juce::Colour(0x66ffffff));
    }
    else
    {
        g.setColour(juce::Colour(0xff333333));
    }
    
    const float width = float(button.getWidth());
    const float height = float(button.getHeight());
    const float hspace  = width/8.f;
    const float vspace = height/8.f;
    
    float fontHeight = button.getHeight()-vspace*2.f;
    g.setFont(getNarrowFont(fontHeight));
    g.drawFittedText(button.getButtonText(),
                     int(hspace), int(vspace),
                     button.getWidth()-int(hspace*2.f), int(fontHeight),
                     Justification::centred, 1);
}

Font UwynLookAndFeel::getNarrowFont(float height)
{
#if JUCE_MAC
    return Font("Arial Narrow", height, Font::plain);
#else
    return Font("Tahoma", height, Font::plain).withHorizontalScale(0.88f);
#endif
}
