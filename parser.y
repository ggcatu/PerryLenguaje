%error-verbose
%{
#include <iostream>
#include "definiciones.h"

using namespace std;

#define YYDEBUG 1

extern int yylex(void);
extern int yycolumn;
extern int yylineno;
extern char * yytext;

ArbolSintactico * root_ast;

bool error_sintactico = 0; 

void yyerror (char const *s) {
	error_sintactico = 1;
	cout << "Parse error:" << s << "\nFila: " << yylineno << "\n" << "Columna: " << yycolumn-1-strlen(yytext) << "\n" ; 
}

%}

%define parse.lac full

%union {	
			int num; 
			bool boolean;
			char * str;
			char ch;
		}

%locations
%start S

%left CONJUNCION DISYUNCION
%left IGUALA DISTINTOA MENOR MAYOR MAYORIGUAL MENORIGUAL
%left SUMA RESTA
%left POW MULT DIV MOD
%right NEGACION

%token LLAVEABRE 1 LLAVECIERRA 2 CORCHETEABRE 3 CORCHETECIERRA 4 PARABRE 5 PARCIERRA 6
%token MAIN 7
%token INCLUDE 10 CREATE 11 EXECUTE 12 MODULE 13
%token PUNTOCOMA 14
%token WHILE 15 FOR 16 BREAK 17 IF 18 ELSE 19 RETURN 20
%token STRUCT 21 UNION 22 ARRAY 23 LIST 24 TUPLE 25 LSTRING 26 LFLOAT 27 LCHAR 28 BOOL 29 LINT 30 POINTER 31
%token TRUE 32 FALSE 33
%token IGUALA 34 DISTINTOA 35
%token SUMA 36 RESTA 37 MOD 38 DIV 39 POW 40 MULT 41 IGUAL 42 MENORIGUAL 43 MENOR 44 MAYORIGUAL 45 MAYOR 46
%token NEGACION 47 CONJUNCION 48 DISYUNCION 49
%token SPACE 50 TAB 51 COMA 52
%token SALIDA 53 ENTRADA 54
%token IDENTIFIER 55 CHAR 56 FLOAT 57 PUNTO 58 INT 59 STRING 60
%token ERROR 61
%token ENTER 62 
%token COMENTARIO 63
%token TYPE 64 NEW 65 FREE 66
%token REFERENCE 67 OPTR 68 UNIT 69
%token <num> number
//%token <str> IDENTIFIER
%token <ch> CHARACTER
%token <arb> arbol
//%token <boolean> TRUE FALSE

%type <void> S Includelist Start Scope Typedef Varlist Declist Inst Sec Exp Ids List

%%

S			: Includelist Start 								{$$ = new raiz($1,$2); root_ast = new ArbolSintactico($$); }
			| MODULE Start										{$$ = new raiz($2); root_ast = new ArbolSintactico($$); }
			| Start      										{$$ = new raiz($1); root_ast = new ArbolSintactico($$); }
			|													{$$ = new raiz(); root_ast = new ArbolSintactico($$); }
			;

Includelist : INCLUDE Exp Includelist							{ cout << "No listo"; }
			| INCLUDE Exp 										{ cout << "No listo"; }
			;

Start 		: MAIN LLAVEABRE Sec LLAVECIERRA Start 				{ cout << "No listo"; }		
	 		| MAIN LLAVEABRE Sec LLAVECIERRA					{ cout << "No listo"; }
			
			| Typedef IDENTIFIER PARABRE Varlist PARCIERRA	LLAVEABRE Sec LLAVECIERRA Start		{ cout << "No listo"; }
			| Typedef IDENTIFIER PARABRE Varlist PARCIERRA	LLAVEABRE Sec LLAVECIERRA			{ cout << "No listo"; }

			| TYPE STRUCT IDENTIFIER LLAVEABRE Declist LLAVECIERRA Start 	{ cout << "No listo"; }
			| TYPE STRUCT IDENTIFIER LLAVEABRE Declist LLAVECIERRA			{ cout << "No listo"; }
			
			| TYPE UNION IDENTIFIER LLAVEABRE Declist LLAVECIERRA Start  	{ cout << "No listo"; }
			| TYPE UNION IDENTIFIER LLAVEABRE Declist LLAVECIERRA 			{ cout << "No listo"; }
			
			| TYPE IDENTIFIER IGUAL Typedef PUNTOCOMA Start					{ cout << "No listo"; }
			| TYPE IDENTIFIER IGUAL Typedef	PUNTOCOMA						{ cout << "No listo"; }
			; 

Scope 		: CREATE LLAVEABRE Declist LLAVECIERRA EXECUTE LLAVEABRE Sec LLAVECIERRA 	{ cout << "No listo"; }
			| CREATE LLAVEABRE LLAVECIERRA EXECUTE LLAVEABRE Sec LLAVECIERRA  			{ cout << "No listo"; }
			| EXECUTE LLAVEABRE Sec LLAVECIERRA  										{ cout << "No listo"; }
			;

Typedef		: LCHAR 											{ cout << "No listo"; }
			| BOOL  											{ cout << "No listo"; }
			| LINT  											{ cout << "No listo"; }
			| LFLOAT  											{ cout << "No listo"; }
			| LSTRING  											{ cout << "No listo"; }
			| ARRAY Typedef CORCHETEABRE Exp CORCHETECIERRA 	{ cout << "No listo"; }
			| LIST Typedef 										{ cout << "No listo"; }
			| TUPLE PARABRE Typedef COMA Typedef PARCIERRA  	{ cout << "No listo"; }
			| POINTER Typedef  									{ cout << "No listo"; }
			| IDENTIFIER										{ cout << "No listo"; }
			| UNIT 												{ cout << "No listo"; }
			;

Varlist 	: Typedef IDENTIFIER COMA Varlist 					{ cout << "No listo"; }
			| Typedef REFERENCE IDENTIFIER COMA Varlist			{ cout << "No listo"; }
			| Typedef IDENTIFIER 								{ cout << "No listo"; }
			| Typedef REFERENCE IDENTIFIER						{ cout << "No listo"; }
			;

Declist 	: Typedef IDENTIFIER PUNTOCOMA Declist				{ cout << "No listo"; }
			| Typedef IDENTIFIER IGUAL Exp PUNTOCOMA Declist	{ cout << "No listo"; }
			| Typedef IDENTIFIER PUNTOCOMA 						{ cout << "No listo"; }
			| Typedef IDENTIFIER IGUAL Exp PUNTOCOMA 			{ cout << "No listo"; }
			;

Sec 		: Inst PUNTOCOMA Sec  								{ cout << "No listo"; }
			| Inst PUNTOCOMA									{ cout << "No listo"; }
			;

Inst		: Scope					 							{ cout << "No listo"; }
			| Ids IGUAL Exp										{ cout << "No listo"; }
			| SALIDA Exp 										{ cout << "No listo"; }
			| ENTRADA Exp  										{ cout << "No listo"; }

			| IF PARABRE Exp PARCIERRA LLAVEABRE Sec LLAVECIERRA ELSE LLAVEABRE Sec LLAVECIERRA		{ cout << "No listo"; }
			| IF PARABRE Exp PARCIERRA LLAVEABRE Sec LLAVECIERRA  									{ cout << "No listo"; }

			| FOR PARABRE Typedef IDENTIFIER IGUAL Exp COMA CORCHETEABRE Exp COMA Exp CORCHETECIERRA COMA Exp PARCIERRA LLAVEABRE Sec LLAVECIERRA 	{ cout << "No listo"; }

			| WHILE PARABRE Exp PARCIERRA LLAVEABRE Sec LLAVECIERRA  		{ cout << "No listo"; }
			| NEW PARABRE IDENTIFIER PARCIERRA					{ cout << "No listo"; }
			| FREE PARABRE IDENTIFIER PARCIERRA					{ cout << "No listo"; }
			| IDENTIFIER PARABRE List PARCIERRA 				{ cout << "No listo"; }
			| RETURN Exp										{ cout << "No listo"; }
			| BREAK												{ cout << "No listo"; }
			| 													{ cout << "No listo"; }
			;

Exp	 		: Exp SUMA Exp										{ cout << "No listo"; }
			| Exp RESTA Exp		 								{ cout << "No listo"; }
			| Exp MULT Exp										{ cout << "No listo"; }
			| Exp DIV Exp										{ cout << "No listo"; }
			| Exp MOD Exp										{ cout << "No listo"; }
			| PARABRE Exp PARCIERRA								{ cout << "No listo"; }
			| RESTA Exp	 										{ cout << "No listo"; }
			| Exp POW Exp 										{ cout << "No listo"; }
			
			| Exp IGUALA Exp									{ cout << "No listo"; }
			| Exp DISTINTOA Exp									{ cout << "No listo"; }
			| Exp MENOR Exp										{ cout << "No listo"; }
			| Exp MAYOR Exp										{ cout << "No listo"; }
			| Exp MENORIGUAL Exp								{ cout << "No listo"; }
			| Exp MAYORIGUAL Exp								{ cout << "No listo"; }
			| Exp DISYUNCION Exp								{ cout << "No listo"; }
			| Exp CONJUNCION Exp								{ cout << "No listo"; }
			| NEGACION Exp										{ cout << "No listo"; }

			| IDENTIFIER PARABRE List PARCIERRA 				{ cout << "No listo"; }
			| IDENTIFIER CORCHETEABRE Exp CORCHETECIERRA 		{ cout << "No listo"; }

			| OPTR IDENTIFIER	 								{ cout << "No listo"; }
			| Ids												{ cout << "No listo"; }
			| CHAR 												{ cout << "No listo"; }
			| FLOAT 											{ cout << "No listo"; }
			| INT 												{ cout << "No listo"; }
			| STRING 											{ cout << "No listo"; }
			| LLAVEABRE List LLAVECIERRA 						{ cout << "No listo"; }
			| CORCHETEABRE List CORCHETECIERRA 					{ cout << "No listo"; }
			| PARABRE Exp COMA Exp PARCIERRA 					{ cout << "No listo"; }
			| TRUE 												{ cout << "No listo"; }
			| FALSE 											{ cout << "No listo"; }
			;

Ids 		: IDENTIFIER PUNTO Ids 								{ cout << "No listo"; }
			| IDENTIFIER 										{ cout << "No listo"; }
			;

List		: Exp COMA List 									{ cout << "No listo"; }
			| Exp 												{ cout << "No listo"; }
			;
