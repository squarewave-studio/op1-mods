/* Copyright (c) 2026 SquareWave Studio. MIT; see LICENSE.
 * libpo32-derived portions: Copyright (c) 2026 Eric Lewis. */
/* Owned bounded Blackfin filter arithmetic; libpo32-derived closure, see LICENSE.
 * All operations match the original paired-word rounding and saturation. */
#ifndef GIN_FILTER_MATH_H
#define GIN_FILTER_MATH_H
#include "gin_integer.h"
/* Exact bounded Q30 kernels. The generic Q44 path remains the fallback. */
static gin_pair_t ff_negate(gin_pair_t a){gin_pair_t r;r.lo=0u-a.lo;r.hi=~a.hi+(r.lo==0);return r;}
static inline __attribute__((always_inline)) gin_pair_t ff_product16(uint32_t a,uint32_t b){
 uint32_t low,high;gin_pair_t p;
#if defined(__bfin__) && !defined(GIN_PORTABLE_ARITHMETIC)
 __asm__("%0 = %2.L * %3.L (FU);\n\t%1 = %2.L * %3.H (FU);":"=&d"(low),"=&d"(high):"d"(a),"d"(b):"ASTAT");
#else
 low=a*(b&65535u);high=a*(b>>16);
#endif
 p.lo=low+(high<<16);p.hi=(high>>16)+(p.lo<low);return p;
}
static inline __attribute__((always_inline)) gin_pair_t ff_signal30(int32_t x,int32_t coefficient){
 uint32_t negative=(x<0)!=(coefficient<0),old;gin_pair_t p=gin_product32(gin_magnitude(x),gin_magnitude(coefficient)),r;
 old=p.lo;p.lo+=32u;p.hi+=p.lo<old;
 r.lo=(p.lo>>6)|(p.hi<<26);r.hi=p.hi>>6;
 return negative?ff_negate(r):r;
}
static inline __attribute__((always_inline)) gin_pair_t ff_input(int32_t x,gin_coefficient_t c){
 return c.fraction==30u?ff_signal30(x,c.value):gin_wide_signal_multiply(x,c);
}
/* mag.hi<65536: |input|<2^48 and |coefficient|<=2^31, so the rounded
 * Q30 result is <2^49. Neither signed64 saturation nor96-bit overflow can occur. */
static inline __attribute__((always_inline)) gin_pair_t ff_feedback30(gin_pair_t mag,uint32_t negative,int32_t coefficient){
 uint32_t m=gin_magnitude(coefficient),mid,high,low,old;gin_pair_t a=gin_product32(mag.lo,m),b=ff_product16(mag.hi,m),r;
 mid=a.hi+b.lo;high=b.hi+(mid<a.hi);
 low=a.lo+(1u<<29);if(low<a.lo){old=mid;++mid;high+=mid<old;}
 r.lo=(low>>30)|(mid<<2);r.hi=(mid>>30)|(high<<2);
 return negative!=(coefficient<0)?ff_negate(r):r;
}
static int32_t filter(int32_t input,const gin_filter_coefficients_t *p,gin_pair_t *z) {
 gin_pair_t out=gin_wide_add(ff_input(input,p->c[0]),z[0]),a1,a2,mag=out;uint32_t negative=out.hi>>31;
 if(negative)mag=ff_negate(mag);
 if(p->c[3].fraction==30u&&p->c[4].fraction==30u&&mag.hi<65536u){
  a1=ff_feedback30(mag,negative,p->c[3].value);a2=ff_feedback30(mag,negative,p->c[4].value);
 }else{a1=gin_wide_multiply(out,p->c[3]);a2=gin_wide_multiply(out,p->c[4]);}
 z[0]=gin_wide_add(gin_wide_sub(ff_input(input,p->c[1]),a1),z[1]);
 z[1]=gin_wide_sub(ff_input(input,p->c[2]),a2);
 return gin_wide_to_signal(out);
}

#endif
