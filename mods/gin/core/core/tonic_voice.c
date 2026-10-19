/* Copyright (c) 2026 SquareWave Studio. MIT; see LICENSE.
 * libpo32-derived portions: Copyright (c) 2026 Eric Lewis. */
#include "tonic_voice.h"

/* Curated owned tonal kick: sine, short decay, falling pitch, no noise mix. */
const po32_patch_params_t tonic_kick = {
    .OscWave = 0.0f, .OscFreq = 0.18f, .OscAtk = 0.0f, .OscDcy = 0.48f,
    .ModMode = 0.0f, .ModRate = 0.17f, .ModAmt = 0.61f,
    .NFilMod = 0.0f, .NFilFrq = 0.25f, .NFilQ = 0.1f,
    .NEnvMod = 0.0f, .NEnvAtk = 0.0f, .NEnvDcy = 0.2f,
    .Mix = 0.0f, .DistAmt = 0.0f, .EQFreq = 0.5f, .EQGain = 0.5f,
    .Level = 0.8f, .OscVel = 0.5f, .NVel = 0.5f, .ModVel = 0.0f
};

static int unit(float x) { return x >= 0.0f && x <= 1.0f; }

void tonic_voice_reset(tonic_voice_t *v) {
    size_t i;
    unsigned char *bytes;
    if (!v) return;
    bytes = (unsigned char *)v;
    for (i = 0; i < sizeof(*v); ++i) bytes[i] = 0;
    v->level = v->level_target = 1.0f;
}

po32_status_t tonic_voice_trigger(tonic_voice_t *v,
    const po32_patch_params_t *p, int velocity, uint32_t rate, float duration) {
    if (!v || !p) return PO32_ERR_INVALID_ARG;
    if (velocity < 0 || velocity > 127 || rate < TONIC_MIN_RATE ||
        rate > TONIC_MAX_RATE || !(duration >= 1.0f / (float)rate &&
        duration <= TONIC_MAX_DURATION) ||
        !((float)rate * duration >= 1.0f)) return PO32_ERR_RANGE;
#define VALIDATE(name) if (!unit(p->name)) return PO32_ERR_RANGE;
    TONIC_PARAMETERS(VALIDATE)
#undef VALIDATE
    /* Upstream init clears state, so all rejection must precede it. */
    po32_synth_voice_init(&v->dsp, rate, p, velocity, duration);
    v->active = !po32_synth_voice_done(&v->dsp);
    v->level = v->level_target;
    v->level_remaining = 0;
    v->level_step = 0.0f;
    return PO32_OK;
}

po32_status_t tonic_voice_set_level(tonic_voice_t *v, float level) {
    if (!v) return PO32_ERR_INVALID_ARG;
    if (!unit(level)) return PO32_ERR_RANGE;
    v->level_target = level;
    v->level_step = (level - v->level) / (float)TONIC_LEVEL_RAMP_SAMPLES;
    v->level_remaining = TONIC_LEVEL_RAMP_SAMPLES;
    return PO32_OK;
}

void tonic_voice_stop(tonic_voice_t *v) { if (v) v->active = 0; }
int tonic_voice_done(const tonic_voice_t *v) { return !v || !v->active; }

int16_t tonic_pcm16(float x) {
    if (x != x) return 0;
    if (x >= 1.0f) return 32767;
    if (x <= -1.0f) return -32768;
    return (int16_t)(x * 32768.0f);
}

po32_status_t tonic_voice_render(tonic_voice_t *v, int16_t *out, size_t count) {
    size_t offset = 0, i, n, made;
    if (!v || (!out && count)) return PO32_ERR_INVALID_ARG;
    if (count > TONIC_MAX_BLOCK_SAMPLES) return PO32_ERR_RANGE;
    while (offset < count) {
        n = count - offset;
        if (n > TONIC_SCRATCH_SAMPLES) n = TONIC_SCRATCH_SAMPLES;
        made = 0;
        if (v->active) {
            po32_status_t status = po32_synth_voice_render_f32(
                &v->dsp, v->scratch, n, &made);
            if (status != PO32_OK) {
                v->active = 0;
                for (i = offset; i < count; ++i) out[i] = 0;
                return status;
            }
            v->active = !po32_synth_voice_done(&v->dsp);
        }
        for (i = 0; i < n; ++i) {
            if (v->level_remaining) {
                v->level += v->level_step;
                if (--v->level_remaining == 0) v->level = v->level_target;
            }
            out[offset+i] = i < made ? tonic_pcm16(v->scratch[i] * v->level) : 0;
        }
        offset += n;
    }
    return PO32_OK;
}
