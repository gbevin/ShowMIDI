#! /bin/bash

APP="ShowMIDI"

rm -f -Rf "/Users/$USER/Applications/$APP.app";
rm -f -Rf "/Applications/$APP.app";

rm -f -Rf "/Users/$USER/Library/Audio/Plug-Ins/Components/$APP.component";
rm -f -Rf "/Library/Audio/Plug-Ins/Components/$APP.component";

rm -f -Rf "/Users/$USER/Library/Audio/Plug-Ins/VST/$APP.vst";
rm -f -Rf "/Library/Audio/Plug-Ins/VST/$APP.vst";

rm -f -Rf "/Users/$USER/Library/Audio/Plug-Ins/VST3/$APP.vst3";
rm -f -Rf "/Library/Audio/Plug-Ins/VST3/$APP.vst3";

exit 0
