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

#ifndef __MODEL_H__
#define __MODEL_H__

#include <list>
#include "filesys.h"
#include "error.h"

class model_t
{
public:

	model_t(const char *);

	~model_t();

private:

	model_t(const model_t &);

public:

	struct vertex_t
	{
		float	u, v;
		float	x, y, z;
	};

	typedef vertex_t triangle_t[3];

	struct mesh_t
	{
		std::string name;
		float	bbox[6];
		int		numTris;
		triangle_t *tris;

		~mesh_t();
	};

	typedef std::list<mesh_t *> meshList_t;
	meshList_t meshList;

	mesh_t* getMesh(const char *);

protected:

	void readBIN(const char *, int, mesh_t *);
	void readTXT(const char *);

};


#include <vector>

class modelManager_t
{
public:
	typedef std::vector<model_t *> list_t;
	list_t list;

	inline modelManager_t()
	{
	}

	~modelManager_t();

	void operator() (const char *filename);

	inline model_t* operator[] (int offset)
	{
		return list[offset];
	}
};

#endif //__MODEL_H__