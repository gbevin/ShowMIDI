# JUCE patches

JUCE is pinned here as a git submodule, so these patches cannot be committed
with the rest of the source. `Scripts/apply-juce-patches.sh` applies them to the
checkout, and the Linux build calls it.

- **`juce_alsa-midi-1-bytestream.patch`**: JUCE 9 registers every ALSA client as
  MIDI 2.0, which makes the sequencer carry UMP. Sending then combines the
  controllers of an RPN or NRPN into one MIDI 2.0 message, and receiving decodes
  that message to nothing, so RPN and NRPN traffic disappears between MIDI 1.0
  applications on Linux, and ShowMIDI's RPN and NRPN panels stay empty. The fix
  registers as a MIDI 1.0 client and always sends a bytestream.

Only Linux is affected: the patched file is not compiled on the other
platforms. The same patch is applied in SendMIDI, ReceiveMIDI and RouteMIDI,
which vendor their JUCE modules and so carry it in their own sources.

## Applying

```
./Scripts/apply-juce-patches.sh
```

Running it again is harmless, and it reports which patches were already
applied. After changing the submodule to a new JUCE release, run it again and
regenerate any patch it can no longer apply. JUCE ships these sources with CRLF
line endings while the patches are LF, which is why they are applied with
`git apply --ignore-whitespace`.
