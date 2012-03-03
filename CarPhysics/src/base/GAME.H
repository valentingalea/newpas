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

#ifndef __GAME_H__
#define __GAME_H__

#include "window.h"
#include "timer.h"
#include "camera.h"

class game_t;

class view_t
{
public:

// Cconstructors

	inline view_t(game_t *owner) 
		: parent(owner), camera()
	{
	}

private:
	inline view_t(const view_t &);

public:

// Destructor

	inline virtual ~view_t()
	{
	}

// Methods (virtual)

	virtual void setup();
	virtual void draw();
	virtual void handleInput(real_t, real_t);

// Data

//protected:

	game_t *parent;
	camera_t *camera;
};

class game_t
{
public:

// Constructor

	inline game_t() :
		video(0),
		viewport(0)
	{
	}

// Destructor

	inline virtual ~game_t()
	{
	}

// Heartbeat

	inline /*virtual*/ void run()
	{
		while ( 1 )
		{
			if (
				!video->startFrame() )
				return;
			
			think();
			
			video->endFrame();
			
			(timer_t::get()).update();
			//printf("\r%f", 1.0f / timer_t::get().frameTime);
		}
	}

protected:

	inline virtual void think()
	{
	}

// Data

	window_t *video;

	friend class view_t;
	view_t *viewport;

};

#endif //__GAME_H__
