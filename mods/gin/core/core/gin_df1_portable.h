/* Copyright (c) 2026 SquareWave Studio. MIT; see LICENSE. */
#ifndef GIN_DF1_PORTABLE_H
#define GIN_DF1_PORTABLE_H
/* Exact floor((x*b0+x1*b1+x2*b2-y1*a1-y2*a2)/2^30).
 * Five signed products need more than64 bits before cancellation. Retain
 * three unsigned words, then clamp only after the final arithmetic shift.
 * Optional GIN_DF1_DOT5 has this argument order and must OR strict
 * out-of-range status into *overflow; exact INT_MIN/MAX are not overflow.
 * Included after gin_product32, with gin_filter_coefficients_t available. */
static inline __attribute__((always_inline)) int32_t gin_df1_dot5(
 int32_t x,int32_t x1,int32_t x2,int32_t y1,int32_t y2,
 const gin_filter_coefficients_t *p,int *overflow){
#if defined(GIN_DF1_DOT5)
 return GIN_DF1_DOT5(x,x1,x2,y1,y2,p,overflow);
#else
 const int32_t values[5]={x,x1,x2,y1,y2};
 uint32_t lo=0,mid=0,hi=0;
 for(unsigned j=0;j<5;j++){
  gin_pair_t product=gin_product32(gin_magnitude(values[j]),gin_magnitude(p->c[j].value));
  uint32_t tlo=product.lo,tmid=product.hi,thi=0;
  if(((values[j]<0)!=(p->c[j].value<0))!=(j>=3)){
   tlo=0u-tlo;tmid=~tmid+(tlo==0);thi=UINT32_MAX+(uint32_t)(tlo==0&&tmid==0);
  }
  uint32_t old=lo;lo+=tlo;uint32_t carry=lo<old;
  old=mid;mid+=tmid;uint32_t upper=mid<old;
  old=mid;mid+=carry;upper|=mid<old;
  hi+=thi+upper;
 }
 const uint32_t negative=hi>>31;
 if(negative?(hi!=UINT32_MAX||mid<0xe0000000u):(hi!=0||mid>=0x20000000u)){
  *overflow|=1;return negative?INT32_MIN:INT32_MAX;
 }
 const uint32_t word=(lo>>30)|(mid<<2);
 return word>INT32_MAX?-1-(int32_t)~word:(int32_t)word;
#endif
}
#endif
