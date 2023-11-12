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

struct UwynLookAndFeel::Pimpl
{
    Pimpl() :
        jetbrainMono_(Typeface::createSystemTypefaceFor(BinaryData::JetBrainsMonoRegular_ttf, BinaryData::JetBrainsMonoRegular_ttfSize)),
        jetbrainMonoItalic_(Typeface::createSystemTypefaceFor(BinaryData::JetBrainsMonoItalic_ttf, BinaryData::JetBrainsMonoItalic_ttfSize)),
        jetbrainMonoSemiBold_(Typeface::createSystemTypefaceFor(BinaryData::JetBrainsMonoSemiBold_ttf, BinaryData::JetBrainsMonoSemiBold_ttfSize)),
        jetbrainMonoSemiBoldItalic_(Typeface::createSystemTypefaceFor(BinaryData::JetBrainsMonoSemiBoldItalic_ttf, BinaryData::JetBrainsMonoSemiBoldItalic_ttfSize))
    {
    }

    Typeface::Ptr getTypefaceForFont(const Font& font)
    {
        bool bold = font.getTypefaceStyle().toLowerCase().contains("bold") || font.isBold();
        bool italic = font.getTypefaceStyle().toLowerCase().contains("italic") || font.isItalic();
        if (bold)
        {
            if (italic)
            {
                return jetbrainMonoSemiBoldItalic_;
            }
            else
            {
                return jetbrainMonoSemiBold_;
            }
        }

        if (italic)
        {
            return jetbrainMonoItalic_;
        }
        else
        {
            return jetbrainMono_;
        }
    }

    Typeface::Ptr jetbrainMono_;
    Typeface::Ptr jetbrainMonoItalic_;
    Typeface::Ptr jetbrainMonoSemiBold_;
    Typeface::Ptr jetbrainMonoSemiBoldItalic_;
};

UwynLookAndFeel::UwynLookAndFeel() : pimpl_(new Pimpl())
{
    setColourScheme(getDarkColourScheme());
    
    setColour(TextEditor::focusedOutlineColourId, Colour(0x00000000));
    setColour(TextEditor::outlineColourId, Colour(0x00000000));
    setColour(ComboBox::outlineColourId, Colour(0x00000000));
    setColour(ComboBox::buttonColourId, Colour(0xff2e2e2e));
    setColour(TextButton::buttonColourId, Colour(0xff2e2e2e));
    setColour(ScrollBar::thumbColourId, Colour(0xff999999));
    setColour(ScrollBar::trackColourId, Colour(0xff666666));
    setColour(AlertWindow::backgroundColourId, Colour(0xff1e1e1e));
    setColour(AlertWindow::outlineColourId, Colour(0xff2e2e2e));
}

UwynLookAndFeel::~UwynLookAndFeel() = default;

Typeface::Ptr UwynLookAndFeel::getTypefaceForFont(const Font& font)
{
    Typeface::Ptr r = pimpl_->getTypefaceForFont(font);
    if (r)
    {
        return r;
    }
    return LookAndFeel::getTypefaceForFont(font);
}

LookAndFeel_V4::ColourScheme UwynLookAndFeel::getDarkColourScheme()
{
    return {
        0xff1e1e1e, 0xff323232, 0xff1e1e1e,
        0xff989898, 0xffffffff, 0xffa2a2a2,
        0xffffffff, 0xff4f4f4f, 0xffffffff };
}
