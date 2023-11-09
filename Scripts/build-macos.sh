#!/bin/bash

if [ $# -eq 0 ]; then
    echo "The release version is required as an argument"
    exit 1
fi

export RELEASE_VERSION="$1"
export PATH_TO_JUCE=/Users/gbevin/JUCE

BUILD_DIR=$PWD/Builds/MacOSX/build
rm -rfv $BUILD_DIR

# build release artifacts
xcodebuild -project ./Builds/MacOSX/showmidi.xcodeproj -config Release SYMROOT=build -scheme "ShowMIDI - All"

cmake -BBuilds/MacOSX/build/clap -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64"
cmake --build $BUILD_DIR/clap --config Release

# update packager file with version number and generate it in the build
# directory, updating relevant paths
sed "s/REPLACE_VERSION/$RELEASE_VERSION/g" Installers/ShowMIDI.pkgproj | \
    sed "s/Builds\/MacOSX/..\/..\/Builds\/MacOSX/g" > $BUILD_DIR/ShowMIDI_versioned.pkgproj
# copy other files that are needed by the installer to the new location
cp Installers/uninstall.sh $BUILD_DIR
cp Installers/gpl-3.0.txt $BUILD_DIR

# build the installer package in the build directory
/usr/local/bin/packagesbuild --project $BUILD_DIR/ShowMIDI_versioned.pkgproj -v
mv $BUILD_DIR/ShowMIDI.pkg $BUILD_DIR/ShowMIDI-${RELEASE_VERSION}.pkg

ARCHIVE_FILE="$BUILD_DIR/ShowMIDI-macOS-${RELEASE_VERSION}.zip"
pushd "$BUILD_DIR"
zip -r  "${ARCHIVE_FILE}" "ShowMIDI-${RELEASE_VERSION}.pkg"
popd

echo "Finished building `realpath $ARCHIVE_FILE`"
