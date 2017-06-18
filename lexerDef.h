// Group Number : 47
// Anirudh Kumar Bansal 2014A7S081P
// Lakshit Bhutani 2014A7PS095P 

#ifndef LEXERDEF
#define LEXERDEF 

#define MAX_SIZE 77

extern char * keywords[];
extern char * str_characters[];
extern char * str_symbols[];

#define NO_OF_KEYWORDS 30
unsigned long hash_table_keywords[NO_OF_KEYWORDS];

typedef enum
{
	program, moduleDeclarations, moduleDeclaration,
	otherModules, driverModule,	module,	ret, input_plist,
	N1,	output_plist, N2, dataType, type, moduleDef, statements,
	statement, ioStmt, var,	whichId, simpleStmt, assignmentStmt,
	whichStmt, lvalueIDStmt, lvalueARRStmt,	index2,	moduleReuseStmt,
	optional, idList, N3, expression, arithmeticOrBooleanExpr,
	N7,	AnyTerm, N8, arithmeticExpr, N4, term, N5, factor,
	op1, op2, logicalOp, relationalOp, declareStmt, conditionalStmt,
	caseStmts, N9, value, Default, iterativeStmt, range, PLUS,
	MINUS, MUL,	DIV, LT, LE, GT, GE, EQ, NE, DEF, DRIVERDEF,
	ENDDEF,	DRIVERENDDEF, COLON, RANGEOP, SEMICOL, COMMA,
	ASSIGNOP, SQBO,	SQBC, BO, BC, COMMENTMARK, NUM,	RNUM,
	ID,	INTEGER, REAL, BOOLEAN,	OF,	ARRAY, START, END,
	DECLARE, MODULE, DRIVER, PROGRAM, GET_VALUE, PRINT, USE, WITH,
	PARAMETERS,	TRUE, FALSE, TAKES,	INPUT,	RETURNS, AND, OR, FOR,
	IN,	SWITCH,	CASE, BREAK, DEFAULT, WHILE, eps, eof, ERR
}character;

extern char * token_names[]; 

typedef struct
{
	character type;
 	int line;
 	char lexeme[50];
}token;

#endif
