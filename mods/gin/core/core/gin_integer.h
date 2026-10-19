/* Copyright (c) 2026 SquareWave Studio. MIT; see LICENSE.
 * libpo32-derived portions: Copyright (c) 2026 Eric Lewis. */
/* Derived synthesis structure: libpo32, Copyright (c) 2026 Eric Lewis, MIT.
 * See LICENSE in this directory. Owned fixed-point prototype; no native ABI. */
#ifndef GIN_INTEGER_H
#define GIN_INTEGER_H
#include <stdint.h>
/* Hardware candidate builds explicitly enable the lower-cost32-bit path.
 * Default precise mode keeps prior host/oracle comparisons reproducible. */
#ifndef GIN_FAST_FIXED
#define GIN_FAST_FIXED 0
#endif
/* Optional exact block-state staging. Default preserves scalar rendering. */
#ifndef GIN_BLOCK_STATE
#define GIN_BLOCK_STATE 0
#endif
#if GIN_BLOCK_STATE != 0 && GIN_BLOCK_STATE != 1
#error GIN_BLOCK_STATE_must_be_zero_or_one
#endif
/* The combined DSP unit can inline integer helpers without changing the
 * standalone oracle/API build or rounding semantics. */
#if defined(GIN_DSP_INLINE) && defined(__GNUC__)
#define GIN_ARITHMETIC static inline __attribute__((always_inline))
#else
#define GIN_ARITHMETIC
#endif
/* Optional exact table-folding experiment: smaller data working set, with
 * extra index arithmetic. Physical cache benefit is not yet qualified. */
#ifndef GIN_SINE_QUARTER
#define GIN_SINE_QUARTER 0
#endif
#if GIN_SINE_QUARTER != 0 && GIN_SINE_QUARTER != 1
#error GIN_SINE_QUARTER_must_be_zero_or_one
#endif
#define GIN_INTEGER_RATE 44100u
#define GIN_INTEGER_BLOCK 128u
#define GIN_INTEGER_MAGIC 0x47494e33u /* Owned preparation v3: corrected control maps/trigger seeds. */
#define GIN_SIGNAL_BITS 20u
static inline uint32_t gin_magnitude(int32_t x){
#if defined(__bfin__) && !defined(GIN_PORTABLE_ARITHMETIC)
    uint32_t n;
    /* A signed ABS instruction saturates INT32_MIN. Integer magnitudes must
     * retain0x80000000, so keep the non-saturating negation explicit. */
    __asm__("%0 = -%1;":"=d"(n):"d"(x):"ASTAT");
    return x<0?n:(uint32_t)x;
#else
    return x<0?0u-(uint32_t)x:(uint32_t)x;
#endif
}
typedef struct { int32_t value; uint32_t fraction; } gin_coefficient_t;
typedef struct { gin_coefficient_t c[5]; } gin_filter_coefficients_t;
typedef struct { uint32_t hi,lo; } gin_pair_t; /* Explicit two's-complement words. */
typedef struct {
    uint32_t magic,total,wave,mod_mode,noise_mode,use_eq,use_dist;
    uint32_t osc_attack,noise_attack,linear_attack,linear_end;
    uint32_t phase_step,ring_phase,ring_step,ring_enabled;
    uint32_t time_inverse_word,noise_attack_word,ring_inverse_word;
    int32_t pitch_scale; /* Q24 octaves. */
    int32_t osc_decay_seed,noise_decay_seed,linear_decay_seed;
    int32_t linear_attack_step,linear_decay_step; /* Q30. */
    gin_coefficient_t osc_attack_c,osc_decay_c,noise_attack_c,noise_decay_c;
    gin_coefficient_t mod_decay_c,rot_s,rot_c,alpha,one_minus_alpha;
    gin_coefficient_t ring_gain,osc_gain,noise_gain,level;
    gin_coefficient_t dist_drive,dist_gain;
    int32_t dist_clip,dist_bias; /* Q20. */
    uint32_t dist_strong;
    gin_filter_coefficients_t noise_filter,eq_filter;
} gin_prepared_t;
typedef struct {
    gin_prepared_t p;
    uint32_t n,active,phase,random,ring_phase;
    int32_t osc_env,noise_env,mod_env,mod_s,mod_c,mod_state,linear_env;
    gin_pair_t noise_z[2],eq_z[2]; /* Precise Q44; fast DF2T2x32 or DF1 four32-bit histories. */
    uint32_t noise_compact,eq_compact; /* Precise: boolean Q28 flag. Fast: fraction28/24/20/16/12, history layout selected by immutable coefficients. */
} gin_integer_voice_t;
/* All calls serialised by one owner. Prepared input and output must not alias
 * voice storage. Future native lifetime/decoder/owner/preparation gates open. */
GIN_ARITHMETIC int32_t gin_mul_shift(int32_t,int32_t,uint32_t); /* precise nearest/ties-away; fast Q30floor; saturation */
GIN_ARITHMETIC int32_t gin_add_sat(int32_t,int32_t);
GIN_ARITHMETIC int32_t gin_sub_sat(int32_t,int32_t);
GIN_ARITHMETIC gin_pair_t gin_product32(uint32_t,uint32_t);
GIN_ARITHMETIC gin_pair_t gin_wide_add(gin_pair_t,gin_pair_t);
GIN_ARITHMETIC gin_pair_t gin_wide_sub(gin_pair_t,gin_pair_t);
GIN_ARITHMETIC gin_pair_t gin_wide_multiply(gin_pair_t,gin_coefficient_t);
GIN_ARITHMETIC gin_pair_t gin_wide_from_signal(int32_t);
GIN_ARITHMETIC gin_pair_t gin_wide_signal_multiply(int32_t,gin_coefficient_t);
GIN_ARITHMETIC int32_t gin_wide_to_signal(gin_pair_t);
/* Positive finite binary32 word model, RNE at each operation; no float types.
 * Subtraction clamps negative input differences to0. Multiplication supports
 * finite inputs and under/overflow results; callback operands are bounded. */
GIN_ARITHMETIC uint32_t gin_u32_word(uint32_t);
GIN_ARITHMETIC uint32_t gin_word_mul(uint32_t,uint32_t);
GIN_ARITHMETIC uint32_t gin_word_sub(uint32_t,uint32_t);
GIN_ARITHMETIC uint32_t gin_word_fraction(uint32_t);
int gin_integer_trigger(gin_integer_voice_t *,const gin_prepared_t *);
/* Render 0..128 logical samples. Valid calls require non-NULL pointers even
 * for zero frames; zero frames changes nothing. Invalid calls return -1 before
 * any mutation or writes. Fill the whole requested extent, including zeroes
 * after release/expiry; samples beyond the extent remain untouched. Reblocking
 * alone neither resamples nor changes the physical consumption clock. */
int gin_integer_render_frames(gin_integer_voice_t *,int16_t *,uint32_t frames);
int gin_integer_render64(gin_integer_voice_t *,int16_t *); /* exactly64 writes */
int gin_integer_render(gin_integer_voice_t *,int16_t *); /* exactly128 writes */
void gin_integer_release(gin_integer_voice_t *);
uint32_t gin_integer_status(const gin_integer_voice_t *);
#endif
