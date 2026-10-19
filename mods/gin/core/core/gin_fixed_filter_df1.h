/* Copyright (c) 2026 SquareWave Studio. MIT; see LICENSE.
 * libpo32-derived portions: Copyright (c) 2026 Eric Lewis. */
/* Owned DF1 common path for the BF524 dual16-bit MACs.
 * Coefficients and samples retain32-bit precision. Five32x32 products are
 * accumulated with40-bit native partial sums; no target C wide integer.
 * Sensitive pole settings retain the previous DF2T recurrence. Selection is
 * cached outside the sample loop and never changes within a prepared hit.
 * The host-only wide expression is an arithmetic oracle for the native kernel.
 * Included by gin_fixed_filter.h after the unchanged DF2T helpers.
 */
#ifndef GIN_FIXED_FILTER_DF1_H
#define GIN_FIXED_FILTER_DF1_H
/* Register-input dot product: preserves five C history operands. */
static inline __attribute__((always_inline)) int32_t df1_register_dot(int32_t x,int32_t x1,int32_t x2,int32_t y1,int32_t y2,const gin_filter_coefficients_t *p){
#if defined(__bfin__) && !defined(GIN_PORTABLE_ARITHMETIC)
 int32_t result;
 __asm__ volatile(
  "I1 = %6;\n\t"
  "%0 = 0;\n\t"
  "L1 = %0;\n\t"
  "%0 = 8;\n\t"
  "M1 = %0;\n\t"
  "%0 = -8;\n\t"
  "A0 = %0;\n\t"
  "A1 = 0;\n\t"
  "%0 = 4;\n\t"
  "A1.X = %0.L;\n\t"
  "%0 = [I1++M1];\n\t"
  "A1 += %1.L * %0.L (FU) || %0 = [I1++M1];\n\t"
  "A1 += %2.L * %0.L (FU) || %0 = [I1++M1];\n\t"
  "A1 += %3.L * %0.L (FU) || %0 = [I1++M1];\n\t"
  "A1 -= %4.L * %0.L (FU) || %0 = [I1++M1];\n\t"
  "A1 -= %5.L * %0.L (FU);\n\t"
  "A1 = A1 >>> 16;\n\t"
  "I1 = %6;\n\t"
  "%0 = [I1++M1];\n\t"
  "A1 += %1.H * %0.L (M), A0 += %1.H * %0.H;\n\t"
  "A1 += %0.H * %1.L (M) || %0 = [I1++M1];\n\t"
  "A1 += %2.H * %0.L (M), A0 += %2.H * %0.H;\n\t"
  "A1 += %0.H * %2.L (M) || %0 = [I1++M1];\n\t"
  "A1 += %3.H * %0.L (M), A0 += %3.H * %0.H;\n\t"
  "A1 += %0.H * %3.L (M) || %0 = [I1++M1];\n\t"
  "A1 -= %4.H * %0.L (M), A0 -= %4.H * %0.H;\n\t"
  "A1 -= %0.H * %4.L (M) || %0 = [I1++M1];\n\t"
  "A1 -= %5.H * %0.L (M), A0 -= %5.H * %0.H;\n\t"
  "A1 -= %0.H * %5.L (M);\n\t"
  "A0 = A0 << 1;\n\t"
  "A1 = A1 >>> 14;\n\t"
  "%0 = (A0 += A1);\n\t"
  : "=&d"(result) : "d"(x),"d"(x1),"d"(x2),"d"(y1),"d"(y2),"a"(p)
  : "I1","L1","M1","A0","A1","ASTAT","memory");
 return result;
#else
 int32_t values[5]={x,x1,x2,y1,y2};__int128 wide=0;
 for(unsigned j=0;j<5;j++){__int128 term=(__int128)values[j]*p->c[j].value;wide+=j<3?term:-term;}
 wide>>=30;return wide>INT32_MAX?INT32_MAX:wide<INT32_MIN?INT32_MIN:(int32_t)wide;
#endif
}
#if defined(__bfin__) && !defined(GIN_PORTABLE_ARITHMETIC)
static inline __attribute__((always_inline)) int32_t df1_dot5(const int32_t *values,const gin_filter_coefficients_t *p){return df1_register_dot(values[0],values[1],values[2],values[3],values[4],p);}
#endif
static __attribute__((noinline)) int32_t df1_scale(int32_t x,uint32_t from,uint32_t to,int *overflow){
 uint32_t m=gin_magnitude(x),neg=x<0;
 if(to>from){uint32_t s=to-from,limit=((uint32_t)INT32_MAX+neg)>>s;if(m>limit){*overflow=1;return neg?INT32_MIN:INT32_MAX;}m<<=s;}
 else if(to<from){uint32_t s=from-to;m=(m+(1u<<(s-1)))>>s;}
 return neg?(m==0x80000000u?INT32_MIN:-(int32_t)m):(int32_t)m;
}
#if defined(__GNUC__) && defined(__bfin__)
__attribute__((noinline,optimize("Os")))
#endif
static int32_t df1_filter(int32_t input,const gin_filter_coefficients_t *p,gin_pair_t *z,uint32_t *fraction){
 uint32_t bits=*fraction;int32_t x1=(int32_t)z[0].hi,x2=(int32_t)z[1].hi,y1=(int32_t)z[0].lo,y2=(int32_t)z[1].lo;
 for(;;){
  int overflow=0;int32_t x;
  if(bits==20u)x=input;
  else if(bits==24u&&input<=134217727&&input>=-134217728)x=input*16;
  else x=df1_scale(input,20,bits,&overflow);
  int32_t sum;
#if defined(__bfin__) && !defined(GIN_PORTABLE_ARITHMETIC)
  int32_t values[5]={x,x1,x2,y1,y2};
  sum=df1_dot5(values,p);
  overflow|=sum==INT32_MAX||sum==INT32_MIN;
#else
  __int128 wide=0;int32_t values[5]={x,x1,x2,y1,y2};
  for(unsigned i=0;i<5;i++){__int128 term=(__int128)values[i]*p->c[i].value;wide+=i<3?term:-term;}
  wide>>=30;overflow|=wide>INT32_MAX||wide<INT32_MIN;sum=wide>INT32_MAX?INT32_MAX:wide<INT32_MIN?INT32_MIN:(int32_t)wide;
#endif

  if(overflow&&bits>12){int ignored=0;x1=df1_scale(x1,bits,bits-4,&ignored);x2=df1_scale(x2,bits,bits-4,&ignored);y1=df1_scale(y1,bits,bits-4,&ignored);y2=df1_scale(y2,bits,bits-4,&ignored);bits-=4;
#if defined(GIN_APPROX_METRICS)
 gin_approx_downshifts++;
#endif
 continue;}
  int32_t y=sum;
#if defined(GIN_APPROX_METRICS)
  gin_approx_saturations+=overflow!=0;if(bits<gin_approx_min_fraction)gin_approx_min_fraction=bits;
#endif
  z[0].hi=(uint32_t)x;z[1].hi=(uint32_t)x1;z[0].lo=(uint32_t)y;z[1].lo=(uint32_t)y1;*fraction=bits;
  if(bits==20u)return y;
  if(bits==24u){uint32_t m=gin_magnitude(y);int32_t out=(int32_t)((m+8u)>>4);return y<0?-out:out;}
  int ignored=0;return df1_scale(y,bits,20,&ignored);
 }
}
static inline __attribute__((always_inline)) int32_t df1_q28(int32_t input,const gin_filter_coefficients_t *p,gin_pair_t *z,uint32_t *fraction){
 if(*fraction==28u&&input<=8388607&&input>=-8388608){
  int32_t x=input*256,values[5]={x,(int32_t)z[0].hi,(int32_t)z[1].hi,(int32_t)z[0].lo,(int32_t)z[1].lo};
  int32_t y;
#if defined(__bfin__) && !defined(GIN_PORTABLE_ARITHMETIC)
  y=df1_dot5(values,p);
#else
  __int128 wide=0;for(unsigned i=0;i<5;i++){__int128 term=(__int128)values[i]*p->c[i].value;wide+=i<3?term:-term;}wide>>=30;y=wide>INT32_MAX?INT32_MAX:wide<INT32_MIN?INT32_MIN:(int32_t)wide;
#endif
  if(y!=INT32_MAX&&y!=INT32_MIN){
   z[1].hi=z[0].hi;z[0].hi=(uint32_t)x;z[1].lo=z[0].lo;z[0].lo=(uint32_t)y;
   uint32_t m=gin_magnitude(y);int32_t out=(int32_t)((m+128u)>>8);return y<0?-out:out;
  }
 }
 return df1_filter(input,p,z,fraction);
}
/* Keep the previous recurrence for poles very near DC/Nyquist or the unit circle.
 * The selector depends only on immutable prepared coefficients, so a hit never
 * reinterprets its running state. Q30 sums below use safe unsigned offsets. */
static int df1_eligible(const gin_filter_coefficients_t *p){
 const int32_t a2=p->c[4].value;
 const uint32_t one=0x40000000u,margin=one>>10;
 return p->c[0].fraction==30u && gin_magnitude(a2)<one-margin &&
        gin_magnitude(p->c[3].value)<=one+(uint32_t)a2-margin;
}
#if GIN_BLOCK_STATE
#define GIN_FILTER_UNUSED __attribute__((unused))
#else
#define GIN_FILTER_UNUSED
#endif
static GIN_FILTER_UNUSED int32_t noise_filter(int32_t input,gin_integer_voice_t *v,uint32_t common){
 if(!(common&2u))return original_noise_filter(input,v,common);
 return df1_q28(input,&v->p.noise_filter,v->noise_z,&v->noise_compact);
}
static GIN_FILTER_UNUSED int32_t eq_filter_compact(int32_t input,gin_integer_voice_t *v,uint32_t common){
 if(!(common&2u))return original_eq_filter_compact(input,v,common);
 return df1_q28(input,&v->p.eq_filter,v->eq_z,&v->eq_compact);
}

#undef GIN_FILTER_UNUSED
#if GIN_BLOCK_STATE
static __attribute__((noinline)) void gin_filter_block(int32_t *data,uint32_t count,const gin_filter_coefficients_t *p,gin_pair_t *z,uint32_t *fraction,uint32_t common){
 if(!(common&2u)){for(uint32_t i=0;i<count;i++)data[i]=filter_q28(data[i],p,z,fraction,common);return;}
 uint32_t i=0;
 while(i<count){
  uint32_t bits=*fraction;
  if(bits<20u||bits>28u){for(;i<count;i++)data[i]=df1_filter(data[i],p,z,fraction);return;}
  uint32_t shift=bits-20u,rounding=shift?1u<<(shift-1u):0;
  int32_t limit=INT32_MAX>>shift;
  int32_t x1=(int32_t)z[0].hi,x2=(int32_t)z[1].hi,y1=(int32_t)z[0].lo,y2=(int32_t)z[1].lo;
  for(;i<count;i++){
   int32_t input=data[i];if(input>limit||input<-limit-1)break;
   int32_t x=(int32_t)((uint32_t)input<<shift),y=df1_register_dot(x,x1,x2,y1,y2,p);
   if(y==INT32_MAX||y==INT32_MIN)break;
   x2=x1;x1=x;y2=y1;y1=y;
   uint32_t magnitude=gin_magnitude(y);int32_t sample=(int32_t)((magnitude+rounding)>>shift);data[i]=y<0?-sample:sample;
  }
  z[0].hi=(uint32_t)x1;z[1].hi=(uint32_t)x2;z[0].lo=(uint32_t)y1;z[1].lo=(uint32_t)y2;
  if(i<count){data[i]=df1_filter(data[i],p,z,fraction);i++;}
 }
}
#endif
#endif
