// Group Number : 47
// Anirudh Kumar Bansal 2014A7S081P
// Lakshit Bhutani 2014A7PS095P 

#include "parser.h"
#include "symboltable.h"
#include "symboltableDef.h"
#include "lexer.h"

int sno, soffset;

char * char_type[] = {"INTEGER" , "REAL" , "BOOLEAN"};
char * char_btype[] = {"ROOT","FUNCTION","WHILE","FOR","SWITCH"};

void init_sym_table()
{
	int i;
	err = false;
	broot = (bptr)malloc(sizeof(bnode));

	for(i = 0 ; i < 100 ; i++)
		broot->var_table[i] = NULL;
	
	broot->type = ROOT;
	//broot->f = NULL;
	broot->no_of_children = 0;
	broot->parent = NULL;
	broot->children = NULL;
	bcur = broot;
	broot->counter = 0;
	scopecur = 0;
}

int get_int(char *arr)
{
  int num = 0;
  for(int i=0 ; arr[i]!='\0' ; i++)
  {
    num = num*10 + ((int)arr[i]-'0');
  }
  return num;
}

void insert_var(variable v , bptr b)
{
	int pos = hash(v.name)%100;

	if(b->var_table[pos] == NULL)
	{
		varptr vnode = (varptr)malloc(sizeof(varnode));
		vnode->var = v;
		vnode->next = NULL;
		b->var_table[pos] = vnode;
	}
	else
	{
		varptr vnode = (varptr)malloc(sizeof(varnode));
		vnode->var = v;
		vnode->next = NULL;
		varptr temp = b->var_table[pos];

		while(temp->next != NULL)
		{
			if(strcmp(temp->var.name , v.name) == 0 && temp->var.offset == v.offset)
			{
				err = true;
				printf( "ERROR: Variable '%s' already declared at line %d and cannot be redeclared at line %d\n", v.name, temp->var.line, v.line );
				return;
			}
			temp = temp->next;
		}
		if((strcmp(temp->var.name , v.name) == 0) && (temp->var.offset == v.offset))
		{
				err = true;
				printf( "ERROR: Variable '%s' already declared at line %d and cannot be redeclared at line %d\n", v.name, temp->var.line, v.line );
				return;
		}
		
		temp->next = vnode;
	}
}

void insert_func(function f, int s, int e,int li)
{
	int i;
	if(broot->no_of_children > 0)
	{
		bptr temp = broot->children[0];
		int i;

		for(i = 0 ; i < broot->no_of_children ; i++)
		{
			if(strcmp(broot->children[i]->f.name , f.name) == 0)
			{
				if(!broot->children[i]->f.defined)
				{
					broot->children[i]->f = f;
					bcur = broot->children[i];
					varptr inp = f.input_type;
					while(inp != NULL)
					{
						insert_var(inp->var,bcur);
						inp = inp->next;
					}
					varptr op = f.output_type;
					while(op != NULL)
					{
						insert_var(op->var,bcur);
						op = op->next;
					}
					broot->children[i]->start = s;
					broot->children[i]->end = e;
					return;
				}	
				else
				{
					err = true;
					printf( "ERROR : Redefinition of function %s\n" , f.name );
				}	
				
			}
		}

	}

	bptr t = (bptr)malloc(sizeof(bnode));
	t->type = FUNCTION;
	f.occ = li;
	t->f = f;
	t->no_of_children = 0;
	for(i = 0 ; i < 100 ; i++)
		t->var_table[i] = NULL;
	t->parent = broot;
	t->children = NULL;
	t->counter = 0;

	varptr inp = f.input_type;
	while(inp != NULL)
	{
		insert_var(inp->var,t);
		inp = inp->next;
	}
	varptr op = f.output_type;
	while(op != NULL)
	{
		insert_var(op->var,t);
		op = op->next;
	}
	t->start = s;
	t->end = e;
	broot->children = (bptr*)realloc(broot->children,(broot->no_of_children+1)*sizeof(bptr));
	broot->children[broot->no_of_children] = t;
	bcur = broot->children[broot->no_of_children];
	broot->no_of_children++;
}

var_type get_type(pnode typenode)
{
	var_type vtype;
	if(typenode->no_of_children == 1)
	{
		vtype.isarr =  false;
		vtype.high = vtype.low = 0;
		if(typenode->children[0]->name.tok.type == INTEGER)
			vtype.type = INT;
		else if(typenode->children[0]->name.tok.type == REAL)
			vtype.type = RE;
		else
			vtype.type = BOOL;
		return vtype;
	}
	else
	{
		vtype.isarr = true;
		if(typenode->children[5]->children[0]->name.tok.type == INTEGER)
			vtype.type = INT;
		else if(typenode->children[5]->children[0]->name.tok.type == REAL)
			vtype.type = RE;
		else
			vtype.type = BOOL;
		vtype.low = get_int(typenode->children[2]->children[0]->name.tok.lexeme);
		vtype.high = get_int(typenode->children[2]->children[2]->name.tok.lexeme);
		return vtype;
	}

}

void add_vid(pnode node, var_type vtype, bptr b)
{
	if(node->is_terminal && node->name.tok.type == ID)
	{
		variable v;
		strcpy(v.name,node->name.tok.lexeme);
		v.line = node->name.tok.line;
		v.assigned = false;
		v.type = vtype;
		v.scope = scopecur;
		if(!vtype.isarr)
		{
			v.offset = 0;
			insert_var(v, b);
		}
		else
		{
			int i;
			if(vtype.low > vtype.high || vtype.low < 0 || vtype.high < 0)
			{
				err = true;
				printf( "ERROR : Array ranges incorrect for array declaration of %s on line %d\n" , node->name.tok.lexeme, node->name.tok.line );
				return;
			}

			for(i = vtype.low; i <= vtype.high; i++)
			{
				variable v;
				strcpy(v.name,node->name.tok.lexeme);
				v.line = node->name.tok.line;
				v.assigned = false;
				v.type = vtype;
				v.offset = i;
				v.scope = scopecur;
				insert_var(v, b);
			}
		}
		return;
	}
	int i;
	for(i = 0; i < node->no_of_children ; i++)
		add_vid(node->children[i],vtype,b);
}

void add_fid(pnode node)
{
	function f;
	int i;
	strcpy(f.name,node->name.tok.lexeme);
	f.input_count = 0;
	f.output_count = 0;
	f.defined = false;
	f.input_type = NULL;
	f.output_type = NULL;
	f.occ = node->name.tok.line;

	bptr t = (bptr)malloc(sizeof(bnode));
	t->type = FUNCTION;
	t->f = f;
	for(i = 0 ; i < 100 ; i++)
		t->var_table[i] = NULL;
	t->no_of_children = 0;
	t->parent = broot;
	t->children = NULL;

	if(broot->no_of_children > 0)
	{
		bptr temp = broot->children[0];
		int i;

		for(i = 0 ; i < broot->no_of_children ; i++)
		{
			if(strcmp(broot->children[i]->f.name , f.name) == 0)
			{
				err = true;
				printf( "ERROR : Redeclaration of function %s\n" , f.name );
				return;
			}
		}
	}

	broot->children = (bptr*)realloc(broot->children,(broot->no_of_children+1)*sizeof(bptr));
	broot->children[broot->no_of_children] = t;
	t->counter = 0;
	broot->no_of_children++;
}

void get_input(pnode node, function *f)
{
	if(node->is_terminal && (node->name.tok.type == ID))
	{
		var_type vtype;
		if(node->parent->name.c == input_plist)
			vtype = get_type(node->parent->children[2]);
		else
			vtype = get_type(node->parent->children[3]);

		varptr vptr = (varptr)malloc(sizeof(varnode));
		// printf("%s\n" , node->name.tok.lexeme);
		strcpy(vptr->var.name,node->name.tok.lexeme);
		vptr->var.line = node->name.tok.line;
		vptr->var.assigned = false;
		vptr->var.type = vtype;
		vptr->var.scope = scopecur; 
		vptr->next = NULL;
		if(!vtype.isarr)
		{
			vptr->var.offset = 0;
			f->input_count++;
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
			int i;

			if(vtype.low > vtype.high)
			{
				err = true;
				printf( "ERROR : Array ranges incorrect for array declaration of %s on line %d\n" , vptr->var.name, vptr->var.line );
				return;
			}

			for(i = vtype.low;i <= vtype.high ; i++)
			{
				varptr vptr = (varptr)malloc(sizeof(varnode));
				strcpy(vptr->var.name,node->name.tok.lexeme);
				vptr->var.line = node->name.tok.line;
				vptr->var.assigned = false;
				vptr->var.type = vtype; 
				vptr->next = NULL;
				vptr->var.offset = i;
				vptr->var.scope = scopecur; 
				f->input_count++;	
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
	int i;
	for(i = 0; i < node->no_of_children ; i++)
		get_input(node->children[i],f);
}

void get_output(pnode node,function *f)
{
	int i;
	if(node->is_terminal && (node->name.tok.type == ID))
	{
		var_type vtype;
		
		if(node->parent->name.c == output_plist)
			vtype = get_type(node->parent->children[2]);
		else
			vtype = get_type(node->parent->children[3]);

		varptr vptr = (varptr)malloc(sizeof(varnode));
		strcpy(vptr->var.name,node->name.tok.lexeme);
		vptr->var.line = node->name.tok.line;
		vptr->var.scope = 0;
		vptr->var.assigned = false;
		vptr->var.type = vtype; 
		vptr->next = NULL;
		vptr->var.offset = 0;
		vptr->var.scope = scopecur; 

		f->output_count++;
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

	for(i = 0; i < node->no_of_children ; i++)
		get_output(node->children[i],f);
}


void create_symboltable(pnode root)
{
	if(err)
		return;
	// if(root->is_terminal)
	// 	printf("%s %d\n",root->name.tok.lexeme,root->name.tok.line);
	// else
	// 	printf("%s\n",str_characters[(int)root->name.c]);

	if(root->is_terminal && (root->name.tok.type == DECLARE))
	{
		if(root->parent->children[1]->name.c == idList)
		{
			var_type vtype = get_type(root->parent->children[3]);
			add_vid(root->parent->children[1], vtype , bcur);
		}
		else
			add_fid(root->parent->children[2]);
	}
	else if(root->is_terminal && (root->name.tok.type == DEF))
	{
		function f;
		f.input_count = 0;
		f.output_count = 0;
		strcpy(f.name,root->parent->children[2]->name.tok.lexeme);
		f.defined = true;
		f.input_type = NULL;
		f.output_type = NULL;
		get_input(root->parent->children[7] , &f);
		variable temp = f.input_type->var;

		if(root->parent->children[10]->no_of_children == 1)
		{
			f.output_count = 0;
			f.output_type = NULL;	
		}
		else
			get_output(root->parent->children[10]->children[2],&f);

		int start;
		int i;
		bool found = false;
		start = root->parent->children[11]->children[0]->name.tok.line;
		int end = root->parent->children[11]->children[2]->name.tok.line;

		insert_func(f , start , end,root->parent->children[2]->name.tok.line);
	}
	else if(root->is_terminal && (root->name.tok.type == DRIVERDEF))
	{
		function f;
		f.input_count = 0;
		f.output_count = 0;
		strcpy(f.name,"DRIVER");
		f.defined = true;
		f.input_type = NULL;
		f.output_type = NULL;
		int start = root->parent->children[4]->children[0]->name.tok.line;
		int end = root->parent->children[4]->children[2]->name.tok.line;
		insert_func(f,start,end,root->name.tok.line);
	}	
	else if(root->is_terminal && (root->name.tok.type == START) && (root->parent->name.c != moduleDef))
	{
		bptr t = (bptr)malloc(sizeof(bnode));
		int i;

		if(root->parent->children[0]->name.c == WHILE)
			t->type = WH;
		else if(root->parent->children[0]->name.c == FOR)
			t->type = FR;
		else
			t->type = SW;

		t->no_of_children = 0;
		t->parent = bcur;
		for(i = 0 ; i < 100 ; i++)
			t->var_table[i] = NULL;
		t->children = NULL;
		bcur->children = (bptr*)realloc(bcur->children,(bcur->no_of_children+1)*sizeof(bptr));
	    bcur->children[bcur->no_of_children] = t;
		bcur->no_of_children++;	
		bcur = t;
		t->counter = 0;
		scopecur++;	
	}
	else if(root->is_terminal && (root->name.tok.type == END) && (root->parent->name.c != moduleDef))
	{
		bcur = bcur->parent;
		scopecur--;
	}
		
	int i;
	for(i = 0; i < root->no_of_children ; i++)
		create_symboltable(root->children[i]);	
}


void print_symboltable(bptr br)
{
	int i;

	if(br->type != ROOT)
	{
		for(i = 0 ; i < 100 ; i++)
		{
			varptr temp = br->var_table[i]; 
			while(temp != NULL)
			{
				printf("%5d   %8s " , sno, temp->var.name); 
				char arname[50];
				strcpy(arname , temp->var.name);

				if(temp->var.type.isarr)
				{
					char tp[50];
					sprintf(tp , "array(%d,%s)" , temp->var.type.high - temp->var.type.low + 1 , char_type[(int)temp->var.type.type]);
					printf("%20s" , tp);
					bptr btemp = br;

					while(btemp->type != FUNCTION)
						btemp = btemp->parent;

					int width = temp->var.type.high - temp->var.type.low + 1 ;
					
					if(temp->var.type.type == INT)
						width = width * 2;
					else if(temp->var.type.type == RE)
						width = width * 4;

					printf("%12s        %d to %d    %13d             %d            %d\n" , btemp->f.name , btemp->start , btemp->end, temp->var.scope+1, width, soffset);
					soffset += width;

					while(temp != NULL && strcmp(arname , temp->var.name) == 0)
						temp = temp->next;

				}
				else
				{
					printf("%20s" , char_type[(int)temp->var.type.type]);

					bptr btemp = br;

					while(btemp->type != FUNCTION)
						btemp = btemp->parent;

					int width = 1;

					if(temp->var.type.type == INT)
						width = 2;
					else if(temp->var.type.type == RE)
						width = 4;

					printf("%12s        %d to %d    %13d             %d            %d\n" , btemp->f.name, btemp->start, btemp->end, temp->var.scope+1, width, soffset);
					soffset += width;

					temp = temp->next;
				}
				sno++;

			}
		}
	}
	for(i = 0 ; i < br->no_of_children ; i++)
		print_symboltable(br->children[i]);	

}

void print_symboltable_util()
{
	printf(ANSI_COLOR_MAGENTA "S.No.     lexeme                type     scope_module 	 scope_line    scope_nesting 	     width        offset\n" ANSI_COLOR_RESET);
	sno = 1;
	print_symboltable(broot);
	printf(ANSI_COLOR_GREEN "Symbol table generated successfully\n" ANSI_COLOR_RESET);
}