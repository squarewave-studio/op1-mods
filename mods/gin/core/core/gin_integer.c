/* Copyright (c) 2026 SquareWave Studio. MIT; see LICENSE.
 * libpo32-derived portions: Copyright (c) 2026 Eric Lewis. */
/* Derived from MIT libpo32 synthesis structure; Eric Lewis 2026. See LICENSE.
 * Callback closure has integer arithmetic only, no division/libc/allocation.
 * Saturation is explicit. GIN_FAST_FIXED selects the measured approximate
 * renderer; the unmodified precise path remains the comparison reference. */
#include "gin_integer.h"
#include "tables.h"
#include "drive_curve.h"
#include <limits.h>
#include <stddef.h>
#define ONE (1 << 30)
#define SIGNAL_ONE (1 << GIN_SIGNAL_BITS)

typedef gin_pair_t product_t;
GIN_ARITHMETIC gin_pair_t gin_product32(uint32_t a,uint32_t b) {
#if defined(__bfin__) && !defined(GIN_PORTABLE_ARITHMETIC)
    uint32_t low,cross,high,t;
    /* The unsigned cross sum needs33bits, so retain it in40-bit A1.
     * After shifting16, adding the high product fits32bits. Extract its
     * carry once instead of separately shifting/adding two cross products.
     * No fractional bits or rounding are discarded. */
    __asm__("A1 = %3.L * %4.H (FU);\n\t"
            "A1 += %3.H * %4.L (FU);\n\t"
            "%1 = A1.w;\n\t"
            "A1 = A1 >> 16;\n\t"
            "A1 += %3.H * %4.H (FU);\n\t"
            "%2 = A1.w;\n\t"
            "%0 = %3.L * %4.L (FU);"
            : "=&d"(low),"=&d"(cross),"=&d"(high)
            : "d"(a),"d"(b) : "A1","ASTAT");
    t=(low>>16)+(cross&65535u);
    product_t p;p.lo=(low&65535u)|(t<<16);p.hi=high+(t>>16);return p;
#else
    uint32_t a0=a&65535u,a1=a>>16,b0=b&65535u,b1=b>>16;
    uint32_t p0=a0*b0,p1=a0*b1,p2=a1*b0,p3=a1*b1;
    uint32_t t=(p0>>16)+(p1&65535u)+(p2&65535u);
    product_t p; p.lo=(p0&65535u)|(t<<16);
    p.hi=p3+(p1>>16)+(p2>>16)+(t>>16); return p;
#endif
}
GIN_ARITHMETIC int32_t gin_add_sat(int32_t a,int32_t b) {
#if defined(__bfin__) && !defined(GIN_PORTABLE_ARITHMETIC)
    int32_t r;__asm__("%0 = %1 + %2 (S);":"=d"(r):"d"(a),"d"(b):"ASTAT");return r;
#else
    if(b>0&&a>INT32_MAX-b)return INT32_MAX;
    if(b<0&&a<INT32_MIN-b)return INT32_MIN;
    return a+b;
#endif
}
GIN_ARITHMETIC int32_t gin_sub_sat(int32_t a,int32_t b) {
#if defined(__bfin__) && !defined(GIN_PORTABLE_ARITHMETIC)
    int32_t r;__asm__("%0 = %1 - %2 (S);":"=d"(r):"d"(a),"d"(b):"ASTAT");return r;
#else
    if(b>0&&a<INT32_MIN+b)return INT32_MIN;
    if(b<0&&a>INT32_MAX+b)return INT32_MAX;
    return a-b;
#endif
}
GIN_ARITHMETIC int32_t gin_mul_shift(int32_t a,int32_t b,uint32_t shift) {
#if defined(__bfin__) && !defined(GIN_PORTABLE_ARITHMETIC)
#if GIN_FAST_FIXED
    if(shift==30u){
        int32_t result;
        /* Native floor rounding: at most one integer result count below nearest. */
        /* Most prepared gains/envelopes use Q30. Immediate shifts keep
         * two count registers and their spills out of this common path. */
        __asm__("A1 = %1.L * %2.L (FU);\n\t"
                "A1 = A1 >> 16;\n\t"
                "A1 += %1.H * %2.L (M), A0 = %1.H * %2.H;\n\t"
                "A1 += %2.H * %1.L (M);\n\t"
                "A0 = A0 << 1;\n\t"
                "A1 = A1 >>> 14;\n\t"
                "%0 = (A0 += A1);"
                : "=d"(result)
                : "d"(a),"d"(b)
                : "A0","A1","ASTAT");
        return result;
    }
#else
    if(shift==30u){
        int32_t result,temporary;
        uint32_t low=(uint32_t)a*(uint32_t)b;
        uint32_t negative=((uint32_t)a^(uint32_t)b)>>31;
        uint32_t bias=1u-(negative&&!(low&0x1fffffffu));
        /* Most prepared gains/envelopes use Q30. Immediate shifts keep
         * two count registers and their spills out of this common path. */
        __asm__("A1 = %2.L * %3.L (FU);\n\t"
                "A1 = A1 >> 16;\n\t"
                "A1 += %2.H * %3.L (M), A0 = %2.H * %3.H;\n\t"
                "A1 += %3.H * %2.L (M);\n\t"
                "A0 = A0 << 1;\n\t"
                "A1 = A1 >>> 13;\n\t"
                "%1 = A1.w;\n\t"
                "%1 = %1 + %4;\n\t"
                "%1 = %1 >>> 1;\n\t"
                "A1 = %1;\n\t"
                "%0 = (A0 += A1);"
                : "=d"(result),"=&d"(temporary)
                : "d"(a),"d"(b),"d"(bias)
                : "A0","A1","ASTAT");
        return result;
    }
#endif
    int32_t high_product=(a>>16)*(b>>16);
    if(shift<=31u&&(shift>=24u||(shift>=18u&&gin_magnitude(high_product)<(1u<<(shift+6u))))){
        int32_t result,temporary,left=(int32_t)(31u-shift);
        int32_t right=17-(int32_t)shift;
        uint32_t low=(uint32_t)a*(uint32_t)b;
        uint32_t negative=((uint32_t)a^(uint32_t)b)>>31;
        uint32_t bias=1u-(negative&&!(low&((1u<<(shift-1u))-1u)));
        /* The low unsigned product supplies guard bits; signed/unsigned
         * cross products and signed high product retain the exact integer
         * result. Shifts24..31 always fit40bits; the narrower-shift guard
         * bounds the high product with one extra bit for the cross terms. */
        __asm__("A1 = %2.L * %3.L (FU);\n\t"
                "A1 = A1 >> 16;\n\t"
                "A1 += %2.H * %3.L (M), A0 = %2.H * %3.H;\n\t"
                "A1 += %3.H * %2.L (M);\n\t"
                "A0 = ASHIFT A0 BY %4.L;\n\t"
                "A1 = ASHIFT A1 BY %5.L;\n\t"
                "%1 = A1.w;\n\t"
                "%1 = %1 + %6;\n\t"
                "%1 = %1 >>> 1;\n\t"
                "A1 = %1;\n\t"
                "%0 = (A0 += A1);"
                : "=d"(result),"=&d"(temporary)
                : "d"(a),"d"(b),"d"(left),"d"(right),"d"(bias)
                : "A0","A1","ASTAT");
        return result;
    }
#endif
    uint32_t negative=(a<0)!=(b<0),ma=gin_magnitude(a),mb=gin_magnitude(b),m,overflow;
    product_t p=gin_product32(ma,mb);
#if GIN_FAST_FIXED
    if(shift==30u){
        m=(p.lo>>30)|(p.hi<<2);overflow=p.hi>>30;
        if(negative&&(p.lo&0x3fffffffu)){++m;if(!m)overflow=1;}
        if(overflow||m>(uint32_t)INT32_MAX+negative)return negative?INT32_MIN:INT32_MAX;
        return negative?(m==0x80000000u?INT32_MIN:-(int32_t)m):(int32_t)m;
    }
#endif
    if(shift>31u)return 0; /* Internal contract0..31; invalid helper is inert. */
    if(shift) {
        uint32_t old=p.lo; p.lo+=1u<<(shift-1u); p.hi+=p.lo<old;
        overflow=p.hi>>shift; m=(p.lo>>shift)|(p.hi<<(32u-shift));
    } else { overflow=p.hi; m=p.lo; }
    if(overflow||m>(uint32_t)INT32_MAX+negative)return negative?INT32_MIN:INT32_MAX;
    if(negative) return m==0x80000000u?INT32_MIN:-(int32_t)m;
    return (int32_t)m;
}
#if GIN_FAST_FIXED
static __attribute__((noinline)) int32_t apply_other(int32_t x,gin_coefficient_t c) {return gin_mul_shift(x,c.value,c.fraction);}
static inline __attribute__((always_inline)) int32_t apply(int32_t x,gin_coefficient_t c) {
    /* Keep the common Q30 operation in the caller; only uncommon shifts
     * pay for the generic helper's preserved registers and stack frame. */
    if(c.fraction==30u)return gin_mul_shift(x,c.value,30u);
    return apply_other(x,c);
}

#else
static int32_t apply(int32_t x,gin_coefficient_t c) {return gin_mul_shift(x,c.value,c.fraction);}

#endif
static uint32_t phase_product(uint32_t a,uint32_t b) {
#if defined(__bfin__) && !defined(GIN_PORTABLE_ARITHMETIC)
    uint32_t low,high;
    /* Only product bits23..54 contribute to a wrapping Q32 phase step.
     * Keep the unsigned cross sum in40-bit A1; discard the unused low16
     * bits before accumulating it, then extract at bit23. The final32-bit
     * add deliberately wraps; an accumulator-to-register saturating extract
     * would change large phase increments. No64-bit C product is formed. */
    __asm__("A1 = %2.L * %3.L (FU);\n\t"
            "A1 = A1 >> 16;\n\t"
            "A1 += %2.L * %3.H (FU);\n\t"
            "A0 = %2.H * %3.H (FU);\n\t"
            "A1 += %2.H * %3.L (FU);\n\t"
            "A1 = A1 >> 7;\n\t"
            "%0 = A1.w;\n\t"
            "%1 = A0.w;"
            : "=d"(low),"=d"(high)
            : "d"(a),"d"(b)
            : "A0","A1","ASTAT");
    return low+(high<<9);
#else
    product_t p=gin_product32(a,b); return (p.lo>>23)|(p.hi<<9); /* modulo one turn */
#endif
}
static inline int32_t sine_at(uint32_t index){
#if GIN_SINE_QUARTER
    index&=2047u;uint32_t folded=index&1023u;
    if(folded>512u)folded=1024u-folded;
    int32_t value=gin_sine_quarter[folded];
    return index&1024u?-value:value;
#else
    return gin_sine[index&2047u];
#endif
}
#if GIN_FAST_FIXED
static int32_t sine(uint32_t phase) {
    uint32_t index=phase>>21;
    int32_t first=sine_at(index),delta=sine_at(index+1u)-first;
    /* Nine rounded fractional bits. Adjacent Q30 table differences times512
     * plus rounding fit signed32; interpolation stays between the samples. */
    int32_t fraction=(int32_t)(((phase&0x1fffffu)+2048u)>>12);
    return first+((delta*fraction+256)>>9);
}
#else
static int32_t sine(uint32_t phase) {
    uint32_t index=phase>>21,frac=phase&0x1fffffu;
    return gin_add_sat(sine_at(index),gin_mul_shift(sine_at(index+1u)-sine_at(index),(int32_t)frac,21));
}
#endif
static uint32_t exp2_pitch(int32_t x) {
    int32_t whole=x>>24;
    uint32_t f=(uint32_t)x&0xffffffu,index=f>>14;
    /* Adjacent table values differ by at most11353. The unsigned product
     * plus its rounding bit fits32bits: no full-width multiply is needed. */
    uint32_t value=gin_exp2[index]+(((gin_exp2[index+1u]-gin_exp2[index])*(f&16383u)+8192u)>>14);
    if(whole>=9)return UINT32_MAX;
    if(whole<=-24)return 0;
    return whole<0?value>>(uint32_t)(-whole):value<<(uint32_t)whole;
}
static int32_t absolute(int32_t x) {return x==INT32_MIN?INT32_MAX:(x<0?-x:x);}
static int32_t clamp(int32_t x,int32_t limit) {return x>limit?limit:(x < -limit?-limit:x);}
static int32_t random_signal(gin_integer_voice_t *v) {
    v->random=v->random*1103515245u+12345u;
    return (int32_t)(v->random&0x7fffffffu)-ONE; /* Q30 bipolar */
}
#if GIN_FAST_FIXED
#include "gin_fixed_filter.h"
#else
#include "gin_filter_math.h"

/* The normal noise path needs32-bit Q28 state, not a96-bit multiply for
 * every recursive term. Keep the wide path for poles close to DC/Nyquist.
 * A held hit owns its immutable coefficients; overflow promotes its state
 * once, before committing any compact update, and stays wide until retrigger.
 * Both noise and EQ use Q28 low-word states, promoting old state to Q44
 * before retrying an overflowing sample. Coefficient/gain guards preserve
 * wide precision where recursive rounding can become audible. */
static uint32_t compact_noise_ok(const gin_filter_coefficients_t *p) {
    int32_t a1,a2,margin;
    if(p->c[3].fraction<29u||p->c[3].fraction>30u||p->c[4].fraction!=30u)return 0;
    a1=apply(ONE,p->c[3]);a2=p->c[4].value;
    /* Near-unit-radius resonators also need distance from DC/Nyquist. */
    if(a2<0||a2>=ONE)return 0;
    margin=gin_add_sat(gin_sub_sat(ONE,absolute(a1)),a2);
    return margin>262144 && (a2<ONE-(ONE>>6)||margin>(ONE>>6)); /* DC/Nyquist and long-resonator precision guards. */
}
static int at_limit(int32_t x){return x==INT32_MAX||x==INT32_MIN;}
static gin_pair_t compact_to_wide(int32_t x){
    gin_pair_t z;z.lo=(uint32_t)x<<16;z.hi=(uint32_t)(x>>16);return z;
}
static int32_t noise_filter(int32_t input,gin_integer_voice_t *v) {
    const gin_filter_coefficients_t *p=&v->p.noise_filter;
    gin_pair_t *z=v->noise_z;
    if(v->noise_compact){
        /* Only called with bipolar Q20 noise: this Q28 conversion fits32bits. */
        int32_t x=input*256,b0,b1,b2,a1,a2,y,t,u,s0,s1;
        b0=apply(x,p->c[0]);y=gin_add_sat(b0,(int32_t)z[0].lo);
        b1=apply(x,p->c[1]);a1=apply(y,p->c[3]);
        b2=apply(x,p->c[2]);a2=apply(y,p->c[4]);
        t=gin_sub_sat(b1,a1);s0=gin_add_sat(t,(int32_t)z[1].lo);
        u=gin_sub_sat(b2,a2);s1=u;
        if(!at_limit(x)&&!at_limit(b0)&&!at_limit(b1)&&!at_limit(b2)&&
           !at_limit(a1)&&!at_limit(a2)&&!at_limit(y)&&!at_limit(t)&&!at_limit(s0)&&!at_limit(u)){
            uint32_t m=gin_magnitude(y);int32_t out=(int32_t)((m+128u)>>8);
            z[0].lo=(uint32_t)s0;z[1].lo=(uint32_t)s1;
            return y<0?-out:out;
        }
        z[0]=compact_to_wide((int32_t)z[0].lo);z[1]=compact_to_wide((int32_t)z[1].lo);
        v->noise_compact=0;
    }
    return filter(input,p,z);
}
static int32_t eq_filter_compact(int32_t input,gin_integer_voice_t *v) {
    const gin_filter_coefficients_t *p=&v->p.eq_filter;
    gin_pair_t *z=v->eq_z;
    if((v->eq_compact)&&input>=-8388607&&input<=8388607){
        /* Driven Q20 input is checked before its Q28 conversion. */
        int32_t x=input*256,b0,b1,b2,a1,a2,y,t,u,s0,s1;
        b0=apply(x,p->c[0]);y=gin_add_sat(b0,(int32_t)z[0].lo);
        b1=apply(x,p->c[1]);a1=apply(y,p->c[3]);
        b2=apply(x,p->c[2]);a2=apply(y,p->c[4]);
        t=gin_sub_sat(b1,a1);s0=gin_add_sat(t,(int32_t)z[1].lo);
        u=gin_sub_sat(b2,a2);s1=u;
        if(!at_limit(x)&&!at_limit(b0)&&!at_limit(b1)&&!at_limit(b2)&&
           !at_limit(a1)&&!at_limit(a2)&&!at_limit(y)&&!at_limit(t)&&!at_limit(s0)&&!at_limit(u)){
            uint32_t m=gin_magnitude(y);int32_t out=(int32_t)((m+128u)>>8);
            z[0].lo=(uint32_t)s0;z[1].lo=(uint32_t)s1;
            return y<0?-out:out;
        }
        z[0]=compact_to_wide((int32_t)z[0].lo);z[1]=compact_to_wide((int32_t)z[1].lo);
        v->eq_compact=0;
    }
    if(v->eq_compact){z[0]=compact_to_wide((int32_t)z[0].lo);z[1]=compact_to_wide((int32_t)z[1].lo);v->eq_compact=0;}
    return filter(input,p,z);
}
#endif
static int32_t envelope(uint32_t n,uint32_t attack,int32_t seed,int32_t *state,gin_coefficient_t a,gin_coefficient_t d) {
    int32_t out;
    if(n==attack)*state=seed;
    out=*state; *state=apply(*state,n<attack?a:d); return out;
}
static int32_t distort(int32_t x,const gin_prepared_t *p) {
    int32_t xc,term;
    const int32_t third=SIGNAL_ONE/3; /* compile-time division */
    if(!p->dist_strong) {
        xc=clamp(gin_sub_sat(x,third),p->dist_clip);
        term=gin_sub_sat(absolute(xc),gin_mul_shift(xc,xc,GIN_SIGNAL_BITS));
        term=gin_add_sat(apply(term,p->dist_drive),SIGNAL_ONE);
        return apply(gin_sub_sat(gin_mul_shift(xc,term,GIN_SIGNAL_BITS),p->dist_bias),p->dist_gain);
    }
    int32_t driven=apply(x,p->dist_drive),sum;
    uint32_t magnitude=gin_magnitude(driven);
    if(magnitude>=2u*SIGNAL_ONE)sum=2*SIGNAL_ONE;
    else {
        uint32_t index=magnitude>>11,frac=magnitude&2047u;
        int32_t low=gin_drive_curve[index],delta=gin_drive_curve[index+1u]-low;
        sum=low+((delta*(int32_t)frac+1024)>>11);
    }
    if(driven<0)sum=-sum;
    return apply(sum>>1,p->dist_gain);
}
int gin_integer_trigger(gin_integer_voice_t *v,const gin_prepared_t *p) {
    uint32_t i;
    if(!v||!p||p->magic!=GIN_INTEGER_MAGIC||!p->total||p->total>441000u)return -1;
    /* Prepared input is copied in full; only the following mutable state needs
     * clearing. Word transfers match the native row/voice staging approach,
     * without permitting compiler-generated memcpy/memset callbacks. */
#if defined(__GNUC__)
    typedef uint32_t trigger_word_t __attribute__((__may_alias__));
    typedef char prepared_words[(sizeof(*p)%4u==0u)?1:-1];
    typedef char state_words[((sizeof(*v)-offsetof(gin_integer_voice_t,n))%4u==0u)?1:-1];
    volatile trigger_word_t *dst=(volatile trigger_word_t *)&v->p;
    const volatile trigger_word_t *src=(const volatile trigger_word_t *)p;
    (void)sizeof(prepared_words);(void)sizeof(state_words);
    for(i=0;i<sizeof(*p)/4u;++i)dst[i]=src[i];
    dst=(volatile trigger_word_t *)&v->n;
    for(i=0;i<(sizeof(*v)-offsetof(gin_integer_voice_t,n))/4u;++i)dst[i]=0;
#else
    volatile unsigned char *dst=(volatile unsigned char *)&v->p;
    const volatile unsigned char *src=(const volatile unsigned char *)p;
    for(i=0;i<sizeof(*p);++i)dst[i]=src[i];
    dst=(volatile unsigned char *)&v->n;
    for(i=0;i<sizeof(*v)-offsetof(gin_integer_voice_t,n);++i)dst[i]=0;
#endif
    v->active=1;v->random=123456789u;v->osc_env=v->noise_env=1073742;
    v->mod_env=ONE;v->ring_phase=p->ring_phase;
    /* Match the corrected reference trigger phase and deterministic hold seed. */
    if(p->mod_mode==1u)v->mod_s=ONE;else v->mod_c=ONE;
    if(p->mod_mode==2u)v->mod_state=random_signal(v);
    /* Preserve the old Q28 rounding where already admitted. Very strong drive, long
     * near-unit resonators and large ring gain retain the wide renderer.
     * Setup chooses the largest fractional precision that fits a coefficient;
     * fractions25/28/26 establish gain<64/<8/<32 without callback division. */
#if GIN_FAST_FIXED
    v->noise_compact=28u;v->eq_compact=28u;
#else
    uint32_t ok=compact_noise_ok(&p->noise_filter);
    v->noise_compact=ok&&!(p->use_dist&&p->use_eq)&&p->noise_filter.c[4].value<ONE-(ONE>>6)?1u:0u;
    if(!v->noise_compact&&ok&&(!p->use_dist||p->dist_drive.fraction>=25u)&&
       (!p->use_dist||p->dist_drive.fraction>=28u||p->noise_filter.c[4].value<ONE-(ONE>>11))&&
       (p->noise_mode!=2u||!p->ring_enabled||p->ring_gain.fraction>=26u))v->noise_compact=1u;
    v->eq_compact=p->use_eq!=0;
#endif
    return 0;
}
void gin_integer_release(gin_integer_voice_t *v) {if(v)v->active=0;}
uint32_t gin_integer_status(const gin_integer_voice_t *v) {return v&&v->active?v->n+1u:0u;}
#if GIN_BLOCK_STATE && GIN_FAST_FIXED
/* Internal bounded block: public validation supplies1..64 frames. */
static __attribute__((noinline)) int gin_integer_render_block64(gin_integer_voice_t *v,int16_t *out,uint32_t frames){

 int32_t sig[64],raw[64],weight[64];
 const gin_prepared_t *p=&v->p;uint32_t count=0,start=v->n;
 const gin_filter_coefficients_t *nf=&p->noise_filter,*ef=&p->eq_filter;
 uint32_t nq=!((nf->c[1].fraction^30u)|(nf->c[2].fraction^30u)|(nf->c[3].fraction^30u)|(nf->c[4].fraction^30u));
 uint32_t eq=!((ef->c[1].fraction^30u)|(ef->c[2].fraction^30u)|(ef->c[3].fraction^30u)|(ef->c[4].fraction^30u));
 if(nq&&df1_eligible(nf))nq|=2u;if(eq&&df1_eligible(ef))eq|=2u;
 for(uint32_t i=0;i<frames;i++){
  int32_t oe,ne,mod,osc=0;
  if(!v->active){out[i]=0;continue;}
        if(p->osc_gain.value){
        oe=envelope(v->n,p->osc_attack,p->osc_decay_seed,&v->osc_env,p->osc_attack_c,p->osc_decay_c);
        if(!p->pitch_scale){
            if(p->mod_mode==2u)(void)random_signal(v);
            v->phase+=p->phase_step;
        }else{
        if(p->mod_mode==0u) {mod=v->mod_env;v->mod_env=apply(v->mod_env,p->mod_decay_c);}
        else if(p->mod_mode==1u) {
            int32_t ms=gin_add_sat(apply(v->mod_s,p->rot_c),apply(v->mod_c,p->rot_s));
            int32_t mc=gin_sub_sat(apply(v->mod_c,p->rot_c),apply(v->mod_s,p->rot_s));
            mod=gin_mul_shift(v->mod_s,v->mod_env,30);v->mod_s=ms;v->mod_c=mc;
            v->mod_env=apply(v->mod_env,p->mod_decay_c);
        } else {
            v->mod_state=gin_add_sat(apply(v->mod_state,p->one_minus_alpha),apply(random_signal(v),p->alpha));mod=v->mod_state;
        }
        v->phase+=phase_product(p->phase_step,exp2_pitch(gin_mul_shift(mod,p->pitch_scale,30)));
        }
        if(p->wave==0u)osc=sine(v->phase);
        else if(p->wave==1u)osc=(int32_t)(((uint32_t)absolute((int32_t)(v->phase>>1)-ONE)<<1)-(uint32_t)ONE);
        else osc=(int32_t)(v->phase>>1)-ONE;
        osc=gin_mul_shift(osc,oe,30)>>(30u-GIN_SIGNAL_BITS);
        osc=apply(osc,p->osc_gain);
        }else if(p->mod_mode==2u)(void)random_signal(v);
        if(p->noise_gain.value){
        if(p->noise_mode==2u) {
            if(v->n<p->noise_attack)ne=ONE;
            else {
                int32_t decay=envelope(v->n,p->noise_attack,p->noise_decay_seed,&v->noise_env,p->noise_attack_c,p->noise_decay_c);
                if(p->ring_enabled) {
#if GIN_FAST_FIXED
                    int32_t tri;
                    /* Q32 turns: off-callback preparation supplies the first
                     * decay sample phase and increment. One wraparound add
                     * replaces the positive-binary32 rounding model. This
                     * deliberately approximates reference phase rounding. */
                    tri=ONE-absolute((int32_t)(v->ring_phase>>1)-ONE);
                    v->ring_phase+=p->ring_step;
#else
                    uint32_t time=gin_word_mul(gin_u32_word(v->n),p->time_inverse_word);
                    uint32_t dt=gin_word_sub(time,p->noise_attack_word);
                    int32_t tri;
                    v->ring_phase=gin_word_fraction(gin_word_mul(dt,p->ring_inverse_word));
                    tri=ONE-absolute((int32_t)(v->ring_phase>>1)-ONE);
#endif
                    ne=gin_mul_shift(sine(0x40000000u+((uint32_t)tri<<1)),decay,30);
                } else ne=decay;
            }
        } else if(p->noise_mode==1u) {
            if(v->n==p->linear_attack)v->linear_env=p->linear_decay_seed;
            if(v->n<p->linear_attack) {ne=v->linear_env;v->linear_env=gin_add_sat(v->linear_env,p->linear_attack_step);}
            else if(v->n<p->linear_end) {ne=v->linear_env;v->linear_env=gin_sub_sat(v->linear_env,p->linear_decay_step);}
            else ne=0;
        } else ne=envelope(v->n,p->noise_attack,p->noise_decay_seed,&v->noise_env,p->noise_attack_c,p->noise_decay_c);

  raw[i]=random_signal(v)>>(30u-GIN_SIGNAL_BITS);weight[i]=ne;
  }else {raw[i]=0;weight[i]=0;(void)random_signal(v);}
  sig[i]=osc;count++;
  if(++v->n>=p->total)v->active=0;
 }
 if(p->noise_gain.value){
  gin_filter_block(raw,count,nf,v->noise_z,&v->noise_compact,nq);
  for(uint32_t i=0;i<count;i++){
   int32_t noise=gin_mul_shift(raw[i],weight[i],30);
   if(p->noise_mode==2u&&start+i>=p->noise_attack)noise=apply(noise,p->ring_gain);
   noise=apply(noise,p->noise_gain);sig[i]=gin_add_sat(sig[i],noise);
  }
 }
 if(p->use_dist)for(uint32_t i=0;i<count;i++)sig[i]=distort(sig[i],p);
 if(p->use_eq)gin_filter_block(sig,count,ef,v->eq_z,&v->eq_compact,eq);
 for(uint32_t i=0;i<count;i++){
  int32_t sample=apply(sig[i],p->level);
  if(sample>=SIGNAL_ONE)out[i]=32767;
  else if(sample<=-SIGNAL_ONE)out[i]=-32768;
  else out[i]=(int16_t)(sample<0?-((-sample)>>(GIN_SIGNAL_BITS-15u)):(sample>>(GIN_SIGNAL_BITS-15u)));
 }
 return 0;

}
#endif
int gin_integer_render_frames(gin_integer_voice_t *v,int16_t *out,uint32_t frames) {
#if GIN_BLOCK_STATE && GIN_FAST_FIXED
    if(!v||!out||frames>GIN_INTEGER_BLOCK)return -1;
    if(!frames)return 0;
    if(frames>64u){
        (void)gin_integer_render_block64(v,out,64u);
        return gin_integer_render_block64(v,out+64u,frames-64u);
    }
    return gin_integer_render_block64(v,out,frames);
#else

    uint32_t i;
    if(!v||!out||frames>GIN_INTEGER_BLOCK)return -1;
    if(!frames)return 0;
#if GIN_FAST_FIXED
    /* Prepared coefficients are immutable throughout this render block.
     * Cache the common filter format once instead of dispatching four
     * multiplications again for every sample; voice/prepared layouts stay fixed. */
    const gin_filter_coefficients_t *nf=&v->p.noise_filter,*ef=&v->p.eq_filter;
    uint32_t nq=!((nf->c[1].fraction^30u)|(nf->c[2].fraction^30u)|(nf->c[3].fraction^30u)|(nf->c[4].fraction^30u));
    uint32_t eq=!((ef->c[1].fraction^30u)|(ef->c[2].fraction^30u)|(ef->c[3].fraction^30u)|(ef->c[4].fraction^30u));
    /* Bit1 selects DF1; bit0 retains the legacy common-format flag. */
    if(nq&&df1_eligible(nf))nq|=2u;
    if(eq&&df1_eligible(ef))eq|=2u;
#endif
    for(i=0;i<frames;++i) {
        int32_t oe,ne,mod,osc=0,noise=0,sample;
        const gin_prepared_t *p=&v->p;
        if(!v->active){out[i]=0;continue;}
        /* Coefficients are immutable for this hit. Muted components cannot
         * become audible later; preserve the shared RNG ticks when bypassed. */
        if(p->osc_gain.value){
        oe=envelope(v->n,p->osc_attack,p->osc_decay_seed,&v->osc_env,p->osc_attack_c,p->osc_decay_c);
        if(!p->pitch_scale){
            if(p->mod_mode==2u)(void)random_signal(v);
            v->phase+=p->phase_step;
        }else{
        if(p->mod_mode==0u) {mod=v->mod_env;v->mod_env=apply(v->mod_env,p->mod_decay_c);}
        else if(p->mod_mode==1u) {
            int32_t ms=gin_add_sat(apply(v->mod_s,p->rot_c),apply(v->mod_c,p->rot_s));
            int32_t mc=gin_sub_sat(apply(v->mod_c,p->rot_c),apply(v->mod_s,p->rot_s));
            mod=gin_mul_shift(v->mod_s,v->mod_env,30);v->mod_s=ms;v->mod_c=mc;
            v->mod_env=apply(v->mod_env,p->mod_decay_c);
        } else {
            v->mod_state=gin_add_sat(apply(v->mod_state,p->one_minus_alpha),apply(random_signal(v),p->alpha));mod=v->mod_state;
        }
        v->phase+=phase_product(p->phase_step,exp2_pitch(gin_mul_shift(mod,p->pitch_scale,30)));
        }
        if(p->wave==0u)osc=sine(v->phase);
        else if(p->wave==1u)osc=(int32_t)(((uint32_t)absolute((int32_t)(v->phase>>1)-ONE)<<1)-(uint32_t)ONE);
        else osc=(int32_t)(v->phase>>1)-ONE;
        osc=gin_mul_shift(osc,oe,30)>>(30u-GIN_SIGNAL_BITS);
        osc=apply(osc,p->osc_gain);
        }else if(p->mod_mode==2u)(void)random_signal(v);
        if(p->noise_gain.value){
        if(p->noise_mode==2u) {
            if(v->n<p->noise_attack)ne=ONE;
            else {
                int32_t decay=envelope(v->n,p->noise_attack,p->noise_decay_seed,&v->noise_env,p->noise_attack_c,p->noise_decay_c);
                if(p->ring_enabled) {
#if GIN_FAST_FIXED
                    int32_t tri;
                    /* Q32 turns: off-callback preparation supplies the first
                     * decay sample phase and increment. One wraparound add
                     * replaces the positive-binary32 rounding model. This
                     * deliberately approximates reference phase rounding. */
                    tri=ONE-absolute((int32_t)(v->ring_phase>>1)-ONE);
                    v->ring_phase+=p->ring_step;
#else
                    uint32_t time=gin_word_mul(gin_u32_word(v->n),p->time_inverse_word);
                    uint32_t dt=gin_word_sub(time,p->noise_attack_word);
                    int32_t tri;
                    v->ring_phase=gin_word_fraction(gin_word_mul(dt,p->ring_inverse_word));
                    tri=ONE-absolute((int32_t)(v->ring_phase>>1)-ONE);
#endif
                    ne=gin_mul_shift(sine(0x40000000u+((uint32_t)tri<<1)),decay,30);
                } else ne=decay;
            }
        } else if(p->noise_mode==1u) {
            if(v->n==p->linear_attack)v->linear_env=p->linear_decay_seed;
            if(v->n<p->linear_attack) {ne=v->linear_env;v->linear_env=gin_add_sat(v->linear_env,p->linear_attack_step);}
            else if(v->n<p->linear_end) {ne=v->linear_env;v->linear_env=gin_sub_sat(v->linear_env,p->linear_decay_step);}
            else ne=0;
        } else ne=envelope(v->n,p->noise_attack,p->noise_decay_seed,&v->noise_env,p->noise_attack_c,p->noise_decay_c);
#if GIN_FAST_FIXED
        noise=noise_filter(random_signal(v)>>(30u-GIN_SIGNAL_BITS),v,nq);
#else
        noise=noise_filter(random_signal(v)>>(30u-GIN_SIGNAL_BITS),v);
#endif
        noise=gin_mul_shift(noise,ne,30);
        if(p->noise_mode==2u&&v->n>=p->noise_attack)noise=apply(noise,p->ring_gain);
        noise=apply(noise,p->noise_gain);
        }else (void)random_signal(v);
        sample=gin_add_sat(osc,noise);
        if(p->use_dist)sample=distort(sample,p);
#if GIN_FAST_FIXED
        if(p->use_eq)sample=eq_filter_compact(sample,v,eq);
#else
        if(p->use_eq)sample=eq_filter_compact(sample,v);
#endif
        sample=apply(sample,p->level);
        if(sample>=SIGNAL_ONE)out[i]=32767;
        else if(sample<=-SIGNAL_ONE)out[i]=-32768;
        else out[i]=(int16_t)(sample<0?-((-sample)>>(GIN_SIGNAL_BITS-15u)):(sample>>(GIN_SIGNAL_BITS-15u)));
        if(++v->n>=p->total)v->active=0;
    }
    return 0;

#endif
}

/* Keep native 128-sample callers and the logical 44.1 kHz core explicit.
 * Reblocking changes no sample operation or owned voice-state layout. */
int gin_integer_render(gin_integer_voice_t *v,int16_t *out) {
    return gin_integer_render_frames(v,out,GIN_INTEGER_BLOCK);
}
int gin_integer_render64(gin_integer_voice_t *v,int16_t *out) {
    return gin_integer_render_frames(v,out,64u);
}
