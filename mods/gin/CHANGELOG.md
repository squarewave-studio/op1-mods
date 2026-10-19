# Release Notes

## 1.0.0 Source Release

GIN for the original OP-1 #246: 24-key kits, four synthesis voices, per-sound
A/B morphing, the full Advanced editor and individual sound names.

The accepted build includes eight factory kits adapted from CNTR RNDM's PO-32
kits, with creator credit retained. Working-slot edits survive restart, while
holding a snapshot button creates a separate named preset.

The final save correction avoids a temporary database journal that the stock
filesystem cannot open. Earlier test candidates with the save crash are withdrawn.

Known limitation: extreme combined load can still cause audio breakup. The
four-voice limit is retained. OP1P support and combined op1r integration are not
part of the standalone hardware acceptance.

The source release includes the reusable synthesis core and links to the
complete native integration, factory kit data and build instructions.
A loadable OP1P file is not part of this release.
