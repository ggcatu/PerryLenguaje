perry: scanner.o lex.yy.c parser.tab.c definiciones.o TAC.o ast.h RegisterManager.o ast.cpp
	g++ scanner.o definiciones.o TAC.o RegisterManager.o lex.yy.c parser.tab.c ast.cpp -o perry

scanner.o: scanner.cpp parser.tab.c definiciones.o
	g++ -c scanner.cpp definiciones.cpp

TAC.o: TAC.h TAC.cpp
	g++ -c TAC.cpp 

RegisterManager.o: RegisterManager.cpp RegisterManager.h
	g++ -c RegisterManager.cpp

definiciones.o: definiciones.h
	g++ -c definiciones.cpp

lex.yy.c: lexer.l
	flex lexer.l

parser.tab.c: parser.y
	bison -d --report=state parser.y

clean:
	rm lex.yy.c parser.tab.* parser.output scanner.o definiciones.o perry