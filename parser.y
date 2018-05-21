%error-verbose
%{
#include <iostream>
#include <map>
#include "definiciones.h"
#include "ast.h"

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
			float flot;
			bool boolean;
			char * str;
			char ch;
			ArbolSintactico * arb;
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
%token IGUALA 34 DISTINTOA 35
%token SUMA 36 RESTA 37 MOD 38 DIV 39 POW 40 MULT 41 IGUAL 42 MENORIGUAL 43 MENOR 44 MAYORIGUAL 45 MAYOR 46
%token NEGACION 47 CONJUNCION 48 DISYUNCION 49
%token SPACE 50 TAB 51 COMA 52
%token SALIDA 53 ENTRADA 54
%token PUNTO 58
%token ERROR 61
%token ENTER 62 
%token COMENTARIO 63
%token TYPE 64 NEW 65 FREE 66
%token REFERENCE 67 OPTR 68 UNIT 69
%token <num> INT
%token <flot> FLOAT
%token <str> IDENTIFIER
%token <ch> CHAR
%token <str> STRING
%token <arb> arbol
%token <boolean> TRUE FALSE

%type <arb> S Includelist Start Typedef Varlist Declist Sec Inst Exp Ids List

%%

S			: Includelist Start 								{ $$ = new raiz($1,$2,false); root_ast = new ArbolSintactico($$); }
			| MODULE Start										{ $$ = new raiz($2,true); root_ast = new ArbolSintactico($$); }
			| Start      										{ $$ = new raiz($1,false); root_ast = new ArbolSintactico($$); }
			|													{ $$ = new raiz(); root_ast = new ArbolSintactico($$); }
			;

Includelist : INCLUDE Exp Includelist							{ $$ = new include($2,$3); }
			| INCLUDE Exp 										{ $$ = new include($2); }
			;

Start 		: MAIN LLAVEABRE Sec LLAVECIERRA Start 				{ $$ = new programa($3,$5); }		
	 		| MAIN LLAVEABRE Sec LLAVECIERRA					{ $$ = new programa($3); }
			
			| Typedef IDENTIFIER PARABRE Varlist PARCIERRA	LLAVEABRE Sec LLAVECIERRA Start		{ $$ = new funcion($1,$2,$4,$7,$9); }
			| Typedef IDENTIFIER PARABRE Varlist PARCIERRA	LLAVEABRE Sec LLAVECIERRA			{ $$ = new funcion($1,$2,$4,$7); }

			| TYPE STRUCT IDENTIFIER LLAVEABRE Declist LLAVECIERRA Start 	{ $$ = new estructura($3,$5,$7); }
			| TYPE STRUCT IDENTIFIER LLAVEABRE Declist LLAVECIERRA			{ $$ = new estructura($3,$5); }
			
			| TYPE UNION IDENTIFIER LLAVEABRE Declist LLAVECIERRA Start  	{ $$ = new estructura($3,$5,$7); }
			| TYPE UNION IDENTIFIER LLAVEABRE Declist LLAVECIERRA 			{ $$ = new estructura($3,$5); }
			
			| TYPE IDENTIFIER IGUAL Typedef PUNTOCOMA Start					{ $$ = new tipo($2,$4,$6); }
			| TYPE IDENTIFIER IGUAL Typedef	PUNTOCOMA						{ $$ = new tipo($2,$4) }
			; 

Typedef		: LCHAR 											{ $$ = new character(); }
			| BOOL  											{ $$ = new booleano(); }
			| LINT  											{ $$ = new entero(); }
			| LFLOAT  											{ $$ = new flotante(); }
			| LSTRING  											{ $$ = new str(); }
			| ARRAY Typedef CORCHETEABRE Exp CORCHETECIERRA 	{ $$ = new arreglo(); }
			| LIST Typedef 										{ $$ = new lista($2); }
			| TUPLE PARABRE Typedef COMA Typedef PARCIERRA  	{ $$ = new tupla($3,$5); }
			| POINTER Typedef  									{ $$ = new ptr(); }
			| IDENTIFIER										{ $$ = new identificador($1); }
			| UNIT 												{ $$ = new unit(); }
			;

Varlist 	: Typedef IDENTIFIER COMA Varlist 					{ $$ = new parametros($4,$1,$2,false); }
			| Typedef REFERENCE IDENTIFIER COMA Varlist			{ $$ = new parametros($4,$1,$2,true); }
			| Typedef IDENTIFIER 								{ $$ = new parametros($1,$2,false); }
			| Typedef REFERENCE IDENTIFIER						{ $$ = new parametros($1,$3,true); }
			;

Declist 	: Typedef IDENTIFIER PUNTOCOMA Declist				{ $$ = new declaracion($4,$1,$2); }
			| Typedef IDENTIFIER IGUAL Exp PUNTOCOMA Declist	{ $$ = new declaracion($4,$1,$2,$4); }
			| Typedef IDENTIFIER PUNTOCOMA 						{ $$ = new declaracion($1,$2); }
			| Typedef IDENTIFIER IGUAL Exp PUNTOCOMA 			{ $$ = new declaracion($1,$2,$4); }
			;

Sec 		: Inst PUNTOCOMA Sec  								{ $$ = new instruccion($3,$1); }
			| Inst PUNTOCOMA									{ $$ = new instruccion($1) }
			;

Inst		: CREATE LLAVEABRE Declist LLAVECIERRA EXECUTE LLAVEABRE Sec LLAVECIERRA 	{ $$ = new bloque($3,$7); }
			| CREATE LLAVEABRE LLAVECIERRA EXECUTE LLAVEABRE Sec LLAVECIERRA  			{ $$ = new bloque($6); }
			| EXECUTE LLAVEABRE Sec LLAVECIERRA  										{ $$ = new bloque($3); }

			| Ids IGUAL Exp										{ $$ = new asignacion($1,$3); }

			| SALIDA Exp 										{ $$ = new entrada_salida($2,false); }
			| ENTRADA Exp  										{ $$ = new entrada_salida($2,true); }

			| IF PARABRE Exp PARCIERRA LLAVEABRE Sec LLAVECIERRA ELSE LLAVEABRE Sec LLAVECIERRA		{ $$ = new inst_guardia($3,$6,$10,1); }
			| IF PARABRE Exp PARCIERRA LLAVEABRE Sec LLAVECIERRA  									{ $$ = new inst_guardia($3,$6,0); }
			| WHILE PARABRE Exp PARCIERRA LLAVEABRE Sec LLAVECIERRA  		{ $$ = new inst_guardia($3,$6,2); }

			| FOR PARABRE Typedef IDENTIFIER IGUAL Exp COMA CORCHETEABRE Exp COMA Exp CORCHETECIERRA COMA Exp PARCIERRA LLAVEABRE Sec LLAVECIERRA 	{ $$ = new inst_guardia(new declaracion($1,$2,$4),$9,$11,$14,$17); }

			
			| NEW PARABRE IDENTIFIER PARCIERRA					{ $$ = new memoria($3,true); }
			| FREE PARABRE IDENTIFIER PARCIERRA					{ $$ = new memoria($3,false); }
			| IDENTIFIER PARABRE List PARCIERRA 				{ cout << "No listo"; }
			| RETURN Exp										{ cout << "No listo"; }
			| BREAK												{ cout << "No listo"; }
			| 													{ cout << "No listo"; }
			;

Exp	 		: Exp SUMA Exp										{ $$ = new exp_aritmetica($1,$3,0); }
			| Exp RESTA Exp		 								{ $$ = new exp_aritmetica($1,$3,1); }
			| Exp MULT Exp										{ $$ = new exp_aritmetica($1,$3,2); }
			| Exp DIV Exp										{ $$ = new exp_aritmetica($1,$3,3); }
			| Exp MOD Exp										{ $$ = new exp_aritmetica($1,$3,4); }
			| PARABRE Exp PARCIERRA								{ $$ = new exp_aritmetica($2,6); }
			| RESTA Exp	 										{ $$ = new exp_aritmetica($2,0); }
			| Exp POW Exp 										{ $$ = new exp_aritmetica($1,$3,5); }
			
			| Exp IGUALA Exp									{ $$ = new exp_booleana($1,$3,0); }
			| Exp DISTINTOA Exp									{ $$ = new exp_booleana($1,$3,1); }
			| Exp MENOR Exp										{ $$ = new exp_booleana($1,$3,2); }
			| Exp MAYOR Exp										{ $$ = new exp_booleana($1,$3,3); }
			| Exp MENORIGUAL Exp								{ $$ = new exp_booleana($1,$3,4); }
			| Exp MAYORIGUAL Exp								{ $$ = new exp_booleana($1,$3,5); }
			| Exp DISYUNCION Exp								{ $$ = new exp_booleana($1,$3,6); }
			| Exp CONJUNCION Exp								{ $$ = new exp_booleana($1,$3,7); }
			| NEGACION Exp										{ $$ = new exp_booleana($2,8); }

			| IDENTIFIER PARABRE List PARCIERRA 				{ $$ = new llamada(new identificador($1),$3); }
			| IDENTIFIER CORCHETEABRE Exp CORCHETECIERRA 		{ cout << "No listo"; }

			| OPTR IDENTIFIER	 								{ cout << "No listo"; }
			| Ids												{ cout << "No listo"; }
			| CHAR 												{ $$ = new character($1); }
			| FLOAT 											{ $$ = new exp_aritmetica(new flotante($1),12); }
			| INT 												{ $$ = new exp_aritmetica(new entero($1),12); }
			| STRING 											{ $$ = new str($1); }
			| LLAVEABRE List LLAVECIERRA 						{ cout << "No listo"; }
			| CORCHETEABRE List CORCHETECIERRA 					{ cout << "No listo"; }
			| PARABRE Exp COMA Exp PARCIERRA 					{ cout << "No listo"; }
			| TRUE 												{ $$ = new exp_booleana(new booleano(true),13); }
			| FALSE 											{ $$ = new exp_booleana(new booleano(false),13); }
			;

Ids 		: IDENTIFIER PUNTO Ids 								{ cout << "No listo"; }
			| IDENTIFIER 										{ cout << "No listo"; }
			;

List		: Exp COMA List 									{ cout << "No listo"; }
			| Exp 												{ cout << "No listo"; }
			;
