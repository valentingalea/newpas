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

typedef struct
{
	int		flags;
	void	(*func)(void);
} builtin_func_t;

builtin_func_frame_t bframe;

// some macros for more readability
#define VM_POPi(n) (bframe.func_stack[n]._int)
#define VM_POPf(n) (bframe.func_stack[n]._float)
#define VM_PUSHi(l, n) bframe.func_stack[l]._int = n
#define VM_PUSHf(l, n) bframe.func_stack[l]._float = n


void Builtin_Write(void)
{
	for ( int i = 0; i < bframe.argc; i++ )
		switch ( bframe.argv[i] )
		{
		case INT_T:
			printf("%i", VM_POPi(i));
			break;
		case FLOAT_T:
			printf("%f", VM_POPf(i));
			break;
		case CHAR_T:
			//FIXME: don't display null chars
			printf("%c", *(bframe.data + VM_POPi(i)));
			i++; //skip length (allways 1)
			break;
		case STRING_T:
			printf("%s", bframe.data + VM_POPi(i));
			i++; //skip length (not needed because of null terminator)
			break;
		default:
			{ //new scope
				int offset, length, j;
				offset = VM_POPi(i);
				length = VM_POPi(++i);
				for ( j = 0; j < length; j++ )
					printf("%3X", *(unsigned char *)(bframe.data + offset + j));
			}
		}
}


void Builtin_Writeln(void)
{
	if ( bframe.argc )
		Builtin_Write();
	printf("\n");
}


void Builtin_Printf(void)
{
	char * str = (bframe.data + VM_POPi(0));

	//trivial case: only the format string
	if ( bframe.argc <= 2 )
	{
		printf("%s", str);
		return;
	}

	int argc = 0;
	int j, i = 0;
	bool mark = false;
	char argv[16][32];
	memset(argv, 0, sizeof(argv));

	// break the format string into smaller format strings (one per each '%')
	while ( *str )
	{
		argv[argc][i++] = *str;

		if ( *str == '%' )
			mark = true;
		if ( mark && ( *str == ' ' || *(str+1) == '%' ) )
		{
			argc++;
			i = 0;
			mark = false;
		}

		str++;
	}
	argc++;

	// the "+ 2" skips over the format string
	for ( j = 0, i = 2; i < argc+2; j++, i++ )
		switch ( bframe.argv[ i ] )
		{
		case INT_T:
			printf(argv[j], VM_POPi( i ));
			break;
		case FLOAT_T:
			printf(argv[j], VM_POPf( i ));
			break;
		case CHAR_T:
			printf(argv[j], *(bframe.data + VM_POPi( i )));
			break;
		case STRING_T:
			printf(argv[j], (bframe.data + VM_POPi( i )));
			break;
		default: 
			;
		}

}

void Builtin_Read(void)
{
	for ( int i = 0; i < bframe.argc; i++ )
		switch ( bframe.argv[i] )
		{
		case INT_T:
			scanf("%i", (int *)(bframe.data + VM_POPi(i)));
			break;
		case FLOAT_T:
			scanf("%f", (int *)(bframe.data + VM_POPi(i)));
			break;
		case CHAR_T:
			*(char *)(bframe.data + VM_POPi(i)) = getc(stdin);
			break;
		case STRING_T:
			scanf("%s", (bframe.data + VM_POPi(i)));
			break;
		default: ;
		}
}

void Builtin_Readln(void)
{
	getc(stdin);
}

void Builtin_Val(void)
{
	char * s = (bframe.data + VM_POPi(0));
	*(int *)(bframe.data + VM_POPi(2)) = atoi(s);
}


void Builtin_Ord(void)
{
	int ord = *(char *)(bframe.data + VM_POPi(0));
	//VM_POPi(1) ignore lenght: allways 1
	VM_PUSHi(0, INT_T);
	VM_PUSHi(1, ord);
}


void Builtin_Chr(void)
{
	int chr = VM_POPi(0);
	*(char *)(bframe.data + bframe.bp) = chr;
	VM_PUSHi(0, CHAR_T);
	VM_PUSHi(1, bframe.bp);
	VM_PUSHi(2, 1);
}


#include <time.h> //for randomize

void Builtin_Randomize(void)
{
	time_t t;
	srand((unsigned) time(&t));
}


void Builtin_Random(void)
{
	int modulus = VM_POPi(0);
	VM_PUSHi(0, INT_T);
	VM_PUSHi(1, rand() % modulus);
}



builtin_func_t	builtin[] =
{
	{ PROC_T, Builtin_Write },
	{ PROC_T, Builtin_Writeln },
	{ PROC_T, Builtin_Printf },
	{ PROC_T, Builtin_Read },
	{ PROC_T, Builtin_Val },
	{ FUNC_T, Builtin_Ord },
	{ FUNC_T, Builtin_Chr },
	{ PROC_T, Builtin_Randomize },
	{ FUNC_T, Builtin_Random },
	{ PROC_T, Builtin_Readln },
	NULL
};
