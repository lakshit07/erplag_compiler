// Group Number : 47
// Anirudh Kumar Bansal 2014A7S081P
// Lakshit Bhutani 2014A7PS095P 
#ifndef semantich
#define semantich
#include <stdbool.h>
#include "symboltableDef.h"

var_type get_type_id(char* name , bptr b,int lineno);
varptr get_id(char* name , bptr b);
void set_defined(varptr v , bptr b);
function check_fn(char* fname, bptr b, int lineno);
bool output_type(astptr root , function *f);
void input_type(astptr root , function *f);
bool compat_fn(function f1, function f2, int line);
void get_asttype(astptr aptr);
void semantic_analysis(astptr root);



#endif