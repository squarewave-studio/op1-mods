/* Copyright (c) 2026 SquareWave Studio. MIT; see LICENSE.
 * libpo32-derived portions: Copyright (c) 2026 Eric Lewis. */
#ifndef TONIC_VOICE_H
#define TONIC_VOICE_H

#include "po32_synth.h"

/* Single-owner API: initialise before use; serialise every call on the audio
 * owner. Preparation/trigger is outside the render callback. No native ABI. */
#define TONIC_SCRATCH_SAMPLES 128u
#define TONIC_MAX_BLOCK_SAMPLES 4096u
#define TONIC_LEVEL_RAMP_SAMPLES 64u
#define TONIC_MIN_RATE 8000u
#define TONIC_MAX_RATE 96000u
#define TONIC_MAX_DURATION 10.0f

/* Stable reference field order, retained independently of stock EdPort. */
#define TONIC_PARAMETERS(X) \
    X(OscWave) X(OscFreq) X(OscAtk) X(OscDcy) \
    X(ModMode) X(ModRate) X(ModAmt) \
    X(NFilMod) X(NFilFrq) X(NFilQ) \
    X(NEnvMod) X(NEnvAtk) X(NEnvDcy) \
    X(Mix) X(DistAmt) X(EQFreq) X(EQGain) \
    X(Level) X(OscVel) X(NVel) X(ModVel)

typedef struct tonic_voice {
    po32_synth_voice_t dsp;
    float scratch[TONIC_SCRATCH_SAMPLES];
    float level, level_target, level_step;
    uint32_t level_remaining;
    int active;
} tonic_voice_t;

extern const po32_patch_params_t tonic_kick;

/* Reset discards the prepared hit and restores unity outer level and silence. */
void tonic_voice_reset(tonic_voice_t *voice);
/* Validates the entire snapshot before any mutation; retrigger restarts the
 * reference seed/phase/envelopes and completes any pending level ramp. */
po32_status_t tonic_voice_trigger(tonic_voice_t *voice,
    const po32_patch_params_t *params, int velocity, uint32_t sample_rate,
    float duration);
/* Outer linear gain [0,1], independent of snapshot Level; retarget from the
 * current gain, reach the target on sample 64, preserve all upstream state. */
po32_status_t tonic_voice_set_level(tonic_voice_t *voice, float level);
/* Immediate hard stop, zero subsequent blocks; click-free choke is deferred. */
void tonic_voice_stop(tonic_voice_t *voice);
int tonic_voice_done(const tonic_voice_t *voice);
/* Fill ALL count samples, zero the completed/stopped suffix. Valid count is
 * 0..4096; zero count permits NULL output. Invalid calls mutate nothing.
 * Output must not alias voice storage. No allocation, locks or coefficients. */
po32_status_t tonic_voice_render(tonic_voice_t *voice, int16_t *out, size_t count);
/* Truncate toward zero after scaling by 32768; saturate endpoints/infinities,
 * map NaN to silence before integer conversion. */
int16_t tonic_pcm16(float sample);

#endif
