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

#include "window.h"
#include <mmsystem.h>
#include "timer.h"

timer_t timer_t::self;

timer_t::timer_t()
:
	baseTime(0),
	currTime(0),
	frameTime(0),
	lastTime(0)
{
	if ( QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER *>(&frequency)) )
	{
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER *>(&performance_timer_start));
		resolution = (double) ( 1.0 / double(frequency));

		updateFunc = &timer_t::performanceUpdate;
	}
	else
	{
		mm_timer_start	= timeGetTime();
		frequency		= 1000;
		resolution		= 1.0 / frequency;

		updateFunc = &timer_t::normalUpdate;
	}
}

double timer_t::normalUpdate()
{
	return ( (double) ( timeGetTime() - mm_timer_start) * resolution);
}

double timer_t::performanceUpdate()
{
	__int64 time;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER *>(&time));
	return ( (double) ( time - performance_timer_start) * resolution);
}

void timer_t::update()
{
	currTime = (this->*updateFunc)() - baseTime;
	frameTime = currTime - lastTime;
	lastTime = currTime;
}

void timer_t::reset()
{
	baseTime = (this->*updateFunc)();
	lastTime = 0;
}
