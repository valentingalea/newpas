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

#include <cstdio>
#include "texture.h"
#include "window.h"
#include "../mmgr/mmgr.h"

/*
 *
 *	2d Textures
 *
 */
texture_t::texture_t(unsigned int w8, unsigned int h8)
{
	width = w8;
	height = h8;
	id = 0;
	data = 0;
	pixSize	= GL_UNSIGNED_BYTE;
	pixFormatIntern = GL_RGB;
	pixFormatSrc = GL_RGB;
	mode = GL_REPLACE;
	filter = GL_NEAREST;
	wrapS = GL_CLAMP;
	wrapT = GL_CLAMP;
}

texture_t::~texture_t()
{
	release();
}

void texture_t::setParams()
{
	if ( !id )
		return;

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, pixel_size);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mode);
	//glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, env_color);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		( filter == GL_NEAREST ) ? GL_NEAREST : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		( filter == GL_NEAREST ) ? GL_NEAREST : GL_LINEAR);
}

void texture_t::upload()
{
	if ( !id )
		return;

	glTexImage2D(
		GL_TEXTURE_2D, //target
		0, //level
		pixFormatIntern, //internal format
		width, height, //:-)
		0, //border
		pixFormatSrc, pixSize, //data format & pix-size
		data //pixels
	);
	
	gluBuild2DMipmaps(
		GL_TEXTURE_2D, //target
		pixFormatIntern, //components
		width, height, //:-)
		pixFormatSrc, pixSize, //data format & pix-size
		data //pixels		
	);
}

void texture_t::release()
{
	delete []data;
	data = 0; //sanity check
}

/*
 *	[GLI]mage
 */
void gliReadTGAWrapper(texture_t &, const char *);


/*
 *
 *	The Manager
 *
 */
textureManager_t::~textureManager_t()
{
	for ( list_t::const_iterator i = list.begin(); i != list.end(); i++ )
		delete (*i);
}

texture_t* textureManager_t::operator() (const char *filename)
{
	texture_t *tex = 0;

	try
	{
		tex = new texture_t(0, 0);
		
		// throws errors by itself
		gliReadTGAWrapper(*tex, filename);
			
		list.push_back( tex );

		return tex;
	}
	catch (...)
	{
		delete tex;
	}

	//by default: return a blank texture
	static texture_t defaultTex(0, 0);
	return &defaultTex;
}

texture_t* textureManager_t::operator [] (int offset)
{
	return list[offset];
}
