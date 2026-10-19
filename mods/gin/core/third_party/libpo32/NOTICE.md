# libpo32

Source: https://github.com/ericlewis/libpo32
Base revision: `3b3a0a758ba452fc809027c5bafcc5e2d0931532`
Copyright (c) 2026 Eric Lewis. MIT, see LICENSE.

This subset contains synthesis headers, the voice renderer and lookup-table
math. The GIN version includes parameter-mapping and EQ interoperability
changes. It does not contain the transfer modem, manufacturer firmware or a
proprietary SDK.

Sine, exp2 and log2 lookup entries have been checked against the corresponding
mathematical functions at float32 precision. GIN's integer tables quantise the
sine and exp2 values, and its drive table is generated from the documented
fixed-point shaping expression.
