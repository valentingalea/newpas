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

#pragma warning(disable: 4786)

#include "model.h"

model_t::mesh_t::~mesh_t()
{
	delete[] tris;
}

model_t::model_t(const char *path)
{
	try {
		readTXT(path);
	} catch (...) {
		/*re*/throw;
	}
}

model_t::~model_t()
{
	for ( meshList_t::const_iterator i = meshList.begin(); i != meshList.end(); i++ )
	{
		delete (*i);
	}
}


void model_t::readBIN(const char *filename, int offset, mesh_t *meshIn)
{
	binaryFile_t *in = 0;

	try
	{
		in = stdio::instance().openBinary(filename, IOflags::read);
		
		in->seek(offset, SEEK_SET);
		
		long signature = in->getLong();
		if ( signature != ('M'|'E'<<8|'S'<<16|'H'<<24) )
			throw error("Invalid BIN file");
		
		try {
			meshIn->tris = new triangle_t[meshIn->numTris];
		} catch (...) {
			throw error("Memory alllocation error");
		}
		
		if ( !in->read(reinterpret_cast<void *>(meshIn->tris), meshIn->numTris * 3 * 5 * sizeof(float), 1) )
			throw error("File read error");
		
		meshList.push_back( meshIn );

		delete in;
	}
	catch(...)
	{
		//dont leak
		delete in;

		/*re*/throw;
	}
}


model_t::mesh_t* model_t::getMesh(const char *name)
{
	for ( meshList_t::const_iterator i = meshList.begin(); i != meshList.end(); i++ )
	{
		if ( (*i)->name == name )
			return (*i);
	}

	return 0;
}


#include <map>


void model_t::readTXT(const char *filename)
{
	typedef std::map<std::string, int> keymap_t;
	static keymap_t keymap;
	static bool oneTime = false;

	enum keyword_t {
		NONE,
		COMMENT,
		OBJECT,
		NAME,
		MESHFILE,
		FORMAT,
		BBOX,
		NUMTRIS,
		ENDOBJ,
		END
	};
	if ( !oneTime )
	{
		keymap["*COMMENT"] = COMMENT;
		keymap["*OBJECT"] = OBJECT;
		keymap["*NAME"] = NAME;
		keymap["*MESHFILE"] = MESHFILE;
		keymap["*FORMAT"] = FORMAT;
		keymap["*BBOX"] = BBOX;
		keymap["*NUMTRIS"] = NUMTRIS;
		keymap["*ENDOBJ"] = ENDOBJ;
		keymap["*END"] = END;
		oneTime = true;
	}

	textFile_t *in = 0;
	try {
		in = stdio::instance().openText(filename, IOflags::read);
	} catch (...) {
		/*re*/throw;
	}

	mesh_t *mesh = 0;
	std::string binFilename;
	int binOffset = 0;

	while ( 1 )
	{
		std::string str;

		str = in->getString();
		if ( str[0] != '*' )
		{
			delete in;
			throw error("error parsing model descrition file");
		}

		int id = keymap[str];
		switch ( id )
		{
		case COMMENT:
			in->getLine();
			break;

		case OBJECT:
			try {
				mesh = new mesh_t;
			} catch (...) {
				delete in;
				throw error("Memory alloc error");
			}	
			break;

		case NAME:
			mesh->name = in->getQuotedString();
			break;

		case MESHFILE:
			binFilename = in->getString();
			binOffset = in->getInt();
			break;

		case FORMAT:
			in->getString();
			break;

		case BBOX:
			mesh->bbox[0] = in->getFloat();
			mesh->bbox[1] = in->getFloat();
			mesh->bbox[2] = in->getFloat();
			mesh->bbox[3] = in->getFloat();
			mesh->bbox[4] = in->getFloat();
			mesh->bbox[5] = in->getFloat();
			break;

		case NUMTRIS:
			mesh->numTris = in->getInt();
			break;

		case ENDOBJ:
			try
			{
				std::string path(filename);

				int j = path.length();
				for ( std::string::reverse_iterator i = path.rbegin(); i != path.rend(); i++ )
				{
					char c = *i;
					if ( c == '\\' || c == '/' )
						break;
					j--;
				}

				readBIN((path.substr(0, j) + binFilename).c_str(), binOffset, mesh);

			}
			catch (...)
			{
				delete mesh;
				delete in;

				/*re*/throw;
			}

			mesh = 0;
			break;

		case END:
			delete in;
			return;

		default:
			delete in;
			throw error("Error parsing model description file");
		}
	}

	delete in;
}

void modelManager_t::operator() (const char *filename)
{
	model_t *model;
	try {
		model = new model_t(filename);
	} catch(error& err) {
		err.print();
		/*re*/throw;			
	} catch(...) {
		/*re*/throw;
	}
	list.push_back( model );
}

modelManager_t::~modelManager_t()
{
	for ( list_t::const_iterator i = list.begin(); i != list.end(); i++ )
		delete (*i);
}
