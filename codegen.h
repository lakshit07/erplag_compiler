// Group Number : 47
// Anirudh Kumar Bansal 2014A7S081P
// Lakshit Bhutani 2014A7PS095P
#ifndef codegenh
#define codegenh
#include "astDef.h"

void gen_code_util(char* str);
void gen_code();
void gen_data(bptr broot);
void gen_prog(astptr aroot,bptr broot);
FILE* fasm;
int label;

#endif