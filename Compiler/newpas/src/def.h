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

#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define stricmp _stricmp

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

/*
**	VERSION 2.x.x
*/
//int version_maj, version_min, version_build;

/*
**	Typedefs
*/
typedef unsigned  char byte;
typedef unsigned short int word;
typedef unsigned  long int dword;

/*
**	Errors
*/
void EN_Sys_Error(int err_code, int extra_params = 0, int param1 = 0, int param2 = 0);
#define Sys_Error EN_Sys_Error
#define Sys_Warn(msg) /*fprintf(stdout, "%s\n", msg)*/

/*
**	Mem debug crap
*/
byte * zdata;
dword zcount;
FILE * zlog;
//#define Z_OUTPUT

void Z_init(dword);
void Z_close();
void * Z_malloc(dword, const char *, dword);
void Z_free(void *, const char *, dword);


void Z_init(dword size)
{
	zdata = (byte *)malloc(size);
	if ( !zdata )
	{
		fprintf(stderr, "mem alloc error.\n");
		//TODO: too drastic...
		exit(2);
	}
	zcount = 0;
	zlog = stderr;
#ifdef Z_OUTPUT
	fprintf(zlog, "[+] ZONE alloc %p (%i bytes)\n", zdata, size);
#endif
}


void Z_close()
{
#ifdef Z_OUTPUT
	fprintf(zlog, "[-] ZONE free %p (%i used)\n", zdata, zcount);
#endif
	free(zdata);
}


void * Z_malloc(dword size, const char * file, dword line)
{
	void * ptr;
	static int num = 1;

	*(dword *)(zdata + zcount) = size;
	zcount += 4;
	ptr = (void *)(zdata + zcount);
	zcount += size;
#ifdef Z_OUTPUT
	int i;
	char name[16];

	i = strlen(file);
	while ( file[i] != '\\' ) i--;
	strcpy(name, file + i + 1);
	fprintf(zlog, "[%i] malloc %p (%i bytes) @ %s line %i\n", num, ptr, size, name, line);
	num++;
#endif
	return ptr;
}


void Z_free(void * ptr, const char * file, dword line)
{
	static int num = 1;
	int size;

	byte * data = (byte *)ptr;
	size = *(dword *)(data - 4);
	zcount -= size + 4;
#ifdef Z_OUTPUT
	int i;
	char name[16];

	i = strlen(file);
	while ( file[i] != '\\' ) i--;
	strcpy(name, file + i + 1);
	fprintf(zlog, "<%i> free %p (%i bytes) @ %s line %i\n", num, ptr, size, name, line);
	num++;
#endif
}

#define Z_malloc(s, t) s = (t *)Z_malloc(sizeof(t), __FILE__, __LINE__)
#define Z_free(n) Z_free(n, __FILE__, __LINE__)

/*
**	Parsing
*/
#define MAX_IDENT_SIZE 256
#define STR_SIZE 32

typedef struct
{
	char	nextchar;
	int		token;
	char *	src;
	int		src_i;
	int		src_size;
	char	value[MAX_IDENT_SIZE];
	double	float_value;
	long int int_value;
	int		line_nr;
	int		token2char; //for correct error reporting
} parse_state_t;

typedef struct 
{
	char	filename[MAX_PATH];
	int		old_line_nr;
	int		end_of_file;
} included_file_info_t;

parse_state_t parser;
included_file_info_t includedFiles[16];
int	includedNum;

void Parse_Init(char *);
void Parse_Close();
void Parse_Next();
void Parse_Undo();
 int Parse_GetKeyword();
 int Parse_MatchKeyword(int);
//char * Parse_GetIdentifier();
 int Parse_GetIdentifier();

typedef double calc_t;
void Parse_Calculator(void);
int calc_allow_float = true;


/*
**	Pascal Language stuff
*/
#define GLOBAL_SPACE	0
#define PARAM_SPACE		1
#define LOCAL_SPACE		2

#define FUNC_EXTERN		2
#define FUNC_VA_LIST	16

class symbol_t;
class function_t;
class array_t;
class record_t;
class user_type_t;
class linklist_t;
class constant_t
{
public:
	word			type;
	 int			name;
	union
	{
		long int	integer;
		long double	real;
		char *		string;	
	} value;
	constant_t *	next;
};
//set to latest type of a parsed expression
word last_type;

void Type_Init();
void Type_CleanUp();
symbol_t * Type_GetSymbol(linklist_t *, char *);
symbol_t * Type_SearchSymbol();
constant_t * Type_SearchConstant();
user_type_t * Type_Get(linklist_t *, char *);
//declared here cause they use user_type_t
user_type_t * Parse_GetType(linklist_t *);
void Parse_Loop( int );

void Lang_Factor();
void Lang_Term();
void Lang_Expression();
void Lang_Relation();

void Lang_IFstatement();
void Lang_WHILEstatement();
void Lang_REPEAT_UNTILstatement();
void Lang_FORstatement();
void Lang_CASEstatement();
 int Lang_Statement();
void Lang_SimpleStatement(symbol_t * = NULL);
symbol_t * Lang_Assigment(symbol_t *);

void		Comp_VarsDeclaration(function_t *, int, int = false);
void		Comp_TypesDeclaration(function_t *);
void		Comp_ConstDeclaration(void);
void		Comp_ArrayDeclaration(array_t &);
symbol_t	Comp_ArrayStatement(symbol_t *, int &);
void		Comp_StructDeclaration(record_t &);
symbol_t	Comp_StructStatement(symbol_t *, int &);
void		Comp_SubroutineDeclaration(int);

int			op_ovrld_count;
void		Comp_OverloadOperator();

/*
**	Virtual Machine
*/
class vm_object_file_t;

void VM_Init();
void VM_Execute(vm_object_file_t *);
void VM_Close();

// VM output opcodes macros
#define VMobj_outbyte( o ) \
		program[ip++] = o

#define VMobj_outdword( o ) \
		*(int*)(program + ip) = o; \
		ip += 4

#define VMobj_outdword2( i, o ) \
		*(int*)(program + i) = o;

#define VMobj_outdwordf( o ) \
		*(float*)(program + ip) = o; \
		ip += 4

#define VMobj_outdwordf2( i, o ) \
		*(float*)(program + i) = o;
/*
#define VMobj_outopcode( P, s, flag ) \
		VMobj_outbyte( s->flags | P ); \
		VMobj_outdword( s->offset ); \
		VMobj_outbyte( flag )
*/
#define VMobj_outopcode( P, s, flag ) \
		(( flag ) ? \
		VMobj_outbyte( s->flags | a##P ) \
		: \
		VMobj_outbyte( s->flags | P )); \
		VMobj_outdword( s->offset ); \
		VMobj_outbyte( flag )

typedef struct
{
	/*** arguments count & types ***/
	 int	argc;
	char	*argv;
	/*** arguments values & stack level ***/
	 int	bp;
	union {
		int		_int;
		float	_float;
	} func_stack[32];
	/*** data segment ***/
	char	*data;
} builtin_func_frame_t;

int MAX_PROCS		= 64;
int CODE_SIZE		= 8192;
int DATA_SIZE		= 4096;

