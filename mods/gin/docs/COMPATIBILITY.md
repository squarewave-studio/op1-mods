# Compatibility

## Source Release

GIN 1.0.0 supplies the reusable synthesis core and links to the original native
instrument source for the original OP-1. The integration profile uses 44.1 kHz
synthesis and four voices.

This release has no `.knl`, `.op1p` or firmware image. Firmware maintainers
provide their own packages, native font rendering and host testing. Use their
instructions and supported-version list for the package you install.

## Original Native Implementation

The linked standalone source targets original OP-1 firmware #246. Its
[source and build instructions](BUILDERS.md) remain available. That pinned
implementation predates the core updates in v1.0.0; it is not a packaged build
of this release. Memory, hooks, preparation data and voice layouts need to
match the host integration.

[Hundekraft's OP-1 Workshop](https://github.com/Hundekraft/op1-workshop) maintains
its own native Gin integration. There is no verified `gin.op1p` for standard
OP1R supplied here. GIN needs a synthesised-drum host route, separate from
the sample-based Drum+ path.

This release does not target OP-1 field, OP-XY or Pocket Operators.

## Before Changing Firmware

Back up tapes, user sounds and other files you need to keep. Keep a known
working firmware and follow the installation instructions for the host you use.
Saved Gin state needs a compatible engine to restore it.
