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

#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "vector.h"

class camera_t
{
public:
	//
	// Camera specific
	//
	enum {
		X, Y
	};
	real_t fov[2]; //in radians
	real_t filmSize[2]; //in pixels
	real_t aspectRatio;

	enum {
		heading, yaw = heading, pitch, roll
	};
	real_t angles[3];

	vector_t position;
	vector_t forward, up, right;

	//
	// Frustum specific
	//
	enum {
		topPlane, bottomPlane, leftPlane, rightPlane, nearPlane, farPlane, planes
	};
	struct planeEquation_t
	{
		vector_t normal;
		real_t dist;
	} frustum[planes];

	//
	// Methods
	//
	camera_t();
	void focus(real_t, real_t);
	bool frustumTest(vector_t&);
};

#endif //__CAMERA_H__
