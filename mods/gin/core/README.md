# GIN Synthesis Core

Reusable C synthesis from GIN by SquareWave Studio, based on Eric Lewis's
[libpo32](https://github.com/ericlewis/libpo32).

This folder contains the synthesis and preparation code. It is not an OP1P
plugin or a firmware installer. The OP-1 instrument adds the native drum host,
four-voice allocation, editor, factory kits and persistence separately.

## Build And Check

A C compiler is required. From this folder:

```sh
make test
```

The functional test uses address and undefined-behaviour sanitizers. It checks
arithmetic, output bounds, retrigger/release and independent voice state. It is
a host test, not an OP-1 performance measurement.

`GIN_FAST_FIXED=1` selects the fast fixed-point profile used by GIN. Its Q30
multiply uses floor rounding. The default precise profile retains its separate
nearest-rounding behaviour. Both profiles use 44.1 kHz synthesis; native output
rate conversion belongs to the host adapter.

## Licence

MIT. Preserve the Eric Lewis and SquareWave Studio notices. The included
libpo32 subset retains its licence and copyright. No op1r GPL host adapter,
Teenage Engineering firmware or font data, Sonic Charge source or factory kits
are included here.

GIN is not affiliated with, authorised by, sponsored by or endorsed by Teenage
Engineering AB or Sonic Charge / NuEdge Development AB. All trademarks belong
to their respective owners.
