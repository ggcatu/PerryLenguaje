/*
Lenguajes de Programación II (CI-4721)

definiciones.h:
Contiene un mapeo palabra -> integer para identificar los tipos de tokens, así como las firmas 
de las clases y funciones implementadas en definiciones.cpp
*/

#include <stdio.h>
#include <stdlib.h> 
#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include "definiciones.h"
#include "ast.h"
#include "table.h"
#include "parser.tab.h"

using namespace std;


/*
Funciones importadas de lex.yy.c, provenientes de FLEX.
	yylex permite leer el próximo token.
	yylineno indica la linea donde está ubicado el cursor.
	yyleng indica el largo del token.
	yytext es un apuntador al inicio del token actual.
	yyin es un apuntador al input stream de yylex.
*/
extern int yylex();
extern int yylineno;
extern int yyleng;
extern int yycolumn;
extern char* yytext;
extern FILE* yyin;
extern ArbolSintactico * root_ast;
extern bool error_sintactico;
extern sym_table table;

// Vectores donde se guardarán nuestros resultados de la tokenización
vector<Token *> tokens;
vector<Token *> errors;

void execute_lexer(){
	cout << "Executing lexer" << endl;
    // Inicialización para nuestro ciclo de lectura
	int ntoken = yylex();
	int posicion = 1;
	int altura = yylineno;
	while (ntoken) {
		// Verifica si hay que reiniciar la columna.
		if (yylineno != altura) {
			altura = yylineno;
		}

		if (ntoken == STRING) {
			// Crea token string y coloca en el vector.
			string completo = yytext;
			string texto = completo.substr(1, completo.length() - 1 );
			tokens.push_back(new TokenIdentificador(ntoken,yylineno,yylloc.first_column,texto));			
		} 
		else if (ntoken == IDENTIFIER){
			// Crea token identificador y coloca en el vector.
			tokens.push_back(new TokenIdentificador(ntoken,yylineno,yylloc.first_column,yytext));			
		}
		else if (ntoken == INT) {
			// Crea token integer y coloca en el vector.
			tokens.push_back(new TokenInteger(ntoken,yylineno,yylloc.first_column,atoi(yytext)));
		}
		else if (ntoken == CHAR) {
			// Crea token caracter y coloca en el vector.
			tokens.push_back(new TokenCharacter(ntoken,yylineno,yylloc.first_column,yytext[1]));
		}
		else if (ntoken == FLOAT) {
			// Crea token caracter y coloca en el vector.
			tokens.push_back(new TokenFloat(ntoken,yylineno,yylloc.first_column,atof(yytext)));
		}
		else if (nToWord[ntoken] != "") {
			if (ntoken != ERROR){
				// Crea token normal y coloca en el vector.
				tokens.push_back(new Token(ntoken,yylineno,yylloc.first_column));
			} else {
				// Crea un token error y lo coloca en el vector de errores.
				errors.push_back(new TokenError(ntoken, yylineno, yylloc.first_column, yytext));
			}
		}
		if (ntoken == COMENTARIO && yytext[1] == '*') {
			; // Es un comentario, simplemente se ignora
		}
		ntoken = yylex();
	}

	/*
	Al final del ciclo, al no haber mas tokens se procede a imprimir
	Si no hay errores, se imprime el vector de tokens.
	*/
	if (errors.empty()){
		print_vector(tokens);
	} else {
		print_errors(errors);
	}

}

void execute_parser(){
	cout << "Executing parser" << endl;
    
    try {
		yyparse();
	}
	catch(const char* const errorMessage){
		cout << "Error: " << endl;
			cout << errorMessage << endl;
	}

	// Si hay errores del lexer, imprimirlos
	if (!errors.empty()){
		print_errors(errors);
	}

	// Imprimir AST
	if (!error_sintactico){
	   try {
		root_ast->imprimir(0);	
		}
		catch(...){
			cout << "error";
			return;
		}
	}
	
	table.print();
}

int main(int argc, char** argv) {	
	init_strings();
	// Cambio del input stream al archivo en argv[1]
    yyin = fopen(argv[1],"r");
    if (yyin == false){
    	cout << "Error de lectura, revise el archivo " << argv[1] << endl;
    	return 0;
    }
	if (argc > 2){
		for (int i = 2; i < argc; i++ ){
			string arg(argv[i]);
			if (arg == "-l"){
				execute_lexer();
			}
			else if (arg == "-p"){
				execute_parser();
			}
		}
	} else {
		// por ahora por defecto ejecuta el lexer
		execute_lexer();
	}

	return 0;
}