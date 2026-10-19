# Compatibility

## GIN 1.0 Standalone Build

The accepted implementation targets the original OP-1 and stock firmware #246.
It is not for OP-1 field, OP-XY or Pocket Operators. Acceptance covers the
standalone GIN build, including sound editing, naming, snapshots and working
slots across restart. It does not establish compatibility with a combined
community firmware.

The [source release](BUILDERS.md) includes the core and links to the complete
native build and integration source. No full Teenage Engineering firmware image
is hosted here. Firmware builders must identify their tested host version,
output checksum and recovery route when distributing an integrated build.
Do not use a different firmware version as input or combine unrelated binary
patches and assume they are compatible.

## OP1P Plugin

`gin.op1p` is not available yet. The named DrumSynth host integration is being
prepared separately from the sample-based Drum+ engine. The eventual release
must name its minimum compatible host and test loading, engine switching,
saving, cold boot, removal and reinstallation with saved GIN state present.

A source PR or a successful host test is not a tested downloadable plugin.

## Before Changing Firmware

Back up tapes, user sounds and other files you need to keep. Keep a known
working firmware and its recovery instructions available. Follow the exact
release's installation instructions rather than treating the OP1P and firmware
update routes as interchangeable.

Saved GIN state needs a compatible engine to restore it. Missing-engine boot
and state preservation are explicit integration requirements, not guarantees
for arbitrary firmware swaps.
