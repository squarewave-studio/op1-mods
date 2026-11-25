/* Copyright (c) 2026 SquareWave Studio. MIT; see LICENSE. */
#include "gin_prepare.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    static const float attacks[] = {0, 1e-45f, 1e-37f, .001f, .112541f, .3f, .751369f, .86f};
    static const unsigned spans[] = {128, 1, 63, 64, 65, 127, 31, 97};
    for (unsigned c = 0; c < sizeof(attacks) / sizeof(*attacks); ++c) {
        po32_patch_params_t patch;
        gin_prepared_t p;
        gin_integer_voice_t one, many;
        memset(&patch, 0, sizeof(patch));
        patch.NEnvMod = 1; patch.NEnvAtk = attacks[c]; patch.NEnvDcy = .5f;
        patch.NFilFrq = 1; patch.Mix = 1; patch.Level = .74f;
        assert(!gin_integer_prepare(&p, &patch, 127, 6));
        assert(!gin_integer_trigger(&one, &p));
        assert(!gin_integer_trigger(&many, &p));
        assert(many.noise_env == 1073741824);
        assert(!p.ring_enabled || (p.ring_step && p.ring_step <= p.noise_attack));
        if (!p.noise_attack) {
            po32_patch_params_t exp_patch = patch;
            gin_prepared_t exp_prepared;
            gin_integer_voice_t exp_voice, mod_voice;
            exp_patch.NEnvMod = 0;
            assert(!gin_integer_prepare(&exp_prepared, &exp_patch, 127, 6));
            assert(!gin_integer_trigger(&exp_voice, &exp_prepared));
            assert(!gin_integer_trigger(&mod_voice, &p));
            for (unsigned i = 0; i < 32; ++i) {
                int16_t exp_pcm[128], mod_pcm[128];
                assert(!gin_integer_render_frames(&exp_voice, exp_pcm, 128));
                assert(!gin_integer_render_frames(&mod_voice, mod_pcm, 128));
                assert(!memcmp(exp_pcm, mod_pcm, sizeof(exp_pcm)));
            }
        }
        unsigned end = p.noise_attack + 1024;
        for (unsigned n = 0, k = 0; n < end; ++k) {
            struct { unsigned before; int16_t pcm[128]; unsigned after; } out;
            int16_t expected[128];
            unsigned count = spans[k % (sizeof(spans) / sizeof(*spans))];
            if (count > end - n) count = end - n;
            out.before = 0x12345678; out.after = 0x87654321;
            for (unsigned i = 0; i < 128; ++i) out.pcm[i] = 12345;
            for (unsigned i = 0; i < count; ++i)
                assert(!gin_integer_render_frames(&one, expected + i, 1));
            assert(!gin_integer_render_frames(&many, out.pcm, count));
            assert(!memcmp(expected, out.pcm, count * sizeof(*expected)));
            assert(one.n == many.n && one.random == many.random);
            assert(one.noise_env == many.noise_env && one.ring_phase == many.ring_phase);
            assert(out.before == 0x12345678 && out.after == 0x87654321);
            for (unsigned i = count; i < 128; ++i) assert(out.pcm[i] == 12345);
            n += count;
        }
    }
    puts("PASS Mod one-frame/mixed-block PCM, burst state, attack/decay boundaries and canaries");
    return 0;
}
