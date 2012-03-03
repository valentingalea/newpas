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
 *	ERROR_CODES
 */
enum error_codes
{
	ERR_0,
	ERR_FILE,
	ERR_BAD_IDENT,
	ERR_PARSE,
	ERR_NO_IDENT,
	ERR_KEY_AS_IDENT,
	ERR_UNKNOWN_IDENT,
	ERR_DUPLIC_IDENT,
	ERR_BAD_STRING,
	ERR_NO_BEGIN,
	ERR_NO_END,
	ERR_NO_THEN,
	ERR_NO_DO,
	ERR_NO_UNTIL,
	ERR_NO_COLON,
	ERR_NO_LEFTPARANTESIS,
	ERR_NO_RIGHTPARANTESIS,
	ERR_NO_ASSIGN_OP,
	ERR_NO_LEFTBRAQUET,
	ERR_NO_RIGHTBRAQUET,
	ERR_NO_DOTS,
	ERR_NO_OF,
	ERR_NO_INT,
	ERR_NO_COMMA,
	ERR_NO_EQ_OP,
	ERR_UNKNOWN_TYPE,
	ERR_TYPE_MISMATCH,
	ERR_EXPRESSION,
	ERR_FOR_COUNTER,
	ERR_FOR_INCOMPLETE,
	ERR_ARRAY_BOUNDS,
	ERR_ARRAY_DIM,
	ERR_ARRAY_INDEX,
	ERR_REC_FIELD,
	ERR_OPERATOR_PARAM,
	ERR_HEAD2BODY_TWICE,
	ERR_HEAD2BODY_NUMPARAMS,
	ERR_HEAD2BODY_MISMATCH,
	ERR_EXTERN_BODY,
	ERR_ENDPRGM_MISMATCH,

	ERR_VM = 666
};

/*
 *	Languages
 */
char * english_msg_table[] =
{
	"Internal compiler error",
	"Input file not found",
	"Bad identifier",
	"Parse error",
	"Identifier expected",
	"Keyword used as identifier",
	"Unknown identifier",
	"Duplicate identifer",
	"Bad string constant",
	"'BEGIN' expected",
	"'END' expected",
	"'THEN' expected",
	"'DO' expected",
	"'UNTIL' expected",
	"':' expected",
	"'(' expected",
	"')' expected",
	"':=' or '=' expected",
	"'[' expected",
	"']' expected",
	"'..' expected",
	"'OF' expected",
	"Integer constant expected",
	"',' expected",
	"'=' expected",
	"Unknown type",
	"Type mismatch",
	"Error in expression",
	"Only integers can be 'for' counters",
	"'TO' or 'DOWNTO' expected",
	"Array bounds mismatch",
	"Array must have maximum 3 dimensions",
	"Indexing requires integer expression",
	"Field not found in record",
	"Overloaded operators must have 2 parameters",
	"Can't 'FORWARD' twice!",
	"Prototype header & body header differ by number of params",
	"Prototype header & body header parameter mismatch",
	"Can't declare the body of an external subroutine.\n"
	"Also note that subroutines with variable number\n"
	"of parameters must be declared external\n",
	"Program must end with a no-arguments procedure",

	NULL
};

char * romanian_msg_table[] =
{
	"Eroare interna",
	"Fisier de intrare negasit",
	"Identificator gresit",
	"Eroare sintactica",
	"Se astepta identificator",
	"Cuvant cheie folosit drept indentificator",
	"Identificator necunoscut",
	"Identificator duplicat",
	"String gresit",
	"Se astepta 'BEGIN'"
	"Se astepta 'END'",
	"Se astepta 'THEN'",
	"Se astepta 'DO'",
	"Se astepta 'UNTIL'",
	"Se astepta ':'",
	"Se astepta '('",
	"Se astepta ')'",
	"Se astepta ':=' sau '='",
	"Se astepta '['",
	"Se astepta ']'",
	"Se astepta '..'",
	"Se astepta 'OF'",
	"Se astepta constanta numerica",
	"Se astepta ','",
	"Se astepta '='",
	"Tip necunoscut",
	"Nepotrivire tipuri",
	"Eroare in expresie",
	"Doar 'integer' poate fi contor 'for'",
	"Se astepta 'TO' sau 'DOWNTO'",
	"Limite tablou gresite",
	"Un tablou poate avea maxim 3 dimensiuni",
	"Indexarea cere expresie intreaga",
	"Camp neprezent in inregistrare",
	"Operatorii supraincarcati trebuie sa aiba 2 parametri",
	"Nu se poate 'FORWARD' de 2 ori",
	"Prototype header & body header differ by number of params",
	"Prototype header & body header parameter mismatch",
	"Can't declare the body of an external subroutine.\n"
	"Also note that subroutines with variable number\n"
	"of parameters must be declared external\n",
	"Program must end with a no-arguments procedure",

	NULL
};


/*
 *	SYS_*
 */
void EN_Sys_Error(int err_code, int extra_params, int param1, int param2)
{
	//if ( !freopen("stderr.txt", "wt", stderr) )
	//	exit( 1 );

	if ( err_code == ERR_VM )
	{
		fprintf(stderr, "VM error.\n");
		VM_Close();
		exit ( 1 );
	}

	static char * type_names[] =
	{
		"integer",
		"real",
		"char",
		"string",
		"array",
		"record",
		"procedure",
		"function",
		NULL
	};

	if ( !parser.value[0] )
	{
		parser.value[0] = parser.token2char;
		parser.value[1] = 0;
	}

	for ( int i = 0; i < includedNum; i++ )
		fprintf(stderr, "in included file '%s':\n", includedFiles[i].filename);

	if ( extra_params && err_code == ERR_TYPE_MISMATCH )
	{
		if ( param1 != param2 )
			fprintf(stderr, "line %i: Type mismatch: %s to %s (stopped at '%s')\n", parser.line_nr,
				type_names[param2], type_names[param1], parser.value);
		else
			fprintf(stderr, "line %i: Type mismatch: unsupported operator on %s (stopped at '%s')\n",
				parser.line_nr, type_names[param1], parser.value);
	}
	else
		fprintf(stderr, "line %i: %s (stopped at '%s')\n", parser.line_nr,
			english_msg_table[err_code], parser.value);

	Type_CleanUp();
	Parse_Close();
	Z_close();
	VM_Close();

	exit( 1 );
}

void RO_Sys_Error(int err_code, int extra_params, int param1, int param2)
{
	//if ( !freopen("stderr.txt", "wt", stderr) )
	//	exit( 0 );

	if ( err_code == ERR_VM )
	{
		fprintf(stderr, "Eroare in MV.\n");
		VM_Close();
		exit ( 1 );
	}

	static char * type_names[] =
	{
		"integer",
		"real",
		"char",
		"string",
		"array",
		"record",
		"procedure",
		"function",
		NULL
	};

	if ( !parser.value[0] )
	{
		parser.value[0] = parser.token2char;
		parser.value[1] = 0;
	}

	for ( int i = 0; i < includedNum; i++ )
		fprintf(stderr, "in fisierul '%s':\n", includedFiles[i].filename);

	if ( extra_params && err_code == ERR_TYPE_MISMATCH )
	{
		if ( param1 != param2 )
			fprintf(stderr, "linia %i: Nepotrivire tipuri: %s - %s (stopped at '%s')\n", parser.line_nr,
				type_names[param2], type_names[param1], parser.value);
		else
			fprintf(stderr, "linia %i: Nepotrivire tipuri: operator nesuportat de %s (oprire la '%s')\n",
				parser.line_nr, type_names[param1], parser.value);
	}
	else
		fprintf(stderr, "linia %i: %s (oprire la '%s')\n", parser.line_nr,
			romanian_msg_table[err_code], parser.value);

	Type_CleanUp();
	Parse_Close();
	Z_close();
	VM_Close();

	exit( 1 );
}
