// Group Number : 47
// Anirudh Kumar Bansal 2014A7S081P
// Lakshit Bhutani 2014A7PS095P 

#include "lexer.h"
#include "parser.h"
#include "symboltable.h"
#include "ast.h"

// char * char_type[] = {"INTEGER" , "REAL" , "BOOLEAN"};
// char * char_btype[] = {"ROOT","FUNCTION","WHILE","FOR","SWITCH"};

void read_ast_rules()
{
    FILE *grammarFile;
    int ruleNo, ruleLen, i;
    char id[MAX_ID_SIZE];

    grammarFile = fopen("ast_rules.txt", "r");

    if(grammarFile == NULL)
    {
    	printf( "AST rules not found in ast_rules.txt\n" );
    	err = true;
        return;
    }
    ruleNo = 0;

    while(ruleNo < NO_OF_RULES) 
    {
        fscanf(grammarFile, "%s", id);
        character nonterm = identify(id);

        while(1)
        {
            gast[ruleNo][0]=nonterm;
            ruleLen = 0;
            
            while(1)
            {
                fscanf(grammarFile, "%s", id);
                
                if((strcmp(id,"|") == 0) || (strcmp(id,".") == 0)) 
                    break;
                
                gast[ruleNo][ruleLen+1] = identify(id);
                ruleLen++;
            }

            astrulelen[ruleNo] = ruleLen;
            ruleNo++;
            
            if(strcmp(id,".") == 0) 
                break;
        }
	}
}	

void print_ast_rules()
{
    for(int i = 0 ; i < NO_OF_RULES ; i++)
    {
        printf("%s -> " , str_characters[gast[i][0]]);

        for(int j = 1 ; j <= astrulelen[i] ; j++)
            printf("%s " , str_characters[gast[i][j]]);
        printf("\n");
    }
}

void init_ast()
{
    astroot = (astptr)malloc(sizeof(astnode));
    astroot->parent = NULL;
    bcur = broot;
}

void create_ast(pnode root, astptr aroot)
{
    aroot->is_terminal = root->is_terminal;
    aroot->name = root->name;
    aroot->children = NULL;
    aroot->no_of_children = 0;
    aroot->scope = bcur;
    // if(root->is_terminal)
    //     printf("%s %d\n",root->name.tok.lexeme,root->name.tok.line);
    // else
    //     printf("%s\n",str_characters[(int)root->name.c]);
    
    if(!root->is_terminal)
    {
        if(root->name.c == module)
        {
            // bptr temp = bcur->children[bcur->counter];
            // bcur->counter++;
            // bcur = temp;  
            char fname[50];
            strcpy(fname , root->children[2]->name.tok.lexeme);
            int i;
            //printf("%s\n",fname);
            for(i = 0; i < bcur->no_of_children ; i++)
            {
                if(strcmp(bcur->children[i]->f.name,fname) == 0)
                {
                    bcur->counter = i+1;
                    bcur = bcur->children[i];
                    //printf("%s %d\n",fname,bcur->counter);
                }
            }
        }
        else if(root->name.c == driverModule)
        {
            int i;
            for(i = 0; i < bcur->no_of_children ; i++)
            {
                if(strcmp(bcur->children[i]->f.name,"DRIVER") == 0)
                {
                    bcur->counter = i+1;
                    bcur = bcur->children[i];
                    //printf(" DRIVER %d\n",bcur->counter);
                }
            }
        }
        int no = root->ruleno;
        int newlen = astrulelen[no];
        int oldlen = root->no_of_children;
        aroot->ruleno = root->ruleno;
        aroot->no_of_children = newlen;
        aroot->scope = bcur;
        aroot->children = (astptr*)malloc(sizeof(astptr)*newlen);

        int i;
        int j;
        for(i = 0 ; i < newlen ; i++)
        {
            aroot->children[i] = (astptr)malloc(sizeof(astnode));
            aroot->children[i]->parent = aroot;
        }

        i = 0;
        j = 0;

        while(i < newlen && j < oldlen)
        {
            if(root->children[j]->is_terminal)
            {
                character temp = root->children[j]->name.tok.type;
                if(temp == gast[no][i+1])
                {
                    create_ast(root->children[j],aroot->children[i]);
                    i++;
                    j++;
                }
                else
                    j++;
            }
            else
            {
                character temp = root->children[j]->name.c;
                if(temp == gast[no][i+1])
                {
                    create_ast(root->children[j],aroot->children[i]);
                    i++;
                    j++;
                }
                else
                    j++;

            }
        }

    }
    /*else if(root->is_terminal && (root->name.tok.type == ID) && (root->parent->name.c == module))
    {
        bptr temp = bcur->children[bcur->counter];
        bcur->counter++;
        bcur = temp;   
    }*/
    else if(root->is_terminal && (root->name.tok.type == START) && (root->parent->name.c != moduleDef))
    {
        bptr temp = bcur->children[bcur->counter];
        bcur->counter++;
        bcur = temp;
        // printf("1\n");
    }
    else if(root->is_terminal && (root->name.tok.type == END))
    {
        bcur = bcur->parent;
    }            
}

/*
    Prints a particular node of the abstract syntax tree
    Equivalent to one row in astree output file
*/

void print_ast(astptr root)
{
    char * char_types[] = {"INTEGER" , "REAL" , "BOOLEAN"};
    char * char_btypes[] = {"ROOT","FUNCTION","WHILE","FOR","SWITCH"};
    //fprintf(fil,"%s ",char_btypes[(int)root->scope->type]);
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
   Prints abstract syntax tree - utility and main function
*/

void printASTreeUtil(astptr root)
{
    int i;
     
    print_ast(root);
    nodesa++;
    
    for(i = 0 ; i < root->no_of_children ; i++)
        printASTreeUtil(root->children[i]);
}

void printASTree()
{
    int i;
    char temp[150];

    for(i = 0 ; i < 150 ; i++)
        temp[i] = ' ';

    temp[144] = '\0';
    temp[143] = '\n'; 

    nodesa = 0;
    strcpy2(temp , "lexemeCurrentNode");
    strcpy2(temp + 20 , "lineno");
    strcpy2(temp + 35 , "token");
    strcpy2(temp + 50 , "valueIfNumber");
    strcpy2(temp + 65 , "parentNodeSymbol");
    strcpy2(temp + 90 , "isLeafNode(yes/no)");
    strcpy2(temp + 110 , "NodeSymbol");

    printf(ANSI_COLOR_MAGENTA);
    printf("%s" , temp);
    printf(ANSI_COLOR_RESET);

    for(i = 0 ; i < 150 ; i++)
        temp[i] = '_';
    temp[144] = '\0';
    temp[143] = '\n';
    printf("%s" , temp );

    printASTreeUtil(astroot);
    printf(ANSI_COLOR_GREEN "AST generated successfully\n" ANSI_COLOR_RESET);
}


void comprr()
{
    int tnodesp = nodesp;
    int tmemp = tnodesp * sizeof(struct parse_tree_node);

    printf("Parse tree : Number of nodes = %d Allocated Memory = %d Bytes\n" , tnodesp , tmemp);

    int tnodesa = nodesa;
    int tmema = tnodesa * (sizeof(struct astnode) - sizeof(bptr) - sizeof(typ) - sizeof(int));

    printf("Abstract syntax tree : Number of nodes = %d Allocated Memory = %d Bytes\n" , tnodesa , tmema);

    double compr = ((double)(tmemp - tmema)) / tmemp;
    compr = compr * 100;

    printf("Compression percentage = %lf %%\n" , compr);
}
