# Release Notes

## 1.0.0 Source Release

GIN for the original OP-1 #246: 24-key kits, four synthesis voices, per-sound
A/B morphing, the full Advanced editor and individual sound names.

It comes with eight factory kits adapted from CNTR RNDM's PO-32 kits, with
creator credit. Working-slot edits survive restart, and holding a snapshot
button saves a separate named preset.

Saving no longer crashes: GIN now avoids a temporary database journal that the
stock filesystem can't open. If you tried an earlier test build, please use this
one instead.

Known limitation: very heavy combined load can still cause audio breakup, so
GIN stays at four voices. OP1P support and op1r integration aren't part of this
release.

The source release includes the reusable synthesis core and links to the
complete native integration, factory kit data and build instructions.
A loadable OP1P file isn't part of this release.
