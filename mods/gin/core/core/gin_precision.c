/* Copyright (c) 2026 SquareWave Studio. MIT; see LICENSE.
 * libpo32-derived portions: Copyright (c) 2026 Eric Lewis. */
/* Owned paired-word arithmetic and bounded positive binary32 rounding model.
 * Integer operations only. No C64-bit type, float, division or library calls. */
#include "gin_integer.h"
#include <limits.h>
static gin_pair_t negate(gin_pair_t a){gin_pair_t r;r.lo=0u-a.lo;r.hi=~a.hi+(r.lo==0);return r;}
static gin_pair_t limit(uint32_t negative){gin_pair_t r;r.hi=negative?0x80000000u:0x7fffffffu;r.lo=negative?0:UINT32_MAX;return r;}
GIN_ARITHMETIC gin_pair_t gin_wide_add(gin_pair_t a,gin_pair_t b){gin_pair_t r;r.lo=a.lo+b.lo;r.hi=a.hi+b.hi+(r.lo<a.lo);if(!((a.hi^b.hi)&0x80000000u)&&((a.hi^r.hi)&0x80000000u))return limit(a.hi>>31);return r;}
GIN_ARITHMETIC gin_pair_t gin_wide_sub(gin_pair_t a,gin_pair_t b){gin_pair_t r;r.lo=a.lo-b.lo;r.hi=a.hi-b.hi-(a.lo<b.lo);if(((a.hi^b.hi)&0x80000000u)&&((a.hi^r.hi)&0x80000000u))return limit(a.hi>>31);return r;}
GIN_ARITHMETIC gin_pair_t gin_wide_multiply(gin_pair_t a,gin_coefficient_t b){
 uint32_t negative=(a.hi>>31)!=(b.value<0),m=gin_magnitude(b.value);
 uint32_t high,mid,low,overflow,s=b.fraction;gin_pair_t p0,p1,r;
 if(a.hi>>31)a=negate(a);
 p0=gin_product32(a.lo,m);p1=gin_product32(a.hi,m);low=p0.lo;mid=p0.hi+p1.lo;high=p1.hi+(mid<p0.hi);
 if(s>30u){r.hi=r.lo=0;return r;}
 if(s){uint32_t old=low;low+=1u<<(s-1u);if(low<old){++mid;high+=mid==0;}overflow=high>>s;r.lo=(low>>s)|(mid<<(32u-s));r.hi=(mid>>s)|(high<<(32u-s));}
 else{overflow=high;r.lo=low;r.hi=mid;}
 if(overflow||r.hi>0x7fffffffu+negative||(negative&&r.hi==0x80000000u&&r.lo))return limit(negative);
 return negative?negate(r):r;
}
/* Exact shortcut for the input's24 zero fractional bits: one32x32 product
 * instead of a generic96-bit product. Same signed64 saturation/ties-away. */
GIN_ARITHMETIC gin_pair_t gin_wide_signal_multiply(int32_t x,gin_coefficient_t c){
 uint32_t negative=(x<0)!=(c.value<0),a=gin_magnitude(x),b=gin_magnitude(c.value);
 gin_pair_t p=gin_product32(a,b);uint32_t s=c.fraction,overflow=0;
 if(s>30u){p.hi=p.lo=0;return p;}
 if(s>24u){s-=24u;uint32_t old=p.lo;p.lo+=1u<<(s-1u);p.hi+=p.lo<old;p.lo=(p.lo>>s)|(p.hi<<(32u-s));p.hi>>=s;}
 else if(s<24u){s=24u-s;overflow=p.hi>>(32u-s);p.hi=(p.hi<<s)|(p.lo>>(32u-s));p.lo<<=s;}
 if(overflow||p.hi>0x7fffffffu+negative||(negative&&p.hi==0x80000000u&&p.lo))return limit(negative);
 return negative?negate(p):p;
}
GIN_ARITHMETIC gin_pair_t gin_wide_from_signal(int32_t x){uint32_t m=gin_magnitude(x);gin_pair_t r;r.hi=m>>8;r.lo=m<<24;return x<0?negate(r):r;}
GIN_ARITHMETIC int32_t gin_wide_to_signal(gin_pair_t a){uint32_t negative=a.hi>>31,old,m;if(negative)a=negate(a);old=a.lo;a.lo+=1u<<23;a.hi+=a.lo<old;if(a.hi>>24)return negative?INT32_MIN:INT32_MAX;m=(a.lo>>24)|(a.hi<<8);if(m>(uint32_t)INT32_MAX+negative)return negative?INT32_MIN:INT32_MAX;return negative?(m==0x80000000u?INT32_MIN:-(int32_t)m):(int32_t)m;}

/* Right shift a positive paired integer, retaining guard/sticky for ties-even. */
static uint32_t rounded(gin_pair_t p,uint32_t s){uint32_t q,half,rem,sticky;
 if(!s)return p.lo;
 if(s<32u){q=(p.lo>>s)|(p.hi<<(32u-s));half=1u<<(s-1u);rem=p.lo&((1u<<s)-1u);sticky=0;}
 else if(s==32u){q=p.hi;half=0x80000000u;rem=p.lo;sticky=0;}
 else if(s<64u){uint32_t n=s-32u;q=p.hi>>n;half=1u<<(n-1u);rem=p.hi&((1u<<n)-1u);sticky=p.lo!=0;}
 else return 0;
 return q+(rem>half||(rem==half&&(sticky||(q&1u))));
}
/* A positive mantissa needs its top-bit index, not a per-bit loop. SIGNBITS
 * counts redundant sign bits on BF524; handle bit31 before treating it signed.
 * The portable path has the identical integer result. Zero is never passed. */
static uint32_t top_bit(uint32_t value){
#if defined(__bfin__) && !defined(GIN_PORTABLE_ARITHMETIC)
 uint32_t n;if(value>>31)return 31u;
 __asm__("%0.L = SIGNBITS %1;":"=d"(n):"d"(value):"ASTAT");
 return 30u-(n&65535u);
#else
 return 31u-(uint32_t)__builtin_clz(value);
#endif
}
static uint32_t packed(gin_pair_t p,int32_t lsb){uint32_t k=0,q,temp;int32_t exponent,shift;
 if(!p.hi&&!p.lo)return 0;
 temp=p.hi?p.hi:p.lo;k=(p.hi?32u:0u)+top_bit(temp);
 exponent=(int32_t)k+lsb;
 if(exponent < -126){shift=-(lsb+149);return shift>0?rounded(p,(uint32_t)shift):p.lo<<(uint32_t)(-shift);}
 shift=(int32_t)k-23;q=shift>0?rounded(p,(uint32_t)shift):p.lo<<(uint32_t)(-shift);
 if(q>=0x1000000u){q>>=1;++exponent;}
 if(exponent>=128)return 0x7f800000u;
 return ((uint32_t)(exponent+127)<<23)|(q&0x7fffffu);
}
static uint32_t mantissa(uint32_t word,int32_t *exponent){uint32_t m=word&0x7fffffu,e=(word>>23)&255u;*exponent=(int32_t)e-127;if(e)return m|0x800000u;*exponent=-126;while(m&&m<0x800000u){m<<=1;--*exponent;}return m;}
GIN_ARITHMETIC uint32_t gin_u32_word(uint32_t value){gin_pair_t p;p.hi=0;p.lo=value;return packed(p,0);}
GIN_ARITHMETIC uint32_t gin_word_mul(uint32_t a,uint32_t b){int32_t ea,eb;uint32_t ma=mantissa(a,&ea),mb=mantissa(b,&eb);if(!ma||!mb)return 0;return packed(gin_product32(ma,mb),ea+eb-46);}
GIN_ARITHMETIC uint32_t gin_word_sub(uint32_t a,uint32_t b){int32_t ea,eb;uint32_t ma,mb,d,old;gin_pair_t p;if(a<=b)return 0;if(!b)return a;ma=mantissa(a,&ea);mb=mantissa(b,&eb);d=(uint32_t)(ea-eb);if(d>=32u)return a;p.hi=d?ma>>(32u-d):0;p.lo=ma<<d;old=p.lo;p.lo-=mb;p.hi-=old<mb;return packed(p,eb-23);}
GIN_ARITHMETIC uint32_t gin_word_fraction(uint32_t word){int32_t e=(int32_t)((word>>23)&255u)-127;uint32_t m=(word&0x7fffffu)|0x800000u;if(e>=23)return 0;if(e>=-9)return m<<(uint32_t)(e+9);return -e-9>=32?0:m>>(uint32_t)(-e-9);}
