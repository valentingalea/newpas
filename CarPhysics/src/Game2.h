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

#ifndef __GAME2_H__
#define __GAME2_H__

#include "base/game.h"
#include "base/model.h"
#include "base/texture.h"
#include "base/sound.h"
#include "car.h"
#include "newpas/vm.h"


class game2_t : public game_t
{
public:

// [con|de]structor
	game2_t();	
	~game2_t();
	void create();

// inherited viewport
	friend class view_t;

// ODE
	ODE::world_t *ode;

// camera
	camera_t fixedCamera;
	camera_t followCamera;

// the car
	CAR::car_t *car;
	bool followCam;

	model_t *model;
	texMan_t texMan;

	vm_object_file_t * vmObj;
	void runVMFunc();

// overide
	void think();
};

#endif //__GAME2_H__
