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

#include "filesys.h"
#include "../mmgr/mmgr.h"
#include "error.h"

//
// Text
//

textFileStd_t::textFileStd_t(const char *path, IOflags::mode_t mode)
{
	static char *combo[4] = { "rt", "wt", "at", "rwt" };

	if ( !(file = fopen(path, combo[mode])) )
		throw error("Can't open file");
		//strcat("Can't open", path));
}


std::string textFileStd_t::getQuotedString()
{
	char name[64];
	
	// get "" embeded string
	int i = 0;
	while ( fgetc(file) != '"' ) ;
	do name[i] = fgetc(file); while ( name[i++] != '"' );
	name[i-1] = 0;
	
	return std::string(name);
}


std::string textFileStd_t::getString()
{
	char name[64];
	fscanf(file, "%s", &name);
	return std::string(name);
}


std::string textFileStd_t::getLine()
{
	char name[256];
	fgets(name, 256, file);
	return std::string(name);
}


//
// Binary
//

binaryFileStd_t::binaryFileStd_t(const char *path, IOflags::mode_t mode)
{
	static char *combo[4] = { "rb", "wb", "ab", "rwb" };

	if ( !(file = fopen(path, combo[mode])) )
		throw error("Can't open file");
		//strcat("Can't open", path));
}


//
// Manager
//

stdio stdio::self;

textFile_t* stdio::openText(const char *path, IOflags::mode_t m)
{
	return new textFileStd_t(path, m);
}

binaryFile_t* stdio::openBinary(const char *path, IOflags::mode_t m)
{
	return new binaryFileStd_t(path, m);
}
