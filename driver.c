// Group Number : 47
// Anirudh Kumar Bansal 2014A7S081P
// Lakshit Bhutani 2014A7PS095P 

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "lexer.h"
#include "parser.h"
#include "symboltable.h"
#include "ast.h"
#include "semantic.h"
#include "codegen.h" 

/*
	MAIN DRIVER CODE
*/

int main(int argc, char *argv[])
{
	int choice;
	bool parsed = false;
	bool symt = false;
	bool ast = false;
	bool sem = false;
	insidefor = false;
	insideswitch = false;
	err = false;

	if(argc != 3)
	{
		printf(ANSI_COLOR_RED "Please enter the source code file and intended asm file in the command line\n" ANSI_COLOR_RESET);
		return 0;
	}

	char code[50];
	strcpy(code , argv[1]);

	printf(ANSI_COLOR_BLUE "Level 4 : Symbol Table, AST, Type Checking, Semantic Rules and Code Generation works\n\n" ANSI_COLOR_RESET);
	printf(ANSI_COLOR_RED "NOTE : The language accepts static array indices and will not report it as an error. Assembly code will be successfully generated in this case. It also handles arrays used with print() or get_value() functions.\n\n");
	printf(ANSI_COLOR_MAGENTA "1. All tree traversals (Parse Tree and AST) are preorder in nature.\n2. Expressions are evaluated using Right associativity.\n\n" ANSI_COLOR_RESET);
	printf(ANSI_COLOR_GREEN "\nPress options for the defined tasks : " ANSI_COLOR_RESET);
	printf(ANSI_COLOR_YELLOW "\n0: EXIT\n1: Print token list\n2: Parse the code and print parse tree\n3: Create a symbol table\n4: Create and print AST\n5: Memory allocation (Parse tree v/s AST)\n6: Perform semantic analysis\n7: Generate assembly code\nChoice : " ANSI_COLOR_RESET);
	scanf("%d" , &choice);

	while(choice != 0)
	{
		if(choice == 1)
		{
			FILE* f = fopen(code , "r");

			if(f == NULL)
		    {
		        printf("File %s does not exist!\n" , code);
		        return 0;
		    }   
		    else
		    {
				printf("The tokens are : \n\n");
				token t;
				init_lexer();

				while(1)
				{
					nextToken(f , &t);

					if(t.type == eof)
						break;

					printf("Token %s of type %s found at line number %d\n" , t.lexeme , str_characters[(int)t.type] , t.line);
				}	
		    	
		    }
		    fclose(f);
		}
		else if(choice == 2)
		{
			FILE* f = fopen(code , "r");

			if(f == NULL)
		    {
		        printf("File %s does not exist!\n" , code);
		        return 0;
		    }
		    else
		    {
		    	parse(f);
		    	printParseTree();
		    	fclose(f);
		    	parsed = true;
		    }
		}
		else if(choice == 3)
		{
			if(!parsed)
				printf(ANSI_COLOR_BLUE "Please parse the code using option 2 first\n" ANSI_COLOR_RESET);
			if(err)
				printf(ANSI_COLOR_RED "Error exists in previous modules\n" ANSI_COLOR_RESET);
			else
			{
				init_sym_table();
				create_symboltable(root);
				symt = true;
				if(!err)
					print_symboltable_util();  			
			}
		}
		else if(choice == 4)
		{
			if(!parsed)
				printf(ANSI_COLOR_BLUE "Please parse the code using option 2 first\n" ANSI_COLOR_RESET);
			else if(!symt)
				printf(ANSI_COLOR_BLUE "Please create symbol table using option 3 first\n" ANSI_COLOR_RESET);
			else if(err)
				printf(ANSI_COLOR_RED "Error exists in previous modules\n"ANSI_COLOR_RESET);
			else
			{
				ast = true;
				read_ast_rules();
				init_ast();
				create_ast(root,astroot);

				if(!err)
					printASTree();
			}

		}
		else if(choice == 5)
		{
			if(!parsed)
				printf(ANSI_COLOR_BLUE "Please parse the code using option 2 first\n" ANSI_COLOR_RESET);
			else if(!symt)
				printf(ANSI_COLOR_BLUE "Please create symbol table using option 3 first\n" ANSI_COLOR_RESET);
			else if(!ast)
				printf(ANSI_COLOR_BLUE "Please create ASTree using option 4 first\n" ANSI_COLOR_RESET);
			else if(err)
				printf(ANSI_COLOR_RED "Error exists in previous modules\n"ANSI_COLOR_RESET);
			else
			{
				comprr();
			}

		}
		else if(choice == 6)
		{
			if(!parsed)
				printf(ANSI_COLOR_BLUE "Please parse the code using option 2 first\n" ANSI_COLOR_RESET);
			else if(!symt)
				printf(ANSI_COLOR_BLUE "Please create symbol table using option 3 first\n" ANSI_COLOR_RESET);
			else if(!ast)
				printf(ANSI_COLOR_BLUE "Please create ASTree using option 4 first\n" ANSI_COLOR_RESET);
			else if(err)
				printf(ANSI_COLOR_RED "Error exists in previous modules\n"ANSI_COLOR_RESET);
			else
			{
				semantic_analysis(astroot);
				if(!err)
					printf(ANSI_COLOR_GREEN "Source code is semantically correct\n" ANSI_COLOR_RESET);
				sem = true;				
			}
		}
		else if(choice == 7)
		{
			if(!parsed)
				printf(ANSI_COLOR_BLUE "Please parse the code using option 2 first\n" ANSI_COLOR_RESET);
			else if(!symt)
				printf(ANSI_COLOR_BLUE "Please create symbol table using option 3 first\n" ANSI_COLOR_RESET);
			else if(!ast)
				printf(ANSI_COLOR_BLUE "Please create ASTree using option 4 first\n" ANSI_COLOR_RESET);
			else if(!sem)
				printf(ANSI_COLOR_BLUE "Please perform semantic analysis using option 6 first\n" ANSI_COLOR_RESET);
			else if(err)
				printf(ANSI_COLOR_RED "Error exists in previous modules\n"ANSI_COLOR_RESET);
			else
			{
				gen_code_util(argv[2]);
				printf(ANSI_COLOR_GREEN "Assembly code generated successfully in " ANSI_COLOR_RESET);
				printf("%s\n" , argv[2]);
			}	
		}
		int i, j;

		for(i = 1 ; i < 100 ; i++)
		{
			printf(ANSI_COLOR_CYAN "-" ANSI_COLOR_RESET);
		}
		printf("\n");
 
	printf(ANSI_COLOR_YELLOW "\n0: EXIT\n1: Print token list\n2: Parse the code and print parse tree\n3: Create a symbol table\n4: Create and print AST\n5: Memory allocation (Parse tree v/s AST)\n6: Perform semantic analysis\n7: Generate assembly code\nChoice : " ANSI_COLOR_RESET);
		scanf("%d" , &choice);

	}
	return 0;

}
