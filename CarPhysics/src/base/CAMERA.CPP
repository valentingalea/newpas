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

#include <cmath>
#include "camera.h"

camera_t::camera_t()
{
	angles[pitch] = angles[yaw] = angles[roll] = 0.0f;
	
	position = "0 0 0";
	
	forward = "0 0 1";
	up		= "0 1 0";
	right	= "1 0 0";
}

void camera_t::focus(real_t newFovX, real_t newFovY)
{
	fov[X] = RAD(newFovX);
	fov[Y] = RAD(newFovY);

	filmSize[X] = static_cast<real_t>( frustum[nearPlane].dist * tan( fov[X]/2 ) * 2 );
	filmSize[Y] = static_cast<real_t>( frustum[nearPlane].dist * tan( fov[Y]/2 ) * 2 );

	frustum[topPlane].normal = forward;
	frustum[topPlane].normal.rotateAround( right, fov[Y]/2 );
	frustum[topPlane].dist = - ( position dot frustum[topPlane].normal );

	frustum[bottomPlane].normal = forward;
	frustum[bottomPlane].normal.rotateAround( right, -fov[Y]/2 );
	frustum[bottomPlane].dist = - ( position dot frustum[bottomPlane].normal );

	frustum[leftPlane].normal = forward;
	frustum[leftPlane].normal.rotateAround( up, fov[X]/2 );
	frustum[leftPlane].dist = - ( position dot frustum[leftPlane].normal );

	frustum[rightPlane].normal = forward;
	frustum[rightPlane].normal.rotateAround( up, -fov[X]/2 );
	frustum[rightPlane].dist = - ( position dot frustum[rightPlane].normal );

	frustum[nearPlane].normal = forward;
	frustum[nearPlane].dist = 1;

	frustum[farPlane].normal = -forward;
	frustum[farPlane].dist = 1000;
}

bool camera_t::frustumTest(vector_t &point)
{
	for ( int i = 0; i < planes; i++ )
	{
		vector_t &coef = frustum[i].normal;
		real_t &d = frustum[i].dist;
		if ( coef[0]*point[0] + coef[1]*point[1] + coef[2]*point[2] + d > 0 )
			return false;
	}

	return true;
}
