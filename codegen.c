// Group Number : 47
// Anirudh Kumar Bansal 2014A7S081P
// Lakshit blutani 2014A7PS095P
 
#include "codegen.h"
#include "semantic.h"
#include "symboltable.h"
#include "ast.h"
#include "parser.h"

int size[] = {2, 4, 1};

void gen_code_util(char* str)
{
	fasm = fopen(str,"w");
	if(fasm == NULL)
		printf("No such file named %s\n",str);
	else
		gen_code();
}

void gen_code()
{
	fprintf(fasm,"section .data\n");
	fprintf(fasm,"formatin : db \"%%d\",0\n");
	fprintf(fasm,"formatout : db \"%%d\",10,0\n");
	fprintf(fasm,"section .bss\n");
	label = 1;
	gen_data(broot);
	fprintf(fasm,"section .text\n extern printf\n extern scanf\n GLOBAL main\nmain:\n");
	gen_prog(astroot,broot);
	fclose(fasm);
}

void get_var_name(char* name, astptr a)
{
	//char n[50];
	strcpy(name , "l");
	varptr vt = get_id(a->name.tok.lexeme,a->scope);
	strcat(name,a->name.tok.lexeme);
	char line[50];
	sprintf(line , "%d" , vt->var.line);
	strcat(name, line);

	// name = n;
}

void left(astptr a)
{
	if(a->type == INT)
		fprintf(fasm,"mov ax,dx\nmovsx rax,ax\npush rax\n");
	else if(a->type == BOOL)
		fprintf(fasm,"mov al,dl\nmovsx rax,ax\npush rax\n");
}

void right(astptr a)
{
	if(a->type == INT)
		fprintf(fasm,"mov bx,dx\n");
	else if(a->type == BOOL)
		fprintf(fasm,"mov bl,dl\n");
}

void get_switch_id(char* name,astptr a)
{
	while(a->name.c != conditionalStmt)
		a = a->parent;
	get_var_name(name,a->children[0]);
}

int get_switch_end(astptr a)
{
	while(a->name.c != conditionalStmt)
		a = a->parent;
	return a->end;
}

void compose(astptr a,typ t)
{
	if(t == INT)
	{
		if(a->name.tok.type == PLUS)
		{
			fprintf(fasm,"pop rax\nadd dx,ax\n");
		}
		else if(a->name.tok.type == MINUS)
			fprintf(fasm,"pop rax\nneg dx\nadd dx,ax\n");
		else if(a->name.tok.type == MUL)
		{
			fprintf(fasm,"pop rax\nimul dx\nmov dx,ax\n");
		}
		else if(a->name.tok.type == DIV)
		{
			fprintf(fasm,"pop rax\nmov si,dx\nmov dx,0\nidiv si\nmov dx,ax\n");
		}
		else if(a->name.tok.type == LT)
		{
			fprintf(fasm,"pop rax\ncmp ax,dx\njl L%d\n",label);
			fprintf(fasm,"mov dl,0\njmp L%d\n",label+1);
			fprintf(fasm,"L%d: mov dl,1\nL%d:\n",label,label+1);
			label+=2;
		}
		else if(a->name.tok.type == LE)
		{
			fprintf(fasm,"pop rax\ncmp ax,dx\njle L%d\n",label);
			fprintf(fasm,"mov dl,0\njmp L%d\n",label+1);
			fprintf(fasm,"L%d: mov dl,1\nL%d:\n",label,label+1);
			label+=2;
		}
		else if(a->name.tok.type == GT)
		{
			fprintf(fasm,"pop rax\ncmp ax,dx\njg L%d\n",label);
			fprintf(fasm,"mov dl,0\njmp L%d\n",label+1);
			fprintf(fasm,"L%d: mov dl,1\nL%d:\n",label,label+1);
			label+=2;
		}
		else if(a->name.tok.type == GE)
		{
			fprintf(fasm,"pop rax\ncmp ax,dx\njge L%d\n",label);
			fprintf(fasm,"mov dl,0\njmp L%d\n",label+1);
			fprintf(fasm,"L%d: mov dl,1\nL%d:\n",label,label+1);
			label+=2;
		}
		else if(a->name.tok.type == EQ)
		{
			fprintf(fasm,"pop rax\ncmp ax,dx\nje L%d\n",label);
			fprintf(fasm,"mov dl,0\njmp L%d\n",label+1);
			fprintf(fasm,"L%d: mov dl,1\nL%d:\n",label,label+1);
			label+=2;
		}
		else if(a->name.tok.type == NE)
		{
			fprintf(fasm,"pop rax\ncmp ax,dx\njne L%d\n",label);
			fprintf(fasm,"mov dl,0\njmp L%d\n",label+1);
			fprintf(fasm,"L%d: mov dl,1\nL%d:\n",label,label+1);
			label+=2;
		}
	}
	else if(t == BOOL)
	{
		if(a->name.tok.type == AND)
		{
			fprintf(fasm,"pop rax\nand dl,al\n");
		}
		else if(a->name.tok.type == OR)
		{
			fprintf(fasm,"pop rax\nor dl,al\n");
		}
	}
}

void gen_data(bptr broot)
{
	int i;
	
	for(i = 0 ; i < 100 ; i++)
	{
		varptr temp = broot->var_table[i]; 
		while(temp != NULL)
		{
			variable v = temp->var;

			if(v.type.isarr)
			{
				int noe = v.type.high - v.type.low + 1;
				char name[50];
				strcpy(name , v.name);
				sprintf(name , "%s%d" , name, v.line);

				if(v.type.type == INT)
					fprintf(fasm , "l%s: resw %d\n" , name , noe);
				else if(v.type.type == RE)
					fprintf(fasm , "l%s: resd %d\n" , name , noe);
				else if(v.type.type == BOOL)
					fprintf(fasm , "l%s: resb %d\n" , name , noe);

				for(int j = 0 ; j < noe - 1 ; j++)
					temp = temp->next;

			}
			else
			{
				char name[50];
				strcpy(name , v.name);
				sprintf(name , "%s%d" , name, v.line);

				if(v.type.type == INT)
					fprintf(fasm , "l%s: resw 1\n" , name);
				else if(v.type.type == RE)
					fprintf(fasm , "l%s: resd 1\n" , name);
				else if(v.type.type == BOOL)
					fprintf(fasm , "l%s: resb 1\n" , name);
			}
			temp = temp->next;
		}
	}

	for(i = 0 ; i < broot->no_of_children ; i++)
		gen_data(broot->children[i]);
}

void gen_prog(astptr aroot , bptr broot)
{
	if(!aroot->is_terminal)
	{
		//printf("%s\n",str_characters[(int)aroot->name.c]);
		if(aroot->name.c == program)
		{
			if(!aroot->children[0]->children[0]->is_terminal)
			{
				printf("ERROR : Module declarations before driver module\n");
				return;
			}
			else if(!aroot->children[1]->children[0]->is_terminal)
			{
				printf("ERROR : Module definitions before driver module\n");
				return;
			}
			else if(!aroot->children[3]->children[0]->is_terminal)
			{
				printf("ERROR : Module definitions after driver module\n");
				return;
			}
			gen_prog(aroot->children[2],broot);
			fprintf(fasm , "mov ebx,0\nmov eax,1\nint 0x80\n");
		}
		else if(aroot->name.c == driverModule)
			gen_prog(aroot->children[0],broot);
		else if(aroot->name.c == moduleDef)
			gen_prog(aroot->children[1],broot->children[0]);
		else if(aroot->name.c == statements)
		{
			if(aroot->children[0]->is_terminal)
				return;
			gen_prog(aroot->children[0],broot);
			gen_prog(aroot->children[1],broot);
		}
		else if(aroot->name.c == statement)
		{
			gen_prog(aroot->children[0],broot);
		}
		else if(aroot->name.c == ioStmt)
		{
			if(aroot->no_of_children == 2)
			{
				//scanning
				typ t = aroot->children[0]->type;
				char name[50];
				get_var_name(name,aroot->children[0]);
				//fprintf(fasm,"lea rsi,[%s]\n",name);
				if(aroot->children[1]->children[0]->is_terminal)
				{
					varptr vt = get_id(aroot->children[0]->name.tok.lexeme,aroot->children[0]->scope);
					if(vt->var.type.isarr)
			    	{
			    		int curr_label = label;
			    		fprintf(fasm,"mov bp,%d\nL%d:\n",vt->var.type.low,label);
			    		label++;
			    		fprintf(fasm,"cmp bp,%d\njg L%d\n",vt->var.type.high,label);
			    		label++;
			    		int index_low = vt->var.type.low;
			    		//int offset = (index_val - index_low)*size[(int)t];
			    		
			    		fprintf(fasm,"movzx rsi,bp\n");
			    		fprintf(fasm,"sub rsi,%d\n",index_low);
			    		fprintf(fasm,"mov rdi,formatin\n");
						fprintf(fasm,"lea rsi,[%s+rsi*%d]\n",name,size[(int)t]);
						fprintf(fasm,"mov al,0\ncall scanf\n");
						fprintf(fasm,"inc bp\njmp L%d\nL%d:\n",curr_label,curr_label+1);
			    	}
			    	else
			    	{
			    		fprintf(fasm,"mov rdi,formatin\n");
						fprintf(fasm,"lea rsi,[%s]\n",name);
						fprintf(fasm,"mov al,0\ncall scanf\n");
					}
				}
				else
				{
					astptr index_node = aroot->children[1]->children[0];
					int index_val;
					varptr vt = get_id(aroot->children[0]->name.tok.lexeme,broot);
					int index_low = vt->var.type.low;

					if(index_node->children[0]->name.tok.type == NUM)
					{
						index_val = get_int(index_node->children[0]->name.tok.lexeme);
						int offset = (index_val - index_low)*size[(int)t];
						fprintf(fasm,"lea rsi,[%s+%d]\n",name,offset);
					}
					else
					{
						char ind_name[50];
						get_var_name(ind_name,index_node->children[0]);
						if(t == INT)
							fprintf(fasm,"movzx rsi,word[%s]\n",ind_name);
						else if(t == BOOL)
							fprintf(fasm,"movzx rsi,byte[%s]\n",ind_name);
						fprintf(fasm,"sub rsi,%d\n",index_low);
						fprintf(fasm,"mov rdi,formatin\n");
						fprintf(fasm,"lea rsi,[%s+rsi*%d]\n",name,size[(int)t]);
						fprintf(fasm,"mov al,0\ncall scanf\n");

						//if array index is a variable

						//fprintf(fasm,"mul ecx,%d,%d\n",size[(int)t],index_low);
						//fprintf(fasm,"add ecx,ecx,%s\n",name);
					}
				}
				
				// else
				// 	fprintf(f)
				//     //mov edx,2
         			//mov ecx,inp
         		// fprintf(fasm,"mov ebx,2\n");
         		// fprintf(fasm,"mov eax,3\n");
         		// fprintf(fasm,"int 80h\n"); 
			}
			else
			{
				//printing
				if(aroot->children[0]->children[0]->name.tok.type == ID)
				{
					astptr idnode = aroot->children[0]->children[0]; 
					typ t = idnode->type;
				    char name[50];
				    get_var_name(name,idnode);
				    if(aroot->children[0]->children[1]->children[0]->is_terminal)
				    {
				    	varptr vt = get_id(idnode->name.tok.lexeme,idnode->scope);
				    	if(vt->var.type.isarr)
				    	{
				    		int curr_label = label;
				    		fprintf(fasm,"mov bp,%d\nL%d:\n",vt->var.type.low,label);
				    		label++;
				    		fprintf(fasm,"cmp bp,%d\njg L%d\n",vt->var.type.high,label);
				    		label++;
				    		int index_low = vt->var.type.low;
				    		//int offset = (index_val - index_low)*size[(int)t];
				    		
				    		fprintf(fasm,"movzx rsi,bp\n");
				    		fprintf(fasm,"sub rsi,%d\n",index_low);
				    		fprintf(fasm,"mov rdi,formatout\n");
				    		if(t == INT)
								fprintf(fasm,"movsx rsi,word[%s+rsi*%d]\n",name,size[(int)t]);
							else if(t == BOOL)
								fprintf(fasm,"movsx rsi,byte[%s+rsi*%d]\n",name,size[(int)t]);
							fprintf(fasm,"mov al,0\ncall printf\n");
							fprintf(fasm,"inc bp\njmp L%d\nL%d:\n",curr_label,curr_label+1);
				    	}
				    	else
				    	{
				    		fprintf(fasm,"mov rdi,formatout\n");
					     	if(t == INT)
								fprintf(fasm,"movsx rsi,word[%s]\n",name);
							else if(t == BOOL)
								fprintf(fasm,"movsx rsi,byte[%s]\n",name);
							fprintf(fasm,"mov al,0\ncall printf\n");
						}
						//fprintf(fasm,"movsx rsi,[%s]\n",name);
				    }
					else
					{
						fprintf(fasm,"mov rdi,formatout\n");
						astptr index_node = aroot->children[0]->children[1]->children[0];
						int index_val;
						varptr vt = get_id(aroot->children[0]->children[0]->name.tok.lexeme,broot);
						int index_low = vt->var.type.low;

						if(index_node->children[0]->name.tok.type == NUM)
						{
							index_val = get_int(index_node->children[0]->name.tok.lexeme);
							int offset = (index_val - index_low)*size[(int)t];
							if(t == INT)
								fprintf(fasm,"movsx rsi,word[%s+%d]\n",name,offset);
							else if(t == BOOL)
								fprintf(fasm,"movsx rsi,byte[%s+%d]\n",name,offset);
							//fprintf(fasm,"movsx rsi,[%s+%d]\n",name,offset);
							fprintf(fasm,"mov al,0\ncall printf\n");
						}
						else
						{
							char ind_name[50];
							get_var_name(ind_name,index_node->children[0]);
							if(t == INT)
								fprintf(fasm,"movzx rsi,word[%s]\n",ind_name);
							else if(t == BOOL)
								fprintf(fasm,"movzx rsi,byte[%s]\n",ind_name);
							
							fprintf(fasm,"sub rsi,%d\n",index_low);
							fprintf(fasm,"mov rdi,formatout\n");
							if(t == INT)
								fprintf(fasm,"movsx rsi,word[%s+rsi*%d]\n",name,size[(int)t]);
							else if(t == BOOL)
								fprintf(fasm,"movsx rsi,byte[%s+rsi*%d]\n",name,size[(int)t]);
							fprintf(fasm,"mov al,0\ncall printf\n");
							//fprintf(fasm,"movsx rsi,[%s+rsi]\n",name);

							//if array index is a variable

							//fprintf(fasm,"mul ecx,%d,%d\n",size[(int)t],index_low);
							//fprintf(fasm,"add ecx,ecx,%s\n",name);
						}
					}
					
				}
				else if(aroot->children[0]->children[0]->name.tok.type == NUM)
				{
					fprintf(fasm,"mov rdi,formatout\n");
				    fprintf(fasm,"mov rsi,%s\n",aroot->children[0]->children[0]->name.tok.lexeme);
				    fprintf(fasm,"mov al,0\ncall printf\n");
				}
				
			}
		}
		else if(aroot->name.c == simpleStmt)
		{
			gen_prog(aroot->children[0],broot);
		}
		else if(aroot->name.c == assignmentStmt)
		{
			gen_prog(aroot->children[1],broot);
			char name[50];
			get_var_name(name,aroot->children[0]);
			if(aroot->children[1]->children[0]->name.tok.type == lvalueIDStmt)
			{	
				if(aroot->children[0]->type == INT)
				{
					fprintf(fasm,"mov word[%s],dx\n",name);
				}
				else if(aroot->children[0]->type == RE)
				{
					fprintf(fasm,"mov dword[%s],edx\n",name);
				}
				else if(aroot->children[0]->type == BOOL)
				{
					fprintf(fasm,"mov byte[%s],dl\n",name);
				}
		    }
		    else
		    {
		    	astptr index_node = aroot->children[1]->children[0]->children[0];
				int index_val;
				varptr vt = get_id(aroot->children[0]->name.tok.lexeme,broot);
				typ t = aroot->children[0]->type;
				int index_low = vt->var.type.low;
				index_val = get_int(index_node->children[0]->name.tok.lexeme);
				int offset = (index_val - index_low)*size[(int)t];
		    	if(index_node->children[0]->name.tok.type == NUM)
		    	{
			    	if(aroot->children[0]->type == INT)
					{
						fprintf(fasm,"mov word[%s+%d],dx\n",name,offset);
					}
					else if(aroot->children[0]->type == RE)
					{
						fprintf(fasm,"mov dword[%s+%d],edx\n",name,offset);
					}
					else if(aroot->children[0]->type == BOOL)
					{
						fprintf(fasm,"mov byte[%s+%d],dl\n",name,offset);
					}
			    }
			    else
			    {
			    	// array index is a variable
			    	char ind_name[50];
					get_var_name(ind_name,index_node->children[0]);
					if(t == INT)
						fprintf(fasm,"movzx rsi,word[%s]\n",ind_name);
					else if(t == BOOL)
						fprintf(fasm,"movzx rsi,byte[%s]\n",ind_name);
				
					fprintf(fasm,"sub rsi,%d\n",index_low);
					if(aroot->children[0]->type == INT)
					{
						fprintf(fasm,"mov word[%s+rsi*%d],dx\n",name,size[(int)t]);
					}
					else if(aroot->children[0]->type == RE)
					{
						fprintf(fasm,"mov dword[%s+rsi*%d],edx\n",name,size[(int)t]);
					}
					else if(aroot->children[0]->type == BOOL)
					{
						fprintf(fasm,"mov byte[%s+rsi*%d],dl\n",name,size[(int)t]);
					}
			    }
		    }
		}
		else if(aroot->name.c == iterativeStmt)
		{
			if(aroot->children[0]->name.tok.type == FOR)
			{
				//char lowbnd[30];
				//char upbnd[30];
				//strcpy(lowbnd,aroot->)
				char name[50];
				get_var_name(name,aroot->children[1]);
				//fprintf(fasm,"mov cx,%s\n",aroot->children[2]->children[0]->name.tok.lexeme);
				fprintf(fasm,"mov word[%s],%s\n",name,aroot->children[2]->children[0]->name.tok.lexeme);
				int curr_label = label;
				fprintf(fasm,"cmp word[%s],%s\njg L%d\nL%d:\n",name,aroot->children[2]->children[1]->name.tok.lexeme,curr_label+1,curr_label);
				label+=2;
				gen_prog(aroot->children[4],broot);
				fprintf(fasm,"inc word[%s]\n",name);
				//fprintf(fasm,"inc cx\n");
				//fprintf(fasm,"mov word[%s],cx\n",name);
				fprintf(fasm,"cmp word[%s],%s\njle L%d\nL%d:\n",name,aroot->children[2]->children[1]->name.tok.lexeme,curr_label,curr_label+1);
			}
			else
			{
				gen_prog(aroot->children[1],broot);
				int curr_label = label;
				fprintf(fasm,"cmp dl,0\nje L%d\nL%d:\n",curr_label+1,curr_label);
				label+=2;
				gen_prog(aroot->children[3],broot);
				gen_prog(aroot->children[1],broot);
				fprintf(fasm,"cmp dl,1\nje L%d\nL%d:\n",curr_label,curr_label+1);
			}
		}
		else if(aroot->name.c == conditionalStmt)
		{
			aroot->end = label;
			label++;
			gen_prog(aroot->children[2],broot);
			gen_prog(aroot->children[3],broot);
			fprintf(fasm,"L%d:\n",aroot->end);
		}
		else if(aroot->name.c == caseStmts)
		{
			char name[50];
			get_switch_id(name,aroot);
			typ sw_type;
			if(aroot->children[0]->children[0]->name.tok.type != NUM)
				sw_type = BOOL;
			else
				sw_type = INT;
		
			if(sw_type == INT)
				fprintf(fasm,"cmp word[%s],%s\n",name,aroot->children[0]->children[0]->name.tok.lexeme);
			else
			{
				if(aroot->children[0]->children[0]->name.tok.type == TRUE)
					fprintf(fasm,"cmp byte[%s],1\n",name);
				else
					fprintf(fasm,"cmp byte[%s],0\n",name);
			}
			int curr_label = label;
			fprintf(fasm,"jne L%d\n",curr_label);
			label++;
			gen_prog(aroot->children[1],broot);
			fprintf(fasm,"jmp L%d\n",get_switch_end(aroot));
			fprintf(fasm,"L%d:\n",curr_label);
			gen_prog(aroot->children[2],broot);
		}
		else if(aroot->name.c == N9)
		{
			if(aroot->no_of_children == 1) return;
			char name[50];
			get_switch_id(name,aroot);
			typ sw_type;
			if(aroot->children[0]->children[0]->name.tok.type != NUM)
				sw_type = BOOL;
			else
				sw_type = INT;
		
			if(sw_type == INT)
				fprintf(fasm,"cmp word[%s],%s\n",name,aroot->children[0]->children[0]->name.tok.lexeme);
			else
			{
				if(aroot->children[0]->children[0]->name.tok.type == TRUE)
					fprintf(fasm,"cmp byte[%s],1\n",name);
				else
					fprintf(fasm,"cmp byte[%s],0\n",name);
			}
			int curr_label = label;
			fprintf(fasm,"jne L%d\n",curr_label);
			label++;
			gen_prog(aroot->children[1],broot);
			fprintf(fasm,"jmp L%d\n",get_switch_end(aroot));
			fprintf(fasm,"L%d:\n",curr_label);
			gen_prog(aroot->children[2],broot);
		}
		else if(aroot->name.c == Default)
		{
			if(aroot->children[0]->name.tok.type == eps) return;
			
			gen_prog(aroot->children[0],broot);
			//fprintf(fasm,"jmp L%d\n",get_switch_end(aroot));
		}
		else if(aroot->name.c == whichStmt)
		{
			if(aroot->children[0]->name.c == lvalueIDStmt)
				gen_prog(aroot->children[0]->children[0],broot);
			else
				gen_prog(aroot->children[0]->children[1],broot);
		}
		else if(aroot->name.c == expression)
		{
			if(aroot->no_of_children == 1 && aroot->children[0]->name.c == arithmeticOrBooleanExpr)
			{
				gen_prog(aroot->children[0],broot);
			}
			else if(aroot->children[0]->name.tok.type == PLUS)
			{
				gen_prog(aroot->children[1],broot);
			}
			else
			{
				gen_prog(aroot->children[1],broot);

				//to write : multiply previous result by -1
			}
		}
		else if(aroot->name.c == arithmeticOrBooleanExpr)
		{
			gen_prog(aroot->children[0],broot);
			if(aroot->children[1]->no_of_children > 1)
			{
				left(aroot->children[0]);
				gen_prog(aroot->children[1],broot);
				compose(aroot->children[1]->children[0]->children[0],aroot->children[0]->type);
			}
		}
		else if(aroot->name.c == N7)
		{
			gen_prog(aroot->children[1],broot);
			if(aroot->children[2]->no_of_children > 1)
			{
				left(aroot->children[1]);
				gen_prog(aroot->children[2],broot);
				compose(aroot->children[2]->children[0]->children[0],aroot->children[1]->type);
			}
		}
		else if(aroot->name.c == AnyTerm)
		{
			gen_prog(aroot->children[0],broot);
			if(aroot->children[1]->no_of_children > 1)
			{
				left(aroot->children[0]);
				gen_prog(aroot->children[1],broot);
				compose(aroot->children[1]->children[0]->children[0],aroot->children[0]->type);
			}
		}
		else if(aroot->name.c == N8)
		{
			gen_prog(aroot->children[1],broot);
			if(aroot->children[2]->no_of_children > 1)
			{
				left(aroot->children[1]);
				gen_prog(aroot->children[2],broot);
				compose(aroot->children[2]->children[0]->children[0],aroot->children[1]->type);
			}
		}
		else if(aroot->name.c == arithmeticExpr)
		{
			gen_prog(aroot->children[0],broot);
			if(aroot->children[1]->no_of_children > 1)
			{
				left(aroot->children[0]);
				gen_prog(aroot->children[1],broot);
				//printf("%s\n",aroot->children[1]->children[0]->children[0]->name.tok.lexeme);
				compose(aroot->children[1]->children[0]->children[0],aroot->children[0]->type);
			}

		}
		else if(aroot->name.c == N4)
		{
			gen_prog(aroot->children[1],broot);
			if(aroot->children[2]->no_of_children > 1)
			{
				left(aroot->children[1]);
				gen_prog(aroot->children[2],broot);
				compose(aroot->children[2]->children[0]->children[0],aroot->children[1]->type);
			}
		}
		else if(aroot->name.c == term)
		{
			gen_prog(aroot->children[0],broot);
			if(aroot->children[1]->no_of_children > 1)
			{
				left(aroot->children[0]);
				gen_prog(aroot->children[1],broot);
				compose(aroot->children[1]->children[0]->children[0],aroot->children[0]->type);
			}
		}
		else if(aroot->name.c == N5)
		{
			gen_prog(aroot->children[1],broot);
			if(aroot->children[2]->no_of_children > 1)
			{
				left(aroot->children[1]);
				gen_prog(aroot->children[2],broot);
				compose(aroot->children[2]->children[0]->children[0],aroot->children[1]->type);
			}
		}
		else if(aroot->name.c == factor)
		{
			gen_prog(aroot->children[0],broot);
		}
		else if(aroot->name.c == var)
		{
			if(aroot->children[0]->name.tok.type == NUM)
			{
				fprintf(fasm,"mov dx,%s\n",aroot->children[0]->name.tok.lexeme);
			}
			else 
			{
				char name[50];
				get_var_name(name,aroot->children[0]);
				typ t = aroot->children[0]->type;
				if(aroot->children[1]->children[0]->is_terminal)
				{
					if(t == INT)
						fprintf(fasm,"mov dx,[%s]\n",name);
					else if(t == BOOL)
						fprintf(fasm,"mov dl,[%s]\n",name);
				}
				else
				{
				    astptr index_node = aroot->children[1]->children[0];					
					varptr vt = get_id(aroot->children[0]->name.tok.lexeme,broot);
					int index_low = vt->var.type.low;
					int index_val;
					
					if(index_node->children[0]->name.tok.type == NUM)
					{
						index_val = get_int(index_node->children[0]->name.tok.lexeme);
						int offset = (index_val - index_low)*size[(int)t];
						if(t == INT)
							fprintf(fasm,"mov dx,[%s+%d]\n",name,offset);
						else if(t == BOOL)
							fprintf(fasm,"mov dl,[%s+%d]\n",name,offset);
					}
					else
					{
						char ind_name[50];
						get_var_name(ind_name,index_node->children[0]);
						if(t == INT)
							fprintf(fasm,"movzx rsi,word[%s]\n",ind_name);
						else if(t == BOOL)
							fprintf(fasm,"movzx rsi,byte[%s]\n",ind_name);
						fprintf(fasm,"sub rsi,%d\n",index_low);

						if(aroot->children[0]->type == INT)
						{
							fprintf(fasm,"mov dx,[%s+rsi*%d]\n",name,size[(int)t]);
						}
						else if(aroot->children[0]->type == RE)
						{
							fprintf(fasm,"mov edx,[%s+rsi*%d]\n",name,size[(int)t]);
						}
						else if(aroot->children[0]->type == BOOL)
						{
							fprintf(fasm,"mov dl,[%s+rsi*%d]\n",name,size[(int)t]);
						}
					}
				}
			}
		}
	} 
}
