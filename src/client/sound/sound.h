/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

// sound.h -- private sound functions

#include "../client.h"

#if USE_SNDDMA
#include "client/sound/dma.h"
#endif

// !!! if this is changed, the asm code must change !!!
typedef struct samplepair_s {
    int         left;
    int         right;
} samplepair_t;

typedef struct sfxcache_s {
    int         length;
    int         loopstart;
    int         width;
#if USE_OPENAL
    int         size;
    int         bufnum;
#endif
    byte        data[1];        // variable sized
} sfxcache_t;

typedef struct sfx_s {
    char        name[MAX_QPATH];
    int         registration_sequence;
    sfxcache_t  *cache;
    char        *truename;
    qerror_t    error;
} sfx_t;

// a playsound_t will be generated by each call to S_StartSound,
// when the mixer reaches playsound->begin, the playsound will
// be assigned to a channel
typedef struct playsound_s {
    struct playsound_s  *prev, *next;
    sfx_t       *sfx;
    float       volume;
    float       attenuation;
    int         entnum;
    int         entchannel;
    qboolean    fixed_origin;   // use origin field instead of entnum's origin
    vec3_t      origin;
    unsigned    begin;          // begin on this sample
} playsound_t;

// !!! if this is changed, the asm code must change !!!
typedef struct channel_s {
    sfx_t       *sfx;           // sfx number
    int         leftvol;        // 0-255 volume
    int         rightvol;       // 0-255 volume
    int         end;            // end time in global paintsamples
    int         pos;            // sample position in sfx
    int         looping;        // where to loop, -1 = no looping OBSOLETE?
    int         entnum;         // to allow overriding a specific sound
    int         entchannel;     //
    vec3_t      origin;         // only use if fixed_origin is set
    vec_t       dist_mult;      // distance multiplier (attenuation/clipK)
    float       master_vol;     // 0.0-1.0 master volume
    qboolean    fixed_origin;   // use origin instead of fetching entnum's origin
    qboolean    autosound;      // from an entity->sound, cleared each frame
#if USE_OPENAL
    int         autoframe;
    int         srcnum;
#endif
} channel_t;

typedef struct {
    char    *name;
    int     rate;
    int     width;
    int     loopstart;
    int     samples;
    byte    *data;
} wavinfo_t;

/*
====================================================================

  SYSTEM SPECIFIC FUNCTIONS

====================================================================
*/

#if USE_SNDDMA
void DMA_SoundInfo(void);
qboolean DMA_Init(void);
void DMA_Shutdown(void);
void DMA_Activate(void);
int DMA_DriftBeginofs(float timeofs);
void DMA_ClearBuffer(void);
void DMA_Update(void);
#endif

#if USE_OPENAL
void AL_SoundInfo(void);
qboolean AL_Init(void);
void AL_Shutdown(void);
sfxcache_t *AL_UploadSfx(sfx_t *s);
void AL_DeleteSfx(sfx_t *s);
void AL_StopChannel(channel_t *ch);
void AL_PlayChannel(channel_t *ch);
void AL_StopAllChannels(void);
void AL_Update(void);
void AL_RawSamples(int samples, int rate, int width, int channels, byte *data, float volume);
void AL_RawSamplesVoice(int samples, int rate, int width, int channels, byte *data, float volume);
void AL_UnqueueRawSamples();

/* number of buffers in flight (needed for ogg) */
extern int active_buffers;
#endif

//====================================================================

// only begin attenuating sound volumes when outside the FULLVOLUME range
#define     SOUND_FULLVOLUME    80

#define     SOUND_LOOPATTENUATE 0.003

extern qboolean s_active;

#define MAX_CHANNELS            64
extern  channel_t   channels[MAX_CHANNELS];
extern  int         s_numchannels;

extern  int         paintedtime;
extern  playsound_t s_pendingplays;

extern  vec3_t      listener_origin;
extern  vec3_t      listener_forward;
extern  vec3_t      listener_right;
extern  vec3_t      listener_up;
extern  int         listener_entnum;

#define S_MAX_RAW_SAMPLES 8192
extern samplepair_t s_rawsamples[S_MAX_RAW_SAMPLES];
extern int          s_rawend;

extern  wavinfo_t   s_info;

extern cvar_t   *s_volume;
extern cvar_t* s_doppler;
extern cvar_t* s_reverb_preset;
extern cvar_t* s_reverb_preset_autopick;
extern cvar_t* s_reverb;
extern cvar_t* s_voiceinput;
extern cvar_t* s_voiceinput_volume;
extern cvar_t* s_underwater;
extern cvar_t* s_underwater_gain_hf;

extern cvar_t* s_occlusion;
extern cvar_t* s_occlusion_strength;

#if USE_SNDDMA
extern cvar_t   *s_khz;
extern cvar_t   *s_testsound;
#endif
extern cvar_t   *s_ambient;
extern cvar_t   *s_show;

#define S_Malloc(x)     Z_TagMalloc(x, TAG_SOUND)
#define S_CopyString(x) Z_TagCopyString(x, TAG_SOUND)

sfx_t *S_SfxForHandle(qhandle_t hSfx);
sfxcache_t *S_LoadSound(sfx_t *s);
channel_t *S_PickChannel(int entnum, int entchannel);
void S_IssuePlaysound(playsound_t *ps);
void S_BuildSoundList(int *sounds);
#if USE_SNDDMA
void S_InitScaletable(void);
void S_PaintChannels(int endTime);
#endif

