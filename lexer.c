// Group Number : 47
// Anirudh Kumar Bansal 2014A7S081P
// Lakshit Bhutani 2014A7PS095P 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "lexer.h"

bool hashed = false;

/*
sdbm
----
This algorithm was created for sdbm (a public-domain reimplementation of ndbm) database library. 
It was found to do well in scrambling bits, causing better distribution of the keys and fewer splits.
It also happens to be a good general hashing function with good distribution. 
The actual function is hash(i) = hash(i - 1) * 65599 + str[i]; what is included below is the faster version used in gawk. 
The magic constant 65599 was picked out of thin air while experimenting with different constants, and turns out to be a prime. 
This is one of the algorithms used in berkeley db. 
*/

unsigned long hash(unsigned char* str)
{
  unsigned long hash = 0;
    int c;

    while (c = *str++)
       hash = c + (hash << 6) + (hash << 16) - hash;

    return hash;
}


char * keywords[] = 
{
    "integer", "real", "boolean", "of", "array", "start", "end", "declare", "module", "driver", "program", "get_value",
    "print", "use", "with", "parameters", "true", "false", "takes", "input", "returns", "AND",
    "OR", "for", "in", "switch", "case", "break", "default", "while"
};

char * str_characters[] = 
{
    "program", "moduleDeclarations", "moduleDeclaration", "otherModules", "driverModule", "module",
    "ret", "input_plist", "N1", "output_plist", "N2", "dataType", "type", "moduleDef", "statements",
    "statement", "ioStmt", "var", "whichId", "simpleStmt", "assignmentStmt", "whichStmt", "lvalueIDStmt",
    "lvalueARRStmt", "index", "moduleReuseStmt", "optional", "idList", "N3", "expression", "arithmeticOrBooleanExpr",
    "N7", "AnyTerm", "N8", "arithmeticExpr", "N4", "term", "N5", "factor", "op1", "op2", "logicalOp",
    "relationalOp", "declareStmt", "conditionalStmt", "caseStmts", "N9", "value", "Default", "iterativeStmt",
    "range", "PLUS", "MINUS", "MUL", "DIV", "LT", "LE", "GT", "GE", "EQ", "NE", "DEF", "DRIVERDEF",
    "ENDDEF", "DRIVERENDDEF", "COLON", "RANGEOP", "SEMICOL", "COMMA", "ASSIGNOP", "SQBO", "SQBC",
    "BO", "BC", "COMMENTMARK", "NUM", "RNUM", "ID", "INTEGER", "REAL", "BOOLEAN", "OF", "ARRAY",
    "START", "END", "DECLARE", "MODULE", "DRIVER", "PROGRAM", "GET_VALUE", "PRINT", "USE",
    "WITH", "PARAMETERS", "TRUE", "FALSE", "TAKES", "INPUT", "RETURNS", "AND", "OR", "FOR",
    "IN", "SWITCH", "CASE", "BREAK", "DEFAULT", "WHILE", "eps"
};

char* str_symbols[] = 
{
    "program", "moduleDeclarations", "moduleDeclaration", "otherModules", "driverModule", "module",
    "ret", "input_plist", "N1", "output_plist", "N2", "dataType", "type", "moduleDef", "statements",
    "statement", "ioStmt", "var", "whichId", "simpleStmt", "assignmentStmt", "whichStmt", "lvalueIDStmt",
    "lvalueARRStmt", "index", "moduleReuseStmt", "optional", "idList", "N3", "expression", "arithmeticOrBooleanExpr",
    "N7", "AnyTerm", "N8", "arithmeticExpr", "N4", "term", "N5", "factor", "op1", "op2", "logicalOp",
    "relationalOp", "declareStmt", "conditionalStmt", "caseStmts", "N9", "value", "Default", "iterativeStmt",
    "range", "+", "-", "*", "/", "<", "<=", ">", ">=", "==", "!=", "<<", "<<<", ">>", ">>>",
    ":", "..", ";", ",", ":=", "[", "]", "(", ")", "**", "NUM", "RNUM",  "ID", "INTEGER", "REAL", "BOOLEAN", "OF", "ARRAY",
    "START", "END", "DECLARE", "MODULE", "DRIVER", "PROGRAM", "GET_VALUE", "PRINT", "USE",
    "WITH", "PARAMETERS", "TRUE", "FALSE", "TAKES", "INPUT", "RETURNS", "AND", "OR", "FOR",
    "IN", "SWITCH", "CASE", "BREAK", "DEFAULT", "WHILE", "eps"    
};

char * token_names[] = {
    "plus", "minus", "mul", "div", "lt", "le", "gt", "ge", "eq", "ne", "def", "driverdef",
    "enddef", "driverenddef", "colon", "rangeop", "semicol", "comma", "assignop", "sqbo",
    "sqbc", "bo", "bc", "commentmark", "num", "rnum", "id"
};


char buffer[MAX_SIZE];
int line_no = 1;
int comment_begin;
int buffer_pos;
int buffer_size = -1;

/*
    Initializes global variables related to lexer
*/

void init_lexer()
{
    line_no = 1;
    buffer_size = -1;
}

/*
    Hashes keywords in hash_table_keywords using sdbm hash function mentioned above    
*/

void hash_keywords()
{
    int i;

    for(i = 0 ; i < NO_OF_KEYWORDS ; i++)
        hash_table_keywords[i] = hash(keywords[i]);
    hashed = true;
}        

/*
    Function to check for if given token t is a keyword using hash table
*/

bool is_keyword(token *t)
{
    int i;
    unsigned long hash_val = hash(t->lexeme);

    for(i = 0 ; i < NO_OF_KEYWORDS ;i++)
    {
        if(hash_val == hash_table_keywords[i])
            {
                t->type = (character)((int)INTEGER + i);
                return true;
            }
    }
    return false;
}

/*
    Reads characters from the source code file MAX_SIZE characters at a time  
*/

char getChar(FILE* f)
{
    if(buffer_size == -1 || buffer_pos == buffer_size)
    {
        buffer_size = fread(buffer , sizeof(char) , MAX_SIZE , f);
        buffer_pos = 0;
    }

    if(buffer_size == 0)
        return '\0';
    else
        return buffer[buffer_pos++];
}

/*
    Removes comments from the testcasefile file
*/

void removeComments(char* testcasefile)
{
    FILE* f1 = fopen(testcasefile , "r");
   
    if(f1 == NULL)
    {
        printf("File %s does not exist!\n" , testcasefile);
        return;
    }    
    
    char c;

    while((c = fgetc(f1)) != EOF)
    {        
        if(c != '*')
            putchar(c);
        else
        {
            c = getc(f1);

            if(c != '*')
            {
                putchar('*');
                putchar(c);
            }
            else
            {
                while((c = fgetc(f1)) != EOF)
                {
                    if(c == '*')
                    {
                        c = fgetc(f1);
                        if(c == '*')
                            break;
                    }
                }
            }
        }

    }
    fclose(f1);
}

/*
    Returns next token from file f in token t
*/

void nextToken(FILE *f , token *t)
{
    if(!hashed)
        hash_keywords();

    char ch;
    int state = 1;
    int count = 0;
    while(true)
    {
        ch = getChar(f);       
        t->lexeme[count++]=ch; 
        t->line= line_no;

        switch(state)
        {
            case 1: 
            switch(ch)
            {
                case '+':    
                    t->type = PLUS;
                    t->lexeme[count] = '\0';
                    return;

                case '-':
                    t->type = MINUS;
                    t->lexeme[count] = '\0';
                    return;                
                
                case '*':
                    state = 2;
                    break;
                
                case '/':
                    t->type = DIV;
                    t->lexeme[count] = '\0';
                    return;
                
                case '<':
                    state = 3;
                    break;
                
                case '>':
                    state = 4;
                    break;
                
                case '=':
                    state = 5;
                    break;
                
                case '!':
                    state = 6; 
                    break;

                case ':':
                    state = 7;
                    break;

                case '.' :
                    state = 8;
                    break;    

                case ';' :
                    t->type = SEMICOL;
                    t->lexeme[count] = '\0';
                    return;    

                case ',' :
                    t->type = COMMA;
                    t->lexeme[count] = '\0';
                    return;

                case '[' :
                    t->type = SQBO;
                    t->lexeme[count] = '\0';
                    return;

                case ']' :
                    t->type = SQBC;
                    t->lexeme[count] = '\0';
                    return;

                case '(' :
                    t->type = BO;
                    t->lexeme[count] = '\0';
                    return;

                case ')' :
                    t->type = BC;
                    t->lexeme[count] = '\0';
                    return;

                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                        state = 9;
                        break; 

                case 'a':
                case 'b':
                case 'c':
                case 'd':
                case 'e':
                case 'f':
                case 'g':
                case 'h':
                case 'i':
                case 'j':
                case 'k':
                case 'l':
                case 'm':
                case 'n':
                case 'o':
                case 'p':
                case 'q':
                case 'r':
                case 's':
                case 't':
                case 'u':
                case 'v':
                case 'w':
                case 'x':
                case 'y':
                case 'z':
                case 'A':
                case 'B':
                case 'C':
                case 'D':
                case 'E':
                case 'F':
                case 'G':
                case 'H':
                case 'I':
                case 'J':
                case 'K':
                case 'L':
                case 'M':
                case 'N':
                case 'O':
                case 'P':
                case 'Q':
                case 'R':
                case 'S':
                case 'T':
                case 'U':
                case 'V':
                case 'W':
                case 'X':
                case 'Y':
                case 'Z':
                case '_':
                        state = 10;
                        break;   

                case '\n':
                        line_no++; 
                case ' ':
                case '\t':
                case '\r': 
                        state = 11;
                        break;

                case '\0':
                    t->type = eof;
                    strcpy(t->lexeme , "eof");
                    return;

                default :
                    t->type = ERR;
                    sprintf(t->lexeme , "ERROR 2 : Unknown symbol %c at line %d" , ch , line_no);
                    return;
            }
            break;


            case 2 :
            switch(ch)
            {
                case '*' : 
                    t->type = COMMENTMARK;
                    t->lexeme[count] = '\0';
                    comment_begin = line_no;
                    state = 12;
                    break;

                default : 
                    buffer_pos--;
                    t->lexeme[count-1] = '\0';
                    t->type = MUL;
                    return;    
            }
            break;

            case 3 :
            switch(ch)
            {
                case '=' :
                    t->type = LE;
                    t->lexeme[count] = '\0';
                    return;

                case '<' :
                    state = 21;
                    break;    

                default : 
                    buffer_pos--;
                    t->type = LT;
                    t->lexeme[count-1] = '\0';
                    return;    
            }
            break;

            case 4 :
            switch(ch)
            {
                case '=' :
                    t->type = GE;
                    t->lexeme[count] = '\0';
                    return;

                case '>' :
                    state = 22;
                    break;

                default : 
                    buffer_pos--;
                    t->type = GT;
                    t->lexeme[count-1] = '\0';
                    return;    
            }
            break;

            case 5 : 
            switch(ch)
            {
                case '=' : 
                    t->type = EQ;
                    t->lexeme[count] = '\0';
                    return;

                default : 
                    buffer_pos--;
                    t->type = ERR;
                    sprintf(t->lexeme , "ERROR 3 : Unknown pattern =%c at line number %d" , ch , line_no);
                    return;    
            }
            break;

            case 6 :
            switch(ch)
            {
                case '=' : 
                    t->type = NE;
                    t->lexeme[count] = '\0';
                    return;

                default : 
                    buffer_pos--;
                    t->type = ERR;
                    sprintf(t->lexeme , "ERROR 3 : Unknown pattern !%c at line number %d" , ch , line_no);
                    return;    
            }
            break;

            case 7:
            switch(ch)
            {
                case '=' : 
                    t->type = ASSIGNOP;
                    t->lexeme[count] = '\0';
                    return;

                default : 
                    buffer_pos--;
                    t->type = COLON;
                    t->lexeme[count-1]='\0';
                    return; 
            }
            break;

            case 8 :
            switch(ch)
            {
                case '.' : 
                    t->type = RANGEOP;
                    t->lexeme[count] = '\0';
                    return;

                default : 
                    buffer_pos--;
                    t->type = ERR;
                    sprintf(t->lexeme , "ERROR 3 : Unknown pattern .%c at line number %d" , ch , line_no);
                    return; 
            }
            break;

            case 9 :
            switch(ch)
            {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                        state = 9;
                        break;
                case '.':
                        state = 13;
                        break;
                default:
                        buffer_pos--;
                        t->type=NUM;
                        t->lexeme[count-1] = '\0';
                        return;                         
            }
            break;

             case 10 :
            switch(ch)
            {
                case 'a':
                case 'b':
                case 'c':
                case 'd':
                case 'e':
                case 'f':
                case 'g':
                case 'h':
                case 'i':
                case 'j':
                case 'k':
                case 'l':
                case 'm':
                case 'n':
                case 'o':
                case 'p':
                case 'q':
                case 'r':
                case 's':
                case 't':
                case 'u':
                case 'v':
                case 'w':
                case 'x':
                case 'y':
                case 'z':
                case 'A':
                case 'B':
                case 'C':
                case 'D':
                case 'E':
                case 'F':
                case 'G':
                case 'H':
                case 'I':
                case 'J':
                case 'K':
                case 'L':
                case 'M':
                case 'N':
                case 'O':
                case 'P':
                case 'Q':
                case 'R':
                case 'S':
                case 'T':
                case 'U':
                case 'V':
                case 'W':
                case 'X':
                case 'Y':
                case 'Z':
                case '_':
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9': 
                         if(count == 11)  //as the maximum length of a keyword can be 10 (PARAMETERS)
                        {
                            while((ch>='A' && ch<='Z')|| (ch>='a' && ch<='z') || (ch>='0' && ch<='9')|| ch=='_')
                                ch = getChar(f);
                            
                            buffer_pos--;
                            t->type = ERR;
                            t->lexeme[8]='\0';
                            char temp[9];
                            strcpy(temp,t->lexeme);
                            sprintf(t->lexeme,"ERROR 1: Identifier %s.. exceeds maximum length of 8 at line number %d",temp,line_no);
                            return;
                        } 
                        state = 10;
                        break;
                default:
                        buffer_pos--;
                        t->lexeme[count-1]='\0';

                        if(!is_keyword(t))
                        {
                            if(count > 9)  //length of identifier is count-1 and it is invalid if length>8
                            {
                                t->type = ERR;
                                char temp[9];
                                t->lexeme[8]='\0';
                                strcpy(temp,t->lexeme);
                                sprintf(t->lexeme,"ERROR 1: Identifier %s.. exceeds maximum length of 8 at line number %d",temp,line_no);
                                return;
                            }
                            t->type = ID;
                        } 
                        return;         
            }
            break;


            case 11 : 
                count = 0;
                switch(ch)
                {
                    case '\n' : 
                        line_no++;
                    case ' ':
                    case '\t':
                    case '\r':
                        break;
                    case '\0':
                        t->type = eof;
                        strcpy(t->lexeme , "eof");
                        return;
                    default:
                        buffer_pos--;
                        state = 1;            
                }
                break;


            case 12 : 
            count = 0;
            switch(ch)
            {
                case '*':
                    state = 18;
                    break;
                case '\n':
                    line_no++;
                    state = 12;
                    break;    
                case '\0':
                    t->type = ERR;
                    sprintf(t->lexeme , "ERROR 4 : End of comment ** beginning at line number %d missing" , comment_begin);    
                    return;
                default :
                     state = 12;
                       
            }
            break;


            case 13 :
            switch(ch)
            {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                        state = 14;
                        break;   
                case '.':
                        buffer_pos = buffer_pos-2;
                        t->lexeme[count-2] = '\0';
                        t->type = NUM;
                        return;             
                default : 
                    t->type = ERR;
                    sprintf(t->lexeme , "ERROR 3 : No digit after . at line number %d" , line_no);
                    return;        
                
            }
            break;

            case 14 :
            switch(ch)
            {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                        state = 14; 
                        break;
                case 'e':
                case 'E': 
                        state = 15;
                        break;
                default:
                        buffer_pos--;
                        t->type = RNUM;
                        t->lexeme[count-1]='\0';
                        return;               
            }
            break;
            case 15 :
            switch(ch)
            {
                case '+':
                case '-':
                        state = 16;
                        break;
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                        state = 17;  
                        break;    
                default : 
                    t->type = ERR;
                    sprintf(t->lexeme , "ERROR 3 : Invalid character %c in number at line number %d" , ch , line_no);
                    return;          
                     
            }
            break;

            case 16 :
            switch(ch)
            {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                        state = 17;
                        break;
                default : 
                    t->type = ERR;
                    sprintf(t->lexeme , "ERROR 3 : Invalid character %c in number at line number %d" , ch , line_no);
                    return;                      
            }
            break;
            case 17 :
            switch(ch)
            {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                        state = 17; 
                        break; 
                default:
                        buffer_pos--;
                        t->type = RNUM;
                        t->lexeme[count-1]='\0';
                        return;         
            }
            break;

            case 18 : 
            switch(ch)
            {
                case '*':
                    state = 1;
                    break;
                
                default : 
                    state = 12;
                    break;;    
            } 
            break;

            case 21 : 
            switch(ch)
            {
                case '<':
                    t->type = DRIVERDEF;
                    t->lexeme[count] = '\0';
                    return;

                default : 
                    buffer_pos--;
                    t->type = DEF;
                    t->lexeme[count-1]='\0';
                    return;                    
            }
            break;

            case 22 : 
            switch(ch)
            {
                case '>' : 
                    t->type = DRIVERENDDEF;
                    t->lexeme[count] = '\0';
                    return;
                default : 
                    buffer_pos--;
                    t->type = ENDDEF;
                    t->lexeme[count-1] = '\0';
                    return;    
            }

        }
    }
}
