// Group Number : 47
// Anirudh Kumar Bansal 2014A7S081P
// Lakshit Bhutani 2014A7PS095P 

#ifndef lexerh
#define lexerh


#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>

#include "lexerDef.h"

char getChar(FILE* f);
void hash_keywords();
unsigned long hash(unsigned char* str);
bool is_keyword(token* t);
void nextToken(FILE *f , token *t);
void removeComments(char* testcasefile);
void init_lexer();

/*
STATE Information : 
(For developer's reference)

1 : Start State
2 : * found
3 : < found
4 : > found
5 : = found
6 : ! found
7 : : found
8 : . found
9 : 0-9 found
10 : a-z A-Z _ found
11 : \n, \t, \t or space found
12 : ** found
13 : . found after NUM 
14 : RNUM found - 0-9 after state 13  
15 : e/E found after RNUM(state 14)
16 : +/- found after state 15
17 : RNUM found - 0-9 after state state 15 or state 16
18 : * after **
19 : + found
20 : - found
21 : << found
22 : >> found

*/

#endif 
