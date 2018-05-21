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
%token IGUALA 32 DISTINTOA 33
%token SUMA 34 RESTA 35 MOD 36 DIV 37 POW 38 MULT 39 IGUAL 40 MENORIGUAL 41 MENOR 42 MAYORIGUAL 43 MAYOR 44
%token NEGACION 45 CONJUNCION 46 DISYUNCION 47
%token SPACE 48 TAB 49 COMA 50
%token SALIDA 51 ENTRADA 52
%token PUNTO 53
%token ERROR 54
%token ENTER 55 
%token COMENTARIO 56
%token TYPE 57 NEW 58 FREE 59
%token REFERENCE 60 OPTR 61 UNIT 62

%token <num> INT
%token <flot> FLOAT
%token <str> IDENTIFIER
%token <ch> CHAR
%token <str> STRING 
%token <boolean> TRUE FALSE
%type <arb> S Includelist Start Typedef Scope Varlist Declist Ids Sec Inst Exp List Literals

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
			
			| Typedef IDENTIFIER PARABRE Varlist PARCIERRA	LLAVEABRE Scope LLAVECIERRA Start		{ $$ = new funcion($1,new identificador($2),$4,$7,$9); }
			| Typedef IDENTIFIER PARABRE Varlist PARCIERRA	LLAVEABRE Scope LLAVECIERRA			{ $$ = new funcion($1,new identificador($2),$4,$7); }

			| TYPE STRUCT IDENTIFIER LLAVEABRE Declist LLAVECIERRA Start 	{  $$ = new estructura(new identificador($3),$5,$7,true); }
			| TYPE STRUCT IDENTIFIER LLAVEABRE Declist LLAVECIERRA			{ $$ = new estructura(new identificador($3),$5,true); }
			
			| TYPE UNION IDENTIFIER LLAVEABRE Declist LLAVECIERRA Start  	{ $$ = new estructura(new identificador($3),$5,$7,false); }
			| TYPE UNION IDENTIFIER LLAVEABRE Declist LLAVECIERRA 			{ $$ = new estructura(new identificador($3),$5,false); }
			
			| TYPE IDENTIFIER IGUAL Typedef PUNTOCOMA Start					{ $$ = new tipo(new identificador($2),$4,$6); }
			| TYPE IDENTIFIER IGUAL Typedef	PUNTOCOMA						{ $$ = new tipo(new identificador($2),$4); }
			; 

Scope 		: CREATE LLAVEABRE Declist LLAVECIERRA EXECUTE LLAVEABRE Sec LLAVECIERRA 	{ $$ = new bloque($3,$7); }
			| CREATE LLAVEABRE LLAVECIERRA EXECUTE LLAVEABRE Sec LLAVECIERRA  			{ $$ = new bloque($6); }
			| EXECUTE LLAVEABRE Sec LLAVECIERRA  										{ $$ = new bloque($3); }
			;

Typedef		: BOOL  											{ $$ = new tipedec(1); }
			| LCHAR 											{ $$ = new tipedec(2); }
			| LSTRING  											{ $$ = new tipedec(3); }
			| LINT  											{ $$ = new tipedec(4); }
			| LFLOAT  											{ $$ = new tipedec(5); }
			| ARRAY Typedef CORCHETEABRE Exp CORCHETECIERRA 	{ $$ = new tipedec(6,$2,NULL,$4); }
			| LIST Typedef 										{ $$ = new tipedec(7,$2); }
			| TUPLE PARABRE Typedef COMA Typedef PARCIERRA  	{ $$ = new tipedec(8,$3,$5); }
			| IDENTIFIER										{ $$ = new identificador($1); }
			| POINTER Typedef  									{ $$ = new tipedec(10,$2); }
			| UNIT 												{ $$ = new tipedec(11); }
			;

Varlist 	: Typedef IDENTIFIER COMA Varlist 					{ $$ = new parametros($4,$1,new identificador($2),false); }
			| Typedef REFERENCE IDENTIFIER COMA Varlist			{ $$ = new parametros($5,$1,new identificador($3),true); }
			| Typedef IDENTIFIER 								{ $$ = new parametros($1,new identificador($2),false); }
			| Typedef REFERENCE IDENTIFIER						{ $$ = new parametros($1,new identificador($3),true);}
			;

Declist 	: Typedef IDENTIFIER PUNTOCOMA Declist				{ $$ = new declaracion($4,$1, new identificador($2),(ArbolSintactico*)(NULL)); }
			| Typedef IDENTIFIER IGUAL Exp PUNTOCOMA Declist	{ $$ = new declaracion($6,$1,new identificador($2),$4); }
			| Typedef IDENTIFIER PUNTOCOMA 						{ $$ = new declaracion($1,new identificador($2)); }
			| Typedef IDENTIFIER IGUAL Exp PUNTOCOMA 			{ $$ = new declaracion($1,new identificador($2),$4);}
			;

Sec 		: Inst PUNTOCOMA Sec  								{ $$ = new instruccion($3,$1); }
			| Inst PUNTOCOMA									{ $$ = new instruccion($1); }
			;

Inst		: Scope					 							{ $$ = $1; }
			| Ids IGUAL Exp										{ $$ = new asignacion($1,$3);}
			| SALIDA Exp 										{ cout << "salida" << endl; $$ = new entrada_salida($2,false); }
			| ENTRADA Exp  										{ $$ = new entrada_salida($2,true); }

			| IF PARABRE Exp PARCIERRA LLAVEABRE Sec LLAVECIERRA ELSE LLAVEABRE Sec LLAVECIERRA		{ $$ = new inst_guardia($3,$6,$10,2); }
			| IF PARABRE Exp PARCIERRA LLAVEABRE Sec LLAVECIERRA  									{ $$ = new inst_guardia($3,$6,1); }

			| FOR PARABRE Typedef IDENTIFIER IGUAL Exp COMA CORCHETEABRE Exp COMA Exp CORCHETECIERRA COMA Exp PARCIERRA LLAVEABRE Sec LLAVECIERRA 	{$$ = new it_determinada(new declaracion($3,new identificador($4),$6),$9,$11,$17);}

			| WHILE PARABRE Exp PARCIERRA LLAVEABRE Sec LLAVECIERRA  		{ $$ = new inst_guardia($3,$6,3); }
			| NEW PARABRE IDENTIFIER PARCIERRA					{ $$ =  new memoria(new identificador($3),true); }
			| FREE PARABRE IDENTIFIER PARCIERRA					{ $$ =  new memoria(new identificador($3),false); }
			| IDENTIFIER PARABRE List PARCIERRA 				{ $$ = new llamada(new identificador($1),$3); }
			| RETURN Exp										{ $$ = new ret_brk(true, $2); }
			| BREAK												{ $$ = new ret_brk(false, (ArbolSintactico*)(NULL)); }
			| 													{ NULL; }
			;

Exp	 		: Exp SUMA Exp										{ $$ = new exp_aritmetica($1,$3,1); }
			| Exp RESTA Exp		 								{ $$ = new exp_aritmetica($1,$3,2); }
			| Exp MULT Exp										{ $$ = new exp_aritmetica($1,$3,3); }
			| Exp DIV Exp										{ $$ = new exp_aritmetica($1,$3,4); }
			| Exp MOD Exp										{ $$ = new exp_aritmetica($1,$3,5); }
			| Exp POW Exp 										{ $$ = new exp_aritmetica($1,$3,6); }
			| PARABRE Exp PARCIERRA								{ $$ = $2; }
			| RESTA Exp	 										{ $$ = new exp_aritmetica($2,2); }
			
			| Exp IGUALA Exp									{ $$ = new exp_booleana($1,$3,1); }
			| Exp DISTINTOA Exp									{ $$ = new exp_booleana($1,$3,2); }
			| Exp MENOR Exp										{ $$ = new exp_booleana($1,$3,3); }
			| Exp MAYOR Exp										{ $$ = new exp_booleana($1,$3,4); }
			| Exp MENORIGUAL Exp								{ $$ = new exp_booleana($1,$3,5); }
			| Exp MAYORIGUAL Exp								{ $$ = new exp_booleana($1,$3,6); }
			| Exp DISYUNCION Exp								{ $$ = new exp_booleana($1,$3,7); }
			| Exp CONJUNCION Exp								{ $$ = new exp_booleana($1,$3,8); }
			| NEGACION Exp										{ $$ = new exp_booleana($2,9); }

			| IDENTIFIER PARABRE List PARCIERRA 				{ $$ = new llamada(new identificador($1),$3); }
			| IDENTIFIER CORCHETEABRE Exp CORCHETECIERRA 		{ $$ = new exp_index(new identificador($1),$3); }

			| OPTR IDENTIFIER	 								{ $$ = new exp_point(new identificador($2)); }
			| Literals											{ $$ = $1; }
			;
			

Literals	: Ids												{$$ = $1;}
			| CHAR 												{ $$ = new character($1); }
			| FLOAT 											{ $$ = new flotante($1); }
			| INT 												{ $$ = new entero($1); }
			| STRING 											{ $$ = new str($1); }
			| LLAVEABRE List LLAVECIERRA 						{ $$ = new lista($2); }
			| CORCHETEABRE List CORCHETECIERRA 					{ $$ = new arreglo($2); }
			| PARABRE Exp COMA Exp PARCIERRA 					{ $$ = new tupla($2,$4); }
			| TRUE 												{ $$ = new booleano(true); }
			| FALSE 											{ $$ = new booleano(false); }
			;

Ids 		: IDENTIFIER PUNTO Ids 								{$$ = new identificador($1); }
			| IDENTIFIER 										{$$ = new identificador($1); }
			;

List		: Exp COMA List 									{ $$ = new elementos($3,$1); }
			| Exp 												{ $$ = new elementos($1); }
			;
