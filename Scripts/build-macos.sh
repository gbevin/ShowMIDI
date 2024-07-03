#!/bin/bash

if [ $# -eq 0 ]; then
    echo "The release version is required as an argument"
    exit 1
fi

PROJECT_DIR=$PWD

export RELEASE_VERSION="$1"
export PATH_TO_JUCE="$PROJECT_DIR/JUCE"

SIGN_ID="Developer ID Application: Uwyn, LLC (AGZT8GVS7G)"
BUILD_DIR=$PWD/Builds/MacOSX/build

rm -rfv $BUILD_DIR

# build release artifacts
echo "Building all ProJucer artifacts"
xcodebuild -project ./Builds/MacOSX/showmidi.xcodeproj -destination "generic/platform=macOS,name=Any Mac" -config Release SYMROOT=build -scheme "ShowMIDI - All"

echo "Building CLAP plugin"
cmake -BBuilds/MacOSX/build/clap -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64"
cmake --build $BUILD_DIR/clap --config Release

# sign each individual artifact
echo "Codesigning all artifacts"
codesign -f -s "$SIGN_ID" Builds/MacOSX/build/Release/ShowMIDI.app --deep --strict --timestamp --options=runtime
codesign -f -s "$SIGN_ID" Builds/MacOSX/build/Release/ShowMIDI.appex --deep --strict --timestamp --options=runtime
codesign -f -s "$SIGN_ID" Builds/MacOSX/build/Release/ShowMIDI.component --deep --strict --timestamp --options=runtime
codesign -f -s "$SIGN_ID" Builds/MacOSX/build/Release/ShowMIDI.vst --deep --strict --timestamp --options=runtime
codesign -f -s "$SIGN_ID" Builds/MacOSX/build/Release/ShowMIDI.vst3 --deep --strict --timestamp --options=runtime
codesign -f -s "$SIGN_ID" Builds/MacOSX/build/Release/ShowMIDI.lv2 --deep --strict --timestamp --options=runtime
codesign -f -s "$SIGN_ID" Builds/MacOSX/build/Release/ShowMIDI.lv2/ShowMIDI --deep --strict --timestamp --options=runtime
codesign -f -s "$SIGN_ID" Builds/MacOSX/build/clap/ShowMIDI_artefacts/Release/ShowMIDI.clap --deep --strict --timestamp --options=runtime

# update packager file with version number and generate it in the build
# directory, updating relevant paths
echo "Generating installer packages project"
sed "s/REPLACE_VERSION/$RELEASE_VERSION/g" Installers/ShowMIDI.pkgproj | \
    sed "s/Builds\/MacOSX/..\/..\/Builds\/MacOSX/g" > $BUILD_DIR/ShowMIDI_versioned.pkgproj
# copy other files that are needed by the installer to the new location
cp Installers/uninstall.sh $BUILD_DIR
cp Installers/gpl-3.0.txt $BUILD_DIR

# build the installer package in the build directory
echo "Building installer package"
/usr/local/bin/packagesbuild --project $BUILD_DIR/ShowMIDI_versioned.pkgproj -v
mv $BUILD_DIR/ShowMIDI.pkg $BUILD_DIR/ShowMIDI-${RELEASE_VERSION}.pkg

ARCHIVE_FILE="$PROJECT_DIR/Installers/ShowMIDI-macOS-${RELEASE_VERSION}.zip"
pushd "$BUILD_DIR"

PKG_FILE="ShowMIDI-${RELEASE_VERSION}.pkg"

# Notarization
echo "Notarizating installer"
xcrun notarytool submit "${PKG_FILE}" --keychain-profile "notary-uwyn.com" --wait
xcrun stapler staple "${PKG_FILE}"
spctl --assess -vv --type install "${PKG_FILE}"

echo "Creating zip archive of installer"
zip -r  "${ARCHIVE_FILE}" "${PKG_FILE}"
popd

echo "Finished building `realpath $ARCHIVE_FILE`"
