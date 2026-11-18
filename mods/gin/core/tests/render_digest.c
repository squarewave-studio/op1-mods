/* Copyright (c) 2026 SquareWave Studio. MIT; see LICENSE. */
#include "gin_prepare.h"
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
static uint32_t rng=0x73e79f21u;
static uint32_t word(void){rng=rng*1664525u+1013904223u;return rng;}
int main(void){
 static const unsigned lengths[]={1,63,64,65,127,128,31,97};
 for(unsigned c=0;c<384;c++){
  po32_patch_params_t patch;float fields[21];
  for(unsigned i=0;i<21;i++)fields[i]=(word()%1001u)/1000.f;
  memcpy(&patch,fields,sizeof(patch));
  patch.OscWave=(c%3)*.5f;patch.ModMode=((c/3)%3)*.5f;patch.NEnvMod=((c/9)%3)*.5f;
  if(c%16==0)patch.Mix=0;if(c%16==1)patch.Mix=1;
  if(c%17==0)patch.OscAtk=0;if(c%19==0)patch.NEnvAtk=0;
  gin_prepared_t p;gin_integer_voice_t v;assert(!gin_integer_prepare(&p,&patch,(int)(word()%128u),.15f));
  memset(&v,0,sizeof(v));assert(!gin_integer_trigger(&v,&p));
  for(unsigned k=0;k<42;k++){
   struct{uint32_t before;int16_t pcm[128];uint32_t after;} b;
   b.before=0x12345678;b.after=0x87654321;
   memset(b.pcm,0x55,sizeof(b.pcm));unsigned n=lengths[k%8];
   if(k==24)gin_integer_release(&v);
   if(k==28)assert(!gin_integer_trigger(&v,&p));
   if(k==34){v.n=p.total-1;v.active=1;}
   int silent=gin_integer_can_retire(&v);
   assert(!gin_integer_render_frames(&v,b.pcm,n));
   if(silent)for(unsigned i=0;i<n;i++)assert(b.pcm[i]==0);
   assert(b.before==0x12345678&&b.after==0x87654321);
   for(unsigned i=n;i<128;i++)assert(b.pcm[i]==0x5555);
   assert(fwrite(b.pcm,2,n,stdout)==n);assert(fwrite(&v,1,sizeof(v),stdout)==sizeof(v));
  }
 }
 return 0;
}
