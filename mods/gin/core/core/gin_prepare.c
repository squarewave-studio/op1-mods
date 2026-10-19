/* Copyright (c) 2026 SquareWave Studio. MIT; see LICENSE.
 * libpo32-derived portions: Copyright (c) 2026 Eric Lewis. */
/* libpo32-derived setup, Copyright (c) 2026 Eric Lewis, MIT; see LICENSE.
 * FLOAT off-callback prototype; never target-linked into the callback closure. */
#include "gin_prepare.h"
#include "po32_lut.h"
#include "po32_param_maps.h"
#include <limits.h>
#include <string.h>
static gin_coefficient_t coefficient(float x) {
    gin_coefficient_t c;double scaled=(double)x*1073741824.0; c.fraction=30;
    while(scaled>INT32_MAX||scaled<INT32_MIN){scaled*=0.5;--c.fraction;}
    c.value=scaled>=INT32_MAX-0.5?INT32_MAX:(scaled<=INT32_MIN+0.5?INT32_MIN:(int32_t)(scaled>=0?scaled+0.5:scaled-0.5));return c;
}
static int32_t q30(float x) {if(x>=2.0f)return INT32_MAX;if(x<=-2.0f)return INT32_MIN;return coefficient(x).value;}
static int32_t signal(float x) {return (int32_t)((double)x*(1u<<GIN_SIGNAL_BITS));}
static uint32_t first(float t,float inv) {
    uint32_t n=(uint32_t)(t*44100.0f);
    while((float)n*inv<t)++n;
    while(n&&(float)(n-1u)*inv>=t)--n;
    return n;
}
static uint32_t phase(float turns) {
    double x=(double)turns; x-=(double)(int32_t)x;if(x<0)x+=1;
    return (uint32_t)(x*4294967296.0);
}
static float root(float x) {
    union {float f;uint32_t u;} v;float y,half=x*0.5f;
    v.f=x;v.u=0x5F375A86u-(v.u>>1);y=v.f;
    y=y*(1.5f-half*y*y);y=y*(1.5f-half*y*y);return x*y;
}
static uint32_t word(float x) {union{float f;uint32_t u;}v;v.f=x;return v.u;}
int gin_integer_prepare(gin_prepared_t *out,const po32_patch_params_t *params,int velocity,float duration) {
    tonic_voice_t ref;gin_prepared_t p;float drive,dt;unsigned i;
    if(!out||!params)return -1;
    tonic_voice_reset(&ref);
    if(tonic_voice_trigger(&ref,params,velocity,44100u,duration)!=PO32_OK)return -1;
    memset(&p,0,sizeof(p));
    p.total=(uint32_t)ref.dsp.total_samples;p.wave=(uint32_t)ref.dsp.wave_sel;
    p.mod_mode=ref.dsp.mod_is_exp?0u:(ref.dsp.mod_is_sine?1u:2u);
    p.noise_mode=ref.dsp.nenv_is_ring?2u:(ref.dsp.nenv_is_linear?1u:0u);
    p.use_eq=(uint32_t)ref.dsp.use_eq;p.use_dist=params->DistAmt>0.01f;
    p.osc_attack=first(ref.dsp.osc_atk,ref.dsp.inv_sr);
    p.noise_attack=first(ref.dsp.n_atk,ref.dsp.inv_sr);
    p.linear_attack=first(ref.dsp.lin_atk,ref.dsp.inv_sr);
    p.linear_end=first(ref.dsp.lin_atk+ref.dsp.lin_dcy,ref.dsp.inv_sr);
    p.phase_step=(uint32_t)((double)ref.dsp.osc_freq_step*4294967296.0);
    p.pitch_scale=(int32_t)(ref.dsp.mod_pitch_scale*16777216.0f);
    p.osc_attack_c=coefficient(p.osc_attack<=1u?1.0f:ref.dsp.osc_atk_coeff);
    p.osc_decay_c=coefficient(ref.dsp.osc_dcy_coeff);
    p.noise_attack_c=coefficient(p.noise_attack<=1u?1.0f:ref.dsp.nenv_atk_coeff);
    p.noise_decay_c=coefficient(ref.dsp.nenv_dcy_coeff);
    p.osc_decay_seed=q30(po32_lut_powf(0.001f,((float)p.osc_attack*ref.dsp.inv_sr-ref.dsp.osc_atk)/ref.dsp.osc_dcy));
    p.noise_decay_seed=q30(po32_lut_powf(0.001f,((float)p.noise_attack*ref.dsp.inv_sr-ref.dsp.n_atk)/ref.dsp.n_dcy));
    p.linear_attack_step=q30(ref.dsp.inv_sr*ref.dsp.inv_lin_atk);
    p.linear_decay_step=q30(ref.dsp.inv_sr*ref.dsp.inv_lin_dcy);
    p.linear_decay_seed=q30(1.0f-((float)p.linear_attack*ref.dsp.inv_sr-ref.dsp.lin_atk)*ref.dsp.inv_lin_dcy);
    p.mod_decay_c=coefficient(ref.dsp.mod_dcy_coeff);
    p.rot_s=coefficient(ref.dsp.mod_rot_s);p.rot_c=coefficient(ref.dsp.mod_rot_c);
    p.alpha=coefficient(ref.dsp.mod_alpha_precomp);p.one_minus_alpha=coefficient(ref.dsp.mod_one_minus_alpha);
    p.ring_gain=coefficient(ref.dsp.mod_env_amp_correction);
    if(ref.dsp.mod_env_period>0) {
        volatile float rounded_time=(float)p.noise_attack*ref.dsp.inv_sr;
        p.ring_enabled=1;
        dt=rounded_time-ref.dsp.n_atk;
        p.ring_phase=phase(dt*ref.dsp.inv_mod_env_period);
        p.ring_step=phase(ref.dsp.inv_sr*ref.dsp.inv_mod_env_period);
        p.time_inverse_word=word(ref.dsp.inv_sr);
        p.noise_attack_word=word(ref.dsp.n_atk);
        p.ring_inverse_word=word(ref.dsp.inv_mod_env_period);
    }
    p.osc_gain=coefficient(ref.dsp.osc_gain_mix*ref.dsp.osc_vel_g);
    p.noise_gain=coefficient(ref.dsp.noise_gain_mix*ref.dsp.noise_vel_g);
    p.level=coefficient(ref.dsp.level);
    for(i=0;i<5u;++i) {p.noise_filter.c[i]=coefficient(ref.dsp.noise_filt[i]);p.eq_filter.c[i]=coefficient(ref.dsp.eq_filt[i]);}
    drive=200.0f*params->DistAmt*params->DistAmt*params->DistAmt;
    p.dist_drive=coefficient(drive);p.dist_strong=drive>=1.0f;
    if(p.use_dist&&!p.dist_strong) {
        p.dist_gain=coefficient(1.0f-drive*(1.0f/30.0f));
        p.dist_clip=signal((1.0f+root((drive+3.0f)/drive))*(1.0f/3.0f));
        p.dist_bias=signal((-2.0f/27.0f)*drive-9.0f/27.0f);
    } else if(p.dist_strong) {p.dist_gain=coefficient(0.2f/(drive-0.4827586f)+0.58f);p.dist_bias=signal(-11.0f/27.0f);}
    p.magic=GIN_INTEGER_MAGIC;*out=p;return 0;
}

int gin_integer_prepare_morph(gin_prepared_t *out,po32_patch_params_t *patch,const float endpoint[2][21],uint32_t morph){
 float *p=(float *)patch,m,tone,noise,duration;unsigned i;
 if(!patch||!endpoint||morph>1000)return -1;m=morph*.001f;
 for(i=0;i<21;i++)p[i]=endpoint[0][i]*(1-m)+endpoint[1][i]*m;
 tone=.01f*po32_lut_powf(1000,patch->OscDcy)+po32_param_attack_seconds(patch->OscAtk);
 noise=.01f*po32_lut_powf(1000,patch->NEnvDcy)+po32_param_attack_seconds(patch->NEnvAtk);
 duration=(tone>noise?tone:noise)+.05f;if(duration>10)duration=10;
 return gin_integer_prepare(out,patch,127,duration);
}
