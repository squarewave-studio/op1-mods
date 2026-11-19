/* Copyright (c) 2026 SquareWave Studio. MIT; see LICENSE. */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "gin_dsp.c"
#include "gin_prepare.h"
static int16_t whole[441000];
int main(void){
 static const float rates[]={0,.1f,.5f,.7f,1};double worst=0;unsigned calls=0;uint64_t hash=1469598103934665603ull;
 for(unsigned rate=0;rate<5;rate++){
  po32_patch_params_t p={0};p.OscFreq=.3f;p.OscAtk=1;p.OscDcy=1;p.ModMode=.5f;p.ModRate=rates[rate];p.ModAmt=1;p.EQGain=.5f;p.Level=.5f;
  gin_prepared_t q;gin_integer_voice_t a,b;assert(!gin_integer_prepare(&q,&p,127,10));assert(!gin_integer_trigger(&a,&q));b=a;
  unsigned off=0;while(off<441000){unsigned n=128;if(n>441000-off)n=441000-off;assert(!gin_integer_render_frames(&a,whole+off,n));off+=n;}
  off=0;while(off<441000){int16_t pcm[128];unsigned n=1+(off*17u)%128u;if(n>441000-off)n=441000-off;assert(!gin_integer_render_frames(&b,pcm,n));assert(!memcmp(pcm,whole+off,2*n));off+=n;double s=b.mod_s/1073741824.,c=b.mod_c/1073741824.;double error=fabs(s*s+c*c-1);if(error>worst)worst=error;assert(error<.001);calls++;}
  assert(!memcmp(&a,&b,sizeof(a)));for(unsigned i=0;i<441000;i++){hash^=(uint16_t)whole[i];hash*=1099511628211ull;}
 }
 /* Absolute schedule boundaries and hostile internal integers remain bounded. */
 int32_t probes[]={INT32_MIN,INT32_MAX,-1073741824,0,1073741824};
 for(unsigned a=0;a<5;a++)for(unsigned b=0;b<5;b++){int32_t s=probes[a],c=probes[b];gin_normalize_sine(254,&s,&c);assert(s==probes[a]&&c==probes[b]);gin_normalize_sine(255,&s,&c);}
 printf("{\"passed\":true,\"rate_cases\":5,\"seconds_per_case\":10,\"worst_squared_norm_error\":%.12g,\"chunk_calls\":%u,\"pcm_hash\":\"%016llx\"}\n",worst,calls,(unsigned long long)hash);return 0;
}
