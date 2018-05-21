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

%token <arb> LLAVEABRE 1 LLAVECIERRA 2 CORCHETEABRE 3 CORCHETECIERRA 4 PARABRE 5 PARCIERRA 6
%token <arb> MAIN 7
%token <arb> INCLUDE 10 CREATE 11 EXECUTE 12 MODULE 13
%token <arb> PUNTOCOMA 14
%token <arb> WHILE 15 FOR 16 BREAK 17 IF 18 ELSE 19 RETURN 20
%token <arb> STRUCT 21 UNION 22 ARRAY 23 LIST 24 TUPLE 25 LSTRING 26 LFLOAT 27 LCHAR 28 BOOL 29 LINT 30 POINTER 31
%token <arb> TRUE 32 FALSE 33
%token <arb> IGUALA 34 DISTINTOA 35
%token <arb> SUMA 36 RESTA 37 MOD 38 DIV 39 POW 40 MULT 41 IGUAL 42 MENORIGUAL 43 MENOR 44 MAYORIGUAL 45 MAYOR 46
%token <arb> NEGACION 47 CONJUNCION 48 DISYUNCION 49
%token <arb> SPACE 50 TAB 51 COMA 52
%token <arb> SALIDA 53 ENTRADA 54
%token <num> FLOAT 57 INT 59
%token <str> IDENTIFIER 55 CHAR 56 STRING 60
%token <arb> ERROR 61
%token <arb> ENTER 62 
%token <arb> COMENTARIO 63
%token <arb> TYPE 64 NEW 65 FREE 66
%token <arb> REFERENCE 67 OPTR 68 UNIT 69
%token <num> number
//%token <str> IDENTIFIER
%token <ch> CHARACTER
%token <arb> arbol
//%token <boolean> TRUE FALSE

%type <arb> S Includelist Start Scope Typedef Varlist Declist Sec Inst Exp Literals Ids List

%%

S			: Includelist Start 								{ $$ = new raiz($1,$2,false); root_ast = new ArbolSintactico($$); }
			| MODULE Start										{ $$ = new raiz($2,true); root_ast = new ArbolSintactico($$); }
			| Start      										{ $$ = new raiz($1,false); root_ast = new ArbolSintactico($$); }
			|													{ $$ = new raiz(); root_ast = new ArbolSintactico($$); }
			;

Includelist : INCLUDE Exp Includelist							{ $$ = new include($2,$3); }
			| INCLUDE Exp 										{ $$ = new include($2); }
			;

Start 		: MAIN LLAVEABRE Scope LLAVECIERRA Start 				{ $$ = new programa($3,$5); }		
	 		| MAIN LLAVEABRE Scope LLAVECIERRA					{ $$ = new programa($3); }
			
			| Typedef IDENTIFIER PARABRE Varlist PARCIERRA	LLAVEABRE Scope LLAVECIERRA Start		{ $$ = new funcion($1,$2,$4,$7,$9); }
			| Typedef IDENTIFIER PARABRE Varlist PARCIERRA	LLAVEABRE Scope LLAVECIERRA			{ $$ = new funcion($1,$2,$4,$7); }

			| TYPE STRUCT IDENTIFIER LLAVEABRE Declist LLAVECIERRA Start 	{  $$ = new estructura($3,$5,true,$7); }
			| TYPE STRUCT IDENTIFIER LLAVEABRE Declist LLAVECIERRA			{ $$ = new estructura($3,$5,true); }
			
			| TYPE UNION IDENTIFIER LLAVEABRE Declist LLAVECIERRA Start  	{ $$ = new estructura($3,$5,false,$7); }
			| TYPE UNION IDENTIFIER LLAVEABRE Declist LLAVECIERRA 			{ $$ = new estructura($3,$5,false); }
			
			| TYPE IDENTIFIER IGUAL Typedef PUNTOCOMA Start					{ $$ = new tipo($2,$4,$6); }
			| TYPE IDENTIFIER IGUAL Typedef	PUNTOCOMA						{ $$ = new tipo($2,$4); }
			; 

Scope 		: CREATE LLAVEABRE Declist LLAVECIERRA EXECUTE LLAVEABRE Sec LLAVECIERRA 	{ $$ = new bloque($3,$7); }
			| CREATE LLAVEABRE LLAVECIERRA EXECUTE LLAVEABRE Sec LLAVECIERRA  			{ $$ = new bloque($6); }
			| EXECUTE LLAVEABRE Sec LLAVECIERRA  										{ $$ = new bloque($3); }
			;

			| BOOL  											{ $$ = new tipedec(1); }
Typedef		: LCHAR 											{ $$ = new tipedec(2); }
			| LSTRING  											{ $$ = new tipedec(3); }
			| LINT  											{ $$ = new tipedec(4); }
			| LFLOAT  											{ $$ = new tipedec(5); }
			| ARRAY Typedef CORCHETEABRE Exp CORCHETECIERRA 	{ $$ = new tipedec(6,$2,NULL,$4); }
			| LIST Typedef 										{ $$ = new tipedec(7,$2); }
			| TUPLE PARABRE Typedef COMA Typedef PARCIERRA  	{ $$ = new tipedec(8,$3,$5); }
			| IDENTIFIER										{ $$ = new tipedec(9); }
			| POINTER Typedef  									{ $$ = new tipedec(10,$2); }
			| UNIT 												{ $$ = new tipedec(11); }
			;

Varlist 	: Typedef IDENTIFIER COMA Varlist 					{ $$ = new parametros($4,$1,$2,false); }
			| Typedef REFERENCE IDENTIFIER COMA Varlist			{ $$ = new parametros($5,$1,$3,true); }
			| Typedef IDENTIFIER 								{ $$ = new parametros($1,$2,false); }
			| Typedef REFERENCE IDENTIFIER						{ $$ = new parametros($1,$3,true);}
			;

Declist 	: Typedef IDENTIFIER PUNTOCOMA Declist				{ $$ = new declaracion($4,$1,$2,NULL); }
			| Typedef IDENTIFIER IGUAL Exp PUNTOCOMA Declist	{ $$ = new declaracion($6,$1,$2,$4); }
			| Typedef IDENTIFIER PUNTOCOMA 						{ $$ = new declaracion($1,$2); }
			| Typedef IDENTIFIER IGUAL Exp PUNTOCOMA 			{ $$ = new declaracion($1,$2,$3);}
			;

Sec 		: Inst PUNTOCOMA Sec  								{ $$ = new instruccion($3,$1); }
			| Inst PUNTOCOMA									{ $$ = new instruccion($1); }
			;

Inst		: Scope					 							{ $$ = $1; }
			| Ids IGUAL Exp										{ $$ = new asignacion($1,$3);}
			| SALIDA Exp 										{ $$ = new entrada_salida($2,false); }
			| ENTRADA Exp  										{ $$ = new entrada_salida($2,true); }

			| IF PARABRE Exp PARCIERRA LLAVEABRE Sec LLAVECIERRA ELSE LLAVEABRE Sec LLAVECIERRA		{ $$ = new inst_guardia($3,$6,$10,2); }
			| IF PARABRE Exp PARCIERRA LLAVEABRE Sec LLAVECIERRA  									{ $$ = new inst_guardia($3,$6,1); }

			| FOR PARABRE Typedef IDENTIFIER IGUAL Exp COMA CORCHETEABRE Exp COMA Exp CORCHETECIERRA COMA Exp PARCIERRA LLAVEABRE Sec LLAVECIERRA 	{$$ = new it_determinada($4,$9,$11,$17);}

			| WHILE PARABRE Exp PARCIERRA LLAVEABRE Sec LLAVECIERRA  		{ $$ = new inst_guardia($3,$6,3); }
			| NEW PARABRE IDENTIFIER PARCIERRA					{ $$ =  new memoria($3,true); }
			| FREE PARABRE IDENTIFIER PARCIERRA					{ $$ =  new memoria($3,false); }
			| IDENTIFIER PARABRE List PARCIERRA 				{ $$ = new llamada($1,$3); }
			| RETURN Exp										{ $$ = new ret_brk(true); }
			| BREAK												{ $$ = new ret_brk(false); }
			| 													{ NULL; }
			;

Exp	 		: Exp SUMA Exp										{ $$ = new exp_aritmetica($1,$3,1); }
			| Exp RESTA Exp		 								{ $$ = new exp_aritmetica($1,$3,2); }
			| Exp MULT Exp										{ $$ = new exp_aritmetica($1,$3,3); }
			| Exp DIV Exp										{ $$ = new exp_aritmetica($1,$3,4); }
			| Exp MOD Exp										{ $$ = new exp_aritmetica($1,$3,5); }
			| Exp POW Exp 										{ $$ = new exp_aritmetica($1,$3,6); }
			| PARABRE Exp PARCIERRA								{ $$ = $2; }
			| RESTA Exp	 										{ $$ = new exp_aritmetica($1,2); }
			
			| Exp IGUALA Exp									{ $$ = new exp_booleana($1,$3,1); }
			| Exp DISTINTOA Exp									{ $$ = new exp_booleana($1,$3,2); }
			| Exp MENOR Exp										{ $$ = new exp_booleana($1,$3,3); }
			| Exp MAYOR Exp										{ $$ = new exp_booleana($1,$3,4); }
			| Exp MENORIGUAL Exp								{ $$ = new exp_booleana($1,$3,5); }
			| Exp MAYORIGUAL Exp								{ $$ = new exp_booleana($1,$3,6); }
			| Exp DISYUNCION Exp								{ $$ = new exp_booleana($1,$3,7); }
			| Exp CONJUNCION Exp								{ $$ = new exp_booleana($1,$3,8); }
			| NEGACION Exp										{ $$ = new exp_booleana($1,9); }

			| IDENTIFIER PARABRE List PARCIERRA 				{ $$ = new llamada($1,$3); }
			| IDENTIFIER CORCHETEABRE Exp CORCHETECIERRA 		{ $$ = new exp_index($1,$3); }

			| OPTR IDENTIFIER	 								{ $$ = new exp_point($2); }
			| Literals											{ $$ = $1; }
			;
			

Literals	: Ids												{$$ = $1;}
			| CHAR 												{ $$ = new character(); }
			| FLOAT 											{ $$ = new flotante(); }
			| INT 												{ $$ = new entero(); }
			| STRING 											{ $$ = new str(); }
			| LLAVEABRE List LLAVECIERRA 						{ $$ = new lista($2); }
			| CORCHETEABRE List CORCHETECIERRA 					{ $$ = new arreglo($2); }
			| PARABRE Exp COMA Exp PARCIERRA 					{ $$ = new tupla($2,$4); }
			| TRUE 												{ $$ = new booleano(); }
			| FALSE 											{ $$ = new booleano(); }
			;

Ids 		: IDENTIFIER PUNTO Ids 								{ $$ = $1; }
			| IDENTIFIER 										{ $$ = $1; }
			;

List		: Exp COMA List 									{ $$ = new elementos($3,$1); }
			| Exp 												{ $$ = new elementos($1); }
			;
