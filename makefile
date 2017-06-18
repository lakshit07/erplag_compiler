
CC = gcc
RM = rm -rf
FLAG = -g -Wall
OBJECTS = driver.o parser.o lexer.o symboltable.o ast.o semantic.o codegen.o
EXE = compiler
EXECFILES = run compiler *.o

.PHONY: erplag
all: $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXE)

driver.o: lexer.h parser.h symboltable.h ast.h semantic.h
lexer.o: lexer.h 
parser.o: parser.h lexer.h
symboltable.o: parser.h	
ast.o: ast.h parser.h symboltable.h lexer.h	
semantic.o: parser.h symboltable.h ast.h lexer.h	
codegen.o: codegen.h parser.h symboltable.h ast.h semantic.h

.PHONY: clean
clean:
	$(RM) $(EXECFILES) $(OBJECTS) erplag
