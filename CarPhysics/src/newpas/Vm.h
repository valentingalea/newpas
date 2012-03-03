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

#ifndef __VM_H__
#define __VM_H__

/*
 *	the VMobj data (on disk & mem)
 */
typedef struct
{
	//char	signature[32];
	int		code_size;
	int		stack_start;
	int		proc_num;
	int*	proc_index;
	int*	proc_sizes;
	char*	proc_names;
	char*	code;
	/*******************/
	char*	databuffer;
} vm_object_file_t;

/*
 *	Builtin Functions Interface
 */
enum types
{
	INT_T,
	FLOAT_T,
	CHAR_T,
	STRING_T,
	ARRAY_T,
	RECORD_T,
	PROC_T,
	FUNC_T
};

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

typedef struct
{
	int		flags;
	void	(*func)(void);
} builtin_func_t;

extern builtin_func_frame_t bframe;
extern builtin_func_t	builtin[32];

// some macros for more readability
#define VM_POPi(n) (bframe.func_stack[n]._int)
#define VM_POPf(n) (bframe.func_stack[n]._float)
#define VM_PUSHi(l, n) bframe.func_stack[l]._int = n
#define VM_PUSHf(l, n) bframe.func_stack[l]._float = n

/*
 *	Prototypes
 */
void VM_AddBuiltinFunc(int, void (*)(void));
 int VM_GetFuncAddress(const vm_object_file_t *, const char *);

vm_object_file_t * VM_Init(const char *);
void VM_Execute(vm_object_file_t *, char *, int);
void VM_Close(vm_object_file_t *);


#endif //__VM_H__