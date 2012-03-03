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

#include "def.h"
#include "error.h"
#include "parse.h"
#include "opcode.h"
#include "types.h"
#include "lib.h"
#include "lang.h"
#include "comp.h"
#include "vm.h"

//#define	VMSDK		2.6

void __cdecl main
(int argc, char **argv)
{
	if ( argc <= 1 )
	{
		printf(
			"NewPas Compiler - Copyright (C) Valentin Galea\n"
			"Experimental Pascal compiler and interpretor\n"
			"\n"
			"NEWPAS file command [option value]\n"
			"\n"
			"  file\t\tpath to source Pascal code file\n"
			"  command\tone of the following:\n"
			"  \t\texecvm\n"
			"  \t\t\tcompiles and interprets right away the input\n"
			"  \t\toutvm\n"
			"  \t\t\tcompiles and saves VM object code\n"
			"  option\tone of the following:\n"
			"  \t\t-mem <amount>\n"
			"  \t\t\tamount of memory available in bytes\n"
			"\n"
		);
		return;
	}

	char *scriptFile = argv[1];
	int i = strlen(scriptFile) - 1;
	for( ; i >= 0; i-- )
		if( scriptFile[ i ] == '\\' )
			break;
	strcpy(path, scriptFile);
	if( i < 0 )
		strcpy( path, "." );
	else
		path[ i ] = 0;

	//
	// Language
	//
	//if ( Parse_CmdLine(argc, argv, "-lang:ro", false ) )
	//	Sys_Error = RO_Sys_Error;
	//else
	//	Sys_Error = EN_Sys_Error;

	//
	// ZONE init
	//
	Z_init(Parse_CmdLine(argc, argv, "-mem", (256 * 1024)));

	//
	// VM init
	//
	//MAX_PROCS = Parse_CmdLine(argc, argv, "-maxprocs", 64);
	//CODE_SIZE = Parse_CmdLine(argc, argv, "-codesize", 8192);
	//DATA_SIZE = Parse_CmdLine(argc, argv, "-datasize", 4096);
	VM_Init();

	//
	// Parse & Compile
	//
	Parse_Init(scriptFile);
	Type_Init();

	Parse_Loop(GLOBAL_SPACE);

	//sanity check
#ifndef VMSDK
	{
		symbol_t * s;
		for ( node_s * i = global_var->last; i; i = i->prev )
		{
			s = (symbol_t*)i->data;
			if ( s->type == FUNC_T )
				Sys_Error(ERR_ENDPRGM_MISMATCH);
			if ( s->type == PROC_T )
				if ( s->func.num_params )
					Sys_Error(ERR_ENDPRGM_MISMATCH);
				else
					break;
		}
	}
#endif

	Type_CleanUp();
	Parse_Close();

	//
	// ZONE close
	//
	Z_close();

	//
	// VM exec
	//
	vm_obj.proc_num = proc_id_counter;
	vm_obj.stack_start = ds;
	vm_obj.code_size = ip;
	if ( Parse_CmdLine(argc, argv, "execvm", false) )
		VM_Execute(&vm_obj);

	//
	// VM Obj Write
	//
	if ( Parse_CmdLine(argc, argv, "outvm", false) )
	{
		FILE * obj_file = fopen("out.vm", "wb");
		fwrite(vm_obj.signature, 32, 1, obj_file);
		fwrite(&vm_obj.code_size, 4, 1, obj_file);
		fwrite(&vm_obj.stack_start, 4, 1, obj_file);
		fwrite(&vm_obj.proc_num, 4, 1, obj_file);
		fwrite((char*)vm_obj.proc_index , 4 * vm_obj.proc_num , 1, obj_file);
		fwrite((char*)vm_obj.proc_sizes , 4 * vm_obj.proc_num , 1, obj_file);
		fwrite((char*)vm_obj.code, vm_obj.code_size, 1, obj_file);
		fwrite((char*)vm_obj.proc_names, proc_names_counter, 1, obj_file);
		fclose(obj_file);
	}

	//
	// VM close
	//
	VM_Close();
	
	exit( 0 );
}
