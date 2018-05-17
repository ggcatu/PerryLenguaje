perry: scanner.o lex.yy.c parser.tab.c definiciones.o ast.h ast.cpp
	g++ scanner.o definiciones.o lex.yy.c parser.tab.c ast.cpp -o perry

ast.o: ast.cpp 

scanner.o: scanner.cpp parser.tab.c definiciones.o
	g++ -c scanner.cpp definiciones.cpp

definiciones.o:
	g++ -c definiciones.cpp

lex.yy.c: lexer.l
	flex lexer.l

parser.tab.c: parser.y 
	bison -d --report=state parser.y

clean:
	rm lex.yy.c parser.tab.* parser.output scanner.o definiciones.o perr