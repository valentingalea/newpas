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

#include "mmgr.h"

#ifdef ENABLE

#include <stdio.h>
#include <stdlib.h>

#undef new
#undef delete

#include <new>

void* operator new(size_t n, const char *str, int l)
{
	FILE *f = fopen("memlog.txt", "at");
	if ( f )
	{
		fprintf(f, "[+]Alloc in %s line %i for %i bytes\n", str, l, n);
		fflush(f);
		fclose(f);
	}

	void *p = malloc(n);
	if ( !p )
		throw std::bad_alloc();
	return p;
}

void operator delete(void *ptr, const char *n, int l)
{
	FILE *f = fopen("memlog.txt", "at");
	if ( f )
	{
		fprintf(f, "[-]DeAlloc in %s line %i for ptr %p\n", n, l, ptr);
		fflush(f);
		fclose(f);
	}

	if ( ptr )
		free(ptr);
}

void* operator new[](size_t n, const char *str, int l)
{
	FILE *f = fopen("memlog.txt", "at");
	if ( f )
	{
		fprintf(f, "[-]Alloc array in %s line %i for %i bytes\n", str, l, n);
		fflush(f);
		fclose(f);
	}

	void *p = malloc(n);
	if ( !p )
		throw std::bad_alloc();
	return p;
}

void operator delete[](void *ptr, const char *n, int l)
{
	FILE *f = fopen("memlog.txt", "at");
	if ( f )
	{
		fprintf(f, "[-]DeAlloc array in %s line %i for ptr %p\n", n, l, ptr);
		fflush(f);
		fclose(f);
	}

	if ( ptr )
		free(ptr);
}

#endif
