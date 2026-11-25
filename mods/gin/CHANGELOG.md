# Release Notes

## 1.0.1 — 25 November 2026

The Mod noise envelope follows measured reference behaviour: bursts during the
attack, each falling by a factor of 16 before restarting, followed by Exp decay.
Float and integer voices use the same sample-index schedule. Integer rendering
uses conditional subtraction without callback division or modulo.

Saved normalised parameters and GNS1 kit data keep their formats. Sounds using
the corrected Mod noise-envelope mode can change audibly; Exp and Linear modes
and oscillator reset phase are unchanged. The OP-1 profile remains 44.1 kHz
and four voices. Firmware builders must regenerate prepared voices and factory
dictionaries with the matching preparation code.

This remains a source-only release. Native credits and packaging are maintained
by each host integration, separately from this core.

## 1.0.0

Source-only release of the reusable synthesis core, with links to the
original native editor, artwork, kits and integration source. The OP-1
integration profile stays at 44.1 kHz and four voices, without FAST mode.

Guarded-noise arithmetic does less work. A new retirement predicate identifies
immutable voice states whose future core samples are exactly zero. Host
adapters still need to drain their resampler and fades before retiring a voice.

Long sine pitch modulation is stabilised using Eric Lewis's libpo correction.
Hundekraft contributed the optional block-renderer local-state optimisation.
The core tests cover both rounding profiles and block settings, mixed block
lengths, retrigger/release, filter equivalence and long sine modulation.

Documentation links to libpo under its current name and credits Hundekraft's
separate native resampling, output-level matching and host save improvements.
Native packaging and font rendering remain with each firmware integration.
No installable plugins, firmware images or extracted font data are included.

## Earlier Standalone Source

GIN for the original OP-1 #246: 24-key kits, four synthesis voices, per-sound
A/B morphing, the full Advanced editor and individual sound names.

It comes with eight factory kits adapted from CNTR RNDM's PO-32 kits, with
creator credit. Working-slot edits survive restart, and holding a snapshot
button saves a separate named preset.

Saving no longer crashes: GIN now avoids a temporary database journal that the
stock filesystem can't open. The fix is recorded in the linked standalone source.

Known limitation: very heavy combined load can still cause audio breakup, so
GIN stays at four voices. OP1P support and op1r integration aren't part of this
release.

The source release includes the reusable synthesis core and links to the
complete native integration, factory kit data and build instructions.
A loadable OP1P file isn't part of this release.
