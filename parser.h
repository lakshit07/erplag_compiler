// Group Number : 47
// Anirudh Kumar Bansal 2014A7S081P
// Lakshit Bhutani 2014A7PS095P 

#ifndef parserh
#define parserh

#include "parserDef.h"

void read_grammar(char *filename, grammar g);
character identify(char* id);
void nonterm_in(grammar g);
void firstset_calc(character c , grammar g);
void firstset_nonterm(grammar g);
void firstset_str(character* str ,int siz, firstset f , grammar g);
void followset_rhs(int ruleno, grammar g);
void followset_nonterm(grammar g);
void get_ptable(grammar g);
stack create_stack_node();
stack create_stack_node_util(bool is_terminal,type_of_character name);
void push(stack p , stack* top);
pnode pop(stack* top);
void parse(FILE* source);
void print(pnode root);
void printParseTreeUtil(pnode root);
void printParseTree();
void strcpy2(char* a , char* b);
#endif