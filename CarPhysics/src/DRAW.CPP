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

#define USEGLAUX
#ifdef USEGLAUX
#include <glaux.h>
#pragma comment(lib, "glaux.lib") 
#endif

#ifdef dDOUBLE
#define _glfunc( func ) func##d
#define _glfuncv( func ) func##d##v
#else
#define _glfunc( func ) func##f
#define _glfuncv( func ) func##f##v
#endif

static void drawModel(ODE::body_t *body, const model_t::mesh_t *mesh)
{
	if ( !mesh )
		return;

	const real_t *pos = dGeomGetPosition( *body );
	const real_t *R = dGeomGetRotation( *body );

	glPushMatrix();

	real_t matrix[16] = {
		R[0], R[4], R[8], 0,
		R[1], R[5], R[9], 0,
		R[2], R[6], R[10], 0,
		pos[0], pos[1], pos[2], 1
	};
	_glfunc(glMultMatrix)(matrix);
	_glfunc(glRotate)(90, 0, 1, 0);

	_glfunc(glScale)(.07, .07, .07);

	glInterleavedArrays(GL_T2F_V3F, 0, reinterpret_cast<void*>(mesh->tris));
	glDrawArrays(GL_TRIANGLES, 0, mesh->numTris * 3);

	glPopMatrix();
}


void view_t::setup()
{
	glPixelTransferi(GL_MAP_COLOR, GL_FALSE);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, 
		//GL_FASTEST );
		GL_NICEST);

// generic OFF
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_DITHER);
	glDisable(GL_FOG);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_SCISSOR_TEST);
	glDisable(GL_LOGIC_OP);
	glDisable(GL_TEXTURE_1D);
// variable
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glShadeModel(
		//GL_FLAT);
		GL_SMOOTH );

// textures
#if 0
	// ON
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_TEXTURE_2D);
#else
	// OFF
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_TEXTURE_2D);
#endif

#if 0
	// 2D
	glDepthMask(0);
	glDisable(GL_DEPTH_TEST);

	glDisable(GL_CULL_FACE);
	glDepthFunc(0);

	glClearDepth(0);
#else
	// 3D
	glDepthMask(1);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//glClearDepth(1);
	//glDepthRange(0, 1);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
#endif
}


void view_t::draw()
{
	//
	//	Camera & Viewport setting
	//
	glViewport(0, 0, parent->video->width, parent->video->height);

	camera->focus(45, 45); // camera looks from behind the screen
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	{
		real_t xmin, xmax, ymin, ymax, znear, zfar;
		
		ymax = camera->filmSize[1] / 2;
		ymin = -ymax;
		
		xmax = camera->filmSize[0] / 2;
		xmin = -xmax;
		
		znear = camera->frustum[camera_t::nearPlane].dist;
		zfar = camera->frustum[camera_t::farPlane].dist;
		
		glFrustum(xmin, xmax, ymin, ymax, znear, zfar);
	}

	game2_t *dad = static_cast<game2_t*>(parent);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	{
		vector_t eye = camera->position;
		vector_t center = eye + camera->forward;
		vector_t up = camera->up;
		gluLookAt(
			eye[0], eye[1], eye[2],
			center[0], center[1], center[2],
			up[0], up[1], up[2]
		);
	}

	//
	//	Actual drawing
	//
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glDisable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	const real_t D = 15;
	_glfunc(glColor3)(.5, .5, .5);
	glBegin(GL_QUADS);
		_glfunc(glVertex3)( D, 0, D);
		_glfunc(glVertex3)( D, 0,-D);
		_glfunc(glVertex3)(-D, 0,-D);
		_glfunc(glVertex3)(-D, 0, D);
	glEnd();

	glPointSize(4);
	_glfunc(glColor3)(.75, .75, .75);
	glBegin(GL_POINTS);
		for ( int i = -10; i < 10; i++ )
			for ( int j = -10; j < 10; j++ )
				_glfunc(glVertex3)(real_t(i), 0, real_t(j));
	glEnd();

	glPointSize(5);
	glBegin(GL_POINTS);
		_glfunc(glColor3)(1, 1, 1);
		_glfunc(glVertex3)(0, 0, 0);

		_glfunc(glColor3)(1, 0, 0);
		_glfunc(glVertex3)(1, 0, 0);

		_glfunc(glColor3)(0, 1, 0);
		_glfunc(glVertex3)(0, 1, 0);

		_glfunc(glColor3)(0, 0, 1);
		_glfunc(glVertex3)(0, 0, 1);

	//	_glfunc(glColor3)(1, 1, 1);
	//	_glfunc(glVertex3)( dad->pos[0], dad->pos[1], dad->pos[2] );
	glEnd();

	//
	// rigid bodies
	//
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//glBegin(GL_LINES);
	//	_glfunc(glColor3)(1, 0, 0);
	//	_glfuncv(glVertex3)( dad->pos.getData() );
	//	_glfuncv(glVertex3)( (dad->pos + dad->fwd).getData() );

	//	_glfunc(glColor3)(0, 1, 0);
	//	_glfuncv(glVertex3)( dad->pos.getData() );
	//	_glfuncv(glVertex3)( (dad->pos + dad->up).getData() );

	//	_glfunc(glColor3)(0, 0, 1);
	//	_glfuncv(glVertex3)( dad->pos.getData() );
	//	_glfuncv(glVertex3)( (dad->pos + dad->right).getData() );
	//glEnd();

	dad->ode->draw();

	//
	// Model
	//
	glEnable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindTexture(GL_TEXTURE_2D, (dad->texMan[0])->id);

	model_t *model = dad->model;
	drawModel(dad->car->body.internal, model->getMesh("Body"));
	drawModel(dad->car->wheels[0].internal, model->getMesh("Wheel"));
	drawModel(dad->car->wheels[1].internal, model->getMesh("Wheel"));
	drawModel(dad->car->wheels[2].internal, model->getMesh("Wheel"));
	drawModel(dad->car->wheels[3].internal, model->getMesh("Wheel"));

	//
	// UI
	//
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(
		0, parent->video->width,
		0, parent->video->height
	);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, (dad->texMan[1])->id);

	const real_t w = 256;
	const real_t h = 64;
	const real_t d = 20;

	_glfunc(glTranslate)( parent->video->width - w - d, d, 0 );
	glBegin(GL_QUADS);
		_glfunc(glTexCoord2)( 0, 0 );
		_glfunc(glVertex2)( 0, h );

		_glfunc(glTexCoord2)( 1, 0 );
		_glfunc(glVertex2)( w, h );

		_glfunc(glTexCoord2)( 1, 1 );
		_glfunc(glVertex2)( w, 0 );

		_glfunc(glTexCoord2)( 0, 1 );
		_glfunc(glVertex2)( 0, 0 );
	glEnd();

	//
	// Restore
	//
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

#ifndef USEGLAUX
//
// ripped & modif. from ode's own 'drawstuff.c'
//
static void drawBox (const real_t sides[3])
{
	real_t lx = sides[0]*0.5f;
	real_t ly = sides[1]*0.5f;
	real_t lz = sides[2]*0.5f;

	// sides
	glBegin (GL_TRIANGLE_STRIP);
	_glfunc(glNormal3) (-1,0,0);
	_glfunc(glVertex3) (-lx,-ly,-lz);
	_glfunc(glVertex3) (-lx,-ly,lz);
	_glfunc(glVertex3) (-lx,ly,-lz);
	_glfunc(glVertex3) (-lx,ly,lz);
	_glfunc(glNormal3) (0,1,0);
	_glfunc(glVertex3) (lx,ly,-lz);
	_glfunc(glVertex3) (lx,ly,lz);
	_glfunc(glNormal3) (1,0,0);
	_glfunc(glVertex3) (lx,-ly,-lz);
	_glfunc(glVertex3) (lx,-ly,lz);
	_glfunc(glNormal3) (0,-1,0);
	_glfunc(glVertex3) (-lx,-ly,-lz);
	_glfunc(glVertex3) (-lx,-ly,lz);
	glEnd();

	// top face
	glBegin (GL_TRIANGLE_FAN);
	_glfunc(glNormal3) (0,0,1);
	_glfunc(glVertex3) (-lx,-ly,lz);
	_glfunc(glVertex3) (lx,-ly,lz);
	_glfunc(glVertex3) (lx,ly,lz);
	_glfunc(glVertex3) (-lx,ly,lz);
	glEnd();

	// bottom face
	glBegin (GL_TRIANGLE_FAN);
	_glfunc(glNormal3) (0,0,-1);
	_glfunc(glVertex3) (-lx,-ly,-lz);
	_glfunc(glVertex3) (-lx,ly,-lz);
	_glfunc(glVertex3) (lx,ly,-lz);
	_glfunc(glVertex3) (lx,-ly,-lz);
	glEnd();
}

static void drawPatch (real_t p1[3], real_t p2[3], real_t p3[3], int level)
{
	int i;
	if (level > 0) {
		real_t q1[3],q2[3],q3[3];		 // sub-vertices
		for (i=0; i<3; i++) {
			q1[i] = 0.5f*(p1[i]+p2[i]);
			q2[i] = 0.5f*(p2[i]+p3[i]);
			q3[i] = 0.5f*(p3[i]+p1[i]);
		}
		real_t length1 = (real_t)(1.0/sqrt(q1[0]*q1[0]+q1[1]*q1[1]+q1[2]*q1[2]));
		real_t length2 = (real_t)(1.0/sqrt(q2[0]*q2[0]+q2[1]*q2[1]+q2[2]*q2[2]));
		real_t length3 = (real_t)(1.0/sqrt(q3[0]*q3[0]+q3[1]*q3[1]+q3[2]*q3[2]));
		for (i=0; i<3; i++) {
			q1[i] *= length1;
			q2[i] *= length2;
			q3[i] *= length3;
		}
		drawPatch (p1,q1,q3,level-1);
		drawPatch (q1,p2,q2,level-1);
		drawPatch (q1,q2,q3,level-1);
		drawPatch (q3,q2,p3,level-1);
	}
	else {
		_glfunc(glNormal3) (p1[0],p1[1],p1[2]);
		_glfunc(glVertex3) (p1[0],p1[1],p1[2]);
		_glfunc(glNormal3) (p2[0],p2[1],p2[2]);
		_glfunc(glVertex3) (p2[0],p2[1],p2[2]);
		_glfunc(glNormal3) (p3[0],p3[1],p3[2]);
		_glfunc(glVertex3) (p3[0],p3[1],p3[2]);
	}
}

static int sphere_quality = 2;

// draw a sphere of radius 1
static void drawSphere()
{
	// icosahedron data for an icosahedron of radius 1.0
# define ICX 0.525731112119133606
# define ICZ 0.850650808352039932
	static real_t idata[12][3] = {
		{-ICX, 0, ICZ},
		{ICX, 0, ICZ},
		{-ICX, 0, -ICZ},
		{ICX, 0, -ICZ},
		{0, ICZ, ICX},
		{0, ICZ, -ICX},
		{0, -ICZ, ICX},
		{0, -ICZ, -ICX},
		{ICZ, ICX, 0},
		{-ICZ, ICX, 0},
		{ICZ, -ICX, 0},
		{-ICZ, -ICX, 0}
	};

	static int index[20][3] = {
		{0, 4, 1},	  {0, 9, 4},
		{9, 5, 4},	  {4, 5, 8},
		{4, 8, 1},	  {8, 10, 1},
		{8, 3, 10},   {5, 3, 8},
		{5, 2, 3},	  {2, 7, 3},
		{7, 10, 3},   {7, 6, 10},
		{7, 11, 6},   {11, 0, 6},
		{0, 1, 6},	  {6, 1, 10},
		{9, 0, 11},   {9, 11, 2},
		{9, 2, 5},	  {7, 2, 11},
	};

	static GLuint listnum = 0;
	if (listnum==0) {
		listnum = glGenLists (1);
		glNewList (listnum,GL_COMPILE);
		glBegin (GL_TRIANGLES);
		for (int i=0; i<20; i++) {
			drawPatch (&idata[index[i][2]][0],&idata[index[i][1]][0],
				&idata[index[i][0]][0],sphere_quality);
		}
		glEnd();
		glEndList();
	}
	glCallList (listnum);
}

static void drawCylinder (real_t r, real_t l, real_t zoffset)
{
	int i;
	real_t tmp,ny,nz,a,ca,sa;
	const int n = 24;	// number of sides to the cylinder (divisible by 4)
	
	l *= 0.5;
	a = real_t(M_PI*2.0)/real_t(n);
	sa = (real_t) sin(a);
	ca = (real_t) cos(a);
	
	// draw cylinder body
	ny=1; nz=0;		  // normal vector = (0,ny,nz)
	glBegin (GL_TRIANGLE_STRIP);
	for (i=0; i<=n; i++) {
		glNormal3d (ny,nz,0);
		glVertex3d (ny*r,nz*r,l+zoffset);
		glNormal3d (ny,nz,0);
		glVertex3d (ny*r,nz*r,-l+zoffset);
		// rotate ny,nz
		tmp = ca*ny - sa*nz;
		nz = sa*ny + ca*nz;
		ny = tmp;
	}
	glEnd();
	
	// draw top cap
	ny=1; nz=0;		  // normal vector = (0,ny,nz)
	glBegin (GL_TRIANGLE_FAN);
	glNormal3d (0,0,1);
	glVertex3d (0,0,l+zoffset);
	for (i=0; i<=n; i++) {
		glNormal3d (0,0,1);
		glVertex3d (ny*r,nz*r,l+zoffset);
		
		// rotate ny,nz
		tmp = ca*ny - sa*nz;
		nz = sa*ny + ca*nz;
		ny = tmp;
	}
	glEnd();
	
	// draw bottom cap
	ny=1; nz=0;		  // normal vector = (0,ny,nz)
	glBegin (GL_TRIANGLE_FAN);
	glNormal3d (0,0,-1);
	glVertex3d (0,0,-l+zoffset);
	for (i=0; i<=n; i++) {
		glNormal3d (0,0,-1);
		glVertex3d (ny*r,nz*r,-l+zoffset);
		
		// rotate ny,nz
		tmp = ca*ny + sa*nz;
		nz = -sa*ny + ca*nz;
		ny = tmp;
	}
	glEnd();
}
#endif

void ODE::body_t::draw()
{
	int type = dGeomGetClass(geomID);
	dReal fields[3];
	
	const dReal *pos = dGeomGetPosition(geomID);
	const dReal *R = dGeomGetRotation(geomID);
	
	_glfunc(glColor3)(1, 1, 1);

	glPushMatrix();

	real_t matrix[16] = {
		R[0], R[4], R[8], 0,
		R[1], R[5], R[9], 0,
		R[2], R[6], R[10], 0,
		pos[0], pos[1], pos[2], 1
	};
	glMultMatrixd(matrix);

	switch(type)
	{
	case dBoxClass:
		dGeomBoxGetLengths(geomID, fields);
#ifdef USEGLAUX
		_glfunc(glScale)(fields[0], fields[1], fields[2]);
		auxSolidCube(1);
#else
		drawBox( fields );
#endif
		break;
	case dSphereClass:
#ifdef USEGLAUX
		auxSolidSphere(dGeomSphereGetRadius(geomID));
#else
		fields[0] = dGeomSphereGetRadius(geomID);
		_glfunc(glScale)(fields[0], fields[0], fields[0]);
		drawSphere();
#endif
		break;
	case dCCylinderClass:
		dGeomCCylinderGetParams(geomID, &(fields[0])/*radius*/, &(fields[1])/*length*/);
		// length originally doesn't include the caps
		//fields[1] += 2 * fields[0];
#ifdef USEGLAUX
		auxSolidCylinder(fields[0]/*radius*/, fields[1]/*length*/);
#else		
		drawCylinder(fields[0]/*radius*/, fields[1]/*length*/, 0);
#endif
		break;
	case dCylinderClass:
		// doesn't exist !
		break;
	case dGeomTransformClass: //composite
		break;
	}

	glPopMatrix();
	
}