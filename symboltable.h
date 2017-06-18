// Group Number : 47
// Anirudh Kumar Bansal 2014A7S081P
// Lakshit Bhutani 2014A7PS095P 

#ifndef symboltableh
#define symboltableh
#include "symboltableDef.h"
#include "parserDef.h"
void init_sym_table();
int get_int(char *arr);
void insert_var(variable v,bptr b);
void insert_func(function f, int s, int e,int lis);
var_type get_type(pnode typenode);
void add_vid(pnode node, var_type vtype,bptr b);
void add_fid(pnode node);
void get_input(pnode node, function *f);
void get_output(pnode node,function *f);
void create_symboltable_util();
void create_symboltable(pnode root);
void print_symboltable(bptr br);
void print_symboltable_util();
#endif