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

enum keywords //MRU order
{
	VAR,
	TYPE,
	CONST,
	PROCEDURE,
	FUNCTION,

	BEGIN,
	END,

	FORWARD,
	EXTERNAL,
	OPERATOR,

	INTEGER,
	REAL,
	CHAR,
	STRING,
	ARRAY,
	RECORD,

	IF,
	THEN,
	ELSE,
	WHILE,
	REPEAT,
	UNTIL,
	FOR,
	CASE,
	TO,
	DO,
	OF,
	DOWNTO,

	USES,
	BREAK,
	CONTINUE
};

#define KEYNUM 29

char * keyword_names[] =
{
	//rezerved idenfifiers = keywords
	"VAR",
	"TYPE",
	"CONST",
	"PROCEDURE",
	"FUNCTION",

	"BEGIN",
	"END",

	"FORWARD",
	"EXTERNAL",
	"OPERATOR",

	"INTEGER",
	"REAL",
	"CHAR",
	"STRING",
	"ARRAY",
	"RECORD",

	"IF",
	"THEN",
	"ELSE",
	"WHILE",
	"REPEAT",
	"UNTIL",
	"FOR",
	"CASE",
	"TO",
	"DO",
	"OF",
	"DOWNTO",

	"USES",
	//"BREAK",
	//"CONTINUE",
	NULL
};

enum tokens
{
	// general cases
	IDENT,
	INT,
	FLOAT,
	CHARACTER,
	LITERAL,

	OP_PRIORITY_0 = 128, //un-overloadable
	//() CALL
	//[] ARRAY
	OP_DOT,
	OP_DOTS,
	OP_COLON,

	OP_PRIORITY_1, //unary ops
	OP_NOT,
	OP__NOT,
	OP_AT,
	OP_TILDE,

	OP_PRIORITY_2, //multiplicative
	OP_MUL,
	OP_DIV,
	OP_IDIV = OP_DIV, //FIXME: use overloaded operator
	OP_AND,
	OP_AMPERSTAND,
	OP_MOD,
	OP_PERCENT,
	OP_SHR,
	OP_SHL,
	
	OP_PRIORITY_3, //additive
	OP_ADD,
	OP_SUB,
	OP_OR,
	OP_PIPE,
	OP_XOR,
	OP_CARRET,

	OP_PRIORITY_4, //relational
	OP_LESS,
	OP_LESS_EQ,
	OP_GREAT,
	OP_GREAT_EQ,
	OP_EQ,
	OP_NOT_EQ,
	OP_QUESTION,
	OP_IN,

};

#define is_white(c) (c <= ' ' || c == '{' || c == ';' )
#define is_alpha(c) ((c>='A' && c<='Z') || (c>='a' && c<='z') || c=='_')
#define is_decnum(c) ((c>='0' && c<='9'))


/*
 *	Global vars
 */
parse_state_t saveslot;
byte char2token[256];

#define HASH_MOD 0x0FFFFFC7 //65519

long int hashcode( const char * string )
{
	long int hash, i;
	char str[STR_SIZE];

#define _2upper(c) ( c >= 'a' ) ? c - ' ' : c

	strcpy(str, string);

	for ( i = 0; str[i]; i++ )
		str[i] = _2upper(str[i]);
	for (hash = 0, i = 0; str[i]; i++)
		hash = (hash << 4) ^ (hash >> 2) ^ str[i];
	//if ( hash < 0 ) hash *= -1;

	//printf("%s = %li\n", string, (hash % HASH_MOD));
	return ( hash % HASH_MOD );
}

inline int hashicmp( const char * name1, int name2 )
{
	if ( hashcode(name1) == name2 )
		return 0;
	else
		return 1;
}

inline int hashicmp( int name1, int name2 )
{
	if ( name1 == name2 )
		return 0;
	else
		return 1;
}

inline void hashcpy( int * name, const char * str)
{
	*name = hashcode(str);
}

inline void hashcpy( int * name, int num)
{
	*name = num;
}

/*
 *	PARSE_GETNEXTCHAR
 */
inline void Parse_GetNextChar()
{
	parser.nextchar = *(parser.src + parser.src_i++);
	if ( parser.nextchar == '\n' )
		if ( includedNum && 
			parser.src_i >= includedFiles[includedNum-1].end_of_file )
		{
			includedNum--;
			parser.line_nr = includedFiles[includedNum].old_line_nr;
		}
		else
			parser.line_nr ++;
}

/*
 *	PARSE_SKIPWHITE
 */
void Parse_SkipWhite()
{
	if ( !parser.nextchar )
		return;
	
	while ( is_white(parser.nextchar) ) {
		/*	{ } comments */
		if ( parser.nextchar == '{' ) {
			while ( parser.nextchar != '}' ) {
				Parse_GetNextChar();
				if ( !parser.nextchar ) 
					return;
			}
		}

		Parse_GetNextChar();
		if ( !parser.nextchar )
			return;
	}
	/* (C++ comments) */
	if ( parser.nextchar == '/' && *(parser.src + parser.src_i) == '/' ) {
		while ( parser.nextchar != '\n' )
			Parse_GetNextChar();
		Parse_GetNextChar();
		Parse_SkipWhite();
	}
	/* (* *) comments */
	if ( parser.nextchar == '(' && *(parser.src + parser.src_i) == '*' ) {
		while ( 1 ) {
			Parse_GetNextChar();
			if ( !parser.nextchar )
				return;
			if ( parser.nextchar == '*' && *(parser.src + parser.src_i) == ')' )
				break;
		}
		Parse_GetNextChar(); //eat the ')'
		Parse_GetNextChar(); //eat next space 
		Parse_SkipWhite(); // eat up all
	}
}

/*
 *	PARSE_INIT
 */
char path[MAX_PATH];

void Parse_Init(char * filen)
{
	FILE * file;
	int size;

	file = fopen(filen, "rb");
	if ( !file )
		Sys_Error(ERR_FILE);

	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);
	parser.src = (char *)malloc(size + 1);
	if ( !parser.src )
		Sys_Error(ERR_0);
	Sys_Warn("Parser malloc...");
	if ( !fread(parser.src, size, 1, file) )
		Sys_Error(ERR_0);
	*(parser.src + size) = 0; //sanity check
	parser.src_size = size; //don't add the 1
	includedNum = 0;

	fclose(file);

	memset(char2token, 0, sizeof(char2token));

	char2token[':'] = ':';
	char2token[','] = ',';
	char2token[';'] = ';';
	char2token['\''] = '\'';
	char2token['\\'] = '\\';
	char2token['\"'] = '\"';
	char2token['`'] = '`';
	char2token['['] = '[';
	char2token[']'] = ']';
	char2token['('] = '(';
	char2token[')'] = ')';

	char2token['.'] = OP_DOT;
	//OP_DOTS,

	//OP_NOT,
	char2token['!'] = OP__NOT;
	char2token['@'] = OP_AT;
	char2token['~'] = OP_TILDE;

	char2token['*'] = OP_MUL;
	char2token['/'] = OP_DIV;
	//OP_IDIV;
	//OP_AND;
	char2token['&'] = OP_AMPERSTAND;
	//OP_MOD;
	char2token['%'] = OP_PERCENT;
	//OP_SHR;
	//OP_SHL;
	
	char2token['+'] = OP_ADD;
	char2token['-'] = OP_SUB;
	//OP_OR;
	char2token['|'] = OP_PIPE;
	//OP_XOR;
	char2token['^'] = OP_CARRET;
	char2token[':'] = OP_COLON;

	char2token['<'] = OP_LESS;
	//OP_LESS_EQ;
	char2token['>'] = OP_GREAT;
	//OP_GREAT_EQ;
	char2token['='] = OP_EQ;
	//OP_NOT_EQ;
	char2token['?'] = OP_QUESTION;
	//OP_IN;

	op_ovrld_count = 46;

	memset(parser.value, 0, MAX_IDENT_SIZE);
	parser.line_nr = 1;
	Parse_GetNextChar();
	Parse_Next();
}

/*
 *	PARSE_INCLUDE_FILE
 */
void Parse_IncludeFile()
{
	char filename[MAX_IDENT_SIZE];

	if ( parser.token != LITERAL )
		Sys_Error(ERR_PARSE);

	sprintf(filename, "%s\\%s\0", path, parser.value);

	strcpy(includedFiles[ includedNum ].filename, filename);
	includedFiles[ includedNum ].old_line_nr = parser.line_nr + 1;
	includedFiles[ includedNum ].end_of_file = 0xffff;


	//while ( parser.nextchar != '\n' )
	//	Parse_GetNextChar();

	char* newFileData;
	FILE* includeFile;

	includeFile = fopen(filename, "rb");
	if ( !includeFile )
		Sys_Error(ERR_0);

	int size;
	fseek(includeFile, 0, SEEK_END);
	size = ftell(includeFile);
	fseek(includeFile, 0, SEEK_SET);
	includedFiles[ includedNum ].end_of_file = size;
	
	newFileData = (char *)malloc(size);
	if ( !newFileData )
		Sys_Error(ERR_0);
	if ( !fread((char *)newFileData, size, 1, includeFile) )
		Sys_Error(ERR_0);

	fclose(includeFile);

	char* oldSrc;
	int oldSize;
	
	oldSize = parser.src_size - parser.src_i + 1; //null terminated
	oldSrc = (char *)malloc(oldSize);
	if ( !oldSrc )
		Sys_Error(ERR_0);
	memcpy(oldSrc, parser.src + parser.src_i, oldSize);

#if 0
	free(parser.src);
	parser.src = (char *)malloc(oldSize + size);
	if ( !parser.src )
		Sys_Error(ERR_0);
#else
	parser.src = (char *)realloc(parser.src, oldSize + size);
#endif
	memcpy(parser.src, newFileData, size);
	memcpy(parser.src + size, oldSrc, oldSize);

	free(newFileData);
	free(oldSrc);
	parser.src_size = oldSize + size - 1; //don't count the null terminator
	*(parser.src + parser.src_size) = 0; //sanity check
	parser.src_i = 0;
	parser.line_nr = 1;
	Parse_GetNextChar();
	Parse_Next();

	includedNum ++;

}

/*
 *	PARSE_CLOSE
 */
void Parse_Close()
{
	free(parser.src);
	Sys_Warn("...Parser free");
}

/*
 *	PARSE_GETNAME
 */
void Parse_GetName()
{
	int i;

	parser.token = IDENT;
	i = 0;
	memset(parser.value, 0, MAX_IDENT_SIZE);
	do
	{
		if ( i > MAX_IDENT_SIZE || parser.nextchar < ' ' )
			Sys_Error(ERR_BAD_IDENT);
		parser.value[i++] = parser.nextchar;
		Parse_GetNextChar();
	} while ( is_alpha(parser.nextchar) || is_decnum(parser.nextchar) );

	//check for (3)literal operators
	static int _3letter_op[9][7] = {
		'N', 'n', 'O', 'o', 'T', 't', OP_NOT,
		'A', 'a', 'N', 'n', 'D', 'd', OP_AND,
		'O', 'o', 'R', 'r',  0,   0,  OP_OR,
		'X', 'x', 'O', 'o', 'R', 'r', OP_XOR,
		'D', 'd', 'I', 'i', 'V', 'v', OP_IDIV,
		'M', 'm', 'O', 'o', 'D', 'd', OP_MOD,
		'I', 'i', 'N', 'n',  0,   0,  OP_IN,
		'S', 's', 'H', 'h', 'R', 'r', OP_SHR,
		'S', 's', 'H', 'h', 'L', 'l', OP_SHL
	};

	if ( !parser.value[3] )
		for ( i = 0; i < 8; i++ )
			if (
			(_3letter_op[i][0] == parser.value[0] || _3letter_op[i][1] == parser.value[0]) &&
			(_3letter_op[i][2] == parser.value[1] || _3letter_op[i][3] == parser.value[1]) &&
			(_3letter_op[i][4] == parser.value[2] || _3letter_op[i][5] == parser.value[2]) )
			{
				parser.token = _3letter_op[i][6];
				return;
			}
}

/*
 *	PARSE_GETNUM
 */
void Parse_GetNum(int allow_float)
{
	dword i;
	byte is_hex = false;
	byte is_float = false;

	i = 0;
	memset(parser.value, 0, MAX_IDENT_SIZE);

	if ( parser.nextchar == '$' ) {
		is_hex = true;
		Parse_GetNextChar();
		//force an "0x"
		parser.value[0] = '0';
		parser.value[1] = 'x';
		i = 2;
	}

	char ch;
	while ( 1 ) {
		parser.value[i++] = parser.nextchar;
		Parse_GetNextChar();

		ch = parser.nextchar;
		if ( parser.value[0] == '0' && ch == 'x' ) {
			is_hex = true;
			continue;
		}
		if ( (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F') )
			if ( is_hex ) 
				continue; //catch hex letters
		if ( !is_decnum(ch) || ch == 'e' || ch == 'E' || ch == '.' )
			break;
	}

	if ( !is_hex && allow_float && ch == '.' ) {
		is_float = true;
		while ( 1 ) {
			parser.value[i++] = parser.nextchar;
			Parse_GetNextChar();

			ch = parser.nextchar;
			if ( !is_decnum(ch) || ch == 'e' || ch == 'E' )
				break;
		}
	}

	if ( !is_hex && allow_float && (ch == 'e' || ch == 'E') ) {
		is_float = true;
		parser.value[i++] = ch;

		Parse_GetNextChar();
		ch = parser.nextchar;
		if ( ch == '+' || ch == '-' || is_decnum(ch) ) {
			parser.value[i++] = ch;

			do {
				Parse_GetNextChar();
				parser.value[i++] = parser.nextchar;
			} while ( is_decnum(parser.nextchar) );
		}
	}

	// set the corect token
	if ( is_float ) {
		if ( !allow_float )
			Sys_Error(ERR_TYPE_MISMATCH);
		parser.token = FLOAT;
		parser.float_value = atof(parser.value);
	} else {
		parser.token = INT;
		//support for 10, 16, 8 radix
		sscanf(parser.value, "%i", &i);
		sprintf(parser.value, "%i\0", i);
		parser.int_value = i;//atoi(parser.value);
	}
}

/*
 *	PARSE_GETLITERAL
 */
void Parse_GetLiteral()
{
	int i = 0;

	memset(parser.value, 0, MAX_IDENT_SIZE);
	Parse_GetNextChar(); //skip the '

	while ( parser.nextchar != '\'' )
	{
		parser.value[i++] = parser.nextchar;
		Parse_GetNextChar();
		//if ( /*i > MAX_IDENT_SIZE ||*/ parser.nextchar < ' ' )
		//	Sys_Error(ERR_BAD_STRING);
	}

	if ( i > 1 )
		parser.token = LITERAL;
	else
		parser.token = CHARACTER;

	Parse_GetNextChar();
}

/*
 *	PARSE_GET_C_LITERAL
 */
void Parse_Get_C_Literal()
{
	int i = 0;

	parser.token = LITERAL;
	memset(parser.value, 0, MAX_IDENT_SIZE);
	Parse_GetNextChar(); //skip the "

	while ( parser.nextchar != '\"' )
	{
		if ( parser.nextchar == '\\' )
		{
			Parse_GetNextChar();
			switch ( parser.nextchar )
			{
			case 'b':
				parser.value[i] = '\b';
				break;
			case 'n':
				parser.value[i] = '\n';
				break;
			case 'r':
				parser.value[i] = '\r';
				break;
			case 't':
				parser.value[i] = '\t';
				break;
			case '\\':
				parser.value[i] = '\\';
				break;
			case '\'':
				parser.value[i] = '\'';
				break;
			case '\"':
				parser.value[i] = '\"';
				break;
			default:
				if ( is_decnum(parser.nextchar) )
					//TODO: more then 1 digit
					parser.value[i] = parser.nextchar - '0';
				else
					parser.value[i] = parser.nextchar;
			}
		}
		else
			parser.value[i] = parser.nextchar;
		i++;
		Parse_GetNextChar();
		//if ( /*i > MAX_IDENT_SIZE ||*/ parser.nextchar < ' ' )
		//	Sys_Error(ERR_BAD_STRING);
	}

	if ( i > 1 )
		parser.token = LITERAL;
	else
		parser.token = CHARACTER;

	Parse_GetNextChar();
}

/*
 *	PARSE_GETOPERATOR
 */
void Parse_GetOperator()
{
	int oldchar = parser.nextchar; //token by value
	memset(parser.value, 0, MAX_IDENT_SIZE);
	Parse_GetNextChar();

	// litteral ops are treated in Parse_GetName

	// test for 2 chars ops
	if ( oldchar == '<' && parser.nextchar == '=' ) {
		parser.token = OP_LESS_EQ;
		Parse_GetNextChar();
		return;
	}
	if ( oldchar == '<' && parser.nextchar == '>' ) {
		parser.token = OP_NOT_EQ;
		Parse_GetNextChar();
		return;
	}
	if ( oldchar == '>' && parser.nextchar == '=' ) {
		parser.token = OP_GREAT_EQ;
		Parse_GetNextChar();
		return;
	}
	if ( oldchar == '.' && parser.nextchar == '.' ) {
		parser.token = OP_DOTS;
		Parse_GetNextChar();
		return;
	}

	//default = single char ops
	parser.token = char2token[oldchar];
	parser.token2char = oldchar;
}

/*
 *	PARSE_NEXT
 */
void Parse_Next()
{
	memcpy(&saveslot, &parser, sizeof(parse_state_t));

	Parse_SkipWhite();
	if ( !parser.nextchar )
	{
		parser.token = EOF;
		return;
	}

	if ( is_alpha(parser.nextchar) ) {
		Parse_GetName();

		constant_t* cnst = Type_SearchConstant();
		if ( cnst ) {
			parser.token = cnst->type;
			switch ( cnst->type ) {
			case INT:
				parser.int_value = cnst->value.integer;
				break;
			case FLOAT:
				parser.float_value = cnst->value.real;
				break;
			case CHARACTER:
			case LITERAL:
				strcpy(parser.value, cnst->value.string);
				break;
			}
			return;
		}

		return;
	}
	if ( is_decnum(parser.nextchar) || parser.nextchar == '$' ) {
		Parse_GetNum(calc_allow_float);
		return;
	}
	if ( parser.nextchar == '\"' ) {
		Parse_Get_C_Literal();
		return;
	}
	if ( parser.nextchar == '\'' ) {
		Parse_GetLiteral();
		return;
	}
		Parse_GetOperator();
}

/*
 *  Parse_Undo
 *
 *  Description: returns before the last Parse_Next()
 */
void Parse_Undo()
{
	memcpy(&parser, &saveslot, sizeof(parse_state_t));
}

/*
 *	PARSE_GETKEYWORD
 */
int Parse_GetKeyword()
{
	for ( int i = 0; i < KEYNUM; i++ )
		if ( stricmp(keyword_names[i], parser.value) == 0 )
		{
			//printf("%i iterations\n", i);
			return i;
		}
	return -1;
}

/*
 *	PARSE_MATCHKEYWORD
 */
int Parse_MatchKeyword( int key )
{
	if ( stricmp(keyword_names[key], parser.value) == 0 )
		return true;
	return false;
}

/*
 *	PARSE_GETIDENTIFIER
 */
/*
char * Parse_GetIdentifier()
{
	if ( parser.token != IDENT )
		Sys_Error(ERR_NO_IDENT);
	if ( Parse_GetKeyword() != -1 )
		Sys_Error(ERR_KEY_AS_IDENT);
	return parser.value;
}
*/
int Parse_GetIdentifier()
{
	if ( parser.token != IDENT )
		Sys_Error(ERR_NO_IDENT);
	if ( Parse_GetKeyword() != -1 )
		Sys_Error(ERR_KEY_AS_IDENT);

	return hashcode( parser.value );
}


/*
 *  Parse_Calculator
 *
 *  Description: constant expressions calculator
 */
void Parse_Calculator(void)
{
}

/*
 *
 */
int Parse_CmdLine(const int argc, char** argv, const char * key, const int defaultValue)
{
	if ( argc <= 1 )
		return defaultValue;
	for ( int i = 1; i < argc; i++ )
		if ( stricmp(argv[i], key) == 0 )
				return true;
	return defaultValue;
}

