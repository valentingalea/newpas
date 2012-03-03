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

#include "game2.h"
#include "base/input.h"

keyboard_t keyboard_t::keyboard;
mouse_t mouse_t::mouse;

extern real_t var_inputAcceleration;
extern real_t var_inputBrake;
extern real_t var_inputTurning;
extern void getLookAtFromDirection( const vector_t &dir, vector_t &fwd, vector_t &up, vector_t &right );

void view_t::handleInput(real_t _speed, real_t _tspeed)
{
	keyboard_t &keyb = keyboard_t::getHandle();
	//mouse_t &mouse = mouse_t::getHandle();

	real_t speed = timer_t::get().frameTime * _speed;
	real_t tspeed = timer_t::get().frameTime * _tspeed;

	//switch ( keyb.keycode )
	//{
	//case KEY::UP:
	//	//advance
	//	camera->position += speed * camera->forward;
	//	break;
	//case KEY::DOWN:
	//	//back-up
	//	camera->position -= speed * camera->forward;
	//	break;
	//case KEY::LEFT:
	//	//strafe
	//	camera->position -= speed * camera->right;
	//	break;
	//case KEY::RIGHT:
	//	//strafe
	//	camera->position += speed * camera->right;
	//	break;
	//}

	CAR::car_t *car = ((game2_t*)parent)->car;
	if (!car )
		return;

	double frameTime = (timer_t::get()).frameTime;

	if ( keyb.keydown[KEY::W] || keyb.keydown[KEY::UP] )
	{
		car->throtleLevel += var_inputAcceleration * frameTime;
	}
	else
	{
		if ( car->throtleLevel > 0 )
			car->throtleLevel -= (var_inputAcceleration / 2) * frameTime;
	}

	if ( keyb.keydown[KEY::S] || keyb.keydown[KEY::DOWN] )
	{
		car->brakeLevel += var_inputBrake * frameTime;
		car->throtleLevel = 0;
	}
	else
	{
		if ( car->brakeLevel > 0 )
			car->brakeLevel -= (var_inputBrake / 2) * frameTime;
		if ( car->brakeLevel < 0 )
			car->brakeLevel = 0;
	}
	
	const double rate = RAD<double>(var_inputTurning * frameTime);

	if ( keyb.keydown[KEY::D] || keyb.keydown[KEY::RIGHT] )
	{
		if ( car->steerAngle < RAD<real_t>( car->directionalWheel[0]->steerLock ) )
		{
			car->steerAngle += rate;
		}
	}

	if ( keyb.keydown[KEY::A] || keyb.keydown[KEY::LEFT] )
	{
		if ( car->steerAngle > RAD<real_t>(- car->directionalWheel[0]->steerLock ) )
		{
			car->steerAngle -= rate;
		}
	}

	if ( keyb.keychar == ' ' )
	{
		keyb.keychar = 0;
		((game2_t*)parent)->followCam = !((game2_t*)parent)->followCam;
	}

	if( keyb.keychar == 13 )
	{
		keyb.keychar = 0;
		((game2_t*)parent)->runVMFunc();
	}
}
