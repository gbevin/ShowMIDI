#!/bin/bash
#
# Applies the patches in Patches/ to the JUCE submodule. JUCE is pinned as a
# submodule rather than vendored, so the patches cannot be committed with the
# rest of the source and have to be applied to the checkout before building.
# Running this more than once is harmless: a patch already applied is skipped.
#
# Usage: apply-juce-patches.sh

set -e

PROJECT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
cd "$PROJECT_DIR/JUCE"

for patch in "$PROJECT_DIR"/Patches/*.patch; do
    name="$(basename "$patch")"

    # JUCE ships these sources with CRLF while the patches are LF, so the
    # whitespace of the context lines cannot be compared
    if git apply --ignore-whitespace --reverse --check "$patch" 2>/dev/null; then
        echo "already applied: $name"
    elif git apply --ignore-whitespace "$patch"; then
        echo "applied: $name"
    else
        echo "could not apply: $name"
        exit 1
    fi
done
