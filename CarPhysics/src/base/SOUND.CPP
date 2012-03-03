/*
 * Copyright (c) 2012 Valentin Galea
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#if !defined(WIN32)
	#error "must be MSVC WIN32"
#endif

//#pragma comment(lib, "c:/progs/projects/engine/fmod/lib/fmodvc.lib")

#include "/../fmod/inc/fmod.h"
#include "/../fmod/inc/fmod_errors.h"

//#define DLLDYN

#ifdef DLLDYN
	#include "/../fmod/inc/fmoddyn.h"
	FMOD_INSTANCE *dllfmod;
	#define FMOD() dllfmod->
#else
	#define FMOD()
#endif

#include <cstdio>
#include "sound.h"
#include "error.h"
#include "mmgr/mmgr.h"

/*
 * .name	
 *
 * .info	constructor
 */
sound_t::sound_t(int output, int driver, int mixRate, int maxChannels)
{
	//
	// dll
	//
#ifdef DLLDYN
	dllfmod = FMOD_CreateInstance("fmod.dll");
	if ( !dllfmod )
	{
		dllfmod = 0;
		throw error("fmod: err instance");
	}
#endif

    if (FMOD()FSOUND_GetVersion() < FMOD_VERSION)
        throw error("fmod: wrong FMOD.DLL version");

	//FMOD()FSOUND_SetHWND( 0 ); //0 = picks up foreground

	//
	// callbacks
	//

	//
	// output
	//
	int i;
	switch( output )
	{
	case 0:
		i = FMOD()FSOUND_SetOutput(FSOUND_OUTPUT_NOSOUND);
		break;
	case 1:
		i = FMOD()FSOUND_SetOutput(FSOUND_OUTPUT_WINMM);
		break;
	case 2:
		i = FMOD()FSOUND_SetOutput(FSOUND_OUTPUT_DSOUND);
		break;
	default:
		throw error("fmod: no output");
	}
	if ( !i )
		throw error("fmod: no output");

	//
	// drivers
	//
	/*
	for (i=0; i < FMOD()FSOUND_GetNumDrivers(); i++) {
		printf("%d - %s\n", i+1, FMOD()FSOUND_GetDriverName(i));
		fflush(stdout);
	}
	fflush(stdin);
	printf("Choose driver:"); scanf("%i", &driver);
	driver--;
	*/
	if ( driver >= FMOD()FSOUND_GetNumDrivers() )
		driver = 0;
	if ( !FMOD()FSOUND_SetDriver(driver) )
		throw error("fmod: no driver");

	//
	// init
	//
	if ( !FMOD()FSOUND_Init(
		mixRate,
		maxChannels,
		0 //flags
		) )
		throw error("fmod: cant init");

}

/*
 * .name	
 *
 * .info	destructor
 */
sound_t::~sound_t()
{

	FMOD()FSOUND_Close();

#ifdef DLLDYN
	if ( dllfmod )
		FMOD_FreeInstance(dllfmod);
#endif

}

/*
 * .name	
 *
 * .info	helper
 */
sound_t::sfxParam_t::sfxParam_t()
{
	filename = "";
	flags = FSOUND_NORMAL;
	initVolume = -1;
	initPan = 0; //FSOUND_STEREOPAN
	initPriority = -1;
	minDist = 1.0f;
	maxDist = 100.0f;
}

/*
 * .name	
 *
 * .info	loads a sound
 */
sound_t::sfxID_t sound_t::cacheSFX(sfxParam_t &param)
{
	FSOUND_SAMPLE *sample = 0;

	sample = FMOD()FSOUND_Sample_Load(
		FSOUND_FREE,
		param.filename,
		convertFlags(param.flags),
		0
	);

	if ( !sample )
		return INVALID;

	FMOD()FSOUND_Sample_SetDefaults(
		sample,
		-1,
		param.initVolume,
		param.initPan,
		param.initPriority
	);

	FMOD()FSOUND_Sample_SetMinMaxDistance(
		sample,
		param.minDist,
		param.maxDist
	);

	static int count = 0;
	return count++;
}

/*
 * .name	
 *
 * .info	plays a sound
 */
sound_t::channelID_t sound_t::playSFX(/*sound_t::channelID_t chanID,*/sound_t::sfxID_t sfxID, bool surround)
{
	if ( sfxID == INVALID )
		return INVALID;

	channelID_t c = FMOD()FSOUND_PlaySound(
		FSOUND_FREE,
		FMOD()FSOUND_Sample_Get(sfxID)
	);

	FMOD()FSOUND_SetSurround(c, surround);

	return c;
}

/*
 * .name	
 *
 * .info	stop
 */
signed char sound_t::stopSFX(channelID_t id)
{
	if ( id == INVALID )
		return false;

	return (FMOD()FSOUND_StopSound(id));
}

/*
 * .name	
 *
 * .info	pause
 */
signed char sound_t::pauseSFX(channelID_t id)
{
	if ( id == INVALID )
		return false;

	return (FMOD()FSOUND_SetPaused(id, !FMOD()FSOUND_GetPaused(id)));
}

/*
 * .name	
 *
 * .info	volume stuff
 */
void sound_t::setSFXVolume(channelID_t id, int vol)
{
	FMOD()FSOUND_SetVolumeAbsolute(id, vol);
}
void sound_t::setSFXMasterVolume(int vol)
{
	FMOD()FSOUND_SetSFXMasterVolume(vol);
}


/*
 * 3D
 */
void sound_t::set3DListener(const vector_t& pos, const vector_t& vel, const vector_t& fwrd, const vector_t& up)
{
	FMOD()FSOUND_3D_Listener_SetAttributes(
		const_cast<float *>(pos.getData()),
		const_cast<float *>(vel.getData()),
		fwrd[0], fwrd[1], fwrd[2],
		up[0], up[1], up[2]
	);
}

void sound_t::set3DSource(channelID_t chan, const vector_t& pos, const vector_t& vel)
{
	FMOD()FSOUND_3D_SetAttributes(
		chan,
		const_cast<float *>(pos.getData()),
		const_cast<float *>(vel.getData())
	);
}

void sound_t::update()
{
	FMOD()FSOUND_Update();
}

/*
 * .name	
 *
 * .info	converts 2 internal flags
 */
int sound_t::convertFlags(int oldflg)
{
	int newflg = 0;

	if (oldflg & SFX_2D) newflg |= FSOUND_HW2D;
	if (oldflg & SFX_3D) newflg |= FSOUND_HW3D;
	if (oldflg & SFX_LOOP) newflg |= FSOUND_LOOP_NORMAL;
	if (oldflg & SFX_NOLOOP) newflg |= FSOUND_LOOP_OFF;
	if (oldflg & SFX_ANY) newflg |= FSOUND_NORMAL;

	return newflg;
}

/*
 * .name	
 *
 * .info	debug
 */
void sound_t::dumpInfo()
{
    printf("FSOUND Output Method : ");
    switch (FMOD()FSOUND_GetOutput())
    {
	case FSOUND_OUTPUT_NOSOUND: printf("FSOUND_OUTPUT_NOSOUND\n"); break;
	case FSOUND_OUTPUT_WINMM:   printf("FSOUND_OUTPUT_WINMM\n"); break;
	case FSOUND_OUTPUT_DSOUND:  printf("FSOUND_OUTPUT_DSOUND\n"); break;
	case FSOUND_OUTPUT_ASIO:    printf("FSOUND_OUTPUT_ASIO\n"); break;
	case FSOUND_OUTPUT_OSS:     printf("FSOUND_OUTPUT_OSS\n"); break;
	case FSOUND_OUTPUT_ESD:     printf("FSOUND_OUTPUT_ESD\n"); break;
	case FSOUND_OUTPUT_ALSA:    printf("FSOUND_OUTPUT_ALSA\n"); break;       
    };
	printf("FSOUND Output Rate : %i\n", FMOD()FSOUND_GetOutputRate());
	
    printf("FSOUND Mixer : ");
    switch (FMOD()FSOUND_GetMixer())
    {
	case FSOUND_MIXER_BLENDMODE:      printf("FSOUND_MIXER_BLENDMODE\n"); break;
	case FSOUND_MIXER_MMXP5:          printf("FSOUND_MIXER_MMXP5\n"); break;
	case FSOUND_MIXER_MMXP6:          printf("FSOUND_MIXER_MMXP6\n"); break;
	case FSOUND_MIXER_QUALITY_FPU:    printf("FSOUND_MIXER_QUALITY_FPU\n"); break;
	case FSOUND_MIXER_QUALITY_MMXP5:  printf("FSOUND_MIXER_QUALITY_MMXP5\n"); break;
	case FSOUND_MIXER_QUALITY_MMXP6:  printf("FSOUND_MIXER_QUALITY_MMXP6\n"); break;
    };

    printf("FSOUND Driver : %s\n", FMOD()FSOUND_GetDriverName(FMOD()FSOUND_GetDriver()));
	unsigned int caps = 0;
	FMOD()FSOUND_GetDriverCaps(FMOD()FSOUND_GetDriver(), &caps);
	if (!caps)
		printf("- This driver will support software mode only.\n  It does not properly support 3D sound hardware.\n");
	if (caps & FSOUND_CAPS_HARDWARE)
		printf("- Driver supports hardware 3D sound!\n");
	if (caps & FSOUND_CAPS_EAX2)
		printf("- Driver supports EAX 2 reverb!\n");
	if (caps & FSOUND_CAPS_EAX3)
		printf("- Driver supports EAX 3 reverb!\n");

	printf("FSOUND Channels Playing : %i\n", FMOD()FSOUND_GetChannelsPlaying());

	printf("FSOUND Channels Max : %i\n", FMOD()FSOUND_GetMaxChannels());

}
