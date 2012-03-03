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

#ifndef __CAR_H__
#define __CAR_H__

#include "base/rigidBody.h"

namespace CAR
{
	
	struct wheel_t
	{
		// type geometry
		real_t radius;
		real_t width;
		real_t mass;
		
		// steer angle, always 0 for non-directional wheels
		real_t steerAngle;
		// locking angle for steering in [deg], allways 0 for non-directional wheels
		real_t steerLock;

		// suspension & damping coef
		real_t kS, kD;
		
		// internal representation
		ODE::body_t *internal;
		ODE::joint_t *turningJoint;

	};
	
	
	struct body_t
	{
		// geometry
		vector_t size;
		real_t mass;

		// internal representation
		ODE::body_t *internal;
	};

	
	class car_t
	{
	public:
		
	// constructors
		
		inline car_t(ODE::world_t *w)
			: owner(w)
		{
		}
		
	// destructor
		
		//~car_t()
			
	public:
		
	// methods

		void create(const vector_t position, const quaternion_t orientation);
	private:
		void setupDirectionalWheel(wheel_t &);
		void setupNonDirectionalWheel(wheel_t &);
		
	// Data

		ODE::world_t *owner;

	// parts

	public:

		wheel_t wheels[4];
		body_t body;

		wheel_t *drivingWheel[2];
		wheel_t *directionalWheel[2];
		
	// control

		real_t throtleLevel;
		real_t brakeLevel;
		real_t steerAngle;
		
	};
	
};

#endif //__CAR_H__