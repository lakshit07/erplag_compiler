// Group Number : 47
// Anirudh Kumar Bansal 2014A7S081P
// Lakshit Bhutani 2014A7PS095P 

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>
#include "parser.h"
#include "lexer.h"

const int SIZE_OF_CHARACTER = sizeof(PLUS); 
bool success = true;

/*
    Identifies the type of character 'id' is
*/

character identify(char* id)
{
    int i;

    for(i = 0 ; i < NO_OF_CHARACTERS - 2 ; i++)
    {
        if(strcmp(id , str_characters[i]) == 0)
            return (character)i;
    }
}

/*
   Reads all the rules of grammar g and marks the rule numbers against the non terminals 
   whose production the rule is
*/

void nonterm_in(grammar g)
{
    int i , j;

    for(i = 0 ; i < NO_OF_NONTERMINALS ; i++)
       siz_in[i] = 0;
    
    for(i = 0 ; i < NO_OF_RULES ; i++)
    {
        int nterm = g[i][0];
        rule_in[nterm][siz_in[nterm]] = i;
        siz_in[nterm]++;
    }
}

/*
    Reads the grammar from file 'filename' and stores it in grammar g
    Rules are assumed to terminate with a dot (.) symbol
    OR (|) in the production rules is handled
*/

void read_grammar(char *filename, grammar g)
{
    FILE *grammarFile;
    int ruleNo, ruleLen, i;
    char id[MAX_ID_SIZE];

    grammarFile = fopen(filename, "r");
    ruleNo = 0;

    while(ruleNo < NO_OF_RULES) 
    {
        fscanf(grammarFile, "%s", id);
        character nonterm = identify(id);
        while(1)
        {
            g[ruleNo][0]=nonterm;
            ruleLen = 0;
            
            while(1)
            {
                fscanf(grammarFile, "%s", id);
                
                if((strcmp(id,"|") == 0) || (strcmp(id,".") == 0)) 
                    break;
                
                g[ruleNo][ruleLen+1] = identify(id);
                ruleLen++;
            }

            rulelen[ruleNo] = ruleLen;
            ruleNo++;
            
            if(strcmp(id,".") == 0) 
                break;
        }
    }

    for(i = 0 ; i < NO_OF_CHARACTERS ; i++)
        first_calc[i] = false;

    nonterm_in(g);

}

/*
    Computes the firstset of character c
*/

void firstset_calc(character c , grammar g)
{
    int ch = (int)c;
    int i,j,k,l;

    if(first_calc[ch])
        return;

    firstset f;

    for(i = 0 ; i < NO_OF_CHARACTERS ; i++)
        f[i] = false;

    if(ch > (int)range)
    {
        f[ch] = true;
        first_calc[ch] = true;

        for(i = 0 ; i< NO_OF_CHARACTERS ; i++)
            firstsets[ch][i] = f[i];
        return;
    }

    int epspos=(int)eps;
    bool iseps = false;

    for(i = 0 ; i < siz_in[ch] ; i++)
    {
        int ruleno = rule_in[ch][i];
        bool noneps = false;
        
        for(j = 1; j <= rulelen[ruleno] ; j++)
        {
            firstset_calc(g[ruleno][j] , g);
            firstset h;

            for(l = 0 ; l < NO_OF_CHARACTERS ; l++) 
                h[l] = firstsets[g[ruleno][j]][l];
            
            for(k = 0 ; k < NO_OF_CHARACTERS ; k++)
                f[k] = f[k] || h[k];
        
            if(h[epspos] && !iseps) 
                f[epspos] = false;
            else 
            {
                noneps = true;
                break;
            }
        }

        if(!noneps)
        { 
            f[epspos] = true;
            iseps = true;
        }
    }
    
    for(i = 0; i < NO_OF_CHARACTERS; i++)
        firstsets[ch][i] = f[i];
    
    first_calc[ch] = true;
}

/*
    Computes the firstset of all characters
*/

void firstset_nonterm(grammar g)
{
    int i;

    for(i = 0 ; i < NO_OF_CHARACTERS ; i++)
    {
        firstset_calc((character)i , g);
        nullable[i] = firstsets[i][(int)eps];
    }    
}


/*
    Computes the firstset of a string of characters in 'str' of size 'siz' and returns it in firstset 'f'
*/

void firstset_str(character* str , int siz , firstset f , grammar g)
{
    int i , j;

    for(i = 0 ; i < NO_OF_CHARACTERS ; i++)
        f[i] = false;

    for(i = 0 ; i < siz ; i++)
    {
        for(j = 0 ; j < NO_OF_CHARACTERS ; j++)
            f[j] = f[j] || firstsets[(int)str[i]][j];

        f[(int)eps] = false;

        if (!nullable[(int)str[i]])
            break;
    }

    if(i == siz)
        f[(int)eps] = true;

}

/*
   Updates the followset for all non terminals on the rightside of the rule number 'ruleno'
*/

void followset_rhs(int ruleno, grammar g)
{
    int ruleLen = rulelen[ruleno];
    int i,j;
    firstset f;
    int lhs = (int)g[ruleno][0];

    for(i = 1 ; i <= ruleLen ; i++)
    {
        character ch = g[ruleno][i];
        int val = (int)ch;
        
        if(ch <= range)
        {
            if(i != ruleLen)
            {
                character rside[ruleLen-i];
                
                for(j = 0 ; j < ruleLen-i ; j++)
                    rside[j] = g[ruleno][i+1+j];
            
                firstset_str(rside,ruleLen-i,f,g);

                for(j = 0 ; j < NO_OF_CHARACTERS ; j++)
                    followsets[val][j] = followsets[val][j] || f[j];
            }

            followsets[val][(int)eps] = false;

            if(i == ruleLen || f[(int)eps])
            {
                for(j = 0 ; j < NO_OF_CHARACTERS ; j++)
                    followsets[val][j] = followsets[val][j] || followsets[lhs][j];
            }

        }
    }
}


/*
   Computes the followset for all non terminals
*/

void followset_nonterm(grammar g)
{
    int i,j;
    followset prev[NO_OF_NONTERMINALS];
    
    for(i = 0 ; i < NO_OF_NONTERMINALS ; i++)
    {
        for(j = 0 ; j < NO_OF_CHARACTERS ; j++)
        {
            followsets[i][j] = false;
            prev[i][j] = false;
        }
    }    
    
    followsets[0][(int)eof] = true;
    prev[0][(int)eof] = true;
    int change;

    while(1)
    {
        for(i = 0 ; i < NO_OF_RULES ; i++)
            followset_rhs(i,g);
        
        change = 0;
        
        for(i = 0 ; i < NO_OF_NONTERMINALS ; i++)
        {
            for(j = 0 ; j < NO_OF_CHARACTERS ; j++)
            {
                if(followsets[i][j] != prev[i][j])
                {
                    change = 1;
                    break;
                }
            }

            if(change) 
                break;
        }
        
        if(!change) 
            break;
        
        for(i = 0 ; i < NO_OF_NONTERMINALS ; i++)
        {
            for(j = 0; j < NO_OF_CHARACTERS ; j++)
                prev[i][j] = followsets[i][j];
        }
    }
}

/*
    Generates the parse table and stores it in 'ptable'
*/

void get_ptable(grammar g)
{
    firstset_nonterm(g);
    followset_nonterm(g);

    int i,j;
    firstset f;

    for(i = 0 ; i < NO_OF_NONTERMINALS ; i++)
    {
        for(j = 0 ; j < NO_OF_CHARACTERS ; j++)
            ptable[i][j] = -1;
    }

    for(i = 0 ; i < NO_OF_RULES ; i++)
    {
        int ruleLen = rulelen[i];
        int lhs = (int)g[i][0];
        character rhs[ruleLen];
        
        for(j = 1 ; j <= ruleLen ; j++)
            rhs[j-1] = g[i][j];

        firstset_str(rhs,ruleLen,f,g);

        for(j = 0 ; j < NO_OF_CHARACTERS ; j++)
        {
            if(j != (int)eps && f[j])
                ptable[lhs][j] = i;  
        }

        if(f[(int)eps])
        {
            for(j = 0 ; j < NO_OF_CHARACTERS ; j++)
            {
                if(followsets[lhs][j])
                    ptable[lhs][j] = i;
            }
        }
    }
}

/*
    STACK utility operations - create node , push, pop
    Stack node structure in parserDef.h
*/

stack create_stack_node()
{
     stack s = (stack)malloc(sizeof(struct stack_node));
     s->n = (pnode)malloc(sizeof(struct parse_tree_node));
     s->next = NULL;
     return s;
}

stack create_stack_node_util(bool is_terminal,type_of_character name)
{
    stack s = create_stack_node();
    s->n->is_terminal = is_terminal;
    s->n->name = name;
    s->n->no_of_children = 0;
    return s;
}

void push(stack p , stack* top)
{
    if(*top == NULL)
    {
        *top = p;
        (*top)->next = NULL;
    }
    else
    {
        p->next = *top;
        *top = p;
    }
}    

pnode pop(stack* top)
{
    if(*top != NULL)
    {
        stack node = *top;
        pnode t = node->n;
        *top = node->next;
        free(node);
        return t;
    }
    else
        printf("Stack empty!!");
}


/*
    Parse function of the code file 'source' 
*/

void parse(FILE* source)
{
   read_grammar("grammar.txt", g);
   get_ptable(g);
   bool error4 = false;
   
   stack top = NULL;
   token t;
   int i , j , k;
   type_of_character ch;
   ch.tok.type = eof;
   stack node1,node2;
   node1 = create_stack_node_util(true,ch);
   push(node1,&top);
   ch.c = program;
   node2 = create_stack_node_util(false,ch);
   root = node2->n;
   push(node2,&top);
   init_lexer();
   nextToken(source , &t);
   int last_no;
   success = true;

    while(!(top->n->is_terminal && top->n->name.tok.type == eof))
    {
        
        if(t.type == eof) break;

        last_no = t.line;
        
        if(t.type == ERR)
        {
            char type = t.lexeme[6];

            if(type == '2' || type == '3')
            {
                printf("%s\n" , t.lexeme);
                success = false;
                nextToken(source , &t);
            }
            else if(type == '1')
            {
                printf("%s\n" , t.lexeme);
                success = false;
                t.type = ID;
            }
            else if(type == '4')
            {
                printf("%s\n" , t.lexeme);
                success = false;
                error4 = true;
                break;
            }
        }

        if(top->n->is_terminal)
        {
            if(top->n->name.tok.type == t.type)
            {
                pnode head = pop(&top);
                head->name.tok.line = t.line;
                strcpy(head->name.tok.lexeme , t.lexeme);
                nextToken(source , &t);
            }
            else
            {
                if(top->n->name.tok.type != eof)
                printf( "ERROR 5: expecting %s but found %s in line number : %d\n" , str_symbols[(int)top->n->name.tok.type],t.lexeme, t.line) ;
                err = true;
                success = false;
                pop(&top);
                nextToken(source, &t);
            }    
        }
        else
        {
            int ruleno = ptable[(int)top->n->name.c][(int)t.type];
            pnode root = pop(&top);
            root->children = (pnode*)malloc(rulelen[ruleno]*sizeof(pnode));
            root->no_of_children = rulelen[ruleno];
            root->ruleno = ruleno;
    
            if(ruleno == -1)
            {
                int nonterm_in = (int)top->n->name.c;
                while(!(top->n->is_terminal))
                    pop(&top);

                while((t.type != eof) && (!followsets[nonterm_in][(int)t.type]) && (!firstsets[nonterm_in][(int)t.type] && t.type != SEMICOL))
                {
                    if(t.type == ERR)
                    {
                        char type = t.lexeme[6];

                        if(type == '2' || type == '3')
                        {
                            printf("%s\n" , t.lexeme);
                            success = false;
                            nextToken(source , &t);
                        }
                        else if(type == '1')
                        {
                            printf("%s\n" , t.lexeme);
                            success = false;
                            t.type = ID;
                            continue;
                        }
                    }
                    nextToken(source,&t);
                }    

                if(top->n->name.tok.type != eof)
                printf( "ERROR 6: Missing %s at line number %d\n",str_symbols[(int)top->n->name.tok.type],last_no );
                err = true;
                if(t.type == eof) break;
                nextToken(source,&t);
                success = false;
            }
            else
            {
                for(i = rulelen[ruleno] ; i > 0 ; i--)
                {
                    character cha = g[ruleno][i];

                    if(cha <= range)
                    {
                        type_of_character nonterm;
                        nonterm.c = cha;
                        stack child = create_stack_node_util(false,nonterm);
                        root->children[i-1] = child->n;
                        child->n->parent = root;
                        push(child,&top);
                    }
                    else
                    {
                        type_of_character term;
                        token tk;
                        tk.type = cha;
                        tk.line = -1;
                        strcpy(tk.lexeme , "EMPTY");
                        term.tok = tk;
                        stack child = create_stack_node_util(true,term);
                        root->children[i-1] = child->n;
                        child->n->parent = root;
                        if(cha != eps)
                        push(child,&top);
                    }
                }
            }

        }
    }

        while(!(top->n->is_terminal && top->n->name.tok.type == eof))
        {
            if(top->n->is_terminal)
            {
                if(!error4)
                printf( "ERROR 6 : missing %s at line number %d\n" , str_symbols[(int)top->n->name.tok.type],top->n->name.tok.line );
                err = true;
                success = false;
                pop(&top);
            }

            else
            {
                if(!firstsets[(int)top->n->name.c][(int)eps])
                {
                    if(!error4)
                    printf( "ERROR 6 : missing %s\n" , str_symbols[(int)top->n->name.c] );
                    err = true;
                    pop(&top);
                    success = false;
                }
                else
                {
                    int ruleno = ptable[(int)top->n->name.c][(int)eof];
                    pnode root = pop(&top);
                    root->children = (pnode*)malloc(rulelen[ruleno]*sizeof(pnode));
                    root->no_of_children = rulelen[ruleno];
                    root->ruleno = ruleno;
                    type_of_character term;
                    token tk;
                    tk.type = eps;
                    tk.line = -1;
                    strcpy(tk.lexeme , "EMPTY");
                    term.tok = tk;
                    stack child = create_stack_node_util(true,term);
                    root->children[0] = child->n;
                    child->n->parent = root;
                    //pop(&top);     
                }
            }
        }
    //}
    
    if(success)
     printf(ANSI_COLOR_GREEN "Input source code is syntactically correct\n" ANSI_COLOR_RESET);   

}

/*
    Utility string copier without '\0'
*/

void strcpy2(char* a , char* b)
{
    int i;

    for(i = 0 ; b[i] != '\0' ; i++)
        a[i] = b[i];
}

/*
    Prints a particular node of the parse tree
    Equivalent to one row in parsetree output file
*/

void print(pnode root)
{
    if(root->is_terminal)
    {
        token tok = root->name.tok;
        if(tok.type == NUM || tok.type == RNUM)
        {
            
            int i;
            char temp[150];

            for(i = 0 ; i < 150 ; i++)
                temp[i] = ' ';

            temp[144] = '\0';
            temp[143] = '\n'; 

            strcpy2(temp , tok.lexeme);
            char temp2[20];
            sprintf(temp2 , "%d" , tok.line);
            strcpy2(temp + 20 , temp2);
            strcpy2(temp + 35 , str_characters[(int)tok.type]);
            strcpy2(temp + 50 , tok.lexeme);
            strcpy2(temp + 65 , str_characters[(int)root->parent->name.c]);
            strcpy2(temp + 90 , "yes");
            strcpy2(temp + 110 , tok.lexeme);

            printf("%s" , temp);
           
        }
        else
        {
            int i;
            char temp[150];

            for(i = 0 ; i < 150 ; i++)
                temp[i] = ' ';

            temp[144] = '\0';
            temp[143] = '\n'; 

            strcpy2(temp , tok.lexeme);
            char temp2[20];
            sprintf(temp2 , "%d" , tok.line);
            strcpy2(temp + 20 , temp2);
            strcpy2(temp + 35 , str_characters[(int)tok.type]);
            strcpy2(temp + 50 , "----");
            strcpy2(temp + 65 , str_characters[(int)root->parent->name.c]);
            strcpy2(temp + 90 , "yes");
            strcpy2(temp + 110 , tok.lexeme);
            
            printf("%s" , temp);
        }
    }
    else
    {
        if(root->parent == NULL)
        {
            int i;
            char temp[150];

            for(i = 0 ; i < 150 ; i++)
                temp[i] = ' ';

            temp[144] = '\0';
            temp[143] = '\n'; 

            strcpy2(temp , "----");
            strcpy2(temp + 20 , "----");
            strcpy2(temp + 35 , "----");
            strcpy2(temp + 50 , "----");
            strcpy2(temp + 65 , "ROOT");
            strcpy2(temp + 90 , "no");
            strcpy2(temp + 110 , str_characters[(int)root->name.c]);
            
            printf("%s" , temp);            
        }
        else
        {
             int i;
            char temp[150];

            for(i = 0 ; i < 150 ; i++)
                temp[i] = ' ';

            temp[144] = '\0';
            temp[143] = '\n'; 

            strcpy2(temp , "----");
            strcpy2(temp + 20 , "----");
            strcpy2(temp + 35 , "----");
            strcpy2(temp + 50 , "----");
            strcpy2(temp + 65 , str_characters[(int)root->parent->name.c]);
            strcpy2(temp + 90 , "no");
            strcpy2(temp + 110 , str_characters[(int)root->name.c]);
            
            printf("%s" , temp);     
        }
    }
}

/*
   Prints parse tree - utility and main function
*/

void printParseTreeUtil(pnode root)
{
    int i;
    // if(root->no_of_children > 0)
    //     printParseTreeUtil(root->children[0],fil);
   
    print(root);
    nodesp++;
    
    for(i = 0 ; i < root->no_of_children ; i++)
        printParseTreeUtil(root->children[i]);
}

void printParseTree()
{
    if(!success)
    {
        printf(ANSI_COLOR_RED "Parse tree cannot be constructed as parsing was unsuccessful\n" ANSI_COLOR_RESET);
        return ;
    }
    
    nodesp = 0;
    int i;
    char temp[150];

    for(i = 0 ; i < 150 ; i++)
        temp[i] = ' ';

    temp[144] = '\0';
    temp[143] = '\n'; 

    strcpy2(temp , "lexemeCurrentNode");
    strcpy2(temp + 20 , "lineno");
    strcpy2(temp + 35 , "token");
    strcpy2(temp + 50 , "valueIfNumber");
    strcpy2(temp + 65 , "parentNodeSymbol");
    strcpy2(temp + 90 , "isLeafNode(yes/no)");
    strcpy2(temp + 110 , "NodeSymbol");

        printf(ANSI_COLOR_MAGENTA "\n");

    printf("%s" , temp);
        printf(ANSI_COLOR_RESET);
    for(i = 0 ; i < 150 ; i++)
        temp[i] = '_';
    temp[144] = '\0';
    temp[143] = '\n';

    printf("%s" , temp);


    printParseTreeUtil(root);
    printf(ANSI_COLOR_GREEN "Parse table generated successfully\n" ANSI_COLOR_RESET);
}