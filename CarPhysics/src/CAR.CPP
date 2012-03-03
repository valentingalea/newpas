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

#include "car.h"
#include "mmgr/mmgr.h"

extern real_t var_carMass;
extern real_t var_wheelSteerLock;
extern real_t var_wheelSuspensionERP;
extern real_t var_wheelSuspensionCFM;

namespace CAR
{

	void car_t::create(const vector_t position, const quaternion_t orientation)
	{
	// Chassis
		vector_t B(1, 0.3, 0.6);
		const real_t L = .1;
		body.size = B;
		body.mass = var_carMass;

		ODE::body_t *chassis = owner->createBody(ODE::box, body.mass/10, B[0], B[1], B[2]);
		body.internal = chassis; 

		chassis->setPos( position );
		chassis->setOrient( orientation );

	// weight to get lower center-of-mass
		ODE::body_t *weight = owner->createBody(ODE::box, body.mass, 0.4, 0.1, 0.5);

		weight->setPos( position + vector_t(0, -0.2, 0) );
		weight->setOrient( orientation );
		weight->toggleDrawable();

		ODE::joint_t *jntLink = owner->createJoint(dJointTypeFixed);
		jntLink->attach( *weight, *chassis );
		dJointSetFixed( *jntLink );

	// Wheels
		wheels[0].radius = .1;
		wheels[0].width = .1;
		wheels[0].mass = body.mass / 10;
		wheels[0].kS = var_wheelSuspensionERP;
		wheels[0].kD = var_wheelSuspensionCFM;
		wheels[0].steerAngle = 0;
		wheels[0].steerLock = 0;
		wheels[1] = wheels[2] = wheels[3] = wheels[0];
		const ODE::body_e type = ODE::cylinder;

	// front
		ODE::body_t *wheelLeft = owner->createBody(type, wheels[0].mass, wheels[0].radius, wheels[0].width);
		wheels[0].internal = wheelLeft;
		wheelLeft->setPos(
			chassis->getPoint(vector_t(-0.333, -0.175, 0.300))
		);
		wheelLeft->setOrient(chassis->getOrient());

		ODE::body_t *wheelRight = owner->createBody(type, wheels[0].mass, wheels[0].radius, wheels[0].width);
		wheels[1].internal = wheelRight;
		wheelRight->setPos(
			chassis->getPoint(vector_t(-0.333, -0.175, -0.300))
		);
		wheelRight->setOrient(chassis->getOrient());

	// rear
		wheelLeft = owner->createBody(type, wheels[0].mass, wheels[0].radius, wheels[0].width);
		wheels[2].internal = wheelLeft;
		wheelLeft->setPos(
			chassis->getPoint(vector_t(0.400, -0.175, 0.300))
		);
		wheelLeft->setOrient(chassis->getOrient());

		wheelRight = owner->createBody(type, wheels[0].mass, wheels[0].radius, wheels[0].width);
		wheels[3].internal = wheelRight;
		wheelRight->setPos(
			chassis->getPoint(vector_t(0.400, -0.175, -0.300))
		);
		wheelRight->setOrient(chassis->getOrient());

	// Steering
		drivingWheel[0] = &wheels[0];
		drivingWheel[1] = &wheels[1];
		directionalWheel[0] = &wheels[0];
		directionalWheel[1] = &wheels[1];

		directionalWheel[0]->steerLock = var_wheelSteerLock;
		directionalWheel[1]->steerLock = var_wheelSteerLock;

		setupDirectionalWheel( wheels[0] );
		setupDirectionalWheel( wheels[1] );
		setupDirectionalWheel( wheels[2] );
		setupDirectionalWheel( wheels[3] );

		steerAngle = 0;
		throtleLevel = 0;
		brakeLevel = 0;

		chassis->toggleDrawable();
		wheels[0].internal->toggleDrawable();
		wheels[1].internal->toggleDrawable();
		wheels[2].internal->toggleDrawable();
		wheels[3].internal->toggleDrawable();
	}

	void car_t::setupDirectionalWheel(wheel_t &w)
	{
		ODE::body_t &wheel = *w.internal;

		ODE::joint_t &susp = *owner->createJoint(dJointTypeHinge2);
		susp.attach(*body.internal, wheel);
		w.turningJoint = &susp;

		// link point
		vector_t v = wheel.getPos();
		dJointSetHinge2Anchor(susp, v[0], v[1], v[2]);

		// steering axis
		v = body.internal->getAxis("0 0 0", "0 1 0");
		dJointSetHinge2Axis1(susp, v[0], v[1], v[2]);

		// motor axis
		v = body.internal->getAxis("0 0 0", "0 0 1");
		dJointSetHinge2Axis2(susp, v[0], v[1], v[2]);

		// params for steer axis
		dJointSetHinge2Param(susp, dParamLoStop, 0);
		dJointSetHinge2Param(susp, dParamHiStop, 0);

		dJointSetHinge2Param(susp, dParamSuspensionERP, w.kS);
		dJointSetHinge2Param(susp, dParamSuspensionCFM, w.kD);
	}

	void car_t::setupNonDirectionalWheel(wheel_t &w)
	{
		setupDirectionalWheel(w);
	}
};
