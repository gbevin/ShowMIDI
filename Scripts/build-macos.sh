#!/bin/bash

if [ $# -eq 0 ]; then
    echo "The release version is required as an argument"
    exit 1
fi

export RELEASE_VERSION="$1"
export PATH_TO_JUCE=/Users/gbevin/JUCE

rm -rfv Builds/MacOSX/build/

# build release artifacts
xcodebuild -project ./Builds/MacOSX/showmidi.xcodeproj -config Release SYMROOT=build -scheme "ShowMIDI - All"

cmake -BBuilds/MacOSX/build/clap -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64"
cmake --build Builds/MacOSX/build/clap --config Release

# update packager file with version number and generate it in the build
# directory, updating relevant paths
sed "s/REPLACE_VERSION/$RELEASE_VERSION/g" Installers/ShowMIDI.pkgproj | \
    sed "s/Builds\/MacOSX/..\/..\/Builds\/MacOSX/g" > Builds/MacOSX/build/ShowMIDI_versioned.pkgproj
# copy other files that are needed by the installer to the new location
cp Installers/uninstall.sh Builds/MacOSX/build/
cp Installers/gpl-3.0.txt Builds/MacOSX/build/

# build the installer package in the build directory
/usr/local/bin/packagesbuild --project Builds/MacOSX/build/ShowMIDI_versioned.pkgproj -v
mv Builds/MacOSX/build/ShowMIDI.pkg Builds/MacOSX/build/ShowMIDI-${RELEASE_VERSION}.pkg

ARCHIVE_FILE="Builds/MacOSX/build/ShowMIDI-macOS-${RELEASE_VERSION}.zip"
zip -r  "${ARCHIVE_FILE}" "Builds/MacOSX/build/ShowMIDI-${RELEASE_VERSION}.pkg"

echo "Finished building `realpath $ARCHIVE_FILE`"
