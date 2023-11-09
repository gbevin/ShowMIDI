#!/bin/bash

if [ $# -eq 0 ]; then
    echo "The release version is required as an argument"
    exit 1
fi

export RELEASE_VERSION="$1"

rm -rfv Builds/LinuxMakefile/build/

PROJECT_DIR=$PWD

pushd Builds/LinuxMakefile
make Config=Release
popd

cmake -BBuilds/LinuxMakefile/build/clap -DCMAKE_BUILD_TYPE=Release
pushd Builds/LinuxMakefile
cmake --build build/clap --config Release

STAGE_DIR="build/ShowMIDI-$RELEASE_VERSION"
mkdir -p $STAGE_DIR/vst $STAGE_DIR/vst3 $STAGE_DIR/clap
mv build/ShowMIDI $STAGE_DIR
mv build/ShowMIDI.so $STAGE_DIR/vst
mv build/ShowMIDI.vst3 $STAGE_DIR/vst3
mv build/clap/ShowMIDI_artefacts/Release/ShowMIDI.clap $STAGE_DIR/clap
cp $PROJECT_DIR/*.md $STAGE_DIR

ARCHIVE_FILE="build/showmidi-linux-ubuntu-x64-$RELEASE_VERSION.tar.bz2"
tar cvp $STAGE_DIR | bzip2 > $ARCHIVE_FILE
echo "Finished building `realpath $ARCHIVE_FILE`"

popd