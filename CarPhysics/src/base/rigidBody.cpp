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

#include "rigidBody.h"
#include "../mmgr/mmgr.h"
#include "error.h"
#include "timer.h"

namespace ODE
{

//
// World
//

	dReal world_t::stepSize;

	world_t::world_t(dReal g_x, dReal g_y, dReal g_z, dReal CFM, dReal ERP)
	{
		// ODE world !
		dInitODE();
		ID = dWorldCreate();

		// default integration step-size
		stepSize = dReal(0.05);

		// [m/s^2]
		dWorldSetGravity(ID, g_x, g_y, g_z);
		
		// error red. param = [0.1--0.8] default 0.2
		dWorldSetERP(ID, ERP);
		
		// constraint force mixing = [1e-9 -- 1]
		//dWorldSetCFM(ID, CFM);

		// space(s)
		space = dHashSpaceCreate(0);

			// contact joints group
			contactGroup = dJointGroupCreate(0);
		
			// optional ground plane
			dCreatePlane(space, 0, 1, 0, 0);

			// global surface friction params
			globalSurfParam.mode = 
				//dContactMu2 |
				//dContactFDir1 |
				dContactBounce |
				//dContactSoftERP |
				dContactSoftCFM |
				//dContactMotion1 | dContactMotion2 |
				//dContactSlip1 | dContactSlip2 |
				//dContactApprox1_1 | dContactApprox1_2 | dContactApprox1
				0;
			
			// Coulomb friction coefficient [0-dInfinity]
			globalSurfParam.mu = dInfinity;
			// Optional Coulomb friction coefficient for friction direction 2
			globalSurfParam.mu2 = 0;
			
			// Restitution parameter (0..1). 0 means the surfaces are not bouncy at all
			globalSurfParam.bounce = 0.1;
			// The minimum incoming velocity necessary for bounce (in m/s)
			globalSurfParam.bounce_vel = 0.1;
			
			// Contact normal ``softness'' parameter(s)
			//contact[i].surface.soft_erp = 0;
			globalSurfParam.soft_cfm = 0.01;
			
			// Surface velocity in friction directions 1 and 2 (in m/s)
			//globalSurfParam.motion1 = 0;
			//globalSurfParam.motion2 = 0;
			
			// The coefficients of force-dependent-slip (FDS) for friction directions 1 and 2
			//globalSurfParam.slip1 = 0;
			//globalSurfParam.slip2 = 0;
			
	}
	
	
	world_t::~world_t()
	{
		//delete all allocated joints
		for ( bodyList_t::iterator i = bodyList.begin(); i != bodyList.end(); i++ )
			delete (*i);
		
		//delete all allocated joints
		for ( jointList_t::iterator j = jointList.begin(); j != jointList.end(); j++ )
			delete (*j);

		dJointGroupDestroy(contactGroup);
		dSpaceDestroy(space);
		
		dWorldDestroy(ID);
		dCloseODE();
	}
	

	body_t* world_t::createBody(
			body_e type,
			dReal mass,
			dReal field1, dReal field2, dReal field3)
	{
		body_t *ref = 0;

		try
		{
			ref = new body_t(this, type, mass, field1, field2, field3);

			bodyList.push_back( ref );

			return ref;
		}
		catch(...)
		{
			delete ref;

			throw error("ODE: err while creating new body");
		}
	}


	joint_t* world_t::createJoint(int type)
	{
		joint_t *ref = 0;

		try
		{
			ref = new joint_t(this, type);

			jointList.push_back( ref );

			return ref;
		}
		catch(...)
		{
			delete ref;

			throw error("ODE: err while creating new joint");
		}
	}

	static void contactCallback(void *data, dGeomID o1, dGeomID o2)
	{
		// exit without doing anything if the two bodies are connected by a joint
		dBodyID b1 = dGeomGetBody(o1);
		dBodyID b2 = dGeomGetBody(o2);
		if ( b1 && b2 && dAreConnectedExcluding(b1, b2, dJointTypeContact) )
			return;
		
		// get passed info from the caller
		dWorldID world = dWorldID( ((void**)data)[0] );
		dJointGroupID contactGroup = dJointGroupID( ((void**)data)[1] );
		dSurfaceParameters *globalSurfParam = (dSurfaceParameters*)( ((void**)data)[2] );
		
		dContact contact[8];
		
		int num = dCollide(
			o1, o2,
			8, //flags = lower 16 bits : max_contacts
			&contact[0].geom,
			sizeof(dContact) //someking of stride
			);
			
		for (int i = 0; i < num; i++ )
		{
			// all surfaces share the same param (for now!)
			contact[i].surface = *globalSurfParam;
			
			// Add the contact joints
			dJointID c = dJointCreateContact(world, contactGroup, contact+i);
			dJointAttach(c, b1, b2);
		}
	}
	
	
	void world_t::simulationStep(bool collide)
	{
		if ( collide )
		{
			void *stack[4];
			stack[0] = ID;
			stack[1] = contactGroup;
			stack[2] = &globalSurfParam;
			dSpaceCollide (space, stack, &contactCallback);
		}
		
		const real_t maxStep = 0.025;
		real_t step = timer_t::get().frameTime;
		static real_t oldStep = 0;

		if ( (step <= 0) || (abs(oldStep - step) > maxStep) )
			step = maxStep;

		stepSize = step;
		dWorldStep(ID, stepSize);
		
		dJointGroupEmpty (contactGroup);
	}
	
	
	void world_t::draw()
	{
		for ( bodyList_t::iterator i = bodyList.begin(); i != bodyList.end(); i++ )
			if ( (*i)->drawable )
				(*i)->draw();
	}
	
//
// Rigid Body
//

	body_t::body_t(world_t *owner, body_e type, dReal mass, dReal field1, dReal field2, dReal field3)
	{
		dWorldID world = owner->ID;
		dSpaceID space = owner->space;
		
		bodyID = dBodyCreate(world);
		
		dMass m;
		switch ( type )
		{
		case sphere:

			dMassSetSphereTotal(&m, mass, field1);
			geomID = dCreateSphere(space, field1);
			break;

		case cylinder:

			dMassSetCylinderTotal(&m, mass,
				3, //direction (1=x, 2=y, 3=z)
				field1, field2); //radius, length
			geomID = dCreateCCylinder(space, field1, field2);
			break;

		case cappedCylinder:

			dMassSetCappedCylinderTotal(&m, mass,
				3, //direction (1=x, 2=y, 3=z)
				field1, //radius
				field2); //length : w/o spherical caps
			// capped ! length = along Z
			geomID = dCreateCCylinder(space, field1, field2);
			break;

		case box:
		default:

			dMassSetBoxTotal(&m, mass, field1, field2, field3);
			geomID = dCreateBox(space, field1, field2, field3);

		}

		dBodySetMass(bodyID, &m);
		dGeomSetBody(geomID, bodyID);
		
	}
	
	
	body_t::~body_t()
	{
		if ( bodyID )
		{
			dBodyDestroy(bodyID);
			bodyID = 0;
		}

		//geomIDs destroyed by space
	}

//
//	Joint
//

	joint_t::joint_t(world_t *owner, int type)
	{
		switch ( type )
		{
		case dJointTypeBall:
			ID = dJointCreateBall(owner->ID, 0);
			break;
		case dJointTypeHinge:
			ID = dJointCreateHinge(owner->ID, 0);
			break;
		case dJointTypeHinge2:
			ID = dJointCreateHinge2(owner->ID, 0);
			break;
		case dJointTypeSlider:
			ID = dJointCreateSlider(owner->ID, 0);
			break;
		case dJointTypeUniversal:
			ID = dJointCreateUniversal(owner->ID, 0);
			break;
		case dJointTypeAMotor:
			ID = dJointCreateAMotor(owner->ID, 0);
			break;
		case dJointTypeFixed:
			ID = dJointCreateFixed(owner->ID, 0);
			break;
		default:
			throw error("ODE: joint type unknown");
		}
	}


	joint_t::~joint_t()
	{
		dJointDestroy(ID);
	}


	void joint_t::attach(body_t &b1, body_t &b2)
	{
		dJointAttach(ID, b1, b2);
	}

};
