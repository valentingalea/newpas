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

/*
 *	shared by vm & compiler
 */

/*
 *	the VMobj data (on disk & mem)
 */
class vm_object_file_t
{
public:
	char	signature[32];
	int		code_size;
	int		stack_start;
	int		proc_num;
	int *	proc_index;
	int *	proc_sizes;
	char *	proc_names;
	char *	code;
};

/*
 *	global, local, by reference variables & params flags
 */
#define FORMAL		128
#define LOCAL		64
#define GLOBAL		0
#define POINTER		FORMAL

/*
 *	VM OPCODE enum
 */
enum opcodes 
/*
'a' prefix stands for 'array'
'I' suffix stands for 'integer'
'F' suffix stands for 'float'
*/
{
	NOP,

	PUSHIV,
	aPUSHIV,

	PUSHFV = PUSHIV,
	aPUSHFV = aPUSHIV, 

	PUSHSV,
	aPUSHSV,

	PUSHA,
	aPUSHA,

	POPI,
	aPOPI,
	POPF = POPI,
	aPOPF = aPOPI,
	POPA = POPI,
	aPOPA = aPOPI,
	POPS,
	aPOPS,

	PUSHI,
	PUSHF = PUSHI,
	PUSHS,
	INDEX,

	JMP,
	JMPF,

	ADDI,
	ADDF,
	SUBI,
	SUBF,
	MULI,
	MULF,
	DIVI,
	DIVF,

	CMPI,
	CMPF,
	CMPS,

	LESS,
	LESS_EQ,
	GREAT,
	GREAT_EQ,
	EQUAL,
	NOT_EQ,
	NOT,

	CAST1F,
	CAST2F,

	CALL_LIB,
	CALL,
	RET
};

/*
 *	stuff used only here (in the compiler)
 */
char * data; //data segment
int ds; //data pointer/index
int ip; //instruction pointer/index
vm_object_file_t	vm_obj;
char * program;
