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
#include <cstring>
#include "../base/window.h"
#include "vm.h"

/*
**	Typedefs
*/
typedef unsigned  char byte;
typedef unsigned short int word;
typedef unsigned  long int dword;

builtin_func_frame_t bframe;

void VM_AddBuiltinFunc(int flags, void (*func)(void))
{
	static int num = 9;

	builtin[num].flags = flags;
	builtin[num].func = func;

	num ++;
}

int VM_GetFuncAddress(const vm_object_file_t * vm_obj, const char * name)
{
	char * pname = vm_obj->proc_names;
	char buff[64];

	for ( int n = 0; n < vm_obj->proc_num; n++ )
	{
		int i = 0;
		while ( *pname )
			buff[i++] = *pname++;
		buff[i++] = 0;
		pname++;
		if ( _stricmp(buff, name) == 0 )
			return n;
	}

	return -1;
}

/************************ Builtin functions *************************/

void Builtin_Ord(void) // (char) : integer
{
	int ord = *(char *)(bframe.data + VM_POPi(0));
	//VM_POPi(1) ignore lenght: allways 1
	VM_PUSHi(0, INT_T);
	VM_PUSHi(1, ord);
}

void Builtin_Chr(void) // (integer) : char
{
	int chr = VM_POPi(0);
	*(char *)(bframe.data + bframe.bp) = chr;
	VM_PUSHi(0, CHAR_T);
	VM_PUSHi(1, bframe.bp);
	VM_PUSHi(2, 1);
}

void Builtin_FtoI(void) // (real) : integer
{
	float _float = VM_POPf(0);
	VM_PUSHi(0, INT_T);
	VM_PUSHi(1, (int)_float );
}

void Builtin_Write(void) // (..)
{
	char message[128];
	char buffer[32];

	//memset(message, 0, 128);
	message[0] = 0;
	for ( int i = 0; i < bframe.argc; i++ )
	{
		switch ( bframe.argv[i] )
		{
		case INT_T:
			sprintf(buffer, "%i\0", VM_POPi(i));
			break;
		case FLOAT_T:
			sprintf(buffer, "%f\0", VM_POPf(i));
			break;
		case CHAR_T:
			sprintf(buffer, "%c\0", *(bframe.data + VM_POPi(i)));
			i++;
			break;
		case STRING_T:
			sprintf(buffer, "%s\0", bframe.data + VM_POPi(i));
			i++;
			break;
		default:
			{ //new scope
				int offset, length, j;
				offset = VM_POPi(i);
				length = VM_POPi(++i);
				for ( j = 0; j < length; j++ )
					//sprintf(buffer, "%3X", *(unsigned char *)(bframe.data + offset + j));
					sprintf(buffer, "[pushsv %i %i]\0", offset, length);
			}
		}
		strcat(message, buffer);
	}

	MessageBox(NULL, message, "NewPas", MB_OK | MB_ICONINFORMATION);
}

extern void Script_CreateBody( void );
extern void Script_SetVar( void );

builtin_func_t	builtin[] =
{
	{ FUNC_T, Builtin_FtoI },
	{ FUNC_T, Builtin_Ord },
	{ FUNC_T, Builtin_Chr },
	{ PROC_T, Builtin_Write },

	{ PROC_T, Script_CreateBody },
	{ PROC_T, Script_SetVar },

	{ 0, 0 }
};
