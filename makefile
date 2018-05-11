perry: scanner.o lex.yy.c parser.tab.c definiciones.o 
	g++ scanner.o definiciones.o lex.yy.c parser.tab.c -o perry

scanner.o: scanner.cpp parser.tab.c 
	g++ -c scanner.cpp definiciones.cpp

definiciones.o:
	g++ -c definiciones.cpp

lex.yy.c: lexer.l
	flex lexer.l

parser.tab.c: parser.y 
	bison -d parser.y

clean:
	rm lex.yy.c parser.tab.* scanner.o definiciones.o perry