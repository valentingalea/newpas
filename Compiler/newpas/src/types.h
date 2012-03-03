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

//order is crucial...

class function_t
{
public:
	word	bp;
	word	num_params;
	word	num_vars;
	linklist_t * param;
	linklist_t * var;
	linklist_t * types;
	symbol_t * return_var;
};

class record_t
{
public:
	//char	name[STR_SIZE];
	 int	name;
	word	size;
	function_t members;
};

class array_t
{
public:
	word	size;
	word	numdim;
	signed short int	delta[3];//left bound of array
	word	dim[3];
	user_type_t * type;
};

class user_type_t
{
public:
	//char	name[STR_SIZE];
	 int	name;
	word	size;
	word	kind;
	union
	{
		record_t rec;
		array_t	array;
	};
};

class symbol_t
{
public:
	//char	name[STR_SIZE];
	 int	name;
	word	offset;
	word	size;
	/*
	Flags:
	- for variables: { global | local | by ref }
	- for subroutines: bit0=forwarded bit1=external bit2-16=builtin func nr.
	*/
	word	flags;
	word	type;
	union
	{
		function_t	func;
		user_type_t	* user;
	};
};

enum types
{
	INT_T,
	POINTER_T = INT_T,
	FLOAT_T,
	CHAR_T,
	STRING_T,
	ARRAY_T,
	RECORD_T,
	PROC_T,
	FUNC_T
};

byte SizeOf[8] =
{
	4, //int = integer
	4, //float = real
	1, //char
	32,//default string size;
	0, //array (variable sized)
	0, //record (varible sized)
	4, //procedure
	4, //function
};

/*
 *	Constants
 */
constant_t* global_const = NULL;
constant_t* local_const = NULL;

/*
 *  Type_AddConstant
 *
 *  Description: returns void 
 */
void Type_AddConstant(constant_t** root, int name, int type)
{
	constant_t* temp;

	Z_malloc(temp, constant_t);
	//temp = new constant_t;

	temp->name = name;
	temp->type = type;

	switch ( type ) {
	case INT:
		temp->value.integer = parser.int_value;
		break;
	case FLOAT:
		temp->value.real = parser.float_value;
		break;
	case CHARACTER:
	case LITERAL:
#undef Z_malloc
		temp->value.string = (char*)Z_malloc(strlen(parser.value)+1, __FILE__, __LINE__);
#define Z_malloc(s, t) s = (t *)Z_malloc(sizeof(t), __FILE__, __LINE__)
		strcpy(temp->value.string, parser.value);
		break;
	default:
		Sys_Error(ERR_PARSE);
	}

	constant_t *i, *j;

	if ( !(*root) ) {
		temp->next = NULL;
		(*root) = temp;
	} else {
		i = (*root);
		while ( i )
		{
			j = i;
			i = i->next;
			if ( j->name == name )
				Sys_Error(ERR_DUPLIC_IDENT);
		}
		temp->next = NULL;
		j->next = temp;
	}
}

/*
 *  Type_SearchConstant
 *
 *  Description: returns constant_t * 
 */
constant_t * Type_SearchConstant()
{
	constant_t *i;

	int code = hashcode(parser.value);

	for ( i = local_const; i; i = i->next )
		if ( i->name == code )
			return i;
	for ( i = global_const; i; i = i->next )
		if ( i->name == code )
			return i;
	return NULL;

}

/*
 *  Type_RemoveConstants
 *
 *  Description: returns void 
 */
void Type_RemoveConstants(constant_t** root)
{
	constant_t *i, *temp;

	i = (*root);
	while ( i ) {
		temp = i;
		i = i->next;
		//Z_free(temp);
	}

	(*root) = NULL;
}

/*
**	standard Linklists
*/
struct node_s
{
	void			* data;
	struct node_s	* next;
	struct node_s	* prev;
};

class linklist_t
{
private:
	// iterator
	node_s * i;
public:
	// 2 sentinels
	node_s * root;
	node_s * last;

	// addition
	void add( void * ptr, bool check_duplic = true)
	{
		node_s * temp;

		//temp = new node_s;
		Z_malloc(temp, node_s);
		temp->data = ptr;
		temp->prev = last;
		temp->next = NULL;

		if ( !root )
			root = last = temp;
		else
		{
			if ( check_duplic )
			{
				node_s * i = root;
				while ( i )
				{
					//name is the first field in every struct so the general cast works
					if ( hashicmp(((symbol_t*)i->data)->name, 
						((symbol_t*)temp->data)->name) == 0 )
						Sys_Error(ERR_DUPLIC_IDENT);
					i = i->next;
				}
			}
			last->next = temp;
			last = last->next;
		}
	}
	//clear
	void clear( void )
	{
		node_s * temp;

		i = root;
		while ( i )
		{
			temp = i;
			i = i->next;
			//printf("hash = %i\n", ((symbol_t*)(temp->data))->name);
			// do nothing, Z_close will take care !
			//Z_free(temp);
		}
		root = last = NULL;
	}

	// default constructor
	linklist_t( void )
	{
		root = last = NULL;
	}
	// default destructor
	~linklist_t( void )
	{
		if ( root )
			clear();
	}
};

/*
 *	Variable related
 */
linklist_t * global_var;
symbol_t * curr_proc = NULL;
int proc_id_counter = 0;
int	proc_names_counter = 0;

/*
 *  Type_GetSymbol
 *
 *  Description: helper
 */
symbol_t * Type_GetSymbol(linklist_t * start, char * name)
{
	node_s * i;
	symbol_t * s;

	int code = hashcode(name);

	for ( i = start->root; i; i = i->next )//TODO:go down backwards
	{
		s = (symbol_t *)i->data;
		if ( code == s->name )
			return s;
	}
	return NULL;
}

/*
 *  Type_SearchSymbol
 *
 *  Description: searches for a symbol in all posible sources
 */
symbol_t * Type_SearchSymbol()
{
	symbol_t * s = NULL;

	if ( curr_proc )
	{
		if ( curr_proc->func.return_var )
			if ( hashicmp(parser.value, curr_proc->func.return_var->name) == 0 )
			{
				Parse_Next();
				if ( parser.token == '(' )
					s = curr_proc;
				else
					s = curr_proc->func.return_var;
				Parse_Undo();
				return s;
			}
		if ( !s )
			s = Type_GetSymbol(curr_proc->func.param, parser.value);
		if ( !s )
			s = Type_GetSymbol(curr_proc->func.var, parser.value);
	}
	if ( !s ) //no local match
		s = Type_GetSymbol(global_var, parser.value);

	return s;
}

/*
 *	Type related
 */
linklist_t * global_type;

/*
 *  Type_Init
 *
 *  Description: inits the linklist & stuff
 */
void Type_Init()
{
	global_var = new linklist_t();
	global_type = new linklist_t();
}

/*
 *  Type_CleanUp
 *
 *  Description: removes stuff
 */
void Type_CleanUp()
{
	delete global_var;
	delete global_type;
}

/*
 *  Parse_Loop
 *
 *  Description: the main parser loop used globaly or localy
 */
void Parse_Loop( int type )
{
	function_t * func;
	int is_param, key, last;

	switch ( type )
	{
	case PARAM_SPACE:
		func = &curr_proc->func;
		is_param = LOCAL;
		break;
	case LOCAL_SPACE:
		func = &curr_proc->func;
		is_param = false;
		break;
	case GLOBAL_SPACE:
	default:
		func = NULL;
		is_param = false;
	}

	last = 2002;
	while ( parser.token != EOF )
	{
		key = Parse_GetKeyword();
		if ( key == -1 )
			key = last;
		else
			Parse_Next();

		switch ( key )
		{
		case VAR:
			Comp_VarsDeclaration(func, is_param);
			last = VAR;
			break;
		case TYPE:
			Comp_TypesDeclaration(func);
			last = TYPE;
			break;
		case CONST:
			Comp_ConstDeclaration();
			last = CONST;
			break;
		case PROCEDURE:
			Comp_SubroutineDeclaration(false);
			break;
		case FUNCTION:
			Comp_SubroutineDeclaration(true);
			break;
		case OPERATOR:
			Comp_OverloadOperator();
			break;
		case BEGIN:
			if ( type == LOCAL_SPACE )
			{
				Parse_Undo();
				return;
			}
		case USES:
			if ( type == GLOBAL_SPACE )
				Parse_IncludeFile();
			else
				Sys_Error(ERR_PARSE);
			break;
		default:
			Sys_Error(ERR_PARSE);
		}
	}
}

/*
 *  Parse_GetType
 *
 *  Description: type parser & matcher
 */
user_type_t * Parse_GetType(linklist_t * local)
{
	static user_type_t base_integer = { hashcode("integer"), 4, INT_T, NULL };
	static user_type_t base_real = { hashcode("real"), 4, FLOAT_T, NULL };
	static user_type_t base_char = { hashcode("char"), 1, CHAR_T, NULL };
	
	user_type_t * base_string;//array alias...
	user_type_t * t;

	int mask = 0;/*
	if ( parser.token == OP_CARRET ) {
		mask = POINTER;
		Parse_Next();
	}*/

	if ( Parse_MatchKeyword(INTEGER) ) {
		user_type_t * integer;
		Z_malloc(integer, user_type_t);
		memcpy(integer, &base_integer, sizeof(user_type_t));
		integer->kind |= mask;
		return integer;
	}

	if ( Parse_MatchKeyword(REAL) ) {
		user_type_t * real;
		Z_malloc(real, user_type_t);
		memcpy(real, &base_real, sizeof(user_type_t));
		real->kind |= mask;
		return real;
	}

	if ( Parse_MatchKeyword(CHAR) ) {
		user_type_t * _char;
		Z_malloc(_char, user_type_t);
		memcpy(_char, &base_char, sizeof(user_type_t));
		_char->kind |= mask;
		return _char;
	}

	if ( Parse_MatchKeyword(STRING) )
	{
		Z_malloc(base_string, user_type_t);
		hashcpy(&base_string->name, "string");
		base_string->kind = mask | STRING_T;		
		base_string->array.numdim = 1;

		int size;

		Parse_Next();
		if ( parser.token == '[' )
		{
			calc_allow_float = false;
			Parse_Next();
			Parse_Calculator();
			if ( parser.token != INT )
				Sys_Error(ERR_NO_INT);
			calc_allow_float = true;

			size = parser.int_value;//atoi(parser.value);
			if ( size <= 0)
				size = SizeOf[STRING_T];
			if ( size == 1)
				size = 2;

			Parse_Next();
			if ( parser.token != ']' )
				Sys_Error(ERR_NO_RIGHTBRAQUET);
			//Parse_Next();			
		}
		else
		{
			size = SizeOf[STRING_T];
			Parse_Undo();
		}

		base_string->array.dim[0] = size;
		base_string->array.delta[0] = 0;
		base_string->array.size = size;
		base_string->array.type = &base_char;
		base_string->size = base_string->array.size;
		return base_string;
	}

	if ( Parse_MatchKeyword(ARRAY) )
	{
		Z_malloc(t, user_type_t);
		//t->name[0] = 0;
		t->kind = mask | ARRAY_T;
		Comp_ArrayDeclaration(t->array);
		t->size = t->array.size;

		if ( t->array.type->kind == CHAR_T && t->array.numdim == 1 )
			t->kind = STRING_T;

		return t;
	}

	if ( Parse_MatchKeyword(RECORD) )
	{
		Z_malloc(t, user_type_t);
		//t->name[0] = 0;
		t->kind = mask | RECORD_T;
		//t->rec.name[0] = 0;
		t->rec.size = 0;
		t->rec.members.bp = 0;
		t->rec.members.num_vars = 0;
		Z_malloc(t->rec.members.var, linklist_t);
		t->rec.members.var->root = t->rec.members.var->last = NULL;
		Comp_StructDeclaration(t->rec);
		t->size = t->rec.size;

		return t;
	}

	if ( parser.token == '(' )
	{
		int value = -1;
		while ( 1 )
		{
			Parse_Next();

			int name = Parse_GetIdentifier();
			parser.int_value = ++value;

			Parse_Next();
			if ( parser.token == OP_EQ )
			{
				Parse_Next();
				if ( parser.token != INT )
					Sys_Error(ERR_NO_INT);
				value = parser.int_value;
				Parse_Next();
			}
			if ( parser.token != ',' && parser.token != ')' )
				Sys_Error(ERR_NO_COMMA);

			Type_AddConstant(
				( curr_proc ) ? &local_const : &global_const,
				name, INT);

			if ( parser.token == ')' )
				break;
		}

		user_type_t * enum_t;
		Z_malloc(enum_t, user_type_t);
		memcpy(enum_t, &base_integer, sizeof(user_type_t));
		return enum_t;
	}

	//if ( Parse_MatchKeyword(PROCEDURE) )
	//	return &base_procedure;

	//aliases
	node_s * i;
	
	if ( local )
	for ( i = local->root; i; i = i->next )
		if ( hashicmp(parser.value, ((user_type_t *)i->data)->name) == 0 )
		{
			Z_malloc(t, user_type_t);
			memcpy(t, (user_type_t *)i->data, sizeof(user_type_t));
			t->kind |= mask;
			return t;
		}
		
	for ( i = global_type->root; i; i = i->next )
		if ( hashicmp(parser.value, ((user_type_t *)i->data)->name) == 0 )
		{
			Z_malloc(t, user_type_t);
			memcpy(t, (user_type_t *)i->data, sizeof(user_type_t));
			t->kind |= mask;
			return t;
		}
	
	return NULL;
}
