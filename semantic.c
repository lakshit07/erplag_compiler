// Group Number : 47
// Anirudh Kumar Bansal 2014A7S081P
// Lakshit Bhutani 2014A7PS095P 
// 
#include "lexer.h"
#include "parser.h"
#include "symboltable.h"
#include "ast.h"

var_type get_type_id(char* name , bptr b, int lineno)
{	
	if(b == NULL)
	{
		var_type v;
		v.type = NOTDEF;
		return v;
	}

	int pos = hash(name)%100;
	var_type vt;

	if(b->var_table[pos] == NULL)
	{
		return get_type_id(name , b->parent, lineno);
	}
	else
	{
		varptr temp = b->var_table[pos];

		while(temp != NULL)
		{
			if(strcmp(temp->var.name , name) == 0 && (temp->var.line <= lineno))
			{
				vt = (temp->var.type);
				return vt;
			}
			temp = temp->next;
		}
		return get_type_id(name , b->parent, lineno);
	}
}

varptr get_id(char* name , bptr b)
{	
	if(b == NULL)
		return NULL;

	int pos = hash(name)%100;

	if(b->var_table[pos] == NULL)
	{
		return get_id(name , b->parent);
	}
	else
	{
		varptr temp = b->var_table[pos];

		while(temp != NULL)
		{
			if(strcmp(temp->var.name , name) == 0)
			{
				return temp;
			}
			temp = temp->next;
		}
		return get_id(name , b->parent);
	}
}

void set_defined(varptr v , bptr b)
{
	int pos = hash(v->var.name)%100;

	if(b->var_table[pos] == NULL)
	{
		set_defined(v , b->parent);
	}
	else
	{
		varptr temp = b->var_table[pos];

		while(temp != NULL)
		{
			if(strcmp(temp->var.name , v->var.name) == 0)
			{
				temp->var.assigned = true;
				//printf("%s defined\n" , temp->var.name);
				return;
			}
			temp = temp->next;
		}
		return set_defined(v , b->parent);
	}
}

function check_fn(char* fname, bptr b, int lineno)
{
	bptr temp = b;
	bptr temp2 = b;
	int i;

	while(temp->parent != NULL)
		temp = temp->parent;

	while(temp2->parent->type != ROOT)
		temp2 = temp2->parent;

	if(strcmp(temp2->f.name , fname) == 0)
	{
		printf( "ERROR : Recursion call to function %s in line %d not permitted\n" , fname, lineno );
		err = true;	
		function f;
		f.input_count = -1;
		return f;	
	}

	bool found = false;

	for(i = 0 ; i < temp->no_of_children ; i++)
	{
		if(strcmp(temp->children[i]->f.name , fname) == 0)
		{
			found = true;
			break;
		}	
	}

	if(found)
	{
		if(lineno >= temp->children[i]->f.occ)
		{
			return temp->children[i]->f;
		}
		else
		{
			printf( "ERROR : Function %s called on line %d not declared\n" , fname, lineno );
			err = true;
		}

	}
	else
	{
		printf( "ERROR : Function %s called on line %d not declared\n" , fname, lineno );
		err = true;
	}
	function f;
	f.input_count = -1;
	return f;
}

bool output_type(astptr root , function *f)
{
	int i;

	if(root->is_terminal && root->name.tok.type == ID)
	{
		varptr vptr = (varptr)malloc(sizeof(varnode));
		vptr->var.type = get_type_id(root->name.tok.lexeme , root->scope, root->name.tok.line); 
		f->output_count++;
		
		if(vptr->var.type.type == NOTDEF)
		{
			printf( "ERROR : No such variable %s in line %d\n" , root->name.tok.lexeme , root->name.tok.line );
			err = true;
			return false;
		}

		strcpy(vptr->var.name, root->name.tok.lexeme);
		vptr->var.line = root->name.tok.line;
		if(vptr->var.type.isarr)
		{
			printf( "ERROR : Array %s cannot be returned in call to function at line %d\n",vptr->var.name,vptr->var.line );
			err = true;
			return false;
		}

		vptr->next = NULL;

		if(f->output_type == NULL)
			f->output_type = vptr;
		else
		{
			varptr temp = f->output_type;
			while(temp->next != NULL)
			{
				temp = temp->next;
			}
			temp->next = vptr;
		}		
	}

	for(i = 0 ; i < root->no_of_children ; i++)
	{
		bool res = output_type(root->children[i] , f);
		if(!res)
			return false;
	}
	return true;
}

void input_type(astptr root , function *f)
{
	int i;

	if(root->is_terminal && root->name.tok.type == ID)
	{
		var_type ty = get_type_id(root->name.tok.lexeme,root->scope,root->name.tok.line);

		if(ty.type == NOTDEF)
		{
			printf( "ERROR : No such variable %s in line %d\n" , root->name.tok.lexeme , root->name.tok.line );
			err = true;
			return;
		}
		
		if(!ty.isarr)
		{
			f->input_count++;

			varptr vptr = (varptr)malloc(sizeof(varnode));
			strcpy(vptr->var.name, root->name.tok.lexeme);
			vptr->var.line = root->name.tok.line;
			vptr->var.type = get_type_id(root->name.tok.lexeme , root->scope, root->name.tok.line);
			vptr->var.scope = 0;
			vptr->var.offset = 0; 
			vptr->next = NULL;

			if(f->input_type == NULL)
				f->input_type = vptr;
			else
			{
				varptr temp = f->input_type;
				while(temp->next != NULL)
				{
					temp = temp->next;
				}
				temp->next = vptr;
			}
		}
		else
		{
			int j;
			for(j = ty.low ; j <= ty.high ; j++)
			{
				f->input_count++;

				varptr vptr = (varptr)malloc(sizeof(varnode));
				strcpy(vptr->var.name, root->name.tok.lexeme);
				vptr->var.line = root->name.tok.line;
				vptr->var.type = get_type_id(root->name.tok.lexeme , root->scope, root->name.tok.line);
				vptr->var.scope = 0; 
				vptr->var.offset = j;
				vptr->next = NULL;

				if(f->input_type == NULL)
					f->input_type = vptr;
				else
				{
					varptr temp = f->input_type;
					while(temp->next != NULL)
					{
						temp = temp->next;
					}
					temp->next = vptr;
				}
			}
		}		
	}

	for(i = 0 ; i < root->no_of_children ; i++)
		input_type(root->children[i] , f);
}

bool compat_fn(function f1, function f2, int line)
{
	char * char_type[] = {"INTEGER" , "REAL" , "BOOLEAN"};	

	if(f1.input_count != f2.input_count)
	{
		printf( "ERROR : Count of input variables passed to the call of function %s do not match on line %d\n" , f2.name , line );
		err = true;
		return false;
	}

	if(f1.output_count != f2.output_count)
	{
		printf( "ERROR : Count of output variables returned from call to function %s do not match on line %d\n" , f2.name , line );
		err = true;
		return false;
	}

	varptr tempi1 = f1.input_type;
	varptr tempo1 = f1.output_type;
	varptr tempi2 = f2.input_type;
	varptr tempo2 = f2.output_type;
	int arg_no = 1;
	while(tempi1 != NULL)
	{
		if(tempi1->var.type.type != tempi2->var.type.type)
		{
			printf( "ERROR : Input argument %d of function %s at line %d: Expected type %s but found type %s\n",arg_no,f2.name,line,char_type[(int)tempi2->var.type.type],char_type[(int)tempi1->var.type.type] );
			err = true;
			return false;
		}
		arg_no++;
		tempi1 = tempi1->next;
		tempi2 = tempi2->next;
	}

	arg_no = 1;
	while(tempo1 != NULL)
	{
		if(tempo1->var.type.type != tempo2->var.type.type)
		{
			printf( "ERROR : Output argument %d of function %s at line %d : Expected type %s but found type %s\n",arg_no,f2.name,line,char_type[(int)tempo2->var.type.type],char_type[(int)tempo1->var.type.type] );
			err = true;
			return false;
		}
		arg_no++;
		tempo1 = tempo1->next;
		tempo2 = tempo2->next;
	}


	return true;
}

void get_asttype(astptr aptr)
{
	// if(aptr->is_terminal)
	//  	printf("%s %d\n",aptr->name.tok.lexeme,aptr->name.tok.line);
	// else
	//  	printf("%s\n",str_characters[(int)aptr->name.c]);
	char * char_type[] = {"INTEGER" , "REAL" , "BOOLEAN"};
	if(aptr->name.c == expression)
	{
		if((!aptr->children[0]->is_terminal) && (aptr->children[0]->name.c == arithmeticOrBooleanExpr))
		{
			get_asttype(aptr->children[0]);
			aptr->type = aptr->children[0]->type;
		}
		else if(aptr->children[1]->name.c == arithmeticExpr)
		{
			get_asttype(aptr->children[1]);
			aptr->type = aptr->children[1]->type;
		}
	}
	else if(aptr->name.c == arithmeticOrBooleanExpr)
	{
		get_asttype(aptr->children[0]);
		if(aptr->children[0]->type == NOTDEF)
			aptr->type = NOTDEF;
		else if(aptr->children[1]->children[0]->name.tok.type == eps)
			aptr->type = aptr->children[0]->type;
		else if(aptr->children[0]->type != BOOL)
		{
			aptr->type = NOTDEF;
			printf( "ERROR : Expected boolean expression on left side of operator %s at line %d\n",aptr->children[1]->children[0]->children[0]->name.tok.lexeme,aptr->children[1]->children[0]->children[0]->name.tok.line );
			err = true;
		}
			//printf("Invalid expression type\n");
		else
		{
			get_asttype(aptr->children[1]);
			if(aptr->children[1]->type == NOTDEF)
				aptr->type = NOTDEF;
				//printf("Invalid expression type\n");
			else if(aptr->children[1]->type == BOOL)
				aptr->type = BOOL;
		}
	}
	else if(aptr->name.c == N7)
	{
		get_asttype(aptr->children[1]);
		if(aptr->children[1]->type == BOOL)
		{
			if(aptr->children[2]->children[0]->name.tok.type == eps)
				aptr->type = BOOL;
			else
			{
				get_asttype(aptr->children[2]);
				if(aptr->children[2]->type == NOTDEF)
					aptr->type = NOTDEF;
				else if(aptr->children[2]->type == BOOL)
					aptr->type = BOOL;
			}
		}
		else
		{ 
			aptr->type = NOTDEF;
			printf( "ERROR : Expected boolean expression on right side of operator %s at line %d\n",aptr->children[0]->children[0]->name.tok.lexeme,aptr->children[0]->children[0]->name.tok.line );
			err = true;
		}

	}
	else if(aptr->name.c == AnyTerm)
	{
		get_asttype(aptr->children[0]);
		if(aptr->children[0]->type == NOTDEF)
			aptr->type = NOTDEF;
		else if(aptr->children[1]->children[0]->name.tok.type == eps)
			aptr->type = aptr->children[0]->type;
		else if((aptr->children[0]->type != INT) && (aptr->children[0]->type != RE))
		{
			aptr->type = NOTDEF;
			printf( "ERROR : Expected integer or real expression on left side of operator %s at line %d\n",aptr->children[1]->children[0]->children[0]->name.tok.lexeme,aptr->children[1]->children[0]->children[0]->name.tok.line );
			err = true;

		}
			//printf("Invalid expression type\n");
		else
		{
			get_asttype(aptr->children[1]);
			if(aptr->children[1]->type == NOTDEF)
				aptr->type = NOTDEF;
			else if(aptr->children[0]->type != aptr->children[1]->type)
			{
				//printf("Invalid expression type\n");
				aptr->type = NOTDEF;
				printf( "ERROR : Type mismatch between left side : %s and right side : %s of operator %s at line %d\n",char_type[(int)aptr->children[0]->type],char_type[(int)aptr->children[1]->type],aptr->children[1]->children[0]->children[0]->name.tok.lexeme,aptr->children[1]->children[0]->children[0]->name.tok.line );
				err = true;

			}
			else 
				aptr->type = BOOL;
		}
	}
	else if(aptr->name.c == N8)
	{
		get_asttype(aptr->children[1]);
		if((aptr->children[1]->type == BOOL) || (aptr->children[1]->type == NOTDEF))
		{
			aptr->type = NOTDEF;
			printf( "ERROR : Expected integer or real expression on right side of operator %s at line %d\n",aptr->children[0]->children[0]->name.tok.lexeme,aptr->children[0]->children[0]->name.tok.line );
			err = true;		
		}
		else
		{
			if(aptr->children[2]->children[0]->name.tok.type == eps)
				aptr->type = aptr->children[1]->type;
			else
			{
				get_asttype(aptr->children[2]);
				if(aptr->children[2]->type == NOTDEF)
					aptr->type = NOTDEF;
				else if(aptr->children[1]->type != aptr->children[2]->type)
				{
					aptr->type = NOTDEF;
					printf( "ERROR : Type mismatch between left side : %s and right side : %s of operator %s at line %d\n",char_type[(int)aptr->children[1]->type],char_type[(int)aptr->children[2]->type],aptr->children[0]->children[0]->name.tok.lexeme,aptr->children[0]->children[0]->name.tok.line );
					err = true;		
				}
				else
					aptr->type = aptr->children[2]->type;
			}
		}
	}
	else if(aptr->name.c == arithmeticExpr)
	{
		get_asttype(aptr->children[0]);
		if(aptr->children[0]->type == NOTDEF)
			aptr->type = NOTDEF;
		else if(aptr->children[1]->children[0]->name.tok.type == eps)
			aptr->type = aptr->children[0]->type;
		else if((aptr->children[0]->type != INT) && (aptr->children[0]->type != RE))
		{
			aptr->type = NOTDEF;
			printf( "ERROR : Expected integer or real expression on left side of operator %s at line %d\n",aptr->children[1]->children[0]->children[0]->name.tok.lexeme,aptr->children[1]->children[0]->children[0]->name.tok.line );
			err = true;
		}
		else
		{
			get_asttype(aptr->children[1]);
			if(aptr->children[1]->type == NOTDEF) 
				aptr->type = NOTDEF;
			else if(aptr->children[0]->type != aptr->children[1]->type)
			{
				//printf("Invalid expression type\n");
				aptr->type = NOTDEF;
				printf( "ERROR : Type mismatch between left side : %s and right side : %s of operator %s at line %d\n",char_type[(int)aptr->children[0]->type],char_type[(int)aptr->children[1]->type],aptr->children[1]->children[0]->children[0]->name.tok.lexeme,aptr->children[1]->children[0]->children[0]->name.tok.line );
				err = true;
			}
			else
				aptr->type = aptr->children[1]->type;
		}
	}
	else if(aptr->name.c == N4)
	{
		get_asttype(aptr->children[1]);
		if(aptr->children[1]->type == NOTDEF)
			aptr->type = NOTDEF;
		else if(aptr->children[1]->type == BOOL)
		{
			aptr->type = NOTDEF;
			printf( "ERROR : Expected integer or real expression on right side of operator %s at line %d\n",aptr->children[0]->children[0]->name.tok.lexeme,aptr->children[0]->children[0]->name.tok.line );
			err = true;
		}
		else
		{
			if(aptr->children[2]->children[0]->name.tok.type == eps)
				aptr->type = aptr->children[1]->type;
			else
			{
				get_asttype(aptr->children[2]);
				if(aptr->children[2]->type == NOTDEF)
					aptr->type = NOTDEF;
				else if(aptr->children[1]->type != aptr->children[2]->type)
				{
					aptr->type = NOTDEF;
					printf(  "ERROR : Type mismatch between left side : %s and right side : %s of operator %s at line %d\n",char_type[(int)aptr->children[1]->type],char_type[(int)aptr->children[2]->type],aptr->children[0]->children[0]->name.tok.lexeme,aptr->children[0]->children[0]->name.tok.line );
					err = true;
				}
				else
					aptr->type = aptr->children[2]->type;
			}
		}
	}
	else if(aptr->name.c == term)
	{
		get_asttype(aptr->children[0]);
		if(aptr->children[0]->type == NOTDEF)
			aptr->type = NOTDEF;
		else if(aptr->children[1]->children[0]->name.tok.type == eps)
			aptr->type = aptr->children[0]->type;
		else if((aptr->children[0]->type != INT) && (aptr->children[0]->type != RE))
		{
			aptr->type = NOTDEF;
			printf( "ERROR : Expected integer or real expression on left side of operator %s at line %d\n",aptr->children[1]->children[0]->children[0]->name.tok.lexeme,aptr->children[1]->children[0]->children[0]->name.tok.line );
			err = true;
		}
		else
		{
			get_asttype(aptr->children[1]);
			if(aptr->children[1]->type == NOTDEF)
				aptr->type = NOTDEF;
			else if(aptr->children[0]->type != aptr->children[1]->type)
		    {
				aptr->type = NOTDEF;
				printf( "ERROR : Type mismatch between left side : %s and right side : %s of operator %s at line %d\n",char_type[(int)aptr->children[0]->type],char_type[(int)aptr->children[1]->type],aptr->children[1]->children[0]->children[0]->name.tok.lexeme,aptr->children[1]->children[0]->children[0]->name.tok.line );
				err = true;
			}
			else
				aptr->type = aptr->children[1]->type;
		}
	}
	else if(aptr->name.c == N5)
	{
		get_asttype(aptr->children[1]);
		if(aptr->children[1]->type == NOTDEF)
			aptr->type = NOTDEF;
		else if(aptr->children[1]->type == BOOL)
		{
			aptr->type = NOTDEF;
			printf( "ERROR : Expected integer or real expression on right side of operator %s at line %d\n",aptr->children[0]->children[0]->name.tok.lexeme,aptr->children[0]->children[0]->name.tok.line );
			err = true;
		}
		else
		{
			if(aptr->children[2]->children[0]->name.tok.type == eps)
				aptr->type = aptr->children[1]->type;
			else
			{
				get_asttype(aptr->children[2]);
				if(aptr->children[2]->type == NOTDEF)
					aptr->type = NOTDEF;
				else if(aptr->children[1]->type != aptr->children[2]->type)
				{
					aptr->type = NOTDEF;
					printf( "ERROR : Type mismatch between left side : %s and right side : %s of operator %s at line %d\n",char_type[(int)aptr->children[1]->type],char_type[(int)aptr->children[2]->type],aptr->children[0]->children[0]->name.tok.lexeme,aptr->children[0]->children[0]->name.tok.line );
					err = true;
				}
				else
					aptr->type = aptr->children[2]->type;
			}
		}
	}
	else if(aptr->name.c == factor)
	{
		get_asttype(aptr->children[0]);
		aptr->type = aptr->children[0]->type;
	}
	else if(aptr->name.c == var)
	{
		if(aptr->children[0]->name.tok.type == NUM)
		{
			aptr->type = INT;
			//printf("int %s\n",aptr->children[0]->name.tok.lexeme);
		}
		else if(aptr->children[0]->name.tok.type == RNUM)
		{
			aptr->type = RE;
			//printf("real\n");
		}
		else if(aptr->children[0]->name.tok.type == ID)
		{
			var_type vt = get_type_id(aptr->children[0]->name.tok.lexeme,aptr->children[0]->scope,aptr->children[0]->name.tok.line);
			if(vt.type == NOTDEF)
			{
				aptr->type = NOTDEF;
				printf( "ERROR : No such variable %s in line %d\n" , aptr->children[0]->name.tok.lexeme , aptr->children[0]->name.tok.line );
				err = true;
			}
			else if(aptr->children[1]->children[0]->name.tok.type == eps && vt.isarr)
			{
				aptr->type = NOTDEF;
				printf( "ERROR : Array %s at line %d not allowed in expression\n",aptr->children[0]->name.tok.lexeme,aptr->children[0]->name.tok.line );
				err = true;
			}
			else
				aptr->type = vt.type;
		}
	}
}

void semantic_analysis(astptr root)
{
	// if(root->is_terminal)
	// 	printf("%s %d\n",root->name.tok.lexeme,root->name.tok.line);
	// else
	// 	printf("%s\n",str_characters[(int)root->name.c]);
	char * char_type[] = {"INTEGER" , "REAL" , "BOOLEAN"};

	if(root->is_terminal && root->name.tok.type == ID && root->parent->name.c != module && root->parent->name.c != moduleReuseStmt && root->parent->name.c != moduleDeclaration)
	{
		// var_type vt = get_type_id(root->name.tok.lexeme , root->scope, root->name.tok.line);
		
		// if(vt.type == NOTDEF)
		// {
		// 	printf("ERROR --: No such variable %s in line %d\n" , root->name.tok.lexeme , root->name.tok.line);
		// 	return;
		// }
		var_type vt = get_type_id(root->name.tok.lexeme , root->scope, root->name.tok.line);
		root->type = vt.type;

		if(root->parent->name.c == assignmentStmt)
		{
		
			if(vt.type == NOTDEF)
			{
				printf( "ERROR : No such variable %s in line %d\n" , root->name.tok.lexeme , root->name.tok.line );
				err = true;
			}

			if(root->parent->children[1]->children[0]->name.c == lvalueIDStmt)
			{
				get_asttype(root->parent->children[1]->children[0]->children[0]);
				typ t = root->parent->children[1]->children[0]->children[0]->type;
				if(t == NOTDEF)
				{
					printf( "ERROR : Expression on right side at line %d is not well formed\n",root->name.tok.line );
					err = true;
					return;
				}
				if(vt.isarr || vt.type != t)
				{
					printf( "ERROR : Type mismatch for ID %s at line %d\n",root->name.tok.lexeme,root->name.tok.line );
					err = true;
					return;
				}
				if(insidefor)
				{
					if(strcmp(root->name.tok.lexeme , forvar) == 0)
					{
						printf( "ERROR : Redefinition of index variable %s at line %d\n",root->name.tok.lexeme,root->name.tok.line );
						err = true;
					}	
				}
			}
			else
			{
				var_type vt = get_type_id(root->name.tok.lexeme , root->scope, root->name.tok.line);
		
				if(vt.type == NOTDEF)
				{
					printf( "ERROR : No such variable %s in line %d\n" , root->name.tok.lexeme , root->name.tok.line );
					err = true;
					return;
				}
				get_asttype(root->parent->children[1]->children[0]->children[1]);
				typ t = root->parent->children[1]->children[0]->children[1]->type;
				if(t == NOTDEF)
				{
					printf( "ERROR : Expression on right side at line %d is not well formed\n",root->name.tok.line );
					err = true;
					return;
				}
				if(vt.type != t)
				{
					printf( "ERROR : Type mismatch for ID %s at line %d\n",root->name.tok.lexeme,root->name.tok.line );
					err = true;
					return;
				}
			}

			varptr vpt = get_id(root->name.tok.lexeme , root->scope);
			set_defined(vpt , root->scope);
		}
		else
		{
			if(vt.type == NOTDEF)
			{
				printf( "ERROR : No such variable %s in line %d\n" , root->name.tok.lexeme , root->name.tok.line );
				err = true;
				return;
			}
		}	
	}
	else if(root->is_terminal && root->name.tok.type == ID && root->parent->name.c == moduleReuseStmt)
	{

		function fn = check_fn(root->name.tok.lexeme , root->scope, root->name.tok.line);
		
		if(fn.input_count == -1)
			return;

		function f;
		f.output_count = 0;
		f.output_type = NULL;
		f.input_count = 0;
		f.input_type = NULL;
		bool out_res;

		if(root->parent->children[0]->children[0]->name.c == eps)
		{
			f.output_count = 0;
			f.output_type = NULL;
		}
		else
			out_res = output_type(root->parent->children[0]->children[0] , &f);
		// if(!out_res)
		//  	return;
	

		if(root->parent->children[2]->children[0]->name.c == eps)
		{
			f.input_count = 0;
			f.input_type = NULL;
		}
		else
			input_type(root->parent->children[2] , &f);

		bool compat = compat_fn(f , fn, root->name.tok.line);

		// if(!compat)
		// 	return;

		//varptr vpt = get_id(root->name.tok.lexeme , root->scope);
		varptr vpt = f.output_type;

		while(vpt != NULL)
		{
			set_defined(vpt , root->scope);

			if(insidefor)
			{
				if(strcmp(vpt->var.name , forvar) == 0)
				{
					printf( "ERROR : Redefinition of index variable %s at line %d\n", forvar ,root->name.tok.line );
					err = true;
				}	
			}

			vpt = vpt->next;
		}

		// function func = f; 
		// varptr tempi = func.input_type;

		// printf("INPUT : %d\n",func.input_count);
		// while(tempi != NULL)
		// {
		// 	printf("%s %s %d %d %d\n" , tempi->var.name , char_type[(int)tempi->var.type.type] , tempi->var.line , tempi->var.scope, tempi->var.offset);
		// 	tempi = tempi->next;
		// }
		// varptr tempo = func.output_type;

		// printf("OUTPUT : %d\n",func.output_count);
		// while(tempo != NULL)
		// {
		// 	printf("%s %s %d %d %d\n" , tempo->var.name , char_type[(int)tempo->var.type.type] , tempo->var.line , tempo->var.scope, tempo->var.offset);
		// 	tempo = tempo->next;
		// }

	}
	else if(root->is_terminal && (root->name.tok.type == END) && (root->parent->name.c == moduleDef))
	{
		function f = root->scope->f;
		varptr out_ptr = f.output_type;
		while(out_ptr != NULL)
		{
			varptr t = get_id(out_ptr->var.name,root->scope);
			if(t != NULL && !t->var.assigned)
			{
				printf( "ERROR : Output parameter %s of function %s is not defined\n",out_ptr->var.name,f.name );
				err = true;
			}
			out_ptr = out_ptr->next;
		}
	}
	else if(root->is_terminal && (root->name.tok.type == START) && (root->parent->name.c == iterativeStmt))
	{
		if(root->parent->children[0]->name.tok.type == FOR)
		{
			insidefor = true;
			strcpy(forvar , root->parent->children[1]->name.tok.lexeme);
		}
		else
		{
			get_asttype(root->parent->children[1]);
			if(root->parent->children[1]->type != BOOL)
			{
				printf( "ERROR : Conditional expression of while loop in line %d must be of boolean type\n",root->parent->children[0]->name.tok.line );
				err = true;
			}
		}		
	}
	else if(root->is_terminal && (root->name.tok.type == START) && (root->parent->name.c == conditionalStmt))
	{
		insideswitch = true;
		var_type vt = get_type_id(root->parent->children[0]->name.tok.lexeme,root->scope,root->parent->children[0]->name.tok.line);
		if(vt.type == NOTDEF)
		{
			printf( "ERROR : No such variable %s in line %d\n" , root->parent->children[0]->name.tok.lexeme , root->parent->children[0]->name.tok.line );
			err = true;
			return;
		}
		sw_type = vt.type;
		if(sw_type == RE)
		{
			printf( "ERROR : Switch case variable cannot be real at line %d\n",root->parent->children[0]->name.tok.line );
			err = true;
		}
		else if(vt.isarr)
		{
			printf( "ERROR : Switch case variable %s cannot be of array type at line %d\n",root->parent->children[0]->name.tok.lexeme,root->parent->children[0]->name.tok.line );
			err = true;
		}
		else if(sw_type == INT)
		{
			if(root->parent->children[3]->children[0]->name.tok.type == eps)
			{
				printf( "ERROR : Default case statement required in switch statement starting from line %d\n",root->parent->children[0]->name.tok.line );
				err = true;
			}	
		}
		else if(sw_type == BOOL)
		{
			if(root->parent->children[3]->children[0]->name.tok.type != eps)
			{
				printf( "ERROR : Default case statement not allowed in switch statement starting from line %d\n",root->parent->children[0]->name.tok.line );	
				err = true;
			}
		}
	}
	else if(!root->is_terminal && root->name.c == value)
	{
		if(sw_type == INT && root->children[0]->name.tok.type != NUM)
		{
			printf( "ERROR : Expected integer case label at line %d\n",root->children[0]->name.tok.line );
			err = true;
		}	
		else if(sw_type == BOOL && root->children[0]->name.tok.type == NUM)
		{
			printf( "ERROR : Expected boolean case label at line %d\n",root->children[0]->name.tok.line );
			err = true;
		}	
	}

	else if(root->is_terminal && (root->name.tok.type == END) && (root->parent->name.c == iterativeStmt))
	{
		insidefor = false;
	}
	else if(root->is_terminal && (root->name.tok.type == END) && (root->parent->name.c == conditionalStmt))
	{
		insideswitch = false;
	}
	else if(root->is_terminal && root->name.tok.type == NUM)
		root->type = INT;
	else if(root->is_terminal && root->name.tok.type == RNUM)
		root->type = RE;


	int i;

	for(i = 0 ; i < root->no_of_children ; i++)
		semantic_analysis(root->children[i]);
}