perry: scanner.o lex.yy.c definiciones.o 
	g++ scanner.o definiciones.o lex.yy.c  -o perry

scanner.o: scanner.cpp 
	g++ -c scanner.cpp definiciones.cpp

definiciones.o:
	g++ -c definiciones.cpp

lex.yy.c: lexer.l
	flex lexer.l

clean:
	rm lex.yy.c scanner.o definiciones.o perry