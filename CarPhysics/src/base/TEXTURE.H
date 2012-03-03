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

#ifndef __TEXTURE_H__
#define __TEXTURE_H__

class texture_t
{
public:
	texture_t(unsigned int, unsigned int);
	~texture_t();

	void setParams();
	void upload();
	void release();
	void transform2AlphaMask();
	void addAlphaChannel();

	unsigned int width;
	unsigned int height;
	unsigned char *data;
	unsigned int id;
	int pixSize;
	int pixFormatIntern;
	int pixFormatSrc;
	int mode;
	int wrapS;
	int wrapT;
	int filter;

};

#include <vector>

class textureManager_t
{
public:
	typedef std::vector<texture_t *> list_t;
	list_t list;

	inline textureManager_t()
	{
	}

	~textureManager_t();

	texture_t* operator() (const char *);
	texture_t* operator[] (int);
};

typedef textureManager_t texMan_t;


#endif //__TEXTURE_H__
