/* Copyright (c) 2026 SquareWave Studio. MIT; see LICENSE. */
/* Exact noise-only DF2T specialization against the unchanged guarded recurrence.
 * Synthetic coefficients and histories intentionally include unstable filters. */
#define GIN_FAST_FIXED 1
#define GIN_APPROX_METRICS 1
#include "gin_dsp.c"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static uint32_t rng=0xc1594a17u;
static uint32_t word(void){rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;return rng;}
static void reset(void){gin_approx_saturations=gin_approx_peak_state=gin_approx_downshifts=0;gin_approx_min_fraction=28;}
int main(void){
 const unsigned precision[]={28,24,20,16,12};
 const int32_t edge[]={-1048576,-1048575,-8192,-1,0,1,8192,1048574,1048575};
 unsigned cases=0,steps=0,eligible=0,zero=0,reuse=0,downshift=0,lowest=0,saturated=0;
 for(unsigned f=0;f<5;f++)for(unsigned n=0;n<20000;n++){
  gin_filter_coefficients_t p;
  for(unsigned j=0;j<5;j++){p.c[j].value=(int32_t)word();p.c[j].fraction=30;}
  if((n&3)==0)p.c[1].value=0;
  if((n&3)==1)p.c[2]=p.c[0];
  if((n&15)==0)p.c[0].fraction=29;
  if((n&15)==1)p.c[2].fraction=29;
  if((n&63)==2)p.c[4].value=ONE;
  if((n&63)==3)p.c[4].value=-ONE;
  if((n&63)==4)p.c[4].value=ONE-1;
  if((n&63)==5)p.c[4].value=-ONE+1;
  if((n&127)==6)p.c[0].value=INT32_MIN;
  if((n&127)==7)p.c[0].value=INT32_MAX;
  gin_pair_t a[2]={{word(),word()},{word(),word()}},b[2];memcpy(b,a,sizeof a);
  uint32_t ba=precision[f],bb=ba;
  uint32_t common=!((p.c[1].fraction^30u)|(p.c[2].fraction^30u)|(p.c[3].fraction^30u)|(p.c[4].fraction^30u));
  uint32_t flags=noise_guard_flags(&p,common);eligible+=(flags&GIN_NOISE_BOUNDED)!=0;zero+=(flags&GIN_NOISE_B1_ZERO)!=0;reuse+=(flags&GIN_NOISE_B2_EQUALS_B0)!=0;
  for(unsigned j=0;j<16;j++){
   int32_t input=(n<sizeof(edge)/sizeof(*edge))?edge[n]:(int32_t)(word()&0x1fffffu)-1048576;
   uint32_t before=ba;reset();int32_t x=filter_q28(input,&p,a,&ba,common);
   uint32_t counters[]={gin_approx_saturations,gin_approx_peak_state,gin_approx_downshifts,gin_approx_min_fraction};
   reset();int32_t y=(flags&GIN_NOISE_BOUNDED)?bounded_noise_q28(input,&p,b,&bb,flags):filter_q28(input,&p,b,&bb,flags);
   uint32_t got[]={gin_approx_saturations,gin_approx_peak_state,gin_approx_downshifts,gin_approx_min_fraction};
   assert(x==y&&ba==bb&&!memcmp(a,b,sizeof a)&&!memcmp(counters,got,sizeof got));
   downshift+=ba<before;lowest+=ba==12;saturated+=got[0]!=0;steps++;
  }
  cases++;
 }
 assert(eligible&&zero&&reuse&&downshift&&lowest&&saturated);
 printf("PASS noise-guard cases=%u samples=%u eligible=%u zero=%u reuse=%u precision_changes=%u Q12_samples=%u saturated=%u; exact output, both pair states, precision and all metrics\n",cases,steps,eligible,zero,reuse,downshift,lowest,saturated);
 return 0;
}
