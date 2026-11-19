# GIN For Firmware Builders

GIN 1.0.0 is a source-only release for the original OP-1. The reusable core
is included here; the original native instrument source is linked separately.
Firmware maintainers supply and test their own host integration and packages.

## Version 1.0.0 Core

The core includes guarded-filter optimisations, an exact-silence retirement
predicate, stable sine pitch modulation and Hundekraft's optional block-renderer
local-state optimisation. The OP-1 integration profile uses 44.1 kHz synthesis,
four voices and scalar rendering (`GIN_BLOCK_STATE=0`). There is no FAST mode
in that profile.

The native resampler, output-level matching and host save fixes belong to the
host integration. They are not part of this reusable core. This release does
not provide `.knl`, `.op1p` or firmware files.

## Standalone Source

- [Reusable synthesis core](../core/README.md): MIT-licensed synthesis,
  preparation code, libpo PO-32 subset and functional tests.
- [Complete native source](https://github.com/joseph-holland/op1r-src/tree/ec5d6f722df05b77f80706db239352992937f4d7/synth_sdk/experimental/gin/standalone): the drum adapter,
  editor, eight 24-key factory kits, saved state, SDK inputs and build recipe.
- [Integration guide](https://github.com/joseph-holland/op1r-src/tree/ec5d6f722df05b77f80706db239352992937f4d7/synth_sdk/experimental/gin/standalone/INTEGRATION.md) and
  [machine-readable hook map](https://github.com/joseph-holland/op1r-src/tree/ec5d6f722df05b77f80706db239352992937f4d7/synth_sdk/experimental/gin/standalone/INTEGRATION.json).

The native source link is pinned to the original standalone implementation.
It predates the core updates in this release. It lives in the op1r fork because
it uses GPL-licensed SDK and host code. The reusable core keeps its MIT licence;
the combined native instrument has the GPL terms and additional firmware
permission described in that source tree. Preserve all component notices.

## Build

The core can be checked with a host C compiler:

```sh
cd mods/gin/core
make test
```

For the instrument, follow the native source's README. It pins GNU binutils 2.38
and GCC 8.5.0 and has a relative Makefile. Build inputs derived from stock #246
are generated from the builder's own firmware archive. No firmware image,
extracted font data or original instruction prefixes are distributed.

I've checked the source rebuild with the pinned compiler already installed, but
not with a freshly downloaded build of that compiler.

## Integrate With A Host

The original native source documents the 21 hook sites, original continuations,
callback ABI, memory ranges and preset/save lifecycle. GIN shares code/BSS
ranges and several hooks with op1r. Combined firmware needs one linker layout
and shared dispatchers at those sites.

The core and native integration are separate source versions. An integrator
needs matching preparation data, voice layouts and adapter code when adopting
core changes. The linked standalone source is not a packaged build of this
release.

| Host | Source And Packaging |
| --- | --- |
| Original OP-1, standalone #246 | Original native implementation and build recipe linked above |
| Hundekraft | Native host and packages maintained in the [OP-1 Workshop](https://github.com/Hundekraft/op1-workshop) |
| op1r | A synthesised-drum host route is required; no verified OP1P supplied here |
| Other original OP-1 firmware | Requires host-specific integration and testing |

Drum+ is the sample-based DrumSampler extension. GIN is a DBox-family drum
synthesiser and needs its own host routing. Native font rendering, resource
loading and display layout are part of that host integration. No extracted
font geometry is supplied here.

## Credits

GIN is by [SquareWave Studio](https://squarewave.studio). Its synthesis derives
from Eric Lewis's MIT-licensed [libpo](https://github.com/ericlewis/libpo).
The factory sounds adapt CNTR RNDM's kits, with the source links and creator's
published permission statement in the native source's `PRESET_CREDITS.json`.
Hundekraft contributed native output-level matching and soft-knee limiting.
See [THIRD_PARTY.md](../../../THIRD_PARTY.md) for credits and trademark notices.
