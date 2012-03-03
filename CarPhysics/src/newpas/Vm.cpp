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
#include <cstdlib>
#include "vm.h"

#define DEBUG_VM

#ifdef DEBUG_VM
#define __debug_print(str, num) { printf("[%3i]%10s%5i\n", opcode & 0xF0, str, num); fflush(stdout); }
#else
#define __debug_print(str, num)
#endif

/*
 *	global, local, by reference variables & params flags
 */
#define FORMAL		128
#define LOCAL		64
#define GLOBAL		0

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
	RET,
};


/*
 *  VM_Init
 *
 *  Description: inits the vm
 */
vm_object_file_t * VM_Init(const char * filename)
{
	FILE *fin;

	fopen_s(&fin, filename, "rb");
	if ( !fin )
		return NULL;

	fseek(fin, 0, SEEK_END);
	int size = ftell(fin);
	fseek(fin, 0, SEEK_SET);

	vm_object_file_t * vm_obj = 
		(vm_object_file_t *)malloc(sizeof(vm_object_file_t));
	if ( !vm_obj )
		return NULL;

	vm_obj->databuffer = (char *)malloc(size);
	if ( !vm_obj->databuffer )
		return NULL;

	if ( !fread((char*)vm_obj->databuffer, size, sizeof(char), fin) )
		return NULL;

	fclose(fin);

	//
	// all this stuff avoid multiple freads & 
	// adds support 4 endianess checking
	//
	char * ptr = (char*)(vm_obj->databuffer + 32);

	vm_obj->code_size = *(int*) ptr; 
	ptr += sizeof(int);

	vm_obj->stack_start = *(int*) ptr;
	ptr += sizeof(int);

	vm_obj->proc_num = *(int*) ptr;
	ptr += sizeof(int);

	vm_obj->proc_index = (int*) ptr;
	ptr += vm_obj->proc_num * sizeof(int);

	vm_obj->proc_sizes = (int*) ptr;
	ptr += vm_obj->proc_num * sizeof(int);

	vm_obj->code = (char*) ptr;
	ptr += vm_obj->code_size;

	vm_obj->proc_names = (char*) ptr;

	return vm_obj;
}

/*
 *  VM_Execute
 *
 *  Description: the VM fucker
 */
void VM_Execute(vm_object_file_t * vm_obj, char * data, int entry)
{
#ifdef DEBUG_VM
	freopen("c:/stdout.txt", "w+", stdout);
#endif
	long int i;//instruction pointer;
	long int bp;//base (stack level) pointer
	long int num;//data offset
	char * code;//bytecode pointer

#define __i i
#define __num num
#define __code code

	long int opcode;//current opcode(instruction)
	long int curr_proc;//current procedure
	long int old_num;//temporary copy of 'num'

	static int callers_stack[512];
	static int callers_stack_level = 0;
#define CALLER_PUSH(n) callers_stack[callers_stack_level++] = n
#define CALLER_POP(n) n = callers_stack[--callers_stack_level]

	code = vm_obj->code;
	curr_proc = entry;
	if ( curr_proc < 0 )
		return;
	i = vm_obj->proc_index[curr_proc];
	bp = vm_obj->stack_start;

	bframe.bp = bp + vm_obj->proc_sizes[curr_proc];
	bframe.data = data;

	while ( 1 )
	{
		opcode = code[i++];

		if ( (opcode & (LOCAL-1)) < PUSHI )
		{
			//get the general argument 
			//(could be anything: constant, address, field offset in record )
			num = *(int *)(code + i);
			//save it for later (in case it's a "by ref" record)
			old_num = num;
			//check to see if it's a local variable
			//if it is, add the local stack level
			//note: "by ref" vars will also take the if
			if ( (opcode & LOCAL) == LOCAL )
				num += bp;
			//check to seek if it's a "by ref" value
			if ( (opcode & FORMAL ) == FORMAL )
			{
				//a "by ref" value stores only an offset, so make "num"
				//the value of the real variable witch it initially pointed to !
				num = *(int*)(data + num - old_num);
				//we needed to substract old_num from the address just in case
				//num originally was a field offset
				//now add old_num again (this time we need the field offset)
				num += old_num;
				//note: for ordinary vars. old_num will be 0 so will 
				//get some redundancy
			}

			switch ( opcode & (LOCAL-1) )
			{
			case PUSHIV:
			//case PUSHFV:
				__debug_print("pushv", code[i+4])
				__asm mov edx, dword ptr [data]
				__asm add edx, dword ptr __num
				__asm push dword ptr [edx]
				break;
			case aPUSHIV:
			//case aPUSHFV:
				__debug_print("_pushv", code[i+4])
				__asm pop eax
				__asm add dword ptr __num, eax
				__asm mov edx, dword ptr [data]
				__asm add edx, dword ptr __num
				__asm push dword ptr [edx]
				break;
			case PUSHSV:
				__debug_print("pushsv", code[i+4])
				__asm mov ecx, dword ptr __code
				__asm add ecx, dword ptr __i
				__asm push dword ptr __num //offset
				__asm push dword ptr [ecx+5] //strlen
				__asm add dword ptr __i, 4
				break;
			case aPUSHSV:
				__debug_print("_pushsv", code[i+4])
				__asm mov ecx, dword ptr __code
				__asm add ecx, dword ptr __i
				__asm pop eax
				__asm add dword ptr __num, eax
				__asm push dword ptr __num //offset
				__asm push dword ptr [ecx+5] //strlen
				__asm add dword ptr __i, 4
				break;

			case PUSHA:
				__debug_print("pusha", num)
				__asm push dword ptr __num
				break;
			case aPUSHA:
				__debug_print("_pusha", num)
				__asm pop eax
				__asm add dword ptr __num, eax
				__asm push dword ptr __num
				break;

			case POPI:
			//case POPF:
			//case POPA:
				__debug_print("pop", code[i+4])
				__asm mov edx, dword ptr [data]
				__asm add edx, dword ptr __num
				__asm pop eax
				__asm mov dword ptr [edx], eax
				break;
			case aPOPI:
			//case aPOPF:
			//case aPOPA:
				__debug_print("_pop", code[i+4])
				__asm mov edx, dword ptr [data]
				__asm add edx, dword ptr __num
				__asm pop eax
				__asm pop ebx
				__asm mov dword ptr [edx+ebx], eax
				break;
			case POPS:
				__debug_print("pops", code[i+4])
				__asm mov ecx, dword ptr [esp] //pop ecx

				__asm mov eax, dword ptr [esp+4] //pop eax
				__asm mov edx, dword ptr [data]
				__asm lea esi, dword ptr [edx+eax]
				__asm mov eax, dword ptr __num
				__asm lea edi, dword ptr [edx+eax]

				__asm cld
				__asm rep movsb

				__asm add esp, 8 //clean the stack
				__asm add dword ptr __i, 4
				break;
			case aPOPS:
				__debug_print("_pops", code[i+4])
				__asm mov eax, dword ptr [esp+8]
				__asm add dword ptr __num, eax

				__asm mov ecx, dword ptr [esp] //pop ecx

				__asm mov eax, dword ptr [esp+4] //pop eax
				__asm mov edx, dword ptr [data]
				__asm lea esi, dword ptr [edx+eax]
				__asm mov eax, dword ptr __num
				__asm lea edi, dword ptr [edx+eax]

				__asm cld
				__asm rep movsb

				__asm add esp, 12 //clean the stack
				__asm add dword ptr __i, 4
				break;

			default:
				return;
			}
			i += 5;
		}
		else
		switch ( opcode & (LOCAL-1) )
		{
		case PUSHI:
		//case PUSHF:
			__debug_print("push[f]", *(int *)(code + i))
			__asm mov ecx, dword ptr __code
			__asm add ecx, dword ptr __i
			__asm push dword ptr [ecx]
			__asm add dword ptr __i, 4
			break;
		case PUSHS:
			__debug_print("pushs", code[i])
			__asm mov ebx, dword ptr __code
			__asm add ebx, dword ptr __i

			__asm lea esi, dword ptr [ebx+8] //start of string embeded in the code
			__asm mov edx, dword ptr [data]
			__asm mov eax, dword ptr [ebx] //string offset
			__asm lea edi, dword ptr [edx+eax]

			__asm mov ecx, dword ptr [ebx+4] //string length
			__asm cld
			__asm rep movsb

			__asm push dword ptr [ebx]
			__asm mov eax, dword ptr [ebx+4]
			__asm push eax
			__asm add eax, 8
			__asm add dword ptr __i, eax
			break;
		case INDEX:
			__debug_print("index", code[i])
			__asm xor edx, edx
			__asm mov ecx, dword ptr __code
			__asm add ecx, dword ptr __i
			__asm cmp byte ptr [ecx], 0
			__asm jz _dim
			__asm pop eax
			__asm add edx, eax
_dim:
			__asm xor ebx, ebx
			__asm mov  bl, byte ptr [ecx+1]
			__asm add ecx, 6 // flag(1) + num_dim(1) + elem_size(4)
			//X
			__asm pop eax
			__asm sub eax, dword ptr [ecx+4] //delta x
			__asm add edx, eax
			//Y
			__asm cmp ebx, 2
			__asm jnge _finish
			__asm pop eax
			__asm sub eax, dword ptr [ecx+12] //delta y
			__asm imul eax, dword ptr [ecx] //size x
			__asm add edx, eax
			//Z
			__asm cmp ebx, 3
			__asm jnge _finish
			__asm pop eax
			__asm sub eax, dword ptr [ecx+20] //delta z
			__asm imul eax, dword ptr [ecx] //size x
			__asm imul eax, dword ptr [ecx+8] //size y
			__asm add edx, eax
_finish:
			__asm imul edx, dword ptr [ecx-4] //elem size
			__asm push edx

			__asm shl ebx, 3
			__asm add ebx, 6
			__asm add dword ptr __i, ebx
			break;

		case JMP:
			__debug_print("jmp", *(int *)(code + i))
			__asm mov ecx, dword ptr __code
			__asm add ecx, dword ptr __i
			__asm mov eax, dword ptr [ecx]
			__asm add dword ptr __i, eax
			break;
		case JMPF:
			__debug_print("jmpf", *(int *)(code + i))
			__asm pop eax
			__asm and eax, 255
			__asm cmp eax, 0
			__asm jnz _elsef
			//then jump
			__asm mov ecx, dword ptr __code
			__asm add ecx, dword ptr __i
			__asm mov eax, dword ptr [ecx]
			__asm add dword ptr __i, eax
			__asm jmp _outf
_elsef: 	//else incr. norm.
			__asm add dword ptr __i, 4
_outf:
			break;

		case ADDI:
			__debug_print("add", 0)
			__asm pop eax
			__asm add dword ptr [esp], eax
			break;
		case ADDF:
			__debug_print("addf", 0)
			__asm fld dword ptr [esp+4]
			__asm fadd dword ptr [esp]
			__asm add esp, 4
			__asm fstp dword ptr [esp]
			break;
		case SUBI:
			__debug_print("sub", 0)
			__asm pop eax
			__asm sub dword ptr [esp], eax
			break;
		case SUBF:
			__debug_print("subf", 0)
			__asm fld dword ptr [esp+4]
			__asm fsub dword ptr [esp]
			__asm add esp, 4
			__asm fstp dword ptr [esp]
			break;
		case MULI:
			__debug_print("mul", 0)
			__asm pop eax
			__asm pop ecx
			__asm imul eax, ecx
			__asm push eax
			break;
		case MULF:
			__debug_print("mulf", 0)
			__asm fld dword ptr [esp+4]
			__asm fmul dword ptr [esp]
			__asm add esp, 4
			__asm fstp dword ptr [esp]
			break;
		case DIVI:
			__debug_print("div", 0)
			__asm pop ecx
			__asm pop eax
			__asm cdq
			__asm idiv ecx
			__asm push eax
			break;
		case DIVF:
			__debug_print("divf", 0)
			__asm fld dword ptr [esp+4]
			__asm fdiv dword ptr [esp]
			__asm add esp, 4
			__asm fstp dword ptr [esp]
			break;

		case CMPI:
			__debug_print("cmpi", 0);
			__asm pop eax
			__asm sub dword ptr [esp], eax
			break;
		case CMPF:
			{ //open new scope
			__debug_print("cmpf", 0)
			int gpv1;
			float gpv2;
			//TODO: OPTIMIZE
			__asm fld dword ptr [esp+4]
			__asm fsub dword ptr [esp]
			__asm fstp dword ptr gpv2
			__asm add esp, 8

			if ( gpv2 == 0.0 ) 
				gpv1 = 0;
			else 
				if ( gpv2 > 0.0 ) 
					gpv1 = 1;
				else 
					gpv1 = -1;

			__asm push dword ptr gpv1

			} //close scope
			break;
		case CMPS:
			__debug_print("cmps", 0);
			__asm mov edx, dword ptr data
			__asm mov eax, dword ptr [esp+12] //stroffs1
			__asm lea esi, dword ptr [edx+eax]
			__asm mov ebx, dword ptr [esp+8] //strlen1
			__asm mov eax, dword ptr [esp+4] //stroffs2
			__asm lea edi, dword ptr [edx+eax]
			__asm mov ecx, dword ptr [esp] //strlen2
			__asm xor eax, eax
_charcmp:
			__asm mov  al, byte ptr [esi]
			__asm cmp byte ptr [edi], al
			__asm jne _charend
			//null terminated strings (cannot be single chars)
			__asm cmp byte ptr [esi], 0
			__asm jz _charend
			__asm cmp byte ptr [edi], 0
			__asm jz _charend
			//hardcode single chars
			__asm dec ebx
			__asm dec ecx
			__asm cmp ebx, 0
			__asm jz _charend
			__asm cmp ecx, 0
			__asm jz _charend
			//advance
			__asm inc esi
			__asm inc edi
			__asm jmp _charcmp
_charend:
			__asm mov  al, byte ptr [esi]
			__asm xor ecx, ecx
			__asm mov  cl, byte ptr [edi]
			__asm sub eax, ecx

			__asm add esp, 16
			__asm push eax
			break;

		case LESS:
			__debug_print("less", 0);
			__asm xor eax, eax
			__asm cmp dword ptr [esp], 0
			__asm setl al
			__asm mov dword ptr [esp], eax
			break;
		case LESS_EQ:
			__debug_print("less_eq", 0);
			__asm xor eax, eax
			__asm cmp dword ptr [esp], 0
			__asm setle al
			__asm mov dword ptr [esp], eax
			break;
		case GREAT:
			__debug_print("great", 0);
			__asm xor eax, eax
			__asm cmp dword ptr [esp], 0
			__asm setg al
			__asm mov dword ptr [esp], eax
			break;
		case GREAT_EQ:
			__debug_print("great_eq", 0);
			__asm xor eax, eax
			__asm cmp dword ptr [esp], 0
			__asm setge al
			__asm mov dword ptr [esp], eax
			break;
		case EQUAL:
			__debug_print("equal", 0);
			__asm xor eax, eax
			__asm cmp dword ptr [esp], 0
			__asm sete al
			__asm mov dword ptr [esp], eax
			break;
		case NOT_EQ:
			__debug_print("less", 0);
			__asm xor eax, eax
			__asm cmp dword ptr [esp], 0
			__asm setne al
			__asm mov dword ptr [esp], eax
			break;
		case NOT:
			__debug_print("not", 0);
			__asm not dword ptr [esp]
			__asm and dword ptr [esp], 1
			break;

		case CAST1F:
			__debug_print("cast1f", 0)
			__asm fild dword ptr [esp]
			__asm fstp dword ptr [esp]
			break;
		case CAST2F:
			__debug_print("cast2f", 0)
			__asm fild dword ptr [esp+4]
			__asm fstp dword ptr [esp+4]
			break;

		case CALL_LIB:
			{ //open new scope
			int nr;
			int gpv1;
			float gpv2;

			nr = code[i++];
			bframe.argc = code[i++];
			bframe.argv = (code + i);

			for ( num = bframe.argc - 1; num >= 0; num-- )
				if ( bframe.argv[num] == FLOAT_T )
				{
					__asm pop ebx
					__asm mov dword ptr gpv2, ebx
					bframe.func_stack[num]._float = gpv2;
				} 
				else 
				{
					__asm pop ebx
					__asm mov dword ptr gpv1, ebx
					bframe.func_stack[num]._int = gpv1;
				}

			__debug_print("call_lib", nr);
			builtin[nr].func();

			if ( builtin[nr].flags == FUNC_T )
			{
				nr = bframe.func_stack[0]._int;
					gpv1 = bframe.func_stack[1]._int;
					__asm push dword ptr gpv1
				if ( nr >= CHAR_T ) 
				{
					gpv1 = bframe.func_stack[2]._int;
					__asm push dword ptr gpv1
				}
			}

			i += bframe.argc;

			} //close scope
			break;
		case CALL:
			CALLER_PUSH(curr_proc);
			CALLER_PUSH(i + 4);
			
			__debug_print("call", num)

			//jump to the new proc. address
			num = *(int *)(code + i);
			i = vm_obj->proc_index[num];

			//alloc. enough space on the (imag.)stack
			//as not to touch the current proc. vars.
			bp += vm_obj->proc_sizes[curr_proc];
			bframe.bp += vm_obj->proc_sizes[num]; //one step(proc. size) ahead

			//update the current(new) proc. #
			curr_proc = num;

			break;
		case RET:
			//check for exit program
			//if ( curr_proc >= (vm_obj->proc_num-1) ) return;
			if ( curr_proc == entry ) 
			{
#ifdef DEBUG_VM
				fclose(stdout);
#endif
				return;
			}

			//go back to the place that called the proc.
			CALLER_POP(i);

			//return to this proc:
			CALLER_POP(curr_proc);

			__debug_print("ret", curr_proc)

			//return to the old vars.
			bframe.bp = bp; //one step(proc. size) ahead
			bp -= vm_obj->proc_sizes[curr_proc];
			break;

		default:
			return;
		}
	}
}

/*
 *  VM_Close
 *
 *  Description: returns void
 */
void VM_Close(vm_object_file_t * vm_obj)
{
	if ( vm_obj ) {
		free(vm_obj->databuffer);
		free(vm_obj);
	}
}
