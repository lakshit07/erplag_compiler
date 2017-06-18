// Group Number : 47
// Anirudh Kumar Bansal 2014A7S081P
// Lakshit Bhutani 2014A7PS095P 

#ifndef symboltabledef
#define symboltabledef

typedef enum {INT, RE, BOOL, NOTDEF} typ;
typedef enum {ROOT, FUNCTION, WH, FR, SW} btype;
bool insidefor;
char forvar[50];
bool insideswitch;
typ sw_type;


typedef union val
{
	int ival;
	float fval;
	bool bval;
}valu;

typedef struct
{
	typ type;
	bool isarr;
	int low; 
	int high;		
}var_type;

typedef struct var_type* v_type;

typedef struct 
{
	char name[50];
	var_type type; 
	int line;
	int scope;
	bool assigned;
	valu value;
	int offset;
}variable;

typedef struct varnode
{
	variable var;
	struct varnode* next; 
}varnode;

typedef struct varnode* varptr;

typedef struct 
{
	char name[50];
	int input_count;
	varptr input_type;
	int output_count;
	varptr output_type;
	bool defined;
	int occ;

}function;

typedef struct funcnode
{
	function func;
	struct funcnode* next;
}funcnode;

typedef struct funcnode* funcptr;

typedef struct bnode
{
	btype type;
	function f;
	varptr var_table[100];
	int no_of_children;
	struct bnode* parent;
	struct bnode** children;
	int counter;
	int start,end;
}bnode;

typedef struct bnode* bptr;
bptr broot;
bptr bcur;
int scopecur;

#endif