# GIN Synthesis Core

Reusable C synthesis from GIN by SquareWave Studio, based on Eric Lewis's
[libpo](https://github.com/ericlewis/libpo).

This folder contains the synthesis and preparation code. It is not an OP1P
plugin or a firmware installer. The OP-1 instrument adds the native drum host,
four-voice allocation, editor, factory kits and persistence separately.

## Build And Check

A C compiler is required. From this folder:

```sh
make test
```

The functional test uses address and undefined-behaviour sanitizers. It checks
arithmetic, output bounds, retrigger/release, independent voice state, mixed
block lengths, exact guarded-filter arithmetic and long sine modulation. Both
rounding profiles and block settings are covered. These are host tests, not
OP-1 performance measurements.

`GIN_FAST_FIXED=1` selects the fast fixed-point profile used by GIN. Its Q30
multiply uses floor rounding. The default precise profile retains its separate
nearest-rounding behaviour. Both profiles use 44.1 kHz synthesis; native output
rate conversion belongs to the host adapter.

The optional block renderer requires both `GIN_BLOCK_STATE=1` and
`GIN_FAST_FIXED=1`. Setting `GIN_BLOCK_STATE=1` with the precise profile
(`GIN_FAST_FIXED=0`) still uses the scalar renderer.

In the fast profile, scalar/block equivalence refers to PCM output. Dormant
component state can differ at block boundaries, so matching PCM does not imply
identical full voice state. The original OP-1 integration profile uses the
scalar renderer (`GIN_BLOCK_STATE=0`).

`gin_integer_can_retire()` reports a sufficient condition for all future core
samples of an immutable hit to be exactly zero. A host must still drain its
resampler and fades before retiring a voice.

## Licence

MIT. Preserve the Eric Lewis and SquareWave Studio notices. The included
libpo PO-32 subset retains its licence and copyright. No op1r GPL host adapter,
Teenage Engineering firmware or font data, Sonic Charge source or factory kits
are included here.

GIN is not affiliated with, authorised by, sponsored by or endorsed by Teenage
Engineering AB or Sonic Charge / NuEdge Development AB. All trademarks belong
to their respective owners.
