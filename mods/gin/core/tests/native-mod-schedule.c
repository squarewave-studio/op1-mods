/* Mod noise envelope schedule, read straight from the envelope state of the
 * float reference voice and the integer preview. For each normalised
 * NEnvAtk argument it prints one line:
 *   <N> <L> | <float retrigger samples> | <integer retrigger samples> | <finite>
 * N is the burst count, L the attack length in samples at 44.1 kHz and
 * finite is 1 when every float sample of the hit is a finite number. An
 * argument starting with n (n0.75) prints N alone. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gin_integer.h"
#include "gin_prepare.h"
#include "po32_synth.h"

static int finite(float x) { return x == x && x - x == 0.0f; }

int main(int argc, char **argv) {
  int a;
  for (a = 1; a < argc; a++) {
    po32_patch_params_t params;
    po32_synth_voice_t v;
    gin_prepared_t prepared;
    static gin_integer_voice_t iv;
    int16_t pcm;
    float out, saved;
    size_t got;
    uint32_t n, L, ok = 1;
    memset(&params, 0, sizeof(params));
    params.NEnvMod = 1.0f;
    params.NEnvAtk = strtof(argv[a] + (argv[a][0] == 'n'), NULL);
    params.NEnvDcy = 0.5f;
    params.NFilFrq = 1.0f;
    params.Mix = 1.0f;
    params.Level = 0.74f;
    po32_synth_voice_init(&v, 44100u, &params, 127, 9.9f);
    L = v.mod_attack_samples;
    if (argv[a][0] == 'n') {
      printf("%u\n", v.mod_burst_count);
      continue;
    }
    printf("%u %u |", v.mod_burst_count, L);
    /* Without a restart the burst level only falls by its coefficient, so
     * any other value after a sample means that sample restarted it. */
    for (n = 0; !po32_synth_voice_done(&v); n++) {
      saved = v.nenv_v;
      if (po32_synth_voice_render_f32(&v, &out, 1, &got) != PO32_OK || got != 1) return 1;
      if (!finite(out) || !finite(v.nenv_v)) ok = 0;
      if (n < L && v.nenv_v != saved * v.nenv_atk_coeff) printf(" %u", n);
    }
    printf(" |");
    if (gin_integer_prepare(&prepared, &params, 127, 9.9f) || gin_integer_trigger(&iv, &prepared)) return 1;
    /* The level a sample plays at is unity exactly when that sample restarts
     * a burst; every attack sample is rendered, one-sample attacks included. */
    for (n = 0; n < L; n++) {
      if (iv.noise_env == 1073741824) printf(" %u", n);
      if (gin_integer_render_frames(&iv, &pcm, 1)) return 1;
    }
    printf(" | %u\n", ok);
  }
  return 0;
}
