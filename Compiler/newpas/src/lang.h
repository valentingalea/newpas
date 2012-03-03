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
	byte token;

	byte operand1;
	byte operand2;
	byte return_type;

	byte vm_instructions[4];
	word user_func;

} operator_t;

operator_t op_ovrld_table[256] =
{
	{ OP_ADD, INT_T, INT_T, INT_T, { ADDI, 0, 0, 0 }, 0 },
	{ OP_ADD, INT_T, FLOAT_T, FLOAT_T, { CAST2F, ADDF, 0, 0 }, 0 },
	{ OP_ADD, FLOAT_T, INT_T, FLOAT_T, { CAST1F, ADDF, 0, 0 }, 0 },
	{ OP_ADD, FLOAT_T, FLOAT_T, FLOAT_T, { ADDF, 0, 0, 0 }, 0 },
	{ OP_SUB, INT_T, INT_T, INT_T, { SUBI, 0, 0, 0 }, 0 },
	{ OP_SUB, INT_T, FLOAT_T, FLOAT_T, { CAST2F, SUBF, 0, 0 }, 0 },
	{ OP_SUB, FLOAT_T, INT_T, FLOAT_T, { CAST1F, SUBF, 0, 0 }, 0 },
	{ OP_SUB, FLOAT_T, FLOAT_T, FLOAT_T, { SUBF, 0, 0, 0 }, 0 },
	{ OP_MUL, INT_T, INT_T, INT_T, { MULI, 0, 0, 0 }, 0 },
	{ OP_MUL, INT_T, FLOAT_T, FLOAT_T, { CAST2F, MULF, 0, 0 }, 0 },
	{ OP_MUL, FLOAT_T, INT_T, FLOAT_T, { CAST1F, MULF, 0, 0 }, 0 },
	{ OP_MUL, FLOAT_T, FLOAT_T, FLOAT_T, { MULF, 0, 0, 0 }, 0 },
	{ OP_DIV, INT_T, INT_T, INT_T, { DIVI, 0, 0, 0 }, 0 },
	{ OP_DIV, INT_T, FLOAT_T, FLOAT_T, { CAST2F, DIVF, 0, 0 }, 0 },
	{ OP_DIV, FLOAT_T, INT_T, FLOAT_T, { CAST1F, DIVF, 0, 0 }, 0 },
	{ OP_DIV, FLOAT_T, FLOAT_T, FLOAT_T, { DIVF, 0, 0, 0 }, 0 },
	{ OP_LESS, INT_T, INT_T, INT_T, { CMPI, LESS, 0, 0 }, 0 },
	{ OP_LESS, INT_T, FLOAT_T, INT_T, { CAST2F, CMPF, LESS, 0 }, 0 },
	{ OP_LESS, FLOAT_T, INT_T, INT_T, { CAST1F, CMPF, LESS, 0 }, 0 },
	{ OP_LESS, FLOAT_T, FLOAT_T, INT_T, { CMPF, LESS, 0, 0 }, 0 },
	{ OP_LESS, STRING_T, STRING_T, INT_T, { CMPS, LESS, 0, 0 }, 0 },
	{ OP_LESS_EQ, INT_T, INT_T, INT_T, { CMPI, LESS_EQ, 0, 0 }, 0 },
	{ OP_LESS_EQ, INT_T, FLOAT_T, INT_T, { CAST2F, CMPF, LESS_EQ, 0 }, 0 },
	{ OP_LESS_EQ, FLOAT_T, INT_T, INT_T, { CAST1F, CMPF, LESS_EQ, 0 }, 0 },
	{ OP_LESS_EQ, FLOAT_T, FLOAT_T, INT_T, { CMPF, LESS_EQ, 0, 0 }, 0 },
	{ OP_LESS_EQ, STRING_T, STRING_T, INT_T, { CMPS, LESS_EQ, 0, 0 }, 0 },
	{ OP_GREAT, INT_T, INT_T, INT_T, { CMPI, GREAT, 0, 0 }, 0 },
	{ OP_GREAT, INT_T, FLOAT_T, INT_T, { CAST2F, CMPF, GREAT, 0 }, 0 },
	{ OP_GREAT, FLOAT_T, INT_T, INT_T, { CAST1F, CMPF, GREAT, 0 }, 0 },
	{ OP_GREAT, FLOAT_T, FLOAT_T, INT_T, { CMPF, GREAT, 0, 0 }, 0 },
	{ OP_GREAT, STRING_T, STRING_T, INT_T, { CMPS, GREAT, 0, 0 }, 0 },
	{ OP_GREAT_EQ, INT_T, INT_T, INT_T, { CMPI, GREAT_EQ, 0, 0 }, 0 },
	{ OP_GREAT_EQ, INT_T, FLOAT_T, INT_T, { CAST2F, CMPF, GREAT_EQ, 0 }, 0 },
	{ OP_GREAT_EQ, FLOAT_T, INT_T, INT_T, { CAST1F, CMPF, GREAT_EQ, 0 }, 0 },
	{ OP_GREAT_EQ, FLOAT_T, FLOAT_T, INT_T, { CMPF, GREAT_EQ, 0, 0 }, 0 },
	{ OP_GREAT_EQ, STRING_T, STRING_T, INT_T, { CMPS, GREAT_EQ, 0, 0 }, 0 },
	{ OP_EQ, INT_T, INT_T, INT_T, { CMPI, EQUAL, 0, 0 }, 0 },
	{ OP_EQ, INT_T, FLOAT_T, INT_T, { CAST2F, CMPF, EQUAL, 0 }, 0 },
	{ OP_EQ, FLOAT_T, INT_T, INT_T, { CAST1F, CMPF, EQUAL, 0 }, 0 },
	{ OP_EQ, FLOAT_T, FLOAT_T, INT_T, { CMPF, EQUAL, 0, 0 }, 0 },
	{ OP_EQ, STRING_T, STRING_T, INT_T, { CMPS, EQUAL, 0, 0 }, 0 },
	{ OP_NOT_EQ, INT_T, INT_T, INT_T, { CMPI, NOT_EQ, 0, 0 }, 0 },
	{ OP_NOT_EQ, INT_T, FLOAT_T, INT_T, { CAST2F, CMPF, NOT_EQ, 0 }, 0 },
	{ OP_NOT_EQ, FLOAT_T, INT_T, INT_T, { CAST1F, CMPF, NOT_EQ, 0 }, 0 },
	{ OP_NOT_EQ, FLOAT_T, FLOAT_T, INT_T, { CMPF, NOT_EQ, 0, 0 }, 0 },
	{ OP_NOT_EQ, STRING_T, STRING_T, INT_T, { CMPS, NOT_EQ, 0, 0 }, 0 }
};

/*
 *  Type_Cast
 *
 *  Description: handles operators
 */
void Type_Cast( int op, int type1, int type2 )
{
	//TODO: explain all this crap

	int i, j, found;

	i = op_ovrld_count - 1;
again:
	for ( ; i >= 0; i-- ) {
		if ( op == op_ovrld_table[i].token )
			break;
	}

	found = false;
	j = i;
	while ( op_ovrld_table[j].token == op )
	{
		//TODO: OPTIMIZE:
		if ( type1 == CHAR_T && op_ovrld_table[j].operand1 == STRING_T )
			type1 = STRING_T;
		if ( type2 == CHAR_T && op_ovrld_table[j].operand2 == STRING_T )
			type2 = STRING_T;
		if ( type1 == op_ovrld_table[j].operand1 &&
			type2 == op_ovrld_table[j].operand2 )
		{
			found = true;
			i = j;
			break;
		}
		j --;
	}

	if ( !found )
		if ( op_ovrld_table[i].vm_instructions[0] )
			Sys_Error(ERR_TYPE_MISMATCH, 2, type1, type2);
		else
		{
			i --;
			goto again;
		}

	if ( op_ovrld_table[i].vm_instructions[0] ) {
		j = 0;
		while ( op_ovrld_table[i].vm_instructions[j] )
		{
			VMobj_outbyte(op_ovrld_table[i].vm_instructions[j]);
			j++;
		}
	} else {
		VMobj_outbyte(CALL);
		VMobj_outdword(op_ovrld_table[i].user_func);
	}

	last_type = op_ovrld_table[i].return_type;
}


bool push_only_addr = false;

/*
 *  Lang_SignedFactor
 *
 *  Description: helper 4 Lang_Factor
 */
void Lang_SignedFactor()
{
	bool unary = false;

	if ( parser.token == OP_SUB )
	{
		unary = true;
		Parse_Next();
	}
	else
	if ( parser.token == OP_ADD )
		Parse_Next();

	Lang_Factor();

	if ( parser.token == INT ) {
		VMobj_outbyte(PUSHI);
		VMobj_outdword( (unary) ? -parser.int_value : parser.int_value );
		last_type = INT_T;
		return;
	}
	if ( parser.token == FLOAT ) {
		VMobj_outbyte(PUSHF);
		VMobj_outdwordf( (unary) ? -(float)parser.float_value : (float)parser.float_value );
		last_type = FLOAT_T;
		return;
	}
	if ( parser.token == IDENT && unary ) {
		if ( last_type == INT_T ) {
			VMobj_outbyte(PUSHI);
			VMobj_outdword( -1 );
			VMobj_outbyte(MULI);
			return;
		}
		if ( last_type == FLOAT_T ) {
			VMobj_outbyte(PUSHF);
			VMobj_outdwordf( -1.0 );
			VMobj_outbyte(MULF);
			return;
		}
		Sys_Error(ERR_PARSE);
	}
}

/*
 *  Lang_NegateFactor
 *
 *  Description: helper 4 Lang_Factor
 */
void Lang_NegateFactor()
{
	if ( parser.token == OP_NOT )
	{
		Parse_Next();
		Lang_SignedFactor();
		if ( last_type != INT_T )
			Sys_Error(ERR_TYPE_MISMATCH, 2, last_type, last_type);
		VMobj_outbyte(NOT);
	}
	else
		Lang_SignedFactor();
}

/*
 *  Lang_Factor
 *
 *  Description: the big fucka', the heart of expressions
 */
void Lang_Factor()
{
	int gpv; //general purpose variable

	if ( parser.token == '(' )
	{
		Parse_Next();

		Lang_Relation();
		//Lang_Expression();

		Parse_Next();
		if ( parser.token != ')' )
			Sys_Error(ERR_NO_RIGHTPARANTESIS);
		parser.token = IDENT; //HACK: not safe !

		return;
	}

	if ( parser.token == IDENT )
	{
		symbol_t returned_data;
		symbol_t * s;
		int flag_inheritence;
		int mask = 0x0F;

		s = Type_SearchSymbol();
		if ( !s )
			Sys_Error(ERR_UNKNOWN_IDENT);
/*
		if ( s->type & POINTER )
		{
			Parse_Next();
			if ( parser.token == OP_CARRET ) {
				mask = 0x0F;
				s->flags |= FORMAL;
			} else {
				Parse_Undo();
				s->flags &= FORMAL - 1;
				s->type = POINTER | POINTER_T;
				push_only_addr = false;
			}
		}
*/
		flag_inheritence = s->flags;
		gpv = false; //link to array

		if ( (s->type & mask) == ARRAY_T || (s->type & mask) == STRING_T )
		{
			bool save = push_only_addr;
			push_only_addr = false;
			returned_data = Comp_ArrayStatement(s, gpv);
			s = &returned_data;
			push_only_addr = save;
		}

		if ( (s->type & mask) == RECORD_T )
		{
			bool save = push_only_addr;
			push_only_addr = false;
			returned_data = Comp_StructStatement(s, gpv);
			s = &returned_data;
			push_only_addr = save;
		}

		s->flags = flag_inheritence;

		if ( push_only_addr && ((s->type & mask) != FUNC_T) )
		{
			VMobj_outopcode(PUSHA, s, gpv);
			last_type = s->type;
			push_only_addr = false;
			return;
		}

		switch ( s->type & mask )
		{
		case INT_T:
			VMobj_outopcode(PUSHIV, s, gpv);
			last_type = INT_T;
			break;
		case FLOAT_T:
			VMobj_outopcode(PUSHFV, s, gpv);
			last_type = FLOAT_T;
			break;
		case CHAR_T:
			VMobj_outopcode(PUSHSV, s, gpv);
			VMobj_outdword(1);
			last_type = CHAR_T;
			break;
		case STRING_T:
		case ARRAY_T:
		case RECORD_T:
			VMobj_outopcode(PUSHSV, s, gpv);
			VMobj_outdword(s->size);
			last_type = s->type;
			break;
		case FUNC_T:
			Lang_SimpleStatement(s);
			Parse_Undo();//HACK:cant modify SimpleStatement() so force it like this
			last_type = s->func.return_var->type;
			break;
		default:
			Sys_Error(ERR_EXPRESSION);
		}

		return;
	}

	if ( parser.token == CHARACTER )
	{
		VMobj_outbyte(PUSHS);
		VMobj_outdword(ds);
		VMobj_outdword(1);
		VMobj_outbyte(parser.value[0]);
		//VMobj_outbyte(0);
		ds += 1;

		last_type = CHAR_T;

		return;
	}

	if ( parser.token == LITERAL )
	{
		gpv = strlen(parser.value) + 1;

		VMobj_outbyte(PUSHS);
		VMobj_outdword(ds);
		VMobj_outdword(gpv);
		ds += gpv;

		gpv = 0;
		while ( parser.value[gpv] )
			VMobj_outbyte(parser.value[gpv++]);
		VMobj_outbyte(0); //force null terminated string

		last_type = STRING_T;

		return;
	}

	// default = immediate constant
	//Parse_Calculator();
	if ( push_only_addr )
		Sys_Error(ERR_NO_IDENT);

	if ( parser.token == INT && last_type == FLOAT_T ) {
		parser.token = FLOAT; //promote to float
		parser.float_value = (float)parser.int_value;
	}
	if ( parser.token != INT && parser.token != FLOAT )
		Sys_Error(ERR_EXPRESSION);
}

/*
 *  Lang_Term
 *
 *  Description: returns void
 */
void Lang_Term()
{
	Lang_NegateFactor();
	int type1 = last_type;
	int op;

	while ( 1 )
	{
		Parse_Next();
		if ( parser.token > OP_PRIORITY_2 && parser.token < OP_PRIORITY_3 )
		{
			if ( push_only_addr )
				Sys_Error(ERR_PARSE);
			op = parser.token;

			if ( op == OP_AND )
			{
				int _and1, _and2, _out;

				VMobj_outbyte(JMPF);
				_and1 = ip; ip += 4;

				Parse_Next();
				Lang_NegateFactor();
				VMobj_outbyte(JMPF);
				_and2 = ip; ip += 4;

				VMobj_outbyte(PUSHI);
				VMobj_outdword(true);
				VMobj_outbyte(JMP);
				_out = ip; ip += 4;

				VMobj_outdword2(_and1, ip-_and1);
				VMobj_outdword2(_and2, ip-_and2);
				VMobj_outbyte(PUSHI);
				VMobj_outdword(false);

				VMobj_outdword2(_out, ip-_out);
			}
			else
			{
				Parse_Next();
				Lang_NegateFactor();
				Type_Cast( op, type1, last_type );
			}
		}
		else
		{
			Parse_Undo();
			break;
		}
	}

}

/*
 *  Lang_Expression
 *
 *  Description: express.
 */
void Lang_Expression()
{
	Lang_Term();
	int type1 = last_type;
	int op;

	while ( 1 )
	{
		Parse_Next();
		if ( parser.token > OP_PRIORITY_3 && parser.token < OP_PRIORITY_4 )
		{
			if ( push_only_addr )
				Sys_Error(ERR_PARSE);
			op = parser.token;

			if ( op == OP_OR )
			{
				int _or1, _or2, _out;

				VMobj_outbyte(NOT);
				VMobj_outbyte(JMPF);
				_or1 = ip; ip += 4;

				Parse_Next();
				Lang_Term();
				VMobj_outbyte(JMPF);
				_or2 = ip; ip += 4;

				VMobj_outdword2(_or1, ip-_or1);
				VMobj_outbyte(PUSHI);
				VMobj_outdword(true);
				VMobj_outbyte(JMP);
				_out = ip; ip += 4;

				VMobj_outdword2(_or2, ip-_or2);
				VMobj_outbyte(PUSHI);
				VMobj_outdword(false);

				VMobj_outdword2(_out, ip-_out);
			} 
			else 
			{
				Parse_Next();
				Lang_Term();
				Type_Cast( op, type1, last_type );
			}
		}
		else
		{
			Parse_Undo();
			break;
		}
	}

}

/*
 *  Lang_Relation
 *
 *  Description: returns void
 */
void Lang_Relation()
{
	Lang_Expression();
	int type1 = last_type;
	int op;

	while ( 1 )
	{
		Parse_Next();
		if ( parser.token > OP_PRIORITY_4 )
		{
			op = parser.token;
			Parse_Next();
			Lang_Expression();
			Type_Cast( op, type1, last_type );
		}
		else
		{
			Parse_Undo();
			break;
		}
	}

}

/*
 *  Lang_IFstatement
 *
 *  Description: returns void 
 */
void Lang_IFstatement()
{
	short int _else, _out;

	Lang_Relation();

	Parse_Next();
	if ( !Parse_MatchKeyword(THEN) )
		Sys_Error(ERR_NO_THEN);
	Parse_Next();
	_else = ip;
	ip += 5;
	Lang_Statement();

	if ( Parse_MatchKeyword(ELSE) )
	{
		_out = ip;
		ip += 5;
		program[_else] = JMPF;
		*(int *)(program + _else + 1) = ip - _else - 1;
		Parse_Next();
		Lang_Statement();
		program[_out] = JMP;
		*(int *)(program + _out + 1) = ip - _out - 1;
	} else
	{
		program[_else] = JMPF;
		*(int *)(program + _else + 1) = ip - _else - 1;
	}
}

/*
 *  Lang_WHILEstatement
 *
 *  Description: returns void 
 */
void Lang_WHILEstatement()
{
	short int _again, _out;

	_again = ip;
	Lang_Relation();

	Parse_Next();
	if ( !Parse_MatchKeyword(DO) )
		Sys_Error(ERR_NO_DO);
	Parse_Next();

	program[ip++] = JMPF;
	_out = ip;
	ip += 4;

	Lang_Statement();

	program[ip++] = JMP;
	*(int *)(program + ip) = _again - ip;
	ip += 4;
	*(int *)(program + _out) = ip - _out;
}

/*
 *  Lang_REPEAT_UNTILstatement
 *
 *  Description: returns void 
 */
void Lang_REPEAT_UNTILstatement()
{
	int ret, loop;

	loop = ip;

	ret = REPEAT;
	while ( ret != UNTIL )
	{
		ret = Lang_Statement();
		//if ( ret == BREAK )
		//	printf("break\n");
		if ( ret == EOF )
			Sys_Error(ERR_NO_UNTIL);
	}
	//if ( !Parse_MatchKeyword(END) ) Sys_Error(ERR_NO_END);
	Parse_Next();

	Lang_Relation();
	VMobj_outbyte(JMPF);
	VMobj_outdword(loop - ip);

	Parse_Next();
}

/*
 *  Lang_FORstatement
 *
 *  Description: returns void 
 */
void Lang_FORstatement()
{
	short int _again, _out, invert, i;
	symbol_t * s;

	s = Lang_Assigment(NULL);
	if ( s->type != INT_T )
		Sys_Error(ERR_FOR_COUNTER);

	//Parse_Next();
	invert = false;
	i = Parse_GetKeyword();
	if ( i != TO )
		if ( i == DOWNTO )
			invert = true;
		else
			Sys_Error(ERR_FOR_INCOMPLETE);
	Parse_Next();

	_again = ip;
	VMobj_outopcode(PUSHIV, s, 0);

	Lang_Expression();
	if ( last_type != INT_T )
		Sys_Error(ERR_TYPE_MISMATCH, 2, INT_T, last_type);

	Parse_Next();
	if ( !Parse_MatchKeyword(DO) )
		Sys_Error(ERR_NO_DO);
	Parse_Next();

	VMobj_outbyte(CMPI);
	if ( invert ) VMobj_outbyte(GREAT_EQ); else VMobj_outbyte(LESS_EQ);
	VMobj_outbyte(JMPF);
	_out = ip;
	ip += 4;

	Lang_Statement();
	VMobj_outopcode(PUSHIV, s, 0);
	VMobj_outbyte(PUSHI);
	VMobj_outdword(1);
	if (invert ) VMobj_outbyte(SUBI); else VMobj_outbyte(ADDI);
	VMobj_outopcode(POPI, s, 0);
	VMobj_outbyte(JMP);
	*(int *)(program + ip) = _again - ip;
	ip += 4;
	*(int *)(program + _out) = ip - _out;
}

/*
 *  Lang_CASEstatement
 *
 *  Description: returns void 
 */
void Lang_CASEstatement()
{
	int next_label;
	int exit_label[64];
	int num = 0, st, len, _else;

	st = ip;
	Lang_Expression();
	len = ip - st;

	Parse_Next();
	if ( !Parse_MatchKeyword(OF) )
		Sys_Error(ERR_NO_OF);
	Parse_Next();

	while ( 1 )
	{
		if ( num ) //repush|recopy but not the first time
		{
			//FIXME: pretty barbarian but... it works
			memcpy(program + ip, program + st, len);
			ip += len;
		}

		Lang_Expression();

		Parse_Next();
		if ( parser.token != OP_COLON )
			Sys_Error(ERR_NO_COLON);
		Parse_Next();
		VMobj_outbyte(last_type == INT_T ? CMPI :
					(last_type == FLOAT_T ? CMPF : CMPS));
		VMobj_outbyte(EQUAL);
		VMobj_outbyte(JMPF);
		next_label = ip;
		ip += 4;

		Lang_Statement();

		VMobj_outbyte(JMP);
		exit_label[num++] = ip;
		ip += 4;

		*(int *)(program + next_label) = ip - next_label;

		_else = false;
		if ( Parse_MatchKeyword(END) )
			break;
		if ( Parse_MatchKeyword(ELSE)
			|| stricmp(parser.value, "OTHERWISE")==0 ) /*HACK: support 4 standard pascal*/
		{
			_else = true;
			break;
		}
	}
	if ( _else )
	{
		Parse_Next();
		Lang_Statement();
		if ( !Parse_MatchKeyword(END) ) Sys_Error(ERR_NO_END);
	}
	for ( int i = 0; i < num; i++ )
		*(int *)(program + exit_label[i]) = ip - exit_label[i];

	Parse_Next();
}

/*
 *  Lang_Assigment
 *
 *  Description: assigns stuff to other stuff
 */
symbol_t * Lang_Assigment(symbol_t * lvalue)
{
	//
	// sanity checks
	//
	if ( !lvalue )
	{
		lvalue = Type_SearchSymbol();
		if ( !lvalue )
			Sys_Error(ERR_UNKNOWN_IDENT);
	}

	//
	// Left side
	//
	int ltype = lvalue->type;
	int  link = false;
	symbol_t returned_data;
/*
	if ( ltype & POINTER ) {
		Parse_Next();
		if ( parser.token == OP_CARRET ) {		
			ltype &= 0x0F;
			lvalue->flags |= POINTER;
		} else {
			Parse_Undo();
			lvalue->flags &= FORMAL - 1;
			ltype = POINTER | POINTER_T;
			push_only_addr = true;
		}
	}
*/
	int flag_inheritence = lvalue->flags;

	if ( ltype == ARRAY_T || ltype == STRING_T)
	{
		returned_data = Comp_ArrayStatement(lvalue, link);
		lvalue = &returned_data;
		ltype = lvalue->type;
	}

	if ( ltype == RECORD_T )
	{
		returned_data = Comp_StructStatement(lvalue, link);
		lvalue = &returned_data;
		ltype = lvalue->type;
	}

	lvalue->flags = flag_inheritence; //HACK: dangerous ?
	last_type = ltype;
	//if ( ltype == POINTER )
	//	push_only_addr = true;
	Parse_Next();

	//
	// :=
	//
	if ( parser.token == OP_COLON )
	{
		if ( parser.nextchar != '=' )
			Sys_Error(ERR_PARSE);
		Parse_GetNextChar();
	}
	else
		if ( parser.token != OP_EQ )
			Sys_Error(ERR_NO_ASSIGN_OP);
	Parse_Next();

	//
	// Right side
	//
	Lang_Expression();

	switch ( ltype & 0x0F )
	{
	case INT_T:
		if ( last_type != INT_T && !(ltype & POINTER) )
			Sys_Error(ERR_TYPE_MISMATCH, 2, ltype, last_type);
		VMobj_outopcode(POPI, lvalue, link);
		break;
	case FLOAT_T:
		if ( last_type == INT_T )
			VMobj_outbyte(CAST1F);
		else if ( last_type != FLOAT_T )
			Sys_Error(ERR_TYPE_MISMATCH, 2, ltype, last_type);
		VMobj_outopcode(POPF, lvalue, link);
		break;
	case CHAR_T:
		if ( last_type != CHAR_T )
			Sys_Error(ERR_TYPE_MISMATCH, 2, ltype, last_type);
		VMobj_outopcode(POPS, lvalue, link);
		VMobj_outdword(1);
		break;
	case STRING_T:
		if ( last_type != STRING_T && last_type != CHAR_T )
			Sys_Error(ERR_TYPE_MISMATCH, 2, ltype, last_type);
	case ARRAY_T:
		if ( ltype != last_type && ltype != STRING_T )
			Sys_Error(ERR_TYPE_MISMATCH, 2, ltype, last_type);
		VMobj_outopcode(POPS, lvalue, link);
		VMobj_outdword(lvalue->size);
		break;
	case RECORD_T:
		if ( last_type != ltype )
			Sys_Error(ERR_TYPE_MISMATCH, 2, ltype, last_type);
		VMobj_outopcode(POPS, lvalue, link);
		VMobj_outdword(lvalue->size);
		break;
	default:
		Sys_Error(ERR_TYPE_MISMATCH, 2, ltype, last_type);
	}

	last_type = INT_T;
	Parse_Next();

	return lvalue;
}

/*
 *  Lang_Parse_VA_List
 *
 *  Description: helper, parser a variable argument list
 */
void Lang_Parse_VA_List(int func_nr, int& va_argc, byte * va_argv)
{
	while ( 1 )
	{
		last_type = INT_T; //sanity check
		Lang_Expression();
		va_argv[va_argc++] = (byte)last_type;
		if ( last_type >= CHAR_T )
			va_argv[va_argc++] = (byte)last_type; //HACK:

		Parse_Next();
		if ( parser.token == ')' )
			return;
		if ( parser.token != ',' )
			Sys_Error(ERR_NO_COMMA);
		Parse_Next();
	}
}

/*
 *  Lang_SimpleStatement
 *
 *  Description: simple statem. = assign | proc. call | null
 */
void Lang_SimpleStatement(symbol_t * i)
{
	//HACK: harcoded "exit"(hashcode: 269715)
	if ( hashicmp(parser.value, 269715) == 0 ) {
		VMobj_outbyte(RET);
		Parse_Next();
		return;
	}

	if ( !i )
		i = Type_SearchSymbol();
	if ( !i )
		Sys_Error(ERR_UNKNOWN_IDENT);

	if ( i->type == PROC_T || i->type == FUNC_T )
	{
		int va_funcnum, va_argc;
		byte va_argv[128];
		symbol_t * p;

		Parse_Next();

		va_funcnum = 0;
		va_argc = 0;
		if ( (i->flags & FUNC_EXTERN) == FUNC_EXTERN )
			va_funcnum = i->flags >> 2;

		if ( i->func.num_params && 
			((symbol_t*)i->func.param->root->data)->type != FUNC_VA_LIST &&
			parser.token != '(' )
			Sys_Error(ERR_NO_LEFTPARANTESIS);
		if ( parser.token == '(' )
		{
			Parse_Next();

			for ( node_s * n = i->func.param->root; n; n = n->next )
			{
				p = (symbol_t *)n->data;

				if ( p->type == FUNC_VA_LIST )
				{
					Lang_Parse_VA_List(va_funcnum, va_argc, va_argv);
					break;
				}

				if ( (p->flags & FORMAL) ) //&& !(p->type & POINTER) )
				{
					if ( !(p->type & POINTER) )
						push_only_addr = true;
					Lang_Expression();

					if ( p->type != last_type )
						Sys_Error(ERR_TYPE_MISMATCH);
					push_only_addr = false;

					va_argv[va_argc++] = (byte)last_type;
				}
				else
				{
					last_type = INT_T; //sanity check
					Lang_Expression();

					if ( p->type == FLOAT_T && last_type == INT_T ) {
						VMobj_outbyte(CAST1F);
						last_type = FLOAT_T;
					} else
						if ( p->type != last_type &&
							!(p->type == STRING_T && last_type == CHAR_T ) )
							Sys_Error(ERR_TYPE_MISMATCH, 2, p->type, last_type);

					va_argv[va_argc++] = (byte)last_type;
					if ( last_type >= CHAR_T )
						va_argv[va_argc++] = (byte)last_type;
				}

				Parse_Next();
				if ( n->next && ((symbol_t*)n->next->data)->type == FUNC_VA_LIST 
					&& parser.token == ')' )
					break;
				if ( n->next && parser.token != ',' )
					Sys_Error(ERR_NO_COMMA);
				if ( n->next )
					Parse_Next();
			}

			if ( parser.token != ')' )
				Sys_Error(ERR_NO_RIGHTPARANTESIS);
			Parse_Next();
		}

		if ( (i->flags & FUNC_EXTERN) != FUNC_EXTERN )
		{
			VMobj_outbyte(CALL);
			VMobj_outdword(i->offset);
		}
		else
		{
			VMobj_outbyte(CALL_LIB);
			VMobj_outbyte(va_funcnum);
			VMobj_outbyte(va_argc);
			for ( int i = 0; i < va_argc; i++ )
				VMobj_outbyte( va_argv[i] );
		}
		return;
	}
	
	Lang_Assigment( i );
}

/*
 *  Lang_Statement
 *
 *  Description: handles simple & struct. statements
 */
int Lang_Statement()
{
	if ( parser.token == EOF )
		return EOF;
	if ( parser.token != IDENT )
		Sys_Error(ERR_PARSE);

	int key;
	int stm;
	key = Parse_GetKeyword();
	if ( key == BEGIN )
	{
		Parse_Next();
		do
			stm = Lang_Statement();
		while ( stm != END && stm != EOF );
		if ( stm == EOF )
			Sys_Error(ERR_NO_END);
		Parse_Next();
	}
	else
	switch ( key )
	{
	//case END:
	//	return END;
	case VAR:
		Parse_Next();
		if ( !curr_proc )
			Comp_VarsDeclaration(NULL, false);
		else
			Comp_VarsDeclaration(&curr_proc->func, false);
		break;
	case TYPE:
		Parse_Next();
		if ( !curr_proc )
			Comp_TypesDeclaration(NULL);
		else
			Comp_TypesDeclaration(&curr_proc->func);
		break;
	case CONST:
		Parse_Next();
		Comp_ConstDeclaration();
		break;
	case IF:
		Parse_Next();
		Lang_IFstatement();
		break;
	case WHILE:
		Parse_Next();
		Lang_WHILEstatement();
		break;
	case REPEAT:
		Parse_Next();
		Lang_REPEAT_UNTILstatement();
		break;
	//case UNTIL:
	//	return UNTIL;
	case FOR:
		Parse_Next();
		Lang_FORstatement();
		break;
	case CASE: //:-)
		Parse_Next();
		Lang_CASEstatement();
		break;
	case END:
	case UNTIL:
	//case BREAK:
	//case CONTINUE:
		return key;
	default:
		Lang_SimpleStatement();
	}

	return parser.token;
}
