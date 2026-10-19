# Compatibility

## GIN 1.0 Standalone Build

GIN 1.0 runs on the original OP-1 with stock firmware #246. It isn't for OP-1
field, OP-XY or Pocket Operators. I've tested the standalone build on hardware,
including sound editing, naming, snapshots and working slots across restart. I
haven't tested it inside a combined community firmware.

The [source release](BUILDERS.md) includes the core and links to the complete
native build and integration source. No Teenage Engineering firmware image is
hosted here. If you share an integrated build, it helps to say which host
version you tested, the output checksum and how to recover. Builds from a
different firmware version, or mixed with unrelated binary patches, aren't
known to work.

## OP1P Plugin

`gin.op1p` isn't available yet. I'm working on a DrumSynth host integration,
separate from the sample-based Drum+ engine. When the plugin ships, its release
notes will name the minimum compatible host and confirm loading, engine
switching, saving, cold boot, removal and reinstallation with saved GIN state.

Until then, there is no tested plugin to download.

## Before Changing Firmware

Back up tapes, user sounds and other files you need to keep. Keep a known
working firmware and its recovery instructions to hand. Follow the installation
instructions for the exact release you're using: the OP1P and firmware update
routes aren't interchangeable.

Saved GIN state needs a compatible engine to restore it. Swapping to a firmware
without GIN may not keep that state.
