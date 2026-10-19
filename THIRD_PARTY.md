# Third-party code and assets

The MIT licence covers Squarewave Studio's original work in this repository.
It does not relicense someone else's code, sound design, fonts or trademarks.

## op1r

[op1r-src](https://github.com/pcppcp/op1r-src) is by pcppcp and contributors,
under GPL-3.0-or-later with its additional OP-1 firmware permission.
No op1r source, SDK headers or GPL host adapters are included here.
The [native GIN source](https://github.com/joseph-holland/op1r-src/tree/ec5d6f722df05b77f80706db239352992937f4d7/synth_sdk/experimental/gin/standalone) is maintained separately
in the op1r fork, with its GPL terms and additional firmware permission.

The shared Drum Synth API, loader and native adapters belong in the separate
op1r contribution. If a distributed plugin contains or derives from GPL code,
its corresponding source and licence must reflect that. Keeping the reusable
synthesis core under MIT does not make the combined binary MIT-only.

## libpo32

GIN's synthesis core derives from [libpo32](https://github.com/ericlewis/libpo32),
Copyright (c) 2026 Eric Lewis, under MIT. The synthesis subset is included in `mods/gin/core/third_party/libpo32`,
with its MIT licence and provenance notice. Preserve both when reusing it.

## CNTR RNDM

GIN's current factory bank adapts eight of
[CNTR RNDM's free PO-32 kits](https://youtube.com/playlist?list=PLlkIAiu0_jPw3DgmZm1RbmFe86rzuWU2o).
The original sounds are CNTR RNDM's; the 24-key layout and additional variations
are GIN adaptations. The published [Free808 kit description](https://www.youtube.com/watch?v=NCpdgl2XTQg)
says the creator's samples and kits are copyright free. We retain the source
records and creator credit. Factory-kit data is supplied in the linked native source, with per-kit
source links and the creator's published permission statement. This repository's
MIT core contains no factory-kit data. No endorsement is claimed.

## Host screenshots

The GIN screenshots show our UI rendered on a host with the original OP-1
font. The native renderer is in the linked native source. Extracted fonts are not
shipped in either source tree.
Teenage Engineering's font assets, firmware and trademarks remain theirs; the
MIT licence does not grant rights to those materials. No extracted font data
or stock firmware is included.

## Independence And Trademarks

GIN and these mods are independently developed by Squarewave Studio. They are
not affiliated with, authorised by, sponsored by or endorsed by Teenage
Engineering AB or Sonic Charge / NuEdge Development AB.

OP-1, Pocket Operator, PO-32, Tonic and Teenage Engineering are names and
trademarks of Teenage Engineering AB. Microtonic, µTonic and Sonic Charge are
names and trademarks of NuEdge Development AB. They are used only to identify
compatible devices, software and formats. All trademarks belong to their
respective owners. No ownership of those names or endorsement is claimed.

Code, content and trademark rights are separate. A compatibility statement,
credit or disclaimer does not grant permission to redistribute proprietary
source, firmware, fonts, presets or other assets.

This repository contains documentation, host previews and the reusable synthesis
core. The linked native source has its own component notices. No installable
plugin, stock firmware image or extracted font data is distributed here.
