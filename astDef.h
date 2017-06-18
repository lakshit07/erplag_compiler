
// Group Number : 47
// Anirudh Kumar Bansal 2014A7S081P
// Lakshit Bhutani 2014A7PS095P 

#ifndef astDefh
#define astDefh
#include "parserDef.h"
#include "symboltableDef.h"

grammar gast;
int astrulelen[NO_OF_RULES];

struct astnode
{
	bool is_terminal;
    int ruleno;
    type_of_character name;
    struct astnode *parent;
    struct astnode** children;
    int no_of_children;
    bptr scope;
    typ type;
    int end;
};

typedef struct astnode astnode;
typedef struct astnode* astptr;
astptr astroot;

#endif