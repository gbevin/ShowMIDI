#!/bin/sh
SCRIPT_DIR="$1"
if [ -e /usr/libexec/PlistBuddy ]
then
	/usr/libexec/PlistBuddy -c "add :NSExtension:NSExtensionAttributes:AudioComponents:0:tags:1 string 'size:{296, 600}'" $SCRIPT_DIR/Builds/iOS/Info-AUv3_AppExtension.plist
fi
