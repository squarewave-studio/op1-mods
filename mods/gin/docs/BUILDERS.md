# GIN For Firmware Builders

GIN 1.0.0 is available as source for the original OP-1. The standalone instrument
has been tested on hardware with firmware #246. A clean source export reproduces
all 25 target objects and the complete tested module with the pinned compiler.

## Get The Source

- [Reusable synthesis core](../core/README.md): MIT-licensed synthesis,
  preparation code, libpo32 subset and functional tests.
- [Complete native source](https://github.com/joseph-holland/op1r-src/tree/ec5d6f722df05b77f80706db239352992937f4d7/synth_sdk/experimental/gin/standalone): the drum adapter,
  editor, eight 24-key factory kits, saved state, SDK inputs and build recipe.
- [Integration guide](https://github.com/joseph-holland/op1r-src/tree/ec5d6f722df05b77f80706db239352992937f4d7/synth_sdk/experimental/gin/standalone/INTEGRATION.md) and
  [machine-readable hook map](https://github.com/joseph-holland/op1r-src/tree/ec5d6f722df05b77f80706db239352992937f4d7/synth_sdk/experimental/gin/standalone/INTEGRATION.json).

The native source link is pinned to a commit. It lives in the op1r fork because
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

The native source documents the 21 hook sites, original continuations, callback
ABI, memory ranges and preset/save lifecycle. GIN shares code/BSS ranges and
several hooks with op1r. Combined firmware needs one linker layout and shared
dispatchers at those sites, so appending the standalone payload unchanged won't
work.

| Host | Status |
| --- | --- |
| Original OP-1, standalone #246 | Tested on hardware; exact source rebuild checked |
| op1r | Drum Synth host adapter in development; combined build not tested |
| Hundekraft / HK | Source available for porting; combined build not tested |
| Opus | Source available for porting; combined build not tested |

Drum+ is the sample-based DrumSampler extension. GIN is a DBox-family drum
synthesiser and needs its own host routing. The experimental Drum Synth API
is a separate path towards `gin.op1p`. This source release isn't a loadable
plugin, and it hasn't been accepted upstream.

## Credits

GIN is by [SquareWave Studio](https://squarewave.studio). Its synthesis derives
from Eric Lewis's MIT-licensed [libpo32](https://github.com/ericlewis/libpo32).
The factory sounds adapt CNTR RNDM's kits, with the source links and creator's
published permission statement in the native source's `PRESET_CREDITS.json`.
See [THIRD_PARTY.md](../../../THIRD_PARTY.md) for credits and trademark notices.
