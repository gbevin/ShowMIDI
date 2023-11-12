#!/bin/bash

if [ $# -eq 0 ]; then
    echo "The release version is required as an argument"
    exit 1
fi

export RELEASE_VERSION="$1"
export PATH_TO_JUCE=/home/gbevin/JUCE

rm -rfv Builds/LinuxMakefile/build/

PROJECT_DIR=$PWD

pushd Builds/LinuxMakefile
make CONFIG=Release
popd

cmake -BBuilds/LinuxMakefile/build/clap -DCMAKE_BUILD_TYPE=Release
pushd Builds/LinuxMakefile
cmake --build build/clap --config Release

pushd build
STAGE_DIR="ShowMIDI-$RELEASE_VERSION"
mkdir -p $STAGE_DIR/vst $STAGE_DIR/vst3 $STAGE_DIR/clap $STAGE_DIR/lv2
mv ShowMIDI $STAGE_DIR
mv ShowMIDI.so $STAGE_DIR/vst
mv ShowMIDI.vst3 $STAGE_DIR/vst3
mv clap/ShowMIDI_artefacts/Release/ShowMIDI.clap $STAGE_DIR/clap
mv ShowMIDI.lv2 $STAGE_DIR/lv2
cp $PROJECT_DIR/*.md $STAGE_DIR

ARCHIVE_FILE="$PROJECT_DIR/Installers/ShowMIDI-Linux-Ubuntu-x64-$RELEASE_VERSION.tar.bz2"
tar cvp $STAGE_DIR | bzip2 > $ARCHIVE_FILE
echo "Finished building `realpath $ARCHIVE_FILE`"

popd

popd