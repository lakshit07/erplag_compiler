// Group Number : 47
// Anirudh Kumar Bansal 2014A7S081P
// Lakshit Bhutani 2014A7PS095P 


#ifndef parserdef
#define parserdef

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>

#include "lexerDef.h"
#define NO_OF_RULES 95
#define MAX_RULE_LEN 15
#define MAX_ID_SIZE 30
#define NO_OF_NONTERMINALS 51
#define NO_OF_TERMINALS 60
#define NO_OF_CHARACTERS 111

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

int nodesp,nodesa;
bool err;
typedef bool firstset[NO_OF_CHARACTERS];
firstset firstsets[NO_OF_CHARACTERS];
typedef bool followset[NO_OF_CHARACTERS];
followset followsets[NO_OF_NONTERMINALS];
int siz_in[NO_OF_NONTERMINALS];
int rule_in[NO_OF_NONTERMINALS][NO_OF_RULES];
bool first_calc[NO_OF_CHARACTERS];
typedef character grammar[NO_OF_RULES][MAX_RULE_LEN];
int rulelen[NO_OF_RULES];
bool nullable[NO_OF_CHARACTERS];
typedef int parsetable[NO_OF_NONTERMINALS][NO_OF_CHARACTERS];
parsetable ptable;


typedef union typ
{
	token tok;
	character c;
}type_of_character;

struct parse_tree_node
{
    bool is_terminal;
    int ruleno;
    type_of_character name;
    struct parse_tree_node *parent;
    struct parse_tree_node** children;
    int no_of_children;
};

grammar g;
typedef struct parse_tree_node* pnode;
pnode root;

struct stack_node
{
	pnode n;
    struct stack_node *next;
    
};
typedef struct stack_node* stack;

#endif