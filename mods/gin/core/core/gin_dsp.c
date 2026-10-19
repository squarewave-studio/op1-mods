/* Copyright (c) 2026 SquareWave Studio. MIT; see LICENSE.
 * libpo32-derived portions: Copyright (c) 2026 Eric Lewis. */
/* One compilation unit lets the pinned BF524 compiler inline the paired-word
 * helpers across the sample loop. Same integer operations and rounding; no
 * arithmetic approximation, fast-math, allocation or new interrupt policy. */
/* Optimize only this DSP unit. Match the measured target build without
 * changing the SDK's other modules, float rules or hardware errata policy. */
#if defined(__GNUC__) && defined(__bfin__)
#pragma GCC push_options
#pragma GCC optimize ("O3", "no-peel-loops", "no-unroll-loops")
#endif
#define GIN_DSP_INLINE 1
#include "gin_integer.c"
#include "gin_precision.c"

#if defined(__GNUC__) && defined(__bfin__)
#pragma GCC pop_options
#endif
