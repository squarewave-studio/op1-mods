#ifdef NDEBUG
#error Tests require assertions enabled
#endif
#include "gin_prepare.h"
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

static int32_t oracle(int32_t a,int32_t b,unsigned s) {
    int64_t p=(int64_t)a*b;int negative=p<0;uint64_t m=negative?(uint64_t)-p:(uint64_t)p;
#if GIN_FAST_FIXED
    /* The fast Q30 profile specifies floor rounding. */
    if(s==30)m=(m>>30)+((negative&&(m&(((uint64_t)1<<30)-1)))?1:0);
    else
#endif
    if(s)m=(m+((uint64_t)1<<(s-1)))>>s;
    if(m>(uint64_t)INT32_MAX+(unsigned)negative)return negative?INT32_MIN:INT32_MAX;
    return negative?(m==0x80000000u?INT32_MIN:-(int32_t)m):(int32_t)m;
}
int main(void) {
    int32_t values[]={INT32_MIN,INT32_MAX,-1000000000,-65536,-1,0,1,65536,1000000000};
    unsigned a,b,s,i,k;uint32_t rng=1;gin_prepared_t p;gin_integer_voice_t v,other;
    struct {uint32_t pre;int16_t out[128];uint32_t post;} block;
    for(a=0;a<9;++a)for(b=0;b<9;++b)for(s=0;s<32;++s)assert(gin_mul_shift(values[a],values[b],s)==oracle(values[a],values[b],s));
    for(i=0;i<50000;++i){int32_t x,y;rng=rng*1664525u+1013904223u;x=(int32_t)rng;rng=rng*1664525u+1013904223u;y=(int32_t)rng;s=rng&31;assert(gin_mul_shift(x,y,s)==oracle(x,y,s));}
    assert(gin_add_sat(INT32_MAX,1)==INT32_MAX&&gin_add_sat(INT32_MIN,-1)==INT32_MIN);
    assert(gin_sub_sat(INT32_MIN,1)==INT32_MIN&&gin_sub_sat(INT32_MAX,-1)==INT32_MAX);
    assert(gin_integer_prepare(&p,&tonic_kick,100,0.1f)==0);
    p.total=200;assert(gin_integer_trigger(&v,&p)==0);block.pre=123;block.post=456;
    for(i=0;i<128;++i)block.out[i]=2222;
    assert(gin_integer_render(&v,block.out)==0);assert(gin_integer_status(&v)>0);
    assert(gin_integer_render(&v,block.out)==0);for(i=72;i<128;++i)assert(block.out[i]==0);
    assert(block.pre==123&&block.post==456&&!gin_integer_status(&v));
    assert(gin_integer_trigger(&v,&p)==0);gin_integer_render(&v,block.out);
    assert(gin_integer_trigger(&other,&p)==0);{int16_t out[128];gin_integer_render(&other,out);assert(!memcmp(out,block.out,sizeof(out)));}
    gin_integer_release(&v);for(i=0;i<128;++i)block.out[i]=2222;gin_integer_render(&v,block.out);
    for(i=0;i<128;++i)assert(block.out[i]==0);
    {gin_integer_voice_t voices[6],solos[6];gin_prepared_t patches[6];
     for(i=0;i<6;++i){po32_patch_params_t patch=tonic_kick;patch.OscFreq+=i*0.03f;patch.Mix=i*0.1f;
       assert(!gin_integer_prepare(&patches[i],&patch,40+(int)i*15,0.1f));gin_integer_trigger(&voices[i],&patches[i]);gin_integer_trigger(&solos[i],&patches[i]);}
     for(k=0;k<10;++k)for(i=0;i<6;++i){int16_t expected[128];gin_integer_voice_t saved[6];memcpy(saved,voices,sizeof(saved));
       gin_integer_render(&voices[i],block.out);gin_integer_render(&solos[i],expected);assert(!memcmp(expected,block.out,sizeof(expected)));
       for(a=0;a<6;++a)if(a!=i)assert(!memcmp(&voices[a],&saved[a],sizeof(voices[a])));}}
    {po32_patch_params_t bad=tonic_kick;gin_prepared_t old=p;bad.ModVel=2;assert(gin_integer_prepare(&p,&bad,100,0.1f)==-1);assert(!memcmp(&old,&p,sizeof(p)));}
    printf("PASS arithmetic=%u boundaries+random;128 writes/canaries/tails;retrigger/release;six-voice isolation;invalid preparation atomic;voice=%zu prepared=%zu\n",9u*9u*32u+50000u,sizeof(v),sizeof(p));return 0;
}
