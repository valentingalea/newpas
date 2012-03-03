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

//#define DEBUG_VM

#ifdef DEBUG_VM
#define __debug_print(str, num) printf("%s %i\n", str, num);
#else
#define __debug_print(str, num)
#endif

#define SIZE 4096

/*
 *  VM_Init
 *
 *  Description: inits the vm
 */
void VM_Init()
{
	memset(vm_obj.signature, 0, 32);
	sprintf(vm_obj.signature,
		//"VM bytecode ver. %i.%i", version_maj, version_min);
		"VM bytecode ver. 2.5");

	vm_obj.stack_start = 0;
	vm_obj.code_size = 0;
	vm_obj.code = (char *) malloc (4 * 1024);
	Sys_Warn("VM code malloc...");

	vm_obj.proc_num = 0;
	vm_obj.proc_index = (int *) malloc(256);
	Sys_Warn("VM proc index malloc...");
	vm_obj.proc_sizes = (int *) malloc(256);
	Sys_Warn("VM proc sizes malloc...");

	program = vm_obj.code;
	ip = 0;
	data = (char *) malloc(SIZE);
	Sys_Warn("Data malloc...");
	memset(data, 0, SIZE); //good habit
	ds = 0;
}

/*
 *  VM_Execute
 *
 *  Description: the VM fucker
 */
void VM_Execute(vm_object_file_t * vm_obj)
{
	//vm_obj [ebp+08]
	long int i;
	long int bp;
	long int num;
	long int opcode;
	char * code;
	long int gpv1;
	float gpv2;
	long int curr_proc;

#define __i "-0x04(%ebp)"
#define __num "-0x0C(%ebp)"
#define __code "-0x14(%ebp)"
#define __gpv1 "-0x18(%ebp)"
#define __gpv2 "-0x1C(%ebp)"

	long int old_num;
	vm_stack_t vm_func_stack[32];

	static int callers_stack[512];
	static int callers_stack_level = 0;
#define CALLER_PUSH(n) callers_stack[callers_stack_level++] = n
#define CALLER_POP(n) n = callers_stack[--callers_stack_level]

	code = vm_obj->code;
	curr_proc = vm_obj->proc_num-1;
	i = vm_obj->proc_index[curr_proc];
	bp = vm_obj->stack_start;

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

			if ( num >= SIZE || bp >= SIZE ) {
				printf("\nCorrupt data: num=%i bp=%i.\n", num, bp);
				Sys_Error(ERR_VM);
			}

			switch ( opcode & (LOCAL-1) )
			{
			case PUSHIV:
			//case PUSHFV:
				__debug_print("pushv", code[i+4])
				__asm(
				"movl _data, %edx\n"
				"addl "__num", %edx\n"
				"pushl %edx");
				break;
			case aPUSHIV:
			//case aPUSHFV:
				__debug_print("_pushv", code[i+4])
				__asm(
				"popl %eax\n"
				"addl %eax, "__num"\n"
				"movl _data, %edx\n"
				"addl "__num", %edx\n"
				"pushl %edx");
				break;
			case PUSHSV:
				__debug_print("pushsv", code[i+4])
				__asm(
				"movl "__code", %ecx\n"
				"addl "__i", %ecx\n"
				"pushl "__num"\n"//offset
				"pushl 5(%ecx)\n"//strlen
				"addl $4, "__i);
				break;
			case aPUSHSV:
				__debug_print("_pushsv", code[i+4])
				__asm(
				"movl "__code", %ecx\n"
				"addl "__i", %ecx\n"
				"popl %eax\n"
				"addl %eax, "__num"\n"
				"pushl "__num"\n"//offset
				"pushl 5(%ecx)\n"//strlen
				"addl $4, "__i);
				break;

			case PUSHA:
				__debug_print("pusha", num)
				__asm("pushl "__num);
				break;
			case aPUSHA:
				__debug_print("_pusha", num)
				__asm(
				"popl %eax\n"
				"addl %eax, "__num"\n"
				"pushl "__num);
				break;

			case POPI:
			//case POPF:
			//case POPA:
				__debug_print("pop", code[i+4])
				__asm(
				"movl _data, %edx\n"
				"addl "__num", %edx\n"
				"popl %eax\n"
				"movl %eax, (%edx)");
				break;
			case aPOPI:
			//case aPOPF:
			//case aPOPA:
				__debug_print("_pop", code[i+4])
				__asm(
				"movl _data, %edx\n"
				"addl "__num", %edx\n"
				"popl %eax\n"
				"popl %ebx\n"
				"movl %eax, %ebx(%edx)");
				break;
			case POPS:
				__debug_print("pops", code[i+4])
				__asm(
				"movl (%esp), %ecx\n" //pop ecx

				"movl 4(%esp), %eax\n" //pop eax
				"movl _data, %edx\n"
				"leal %eax(%edx), %esi\n"
				"movl "__num", %eax\n"
				"leal %eax(%edx), %edi\n"

				"cld\n"
				"rep movsb\n"

				"addl $8, %esp\n" //clean the stack
				"addl $4, "__i);
				break;
			case aPOPS:
				__debug_print("_pops", code[i+4])
				__asm(
				"movl 8(%esp), %eax\n"
				"addl %eax, "__num"\n"

				"movl (%esp), %ecx\n" //pop ecx

				"movl 4(%esp), %eax\n" //pop eax
				"movl _data, %edx\n"
				"leal %eax(%edx), %esi\n"
				"movl "__num", %eax\n"
				"leal %eax(%edx), %edi\n"

				"cld\n"
				"rep movsb\n"

				"addl $12, %esp\n" //clean the stack
				"addl $4, "__i);
				break;

			default:
				Sys_Error(ERR_VM);
			}
			i += 5;
		}
		else
		switch ( opcode & (LOCAL-1) )
		{
		case PUSHI:
		//case PUSHF:
			__debug_print("push[f]", *(int *)(code + i))
			__asm(
			"movl "__code", %ecx\n"
			"addl "__i", %ecx\n"
			"pushl (%ecx)\n"
			"addl $4, "__i);
			break;
		case PUSHS:
			__debug_print("pushs", code[i])
			__asm(
			"movl "__code", %ebx\n"
			"addl "__i", %ebx\n"

			"leal 8(%ebx), %esi\n" //start of string embeded in the code
			"movl _data, %edx\n"
			"movl (%ebx), %eax\n" //string offset
			"leal %eax(%edx), %edi\n"

			"movl 4(%ebx), %ecx\n"
			"cld\n"
			"rep movsb\n"

			"pushl %ebx\n"
			"movl 4(%ebx), %eax\n"
			"pushl %eax\n"
			"addl $8, $eax\n"
			"addl %eax, "__i);
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

			__asm(
			"xorl %edx, %edx\n"
			"movl "__code", %ecx\n"
			"addl "__i", %ecx\n"
			"cmpb $0, (%ecx)\n"
			"jz _dim\n"
			"popl %eax\n"
			"addl %eax, %edx\n"
			"_dim:\n"
			"xorl %ebx, %ebx\n"
			"movb 1(%ecx), %bl
			"addl $6, %ecx\n" // flag(1) + num_dim(1) + elem_size(4)
			//X
			"popl %eax\n"
			"subl 4(%ecx), %eax\n" //delta x
			"addl %eax, %edx\n"
			//Y

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
			__debug_print("cmpf", 0)
			//TODO: OPTIMIZE
			__asm fld dword ptr [esp+4]
			__asm fsub dword ptr [esp]
			__asm fstp dword ptr __gpv2
			__asm add esp, 8

			if ( gpv2 == 0.0 ) 
				gpv1 = 0;
			else 
				if ( gpv2 > 0.0 ) 
					gpv1 = 1;
				else 
					gpv1 = -1;

			__asm push dword ptr __gpv1

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
			{ //new scope
			int nr;

			nr = code[i++];
			vm_argc_t vm_func_argc = code[i++];
			vm_arg_t * vm_func_argv = (code + i);

			for ( num = vm_func_argc - 1; num >= 0; num-- )
				if ( vm_func_argv[num] == FLOAT_T ) {
					__asm pop ebx
					__asm mov dword ptr __gpv2, ebx
					vm_func_stack[num]._float = gpv2;
				} else {
					__asm pop ebx
					__asm mov dword ptr __gpv1, ebx
					vm_func_stack[num]._int = gpv1;
				}

			__debug_print("call_lib", nr);
			builtin[nr].func(vm_func_argc, vm_func_argv, vm_func_stack, data);

			if ( builtin[nr].name[0] == 'f' ) { //function
				nr = vm_func_stack[0]._int;
					gpv1 = vm_func_stack[1]._int;
					__asm push dword ptr __gpv1
				if ( nr >= CHAR_T ) {
					gpv1 = vm_func_stack[2]._int;
					__asm push dword ptr __gpv1
				}
			}

			i += vm_func_argc;

			}
			break;
		case CALL:
			CALLER_PUSH(curr_proc);
			CALLER_PUSH(i + 4);
			
			if ( callers_stack_level > 500 ) {
				printf("\nInternal stack overflow.\n");
				Sys_Error(ERR_VM);
			}

			num = *(int *)(code + i);
			__debug_print("call", num)
			//jump to the new proc. address
			i = vm_obj->proc_index[num];
			//alloc. enought space on the (imag.)stack
			//as not to touch the current proc. vars.
			bp += vm_obj->proc_sizes[curr_proc];
			//update the current(new) proc. #
			curr_proc = num;
			break;
		case RET:
			//check for exit program
			//if ( i >= vm_obj->code_size ) return;
			if ( curr_proc >= (vm_obj->proc_num-1) ) return;
			//go back to the place that called the proc.
			CALLER_POP(i);
			//return to this proc:
			CALLER_POP(curr_proc);
			__debug_print("ret", curr_proc)
			//return to the old vars.
			bp -= vm_obj->proc_sizes[curr_proc];
			break;

		default:
			Sys_Error(ERR_VM);
		}
	}
}

/*
 *  VM_Close
 *
 *  Description: returns void
 */
void VM_Close()
{
	free(vm_obj.code);
	Sys_Warn("...VM code free");
	free(vm_obj.proc_index);
	Sys_Warn("...VM proc index free");
	free(vm_obj.proc_sizes);
	Sys_Warn("...VM proc sizes free");
	free(data);
	Sys_Warn("...Data free");
}
