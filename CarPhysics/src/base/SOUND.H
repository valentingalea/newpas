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

#ifndef __SOUND_H__
#define __SOUND_H__

#include "vector.h"

class sound_t
{
public:
	enum sfxTypes {
		INVALID = 0xDEADBEAF,
		OK = 0xC00DBEAF
	};

	//
	// raw
	//
	sound_t(
		//TODO: replace this by a format string
		int = 0, //output device #, 0,1,2,...
		int = 0, //driver #
		int = 44100, //mixrate
		int = 32 //max channels
	);

	~sound_t();

	//
	// sound fx management
	//
	enum flags_t {
		MIN_VOLUME	= 0,
		MAX_VOLUME	= 255,
		SFX_2D		= 0x0001,
		SFX_3D		= 0x0002,
		SFX_MONO	= 0x0004,
		SFX_STEREO	= 0x0008,
		SFX_LOOP	= 0x0010,
		SFX_NOLOOP	= 0x0020,
		SFX_STREAM	= 0x0040,
		SFX_ANY		= 0x0080,
	};

	typedef int sfxID_t;

	typedef struct sfxParam_t
	{
		const char *filename;
		int flags;
		int initVolume;
		int initPan;
		int initPriority;
		float minDist;
		float maxDist;
		sfxParam_t();
	};
	sfxID_t cacheSFX(sfxParam_t&);

	// 
	// playing
	//
	typedef int channelID_t;

	channelID_t playSFX(sfxID_t, bool = false);
	signed char stopSFX(channelID_t);
	signed char pauseSFX(channelID_t);

	void setSFXMasterVolume(int);
	void setSFXVolume(channelID_t, int);

	//
	// 3D
	//
	void set3DListener(
		const vector_t&, //pos
		const vector_t&, //vel
		const vector_t&, //forward
		const vector_t&  //up
	);
	void set3DSource(
		channelID_t,
		const vector_t&, //pos
		const vector_t& //vel
	);
	void update();

	//
	// Internal
	//
	void dumpInfo();

private:

	int convertFlags(int);
};


#include <vector>

class sfxManager_t
{
public:
	typedef std::vector<sound_t::channelID_t> list_t;
	list_t list;

	inline sfxManager_t()
	{
	}

	inline ~sfxManager_t()
	{
	}

	inline void operator() (sound_t::channelID_t chan)
	{
		list.push_back( chan );
	}

	inline sound_t::channelID_t operator[] (int offset) const
	{
		return (list[offset]);
	}
	
};


#endif //__SOUND_H__