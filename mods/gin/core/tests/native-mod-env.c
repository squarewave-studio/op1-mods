/* Renders one hit through the float reference voice at a given sample rate
 * and prints the noise envelope's retrigger times in ms: 0.25 ms frames whose
 * power jumps at least 10 dB above the frame two before. Arguments: the
 * sample rate, then the 21 normalised patch parameters. */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "po32_synth.h"

static float out[48000 * 3];

int main(int argc, char **argv) {
  po32_patch_params_t params;
  po32_synth_voice_t voice;
  float *p = (float *)&params, e[2048];
  size_t n = 0, got, w, frames, i, j, last = 0;
  unsigned sr;
  if (argc != 23) return 2;
  sr = (unsigned)strtoul(argv[1], NULL, 10);
  for (i = 0; i < 21; i++) p[i] = strtof(argv[i + 2], NULL);
  po32_synth_voice_init(&voice, sr, &params, 127, 2.0f);
  while (!po32_synth_voice_done(&voice) && n < sizeof(out) / sizeof(out[0])) {
    if (po32_synth_voice_render_f32(&voice, out + n, sizeof(out) / sizeof(out[0]) - n, &got) != PO32_OK || !got) break;
    n += got;
  }
  w = sr / 4000u;
  frames = n / w < 2048 ? n / w : 2048;
  for (i = 0; i < frames; i++) {
    double s = 0;
    for (j = 0; j < w; j++) s += (double)out[i * w + j] * out[i * w + j];
    e[i] = (float)(10.0 * log10(s / w + 1e-20));
  }
  for (i = 2; i < frames; i++)
    if (e[i] - e[i - 2] >= 10.0f && (!last || i - last > 2)) {
      printf("%.2f ", (double)i * w * 1000.0 / sr);
      last = i;
    }
  printf("\n");
  return 0;
}
