/* Copyright (c) 2026 SquareWave Studio. MIT; see LICENSE.
 * libpo32-derived portions: Copyright (c) 2026 Eric Lewis. */
/* Owned32-bit adaptive fixed-point filter state. Derived recurrence follows
 * libpo32, Eric Lewis2026, MIT; see LICENSE. Coefficients remain unchanged.
 * Q28 normally, bounded same-sample retries Q24/Q20/Q16/Q12 for headroom.
 * No paired-word feedback, division, heap or libc. Included after apply(). */
#ifndef GIN_FIXED_FILTER_H
#define GIN_FIXED_FILTER_H
#if defined(GIN_APPROX_METRICS)
uint32_t gin_approx_saturations,gin_approx_peak_state,gin_approx_downshifts,gin_approx_min_fraction;
#endif
static int endpoint(int32_t x){return x==INT32_MAX||x==INT32_MIN;}
static int32_t rescale_state(int32_t x,uint32_t from,uint32_t to,int *overflow){
    uint32_t m=gin_magnitude(x),negative=x<0;
    if(to>from){uint32_t shift=to-from,limit=((uint32_t)INT32_MAX+negative)>>shift;
        if(m>limit){*overflow=1;return negative?INT32_MIN:INT32_MAX;}
        m<<=shift;
    }else if(to<from){uint32_t shift=from-to;m=(m+(1u<<(shift-1u)))>>shift;}
    return negative?(m==0x80000000u?INT32_MIN:-(int32_t)m):(int32_t)m;
}
static int32_t adaptive_filter(int32_t input,const gin_filter_coefficients_t *p,gin_pair_t *z,uint32_t *fraction){
    uint32_t bits=*fraction;int32_t old0=(int32_t)z[0].lo,old1=(int32_t)z[1].lo;
    for(;;){
        int overflow=0;int32_t x=rescale_state(input,20u,bits,&overflow);
        int32_t b0=apply(x,p->c[0]),y=gin_add_sat(b0,old0);
        int32_t b1=apply(x,p->c[1]),a1=apply(y,p->c[3]);
        int32_t b2=apply(x,p->c[2]),a2=apply(y,p->c[4]);
        int32_t t=gin_sub_sat(b1,a1),s0=gin_add_sat(t,old1),u=gin_sub_sat(b2,a2);
        overflow=overflow||endpoint(b0)||endpoint(y)||endpoint(b1)||endpoint(a1)||
            endpoint(b2)||endpoint(a2)||endpoint(t)||endpoint(s0)||endpoint(u);
        if(overflow&&bits>12u){
            /* Retry from pre-sample states: never commit the failed update. */
            int ignored=0;old0=rescale_state(old0,bits,bits-4u,&ignored);old1=rescale_state(old1,bits,bits-4u,&ignored);bits-=4u;
#if defined(GIN_APPROX_METRICS)
            ++gin_approx_downshifts;
#endif
            continue;
        }
        z[0].lo=(uint32_t)s0;z[1].lo=(uint32_t)u;*fraction=bits;
#if defined(GIN_APPROX_METRICS)
        gin_approx_saturations+=overflow!=0;
        if(bits<gin_approx_min_fraction)gin_approx_min_fraction=bits;
        uint32_t m=gin_magnitude(s0);if(m>gin_approx_peak_state)gin_approx_peak_state=m;
        m=gin_magnitude(u);if(m>gin_approx_peak_state)gin_approx_peak_state=m;
#endif
        int ignored=0;return rescale_state(y,bits,20u,&ignored);
    }
}
/* Constant-fraction lower-headroom paths; preserve pre-sample retry semantics. */
static int32_t filter_lower(int32_t input,const gin_filter_coefficients_t *p,gin_pair_t *z,uint32_t *fraction,uint32_t common){
    uint32_t bits=*fraction;int32_t x;
    if(bits==24u){
        if(input>134217727||input<-134217728)return adaptive_filter(input,p,z,fraction);
        x=input*16;
    }else if(bits==20u)x=input;
    else return adaptive_filter(input,p,z,fraction);
    /* Feed-forward/feedback terms1..4 are Q30 in prepared factory filters.
     * Check the common format once, then avoid four helper dispatches. */
    if(!common)return adaptive_filter(input,p,z,fraction);
    int32_t b0=apply(x,p->c[0]),y=gin_add_sat(b0,(int32_t)z[0].lo);
    int32_t b1=gin_mul_shift(x,p->c[1].value,30u),a1=gin_mul_shift(y,p->c[3].value,30u);
    int32_t b2=gin_mul_shift(x,p->c[2].value,30u),a2=gin_mul_shift(y,p->c[4].value,30u);
    int32_t t=gin_sub_sat(b1,a1),s0=gin_add_sat(t,(int32_t)z[1].lo),u=gin_sub_sat(b2,a2);
    if(endpoint(b0)||endpoint(y)||endpoint(b1)||endpoint(a1)||endpoint(b2)||
       endpoint(a2)||endpoint(t)||endpoint(s0)||endpoint(u))return adaptive_filter(input,p,z,fraction);
    z[0].lo=(uint32_t)s0;z[1].lo=(uint32_t)u;
#if defined(GIN_APPROX_METRICS)
    if(bits<gin_approx_min_fraction)gin_approx_min_fraction=bits;
    uint32_t m=gin_magnitude(s0);if(m>gin_approx_peak_state)gin_approx_peak_state=m;
    m=gin_magnitude(u);if(m>gin_approx_peak_state)gin_approx_peak_state=m;
#endif
    if(bits==20u)return y;
    uint32_t magnitude=gin_magnitude(y);int32_t result=(int32_t)((magnitude+8u)>>4);
    return y<0?-result:result;
}
/* Exact common Q28 specialization; the generic retry sees unchanged states. */
static int32_t filter_q28(int32_t input,const gin_filter_coefficients_t *p,gin_pair_t *z,uint32_t *fraction,uint32_t common){
    if(*fraction!=28u||input>8388607||input<-8388608)return filter_lower(input,p,z,fraction,common);
    int32_t x=input*256;
    /* Feed-forward/feedback terms1..4 are Q30 in prepared factory filters.
     * Check the common format once, then avoid four helper dispatches. */
    if(!common)return adaptive_filter(input,p,z,fraction);
    int32_t b0=apply(x,p->c[0]),y=gin_add_sat(b0,(int32_t)z[0].lo);
    int32_t b1=gin_mul_shift(x,p->c[1].value,30u),a1=gin_mul_shift(y,p->c[3].value,30u);
    int32_t b2=gin_mul_shift(x,p->c[2].value,30u),a2=gin_mul_shift(y,p->c[4].value,30u);
    int32_t t=gin_sub_sat(b1,a1),s0=gin_add_sat(t,(int32_t)z[1].lo),u=gin_sub_sat(b2,a2);
    if(endpoint(b0)||endpoint(y)||endpoint(b1)||endpoint(a1)||endpoint(b2)||
       endpoint(a2)||endpoint(t)||endpoint(s0)||endpoint(u))return adaptive_filter(input,p,z,fraction);
    z[0].lo=(uint32_t)s0;z[1].lo=(uint32_t)u;
#if defined(GIN_APPROX_METRICS)
    if(28u<gin_approx_min_fraction)gin_approx_min_fraction=28u;
    uint32_t m=gin_magnitude(s0);if(m>gin_approx_peak_state)gin_approx_peak_state=m;
    m=gin_magnitude(u);if(m>gin_approx_peak_state)gin_approx_peak_state=m;
#endif
    uint32_t magnitude=gin_magnitude(y);int32_t result=(int32_t)((magnitude+128u)>>8);
    return y<0?-result:result;
}
static int32_t original_noise_filter(int32_t input,gin_integer_voice_t *v,uint32_t common){return filter_q28(input,&v->p.noise_filter,v->noise_z,&v->noise_compact,common);}
static int32_t original_eq_filter_compact(int32_t input,gin_integer_voice_t *v,uint32_t common){return filter_q28(input,&v->p.eq_filter,v->eq_z,&v->eq_compact,common);}
#include "gin_fixed_filter_df1.h"
#endif
