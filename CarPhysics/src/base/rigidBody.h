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

#ifndef __RIGIDBODY_H__
#define __RIGIDBODY_H__

#include <list>
#include <ode/ode.h>
#include "vector.h"

namespace ODE
{
	// ODE special flavours
	//typedef dReal real_t;
	//typedef vector_t_t<dReal> vector_t;
	//typedef quaternion_t_t<dReal> quaternion_t;

	//
	// The World
	//
	
	enum body_e {
		box, cylinder, cappedCylinder, sphere, composite
	};

	// forward declaration
	class body_t;
	class joint_t;

	class world_t
	{
	public:

	// Constructor

		world_t(
			dReal g_x, dReal g_y, dReal g_z,
			dReal CFM = dReal(1e-5),
			dReal ERP = dReal(0.2));
		
	// Destructor

		~world_t();
		
	// Friendship

		friend class body_t;
		friend class joint_t;
		
	// Methods

		body_t* createBody(body_e type, dReal mass, dReal field1 = 1, dReal field2 = 1, dReal field3 = 1);

		joint_t* createJoint(int type);

		void input();

		void simulationStep(bool collisions = true);

		void draw();

		static void setSpringModel(const real_t kS, const real_t kD, real_t *ERP, real_t *CFM)
		{
			*ERP = stepSize * kS / (stepSize * kS + kD);
			*CFM = 1 / (stepSize * kS + kD);
		}

	// Data

		static dReal stepSize;

		dSurfaceParameters globalSurfParam;

	private:

		dWorldID ID;
		dSpaceID space; //top-level

		dJointGroupID contactGroup;

		typedef std::list<body_t*> bodyList_t;
		bodyList_t bodyList;

		typedef std::list<joint_t*> jointList_t;
		jointList_t jointList;
		
	};
	

	//
	// Rigid Body
	//

	class body_t
	{
	public:
		
	// Constructors

		// bodyID = 0 : reprezents the environment
		inline body_t()
			: bodyID(0), drawable(true)
		{
		}
	
	private:

		// prevent local instances & pass-by-value
		// only 'world_t' keeps track of bodies & can create them
		friend class world_t;

		body_t(world_t *, body_e, dReal, dReal, dReal, dReal);

		inline body_t(const body_t &);
		
	public:

	// Destructor

		~body_t();

	// Methods

		inline void setPos(const vector_t &v)
		{
			dBodySetPosition(bodyID, v[0], v[1], v[2]);
		}

		inline const vector_t getPos(void)
		{
			const dReal *pos = dBodyGetPosition(bodyID);
			return vector_t(pos[0], pos[1], pos[2]);
		}

		inline void setOrient(const quaternion_t &q)
		{
			dQuaternion dQ;
			dQ[0] = q.w;
			dQ[1] = q.x;
			dQ[2] = q.y;
			dQ[3] = q.z;
			dBodySetQuaternion(bodyID, dQ);
		}

		inline const quaternion_t getOrient(void)
		{
			const dReal *dQ = dBodyGetQuaternion(bodyID);
			return quaternion_t(dQ[0], dQ[1], dQ[2], dQ[3]);
		}

		// get a body-coords point in world-coords
		inline const vector_t getPoint(const vector_t &p)
		{
			dVector3 dV;
			dBodyGetRelPointPos(bodyID, p[0], p[1], p[2], dV);
			return vector_t(dV[0], dV[1], dV[2]);
		}

		// get a world-coords axis from 2 body-coords points
		inline const vector_t getAxis(const vector_t &p1, const vector_t &p2)
		{
			return getPoint(p2) - getPoint(p1);
		}

		// draw this sucker
		void draw();

		inline void toggleDrawable()
		{
			drawable = !drawable;
		}

	// Cast Operators

		inline operator dBodyID ()
		{
			return bodyID;
		}
		
		inline operator dGeomID ()
		{
			return geomID;
		}
		
	// Data
		
	private:

		dBodyID bodyID;
		dGeomID geomID;

		bool drawable;
	};
	

	//
	// Joint
	//

	class joint_t
	{
	private:

	// Constuctors - not directly allowed, only thru world_t

		friend class world_t;
		
		joint_t(world_t *owner, int type);

		inline joint_t(const joint_t &);

	// Destructor

		~joint_t();
		
	// Methods

	public:

		void attach(body_t &, body_t &);

	// Cast operators

		inline operator dJointID ()
		{
			return ID;
		}
		
	// Data

	private:

		dJointID ID;

	};
	
};

#endif //__RIGIDBODY_H__