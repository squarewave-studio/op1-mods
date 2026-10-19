/* Copyright (c) 2026 SquareWave Studio. MIT; see LICENSE.
 * libpo32-derived portions: Copyright (c) 2026 Eric Lewis. */
#ifndef GIN_PREPARE_H
#define GIN_PREPARE_H
#include "gin_integer.h"
#include "tonic_voice.h"
/* Explicitly intermediate FLOAT preparation, outside callback closure.
 * Native context/cost/coherent snapshots are unqualified. Host44100 only. */
int gin_integer_prepare(gin_prepared_t *,const po32_patch_params_t *,int,float);
/* Setup/GUI only: interpolate one saved sound and derive its finite tail. */
int gin_integer_prepare_morph(gin_prepared_t *,po32_patch_params_t *,const float endpoint[2][21],uint32_t morph);
#endif
