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

#ifndef __FILESYS_H__
#define __FILESYS_H__

#include <cstdio>
#include <string>

namespace IOflags {
	enum mode_t { 
		read, write, append, both 
	};
};

//
// Text Files
//

class textFile_t
{
public:

	inline textFile_t() {}

	inline virtual ~textFile_t() = 0 {}

	virtual int getChar() = 0;

	virtual int getInt() = 0;

	virtual float getFloat() = 0;

	virtual double getDouble() = 0;

	virtual std::string getString() = 0;

	virtual std::string getQuotedString() = 0;

	virtual std::string getLine() = 0;

};

class textFileStd_t : public textFile_t
{
public:

	textFileStd_t(const char *, IOflags::mode_t);

	inline ~textFileStd_t()
	{
		fclose(file);
	}

	inline FILE* getPtr()
	{
		return file;
	}

	inline int getChar()
	{
		return fgetc(file);
	}

	inline int getInt()
	{
		int i;
		fscanf(file, "%i", &i);
		return i;
	}

	inline float getFloat()
	{
		float f;
		fscanf(file, "%f", &f);
		return f;
	}

	inline double getDouble()
	{
		double d;
		fscanf(file, "%lf", &d);
		return d;
	}

	std::string getString();

	std::string getQuotedString();

	std::string getLine();

private:

	FILE *file;

};


//
// Binary Files
//

class binaryFile_t
{
public:

	inline binaryFile_t() {}

	inline virtual ~binaryFile_t() = 0 { }

	virtual size_t read(void *data, size_t size, size_t len) = 0;

	virtual size_t write(void *data, size_t size, size_t len) = 0;

	virtual size_t seek(long pos, int mode) = 0;

	virtual size_t tell() = 0;

	virtual char getChar() = 0;

	virtual short getShort() = 0;

	virtual long getLong() = 0;

};

class binaryFileStd_t : public binaryFile_t
{
public:

	binaryFileStd_t(const char *, IOflags::mode_t);

	inline ~binaryFileStd_t()
	{
		fclose(file);
	}

	inline size_t read(void *data, size_t size, size_t len)
	{
		return fread(data, size, len, file);
	}

	inline size_t write(void *data, size_t size, size_t len)
	{
		return fwrite(data, size, len, file);
	}

	inline size_t seek(long pos, int mode)
	{
		return fseek(file, pos, mode);
	}

	inline size_t tell()
	{
		return ftell(file);
	}

	inline char getChar()
	{
		return fgetc(file);
	}

	inline short getShort()
	{
		int a = fgetc(file);
		int b = fgetc(file);

		return a | b<<8;
	}

	inline long getLong()
	{
		int a = fgetc(file);
		int b = fgetc(file);
		int c = fgetc(file);
		int d = fgetc(file);

		return a | b<<8 | c<<16 | d<<24;
	}

private:

	FILE *file;

};


//
// IO manager
//

class stdio
{
public:

	static inline stdio& instance()
	{
		return self;
	}

	textFile_t* openText(const char *path, IOflags::mode_t m);

	binaryFile_t* openBinary(const char *path, IOflags::mode_t m);

private:

	inline stdio()
	{
	}

	stdio(const stdio &);

	static stdio self;
};

#endif //__FILESYS_H__
