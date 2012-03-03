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
#include "mmgr/mmgr.h"
#include <shlwapi.h>

game2_t *theGame;

char vmData[4096];

void Script_CreateBody( void )
{
	ODE::body_e type = (ODE::body_e)VM_POPi( 0 );
	real_t mass = VM_POPf( 1 );
	real_t field1 = VM_POPf( 2 );
	real_t field2 = VM_POPf( 3 );
	real_t field3 = VM_POPf( 4 );
	char *strPos = bframe.data + VM_POPi( 5 );
	real_t orientAngle = 0;//VM_POPf( 6 );
	char *orientAxis = "0 1 0";//bframe.data + VM_POPi( 7 );

	ODE::body_t *b = theGame->ode->createBody( type, mass, field1, field2, field3 );
	b->setPos( strPos );
	b->setOrient( quaternion_t( RAD<real_t>( orientAngle ), orientAxis ) );
}

vector_t var_carPosition;
real_t var_carMass = 10;
real_t var_carAntiSwayForce = 300;
real_t var_wheelSteerLock = 30;
real_t var_wheelSuspensionERP = 0.15;
real_t var_wheelSuspensionCFM = 0.03;
real_t var_inputAcceleration = 10;
real_t var_inputBrake = 1;
real_t var_inputTurning = 45;
real_t var_camSpringStiff = 1700;
real_t var_camSpringDamping = 600;
real_t var_camMass = 1;
vector_t var_cam1Position = "3 2 0";
vector_t var_cam1Target = "0 1 0";
vector_t var_cam2Position = "-3 10 -3";
vector_t var_cam2Target = "0 2 0";

enum {
	CAR_POSITION,
	CAR_MASS,
	CAR_ANTISWAYFORCE,
	WHEEL_STEERLOCK,
	WHEEL_SUSPENSION_ERP,
	WHEEL_SUSPENSION_CFM,
	INPUT_ACCELERATION,
	INPUT_BRAKE,
	INPUT_TURNING,
	CAM_SPRING_STIFF,
	CAM_SPRING_DAMPING,
	CAM_MASS,
	CAM1_POSITION,
	CAM1_TARGET,
	CAM2_POSITION,
	CAM2_TARGET
};

void Script_SetVar( void )
{
	int type = VM_POPi( 0 );

	void *ptrTable[] =
	{
		&var_carPosition,
		&var_carMass,
		&var_carAntiSwayForce,
		&var_wheelSteerLock,
		&var_wheelSuspensionERP,
		&var_wheelSuspensionCFM,
		&var_inputAcceleration,
		&var_inputBrake,
		&var_inputTurning,
		&var_camSpringStiff,
		&var_camSpringDamping,
		&var_camMass,
		&var_cam1Position,
		&var_cam1Target,
		&var_cam2Position,
		&var_cam2Target
	};

	switch( type )
	{
	case CAR_POSITION:
	case CAM1_POSITION:
	case CAM1_TARGET:
	case CAM2_POSITION:
	case CAM2_TARGET:
		*(vector_t *)ptrTable[ type ] = (char *)(bframe.data + VM_POPi( 1 ));
		break;
	default:
		*(real_t *)ptrTable[ type ] = VM_POPf( 1 );
	}
}

void getLookAtFromDirection( const vector_t &dir, vector_t &fwd, vector_t &up, vector_t &right )
{
	// from http://nehe.gamedev.net/data/articles/article.asp?article=08

	fwd = dir;
	fwd.normalize();

	vector_t projectedDir = dir;

	const real_t epsilon = 0.00001;

	if( fabs( dir[ 0 ] ) < epsilon && fabs( dir[ 2 ]) < epsilon ) // YZ plane
	{
		projectedDir[ 0 ] = 0;
		projectedDir.normalize();

		vector_t tempRight( 1, 0, 0 );
		up = projectedDir cross tempRight;

		right = fwd cross up;
	}
	else // XZ plane
	{
		projectedDir[ 1 ] = 0;
		projectedDir.normalize();

		vector_t tempUp( 0, 1, 0 );
		right = -1 * ( projectedDir cross tempUp );

		up = fwd cross right;
	}

	right.normalize();
	up.normalize();
}

game2_t::game2_t()
:
	game_t(),
	texMan()
{
	ode = 0, car = 0;
}

TCHAR szDataPath[MAX_PATH];

void game2_t::create()
{
	try
	{
		GetModuleFileName( 0, szDataPath, MAX_PATH );
		PathRemoveFileSpec( szDataPath );
		PathAppend( szDataPath, "data/" );
		TCHAR szPath[MAX_PATH];
		
		window_t::vidModeDetails_t mode;
		mode.name = "Car";
		mode.width = 800;
		mode.height = 600;
		mode.bits_per_pixel = 32;
		mode.full_screen = false;
		mode.z_buffer_bits = 32;
		mode.txt_console = false;
		video = new window_t( mode );

		viewport = new view_t((game2_t*)this);
		viewport->setup();

		PathCombine( szPath, szDataPath, "jeep.tga" );
		texture_t *tex = texMan( szPath );
		tex->filter = GL_LINEAR;
		tex->setParams();
		tex->upload();
		tex->release();

		PathCombine( szPath, szDataPath, "help.tga" );
		texture_t *tex2 = texMan( szPath );
		tex2->filter = GL_NEAREST;
		tex2->setParams();
		tex2->upload();
		tex2->release();

		PathCombine( szPath, szDataPath, "jeep.txt" );
		model = new model_t( szPath );

		ode = new ODE::world_t(0, -1, 0);
		ode->stepSize = 0.025;
		ode->globalSurfParam.mode = 
			dContactSlip1 | dContactSlip2 |
			dContactSoftERP | dContactSoftCFM | 
			dContactApprox1;
		ode->globalSurfParam.mu = dInfinity;
		ode->globalSurfParam.slip1 = 0.1;
		ode->globalSurfParam.slip2 = 0.1;
		ode->globalSurfParam.soft_erp = 1.5;
		ode->globalSurfParam.soft_cfm = 0.3;

		PathCombine( szPath, szDataPath, "out.vm" );
		vmObj = VM_Init( szPath );
		if( !vmObj )
			throw error("VM error");
		int vmEntryPoint = VM_GetFuncAddress(vmObj, "OnGameStart");
		VM_Execute( vmObj, vmData, vmEntryPoint );
		vmEntryPoint = VM_GetFuncAddress( vmObj, "OnGameFrame" );
		VM_Execute( vmObj, vmData, vmEntryPoint );

		car = new CAR::car_t(ode);
		car->create(
			var_carPosition,
			quaternion_t(RAD<real_t>(135), "0 1 0")
		);

		followCam = true;
	}
	catch ( error &err )
	{
		err.print();

		throw 0;
	}
	catch (...)
	{
		/*re*/throw;
	}
}


game2_t::~game2_t()
{
	try {
		delete model;
		delete car;
		delete ode;
		delete viewport;
		delete video;

		VM_Close( vmObj );
	} catch(...) {
		throw error("err while deleting ptr");
	}
}

void game2_t::runVMFunc()
{
	VM_Close( vmObj );

	TCHAR szPath[MAX_PATH];
	PathCombine( szPath, szDataPath, "out.vm" );
	vmObj = VM_Init( szPath );
	if( !vmObj )
		throw error("VM error");

	int vmEntryPoint = VM_GetFuncAddress( vmObj, "OnGameFrame" );
	VM_Execute( vmObj, vmData, vmEntryPoint );
}

void game2_t::think()
{
	viewport->handleInput(5, 50);

	ODE::joint_t *susp = car->directionalWheel[0]->turningJoint;
	real_t v = (car->steerAngle - dJointGetHinge2Angle1(*susp)) * 5;
	real_t lockAngle = car->directionalWheel[0]->steerLock;
	
	dJointSetHinge2Param(*susp, dParamVel, v);
	dJointSetHinge2Param(*susp, dParamFMax, 1);
	dJointSetHinge2Param(*susp, dParamLoStop, RAD<real_t>( -lockAngle ));
	dJointSetHinge2Param(*susp, dParamHiStop, RAD<real_t>( +lockAngle ));
	dJointSetHinge2Param(*susp, dParamFudgeFactor, .2);

	susp = car->directionalWheel[1]->turningJoint;
	v = (car->steerAngle - dJointGetHinge2Angle1(*susp)) * 5;
	dJointSetHinge2Param(*susp, dParamVel, v);
	dJointSetHinge2Param(*susp, dParamFMax, 1);
	dJointSetHinge2Param(*susp, dParamLoStop, RAD<real_t>( -lockAngle ));
	dJointSetHinge2Param(*susp, dParamHiStop, RAD<real_t>( +lockAngle ));

	const real_t T_max = 1;
	real_t torque = ((car->throtleLevel>0) ? -car->throtleLevel : car->brakeLevel);
	dJointSetHinge2Param(*car->drivingWheel[0]->turningJoint, dParamVel2, torque);
	dJointSetHinge2Param(*car->drivingWheel[0]->turningJoint, dParamFMax2, T_max);
	dJointSetHinge2Param(*car->drivingWheel[1]->turningJoint, dParamVel2, torque);
	dJointSetHinge2Param(*car->drivingWheel[1]->turningJoint, dParamFMax2, T_max);
	dJointSetHinge2Param(*susp, dParamFudgeFactor, .2);

	{ // anti-sway bar forces
		vector_t bodyPoint;
		vector_t hingePoint;
		vector_t axis;
		real_t displacement;
		for( int i = 0; i < 4; ++i )
		{
			ODE::joint_t *hinge = car->wheels[i].turningJoint;
			ODE::body_t *wheel = car->wheels[i].internal;

			dJointGetHinge2Anchor2( *hinge, &bodyPoint[0] );
			dJointGetHinge2Anchor( *hinge, &hingePoint[0] );
			dJointGetHinge2Axis1( *hinge, &axis[0] );
			displacement = (hingePoint - bodyPoint) dot axis;

			if( displacement > 0.0001 )
			{
				real_t amt = displacement * var_carAntiSwayForce;
			//	if( amt > 15 )
			//		amt = 15;

				dBodyAddForce( *wheel, -axis[0] * amt, -axis[1] * amt, -axis[2] * amt );
				dReal const * wp = dBodyGetPosition( *wheel );
				dBodyAddForceAtPos( *car->body.internal, axis[0]*amt, axis[1]*amt, axis[2]*amt, wp[0], wp[1], wp[2] );
				dBodyAddForce( *car->wheels[i^1].internal, axis[0] * amt, axis[1] * amt, axis[2] * amt );
				wp = dBodyGetPosition( *wheel );
				dBodyAddForceAtPos( *car->body.internal, -axis[0]*amt, -axis[1]*amt, -axis[2]*amt, wp[0], wp[1], wp[2] );
			}
		}
	}


// camera
	static bool firstTime = true;
	vector_t camObjPosHi, camObjPosLow;
	camera_t **cam = &viewport->camera;

	if( followCam )
	{
		*cam = &followCamera;

		camObjPosHi = var_cam1Position;
		camObjPosLow = var_cam1Target;
	}
	else
	{
		*cam = &fixedCamera;

		camObjPosHi = var_cam2Position;
		camObjPosLow = var_cam2Target;

	}

	vector_t newPos = car->body.internal->getPoint( camObjPosHi );
	newPos[ 1 ] = camObjPosHi[ 1 ];
	vector_t newPos2 = car->body.internal->getPoint( camObjPosLow );
	newPos2[ 1 ] = camObjPosLow[ 1 ];

	if( firstTime )
		(*cam)->position = newPos;

	static vector_t velocity( 0, 0, 0 );

	vector_t displacement = (*cam)->position - newPos;
	vector_t force = -var_camSpringStiff * displacement - var_camSpringDamping * velocity;

	double dt = timer_t::get().frameTime;

	vector_t acceleration = force / var_camMass;
	acceleration *= dt;
	velocity += acceleration * dt;
	(*cam)->position += velocity * dt;

	vector_t dir = newPos2 - (*cam)->position;

	getLookAtFromDirection( dir, (*cam)->forward, (*cam)->up, (*cam)->right );

	ode->simulationStep(true);

	viewport->draw();

	firstTime = false;
}


int WINAPI WinMain
(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	LPSTR		lpCmdLine,
	int			nCmdShow
)
{

	try
	{
		theGame = new game2_t;
		theGame->create();
		theGame->run();
		delete theGame;
	}
	catch ( error &err )
	{
		err.print();
		return false;
	}
	catch (...)
	{
		return false;
	}

	return true;

}
