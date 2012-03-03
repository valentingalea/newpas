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
 *  Comp_VarsDeclaration
 *
 *  Description: !
 */
void Comp_VarsDeclaration(function_t * func, int param, int is_member_of_struct)
{
	symbol_t * temp_symb;
	node_s * mark = NULL;
	linklist_t * root;

	while ( 1 )
	{
		Z_malloc(temp_symb, symbol_t);
		hashcpy(&temp_symb->name, Parse_GetIdentifier());

		temp_symb->flags = 0;
		if ( !func ) {
			temp_symb->flags = GLOBAL;
			root = global_var;
		} else
			if ( param )
			{
					temp_symb->flags |= LOCAL;
				if ( param == FORMAL )
					temp_symb->flags |= FORMAL;
				root = func->param;
				func->num_params ++;
			}
			else
			{
				if ( is_member_of_struct )
					temp_symb->flags = 0;
				else
					temp_symb->flags |= LOCAL;
				root = func->var;
				func->num_vars ++;
			}
		root->add ( (symbol_t *)temp_symb );
		if ( !mark ) //start marker
			mark = root->last;

		Parse_Next();
		if ( parser.token == OP_COLON )
		{
			Parse_Next();
			//if ( parser.token != IDENT )
			//	Sys_Error(ERR_PARSE);
			break;
		}
		if ( parser.token != ',' )
			Sys_Error(ERR_NO_COLON);
		Parse_Next();
	}

	user_type_t * t;
	if ( func )
		t = Parse_GetType(func->types);
	else
		t = Parse_GetType(NULL);
	if ( !t )
		Sys_Error(ERR_UNKNOWN_TYPE);

	node_s * i;
	symbol_t * s;
	for ( i = mark; i; i = i->next )
	{
		s = (symbol_t *)i->data;

		s->type = t->kind;
		s->user = t;
		if ( (s->flags & FORMAL) || (s->type & POINTER) )
			s->size = 4;
		else
			s->size = t->size;

		if ( !func )
		{
			s->offset = ds;
			ds += s->size;
		}
		else
		{
			s->offset = func->bp;
			//if ( (s->flags & FORMAL) == FORMAL )
			//	func->bp += 4;
			//else
				func->bp += s->size;
		}
	}

	Parse_Next();
}

/*
 *  Comp_TypesDeclaration
 *
 *  Description: !
 */
void Comp_TypesDeclaration(function_t * func)
{
	//char name[MAX_IDENT_SIZE];

	int name = Parse_GetIdentifier();

	Parse_Next();
	if ( parser.token != OP_EQ )
		Sys_Error(ERR_NO_EQ_OP);
	Parse_Next();

	user_type_t * t, *n;
	if ( func )
		t = Parse_GetType(func->types);
	else
		t = Parse_GetType(NULL);
	if ( !t )
		Sys_Error(ERR_UNKNOWN_TYPE);
/*
	if ( t->kind < STRING_T )
	{
		Z_malloc(n, user_type_t);
		//hashcpy(&n->name, name);
		n->name = name;
		n->kind = t->kind;
		n->size = t->size;
		memset(&n->rec, 0, sizeof(record_t));
	}
	else
	{
		n = t;
		//hashcpy(&n->name, name);
		n->name = name;
	}
*/
	n = t;
	n->name = name;

	if ( func )
		func->types->add((user_type_t *)n);
	else
		global_type->add((user_type_t *)n);

	Parse_Next();
}

/*
 *  Comp_ConstDeclaration
 *
 *  Description: returns void 
 */
void Comp_ConstDeclaration(void)
{
	int name = Parse_GetIdentifier();

	Parse_Next();
	if ( parser.token != OP_EQ )
		Sys_Error(ERR_NO_EQ_OP);

	Parse_Next();
	//Parse_Calculator();

	if ( curr_proc )
		Type_AddConstant(&local_const, name, parser.token);
	else
		Type_AddConstant(&global_const, name, parser.token);

	Parse_Next();
}

/*
 *  Comp_StructDeclaration
 *
 *  Description: returns void 
 */
void Comp_StructDeclaration(record_t &data)
{
	record_t * rec = &data;

	Parse_Next();
	//sanity check
	rec->members.bp = 0;
	rec->members.num_vars = 0;
	rec->members.types = NULL;

	while ( !Parse_MatchKeyword(END) )
	{
		Comp_VarsDeclaration(&rec->members, false, true);
	}

	rec->size = rec->members.bp;
}

/*
 *  Comp_StructStatement
 *
 *  Description: returns symbol_t
 */
symbol_t Comp_StructStatement(symbol_t *rec, int &flag)
{
	node_s * root = rec->user->rec.members.var->root;
	node_s * field;
	symbol_t * s;
	symbol_t field_data;
	/*static*/ symbol_t return_data;

	Parse_Next();
	if ( parser.token == OP_DOT )
	{
		Parse_Next();
		field = root;
		while ( 1 )
		{
			s = (symbol_t*)field->data;
			if ( hashicmp(s->name, Parse_GetIdentifier()) == 0 )
				break;
			field = field->next;
			if ( !field )
				Sys_Error(ERR_REC_FIELD);
		}

		//get a local copy for safety
		memcpy(&field_data, s, sizeof(symbol_t));
		s = &field_data;

		s->offset += rec->offset;
/*
		if ( s->type & POINTER ) {
			Parse_Next();
			if ( parser.token == OP_CARRET ) {
				s->flags |= FORMAL;
			} else {
				Parse_Undo();
				s->flags &= FORMAL - 1;
				s->type = POINTER | POINTER_T;
				return *s;
			}
		}
*/
		if ( s->type == ARRAY_T || s->type == STRING_T )
		{
			return_data = Comp_ArrayStatement(s, flag);
			s = &return_data;
		}

		if ( s->type == RECORD_T )
		{
			return_data = Comp_StructStatement(s, flag);
			s = &return_data;
		}

		{
			memcpy(&return_data, s, sizeof(symbol_t));
			return return_data;
		}
	}
	else
	{
		Parse_Undo();
		return *rec;
	}
}

/*
 *  Comp_ArrayDeclaration
 *
 *  Description: returns array_t *
 */
void Comp_ArrayDeclaration(array_t &data)
{
	int l1, l2, i;
	array_t * a;

	Parse_Next();
	if ( parser.token != '[' )
		Sys_Error(ERR_NO_LEFTBRAQUET);

	a = &data;//by ref. so it can be fucked further more
	a->numdim = 0;
	a->dim[0] = a->dim[1] = a->dim[2] = 0;
	a->delta[0] = a->delta[1] = a->delta[2] = 0;

	while ( parser.token != ']' )
	{
		calc_allow_float = false;
		Parse_Next();
		Parse_Calculator();
		if ( parser.token >= CHAR_T )
			Sys_Error(ERR_ARRAY_INDEX);
		l1 = parser.int_value;

		Parse_Next();
		if ( parser.token != OP_DOTS )
			Sys_Error(ERR_NO_DOTS);

		Parse_Next();
		Parse_Calculator();
		if ( parser.token >= CHAR_T )
			Sys_Error(ERR_ARRAY_INDEX);
		calc_allow_float = true;
		l2 = parser.int_value;
		if ( l2 <= l1 )
			Sys_Error(ERR_ARRAY_BOUNDS);

		a->dim[a->numdim] = l2 - l1 + 1;
		a->delta[a->numdim] = l1;
		a->numdim++;

		Parse_Next();
		if ( parser.token != ',' && parser.token != ']' )
			Sys_Error(ERR_NO_RIGHTBRAQUET);
		if ( a->numdim > 3 )
			Sys_Error(ERR_ARRAY_DIM);
	}

	Parse_Next();
	if ( !Parse_MatchKeyword(OF) )
		Sys_Error(ERR_NO_OF);
	Parse_Next();

	if ( curr_proc )
		a->type = Parse_GetType(curr_proc->func.types);
	else
		a->type = Parse_GetType(NULL);
	if ( !a->type )
		Sys_Error(ERR_UNKNOWN_TYPE);

	if ( a->type->kind == ARRAY_T )
	{
		user_type_t * t = a->type;
		for ( i = 0; i < t->array.numdim; i++ )
			a->dim[a->numdim+i] = t->array.dim[i];
		a->numdim += i;
		if ( a->numdim > 3 )
			Sys_Error(ERR_ARRAY_DIM);
		a->type = t->array.type;
	}

	a->size = 1;
	for ( i = 0; i < a->numdim; i++ )
		a->size *= a->dim[i];
	a->size *= a->type->size;

}

/*
 *  Comp_ArrayStatement
 *
 *  Description: returns symbol_t
 */
symbol_t Comp_ArrayStatement(symbol_t * s, int &flag)
{
	int i;
	array_t * a = &s->user->array;//get a pointer (it's more readable this way:)
	/*static*/ symbol_t elem;

	Parse_Next();
	if ( parser.token != '[' )
	{
		Parse_Undo();
		return *s;
	}
	Parse_Next();

	//TODO: somehow allow sub-index refs.
	for ( i = 0; i < a->numdim; i++ )
	{
		Lang_Expression();
		if ( last_type != INT_T )
			Sys_Error(ERR_ARRAY_INDEX);

		Parse_Next();

		if ( i < a->numdim-1 )
			if ( parser.token == ',' )
				Parse_Next();
			else
				Sys_Error(ERR_NO_COMMA);
	}
	if ( parser.token != ']' )
		Sys_Error(ERR_NO_RIGHTBRAQUET);

	VMobj_outbyte(INDEX);
	VMobj_outbyte(flag);//link 2 other arrays;
	VMobj_outbyte((byte)a->numdim);
	VMobj_outdword(a->type->size);
	
		VMobj_outdword( a->dim[0] );
		VMobj_outdword( a->delta[0] );
	if ( a->numdim >= 2 ) {
		VMobj_outdword( a->dim[1] );
		VMobj_outdword( a->delta[1] );
	}
	if ( a->numdim >= 3 ) {
		VMobj_outdword( a->dim[2] ); //never need size z
		VMobj_outdword( a->delta[2] );
	}

	//array element
	memset(&elem, 0, sizeof(symbol_t));
	elem.offset = s->offset;
	elem.flags = s->flags;
	elem.size = a->type->size;
	elem.type = a->type->kind;
	elem.user = a->type;
/*
	if ( elem.type & POINTER ) {
		Parse_Next();
		if ( parser.token == OP_CARRET ) {
			elem.flags |= FORMAL;
		} else {
			Parse_Undo();
			elem.flags &= FORMAL - 1;
			elem.type = POINTER | POINTER_T;
		}
	}
*/
	flag = true;
	return elem;
}

/*
 *  Comp_SubParamReverseIterate
 *
 *  Description: traverses arguments C style (right to left)
 */
void Comp_SubParamReverseIterate(linklist_t * param)
{
	node_s * i;
	symbol_t * s;

	for ( i = param->last; i; i = i->prev )
	{
		s = (symbol_t*)i->data;

		if ( (s->flags & FORMAL) ) //|| (s->type & POINTER) )
		{
			VMobj_outbyte(LOCAL | POPA);
			VMobj_outdword(s->offset);
			VMobj_outbyte(0);
			continue;
		}

		switch ( s->type )
		{
		case INT_T:
			VMobj_outopcode(POPI, s, 0);
			break;
		case FLOAT_T:
			VMobj_outopcode(POPF, s, 0);
			break;
		case CHAR_T:
			VMobj_outopcode(POPS, s, 0);
			VMobj_outdword(1);
			break;
		default:
			VMobj_outopcode(POPS, s, 0);
			VMobj_outdword(s->size);
		}
	}
}

/*
 *  Comp_SubroutineDeclaration
 *
 *  Description: returns void
 */
void Comp_SubroutineDeclaration(int is_func)
{
	symbol_t * head1;
	symbol_t head2;
	int ret, old_zcount;

	//
	// Check basic stuff (name, is proc or func, etc)
	//
	head1 = Type_SearchSymbol();
	if ( !head1 ) //new declaration
	{
		Z_malloc(head1, symbol_t);
		hashcpy(&head1->name, Parse_GetIdentifier());

		strcpy((vm_obj.proc_names + proc_names_counter), parser.value);
		proc_names_counter += strlen(parser.value);
		vm_obj.proc_names[proc_names_counter++] = 0;

		head1->type = ( is_func ) ? FUNC_T : PROC_T;
		head1->flags = 0;
		head1->offset = proc_id_counter ++;
		memset(&head1->func, 0, sizeof(function_t));
		head1->user = NULL;
		//if ( curr_proc ) //a local sub. 
		//	curr_proc->func.var->add((symbol_t *)head1);
		//else // a global sub
			global_var->add((symbol_t *)head1);
		curr_proc = head1;
	}
	else //redeclaration
	{
		head1->flags |= 1; //allready has a header
		head2.type = ( is_func ) ? FUNC_T : PROC_T;
		head2.flags = 0;
		memset(&head2.func, 0, sizeof(function_t));
		curr_proc = &head2;
	}

	//
	// Parse the argument list
	//
	curr_proc->func.param = new linklist_t();
	curr_proc->func.var = new linklist_t();
	curr_proc->func.types = new linklist_t();
	local_const = NULL;

	Parse_Next();
	if ( parser.token == '(' )
	{
		Parse_Next();
		while ( parser.token != ')' )
		{
			if ( parser.token == OP_DOTS )
			{
				symbol_t * va_list; //variable argument list start
				Z_malloc(va_list, symbol_t);
				memset(va_list, 0, sizeof(symbol_t));

				va_list->type = FUNC_VA_LIST;
				curr_proc->flags |= FUNC_EXTERN;

				curr_proc->func.param->add ((symbol_t *)va_list);
				curr_proc->func.num_params ++;
				Parse_Next();
				break;
			}

			if ( Parse_MatchKeyword(VAR) )
			{
				Parse_Next();
				Comp_VarsDeclaration(&curr_proc->func, FORMAL);
			}
			else
				Comp_VarsDeclaration(&curr_proc->func, LOCAL);
		}
		if ( parser.token != ')' )
			Sys_Error(ERR_NO_RIGHTPARANTESIS);
		Parse_Next();
	}

	if ( is_func )
	{
		if ( parser.token != OP_COLON )
			Sys_Error(ERR_NO_COLON);
		Parse_Next();
		Z_malloc(curr_proc->func.return_var, symbol_t);
		curr_proc->func.var->add((symbol_t*)curr_proc->func.return_var);

		user_type_t * type;
		type = Parse_GetType(NULL);
		if ( !type )
			Sys_Error(ERR_UNKNOWN_TYPE);

		curr_proc->func.return_var->user = type;
		curr_proc->func.return_var->type = type->kind;
		curr_proc->func.return_var->size = type->size;
		curr_proc->func.return_var->flags = LOCAL;
		curr_proc->func.return_var->offset = curr_proc->func.bp;
		curr_proc->func.bp += curr_proc->func.return_var->size;
		curr_proc->func.return_var->name = curr_proc->name;

		Parse_Next();
	}
	else
		curr_proc->func.return_var = NULL;

	old_zcount = zcount; //HACK:
	Sys_Warn("Saved mem pointer before entering function...");

	//
	// Check directives (forward & extern) 
	//
	ret = Parse_GetKeyword();
	if ( ret == FORWARD )
	{
		// just acknowledge & check for 2nd forward decl.
		if ( head1->flags & FUNC_EXTERN )
			Sys_Error(ERR_EXTERN_BODY); //TODO: more explicit
		if ( head1->flags & 1)
			Sys_Error(ERR_HEAD2BODY_TWICE);
		Parse_Next();
		curr_proc = NULL;
		return;
	}

	if ( ret == EXTERNAL )
	{
		//TODO: paranoia: check 4 2nd 'external'
		head1->flags |= FUNC_EXTERN;
		Parse_Next();
		if ( parser.token != INT )
			Sys_Error(ERR_NO_INT);
		head1->flags |= parser.int_value << 2;
		Parse_Next();
		curr_proc = NULL;
		return;
	}

	if ( ret == PROCEDURE || ret == FUNCTION )
		Sys_Error(ERR_NO_BEGIN);

	if ( (curr_proc->flags & FUNC_EXTERN ) == FUNC_EXTERN )
		Sys_Error(ERR_EXTERN_BODY);

	//
	// Parse decl. bettwen header & body (Pascal specific: allow local subs)
	//
	curr_proc->func.num_vars = 0;
	Parse_Loop(LOCAL_SPACE);

	//curr_proc = &head2;

	//
	// Parse the subroutine body
	//
	if ( Parse_MatchKeyword(BEGIN) || ret == BEGIN )
	{
		// match with the prototype
		if ( curr_proc != head1 )
		{
			if ( head1->func.num_params != curr_proc->func.num_params )
				Sys_Error(ERR_HEAD2BODY_NUMPARAMS);
			if ( head1->type != curr_proc->type )
				Sys_Error(ERR_TYPE_MISMATCH);
			if ( head1->type == FUNC_T )
				if ( head1->func.return_var->type != curr_proc->func.return_var->type )
					Sys_Error(ERR_TYPE_MISMATCH);

			node_s * n1, * n2;
			for (	n1 = head1->func.param->root, n2 = curr_proc->func.param->root;
					n1;
					n1 = n1->next, n2 = n2->next )
				if ( ((symbol_t*)n1->data)->type != ((symbol_t*)n2->data)->type 
					|| ((symbol_t*)n1->data)->flags != ((symbol_t*)n2->data)->flags )
					Sys_Error(ERR_HEAD2BODY_MISMATCH);

			curr_proc = head1; //the proto header rules :)
		}
		//	this is the proc. body ... do the usual stuff
		
		vm_obj.proc_index[curr_proc->offset] = ip;

		if ( curr_proc->func.num_params )
			Comp_SubParamReverseIterate(curr_proc->func.param);

		Parse_Next();
		ret = BEGIN;
		while ( ret != END )
		{
			ret = Lang_Statement();
			if ( ret == EOF )
				Sys_Error(ERR_NO_END);
		}

		vm_obj.proc_sizes[curr_proc->offset] = curr_proc->func.bp;
		{
			symbol_t * r = curr_proc->func.return_var;
			if ( r ) {
				switch ( r->type )
				{
				case INT_T:
					VMobj_outopcode(PUSHIV, r, 0);
					break;
				case FLOAT_T:
					VMobj_outopcode(PUSHFV, r, 0);
					break;
				case CHAR_T:
				case STRING_T:
				case ARRAY_T:
				case RECORD_T:
					VMobj_outopcode(PUSHSV, r, 0);
					VMobj_outdword(r->size);
					break;
				default: ;
				}
				last_type = r->type;
				VMobj_outbyte(RET);
			} else {
				VMobj_outbyte(RET);
			}
		}

		//we need curr_proc->func.param so don't remove it
		delete curr_proc->func.var;
		delete curr_proc->func.types;
		curr_proc->func.var = NULL;
		curr_proc->func.types = NULL;
		Type_RemoveConstants(&local_const);
		curr_proc = NULL;
		zcount = old_zcount; //HACK:
		Sys_Warn("...restored mem pointer after function end");

		Parse_Next();
		return;
	}

	Sys_Error(ERR_NO_BEGIN);
}

/*
 *  Comp_OverloadOperator
 *
 *  Description: ops. overload
 */
void Comp_OverloadOperator( void )
{
	int i, old_zcount, ret;
	//symbol_t * dummy;
	//Z_malloc(dummy, symbol_t);
	symbol_t dummy;

	curr_proc = &dummy;
	memset(curr_proc, 0, sizeof(symbol_t));

	i = op_ovrld_count; //don't inc. now, it's crucial
	op_ovrld_table[i].token = parser.token;
	op_ovrld_table[i].user_func = proc_id_counter;
	memset(op_ovrld_table[i].vm_instructions, 0, 4);
	curr_proc->offset = proc_id_counter++;
	{
		char str[64];
		sprintf(str, "operator<%i>\0", parser.token);
		strcpy((vm_obj.proc_names + proc_names_counter), str);
		proc_names_counter += strlen(str);
		vm_obj.proc_names[proc_names_counter++] = 0;
	}

	Parse_Next();
	if ( parser.token != '(' )
		Sys_Error(ERR_NO_LEFTPARANTESIS);

	curr_proc->func.param = new linklist_t();
	curr_proc->func.var = new linklist_t();
	curr_proc->func.types = new linklist_t();
	local_const = NULL;
	old_zcount = zcount; //HACK:

	Parse_Next();
	while ( parser.token != ')' )
	{
		Comp_VarsDeclaration(&curr_proc->func, LOCAL);
	}
	if ( parser.token != ')' )
		Sys_Error(ERR_NO_RIGHTPARANTESIS);

	if ( curr_proc->func.num_params != 2 )
		Sys_Error(ERR_OPERATOR_PARAM);
	op_ovrld_table[i].operand1 = (byte)((symbol_t*)
		(curr_proc->func.param->root->data))->type;
	op_ovrld_table[i].operand2 = (byte)((symbol_t*)
		(curr_proc->func.param->root->next->data))->type;

	Parse_Next();
	if ( parser.token == OP_EQ )
		Parse_Next();

	Z_malloc(curr_proc->func.return_var, symbol_t);
	//sprintf(curr_proc->func.return_var->name, "%s\0", Parse_GetIdentifier());
	hashcpy(&curr_proc->func.return_var->name, Parse_GetIdentifier());
	curr_proc->func.var->add((symbol_t*)curr_proc->func.return_var);

	Parse_Next();
	if ( parser.token != OP_COLON )
		Sys_Error(ERR_NO_COLON);
	Parse_Next();

	user_type_t * type;
	type = Parse_GetType(NULL);
	if ( !type )
		Sys_Error(ERR_UNKNOWN_TYPE);

	curr_proc->func.return_var->user = type;
	curr_proc->func.return_var->type = type->kind;
	curr_proc->func.return_var->size = type->size;
	curr_proc->func.return_var->flags = LOCAL;
	curr_proc->func.return_var->offset = curr_proc->func.bp;
	curr_proc->func.bp += curr_proc->func.return_var->size;
	op_ovrld_table[i].return_type = (byte)curr_proc->func.return_var->type;

	Parse_Next();

	if ( Parse_MatchKeyword(PROCEDURE) || Parse_MatchKeyword(FUNCTION) )
		Sys_Error(ERR_NO_BEGIN);

	Parse_Loop(LOCAL_SPACE);

	if ( !Parse_MatchKeyword(BEGIN) )
		Sys_Error(ERR_NO_BEGIN);

	vm_obj.proc_index[curr_proc->offset] = ip;

	Comp_SubParamReverseIterate(curr_proc->func.param);

	Parse_Next();
	ret = BEGIN;
	while ( ret != END )
	{
		ret = Lang_Statement();
		if ( ret == EOF )
			Sys_Error(ERR_NO_END);
	}
	//TODO: remove ?
	if ( !Parse_MatchKeyword(END) ) Sys_Error(ERR_NO_END);

	vm_obj.proc_sizes[curr_proc->offset] = curr_proc->func.bp;

	{
		symbol_t * r = curr_proc->func.return_var;
		switch ( r->type )
		{
		case INT_T:
			VMobj_outopcode(PUSHIV, r, 0);
			break;
		case FLOAT_T:
			VMobj_outopcode(PUSHFV, r, 0);
			break;
		case CHAR_T:
		case STRING_T:
		case ARRAY_T:
		case RECORD_T:
			VMobj_outopcode(PUSHSV, r, 0);
			VMobj_outdword(r->size);
			break;
		default: ;
		}
		last_type = r->type;
		VMobj_outbyte(RET);
	}

	delete curr_proc->func.param;
	delete curr_proc->func.var;
	delete curr_proc->func.types;
	Type_RemoveConstants(&local_const);
	curr_proc = NULL;
	zcount = old_zcount; //HACK:

	op_ovrld_count ++;

	Parse_Next();
}
