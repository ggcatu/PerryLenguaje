%error-verbose
%{
#include <iostream>
#include <map>
#include "definiciones.h"
#include "ast.h"
#include "table.h"

using namespace std;

#define YYDEBUG 1

extern int yylex(void);
extern int yycolumn;
extern int yylineno;
extern char * yytext;
extern vector<Token *> errors;

ArbolSintactico * root_ast;
sym_table table; 
bool error_sintactico = 0;

void yyerror (char const *s) {
	error_sintactico = 1;
	cout << "Parse error:" << s << "\nFila: " << yylineno << "\n" << "Columna: " << yycolumn-1-strlen(yytext) << "\n" ; 
}

void usar_variable(string identificador, int columna){
	if (table.lookup(identificador, -1) == -1 ){
		errors.push_back(new TokenError(1,yylineno, columna,identificador, NODEFINICION));
		error_sintactico = 1;
	}
}

void declarar_variable(string identificador, int columna){
	if (! table.insert(identificador) ) {
		errors.push_back(new TokenError(1,yylineno, columna, identificador, REDEFINICION));
		error_sintactico = 1;
	}
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
%right ELSE

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
%type <arb> S Create Includelist Start Typedef Scope Varlist Declist Ids Sec Inst Exp List Literals Corchetes Parabre Identifier


%%

S			: Includelist Start 								{ $$ = new raiz($1,$2,false); root_ast = new ArbolSintactico($$); }
			| MODULE Start										{ $$ = new raiz($2,true); root_ast = new ArbolSintactico($$); }
			| Start      										{ $$ = new raiz($1,false); root_ast = new ArbolSintactico($$); }
			|													{ $$ = new raiz(); root_ast = new ArbolSintactico($$); }
			;

Includelist : INCLUDE Exp Includelist							{ $$ = new include($3,$2); }
			| INCLUDE Exp 										{ $$ = new include($2); }
			;

Start 		: MAIN LLAVEABRE Sec LLAVECIERRA Start 				{ $$ = new programa($3,$5); }		
	 		| MAIN LLAVEABRE Sec LLAVECIERRA					{ $$ = new programa($3); }
			
			| Typedef Identifier Parabre Varlist PARCIERRA	LLAVEABRE Sec LLAVECIERRA Start		{ table.exit_scope(); $$ = new funcion($1,$2,$4,$7,$9); }
			| Typedef Identifier Parabre Varlist PARCIERRA	LLAVEABRE Sec LLAVECIERRA			{ table.exit_scope(); $$ = new funcion($1,$2,$4,$7); }

			| TYPE STRUCT Identifier LLAVEABRE Declist LLAVECIERRA Start 	{ $$ = new estructura($3,$5,$7,true); }
			| TYPE STRUCT Identifier LLAVEABRE Declist LLAVECIERRA			{ $$ = new estructura($3,$5,true); }
			
			| TYPE UNION Identifier LLAVEABRE Declist LLAVECIERRA Start  	{ $$ = new estructura($3,$5,$7,false); }
			| TYPE UNION Identifier LLAVEABRE Declist LLAVECIERRA 			{ $$ = new estructura($3,$5,false); }
			
			| TYPE Identifier IGUAL Typedef PUNTOCOMA Start					{ $$ = new tipo($2,$4,$6); }
			| TYPE Identifier IGUAL Typedef	PUNTOCOMA						{ $$ = new tipo($2,$4); }
			; 

Parabre 	: PARABRE { table.new_scope(); }
			;

Scope 		: Create LLAVEABRE Declist LLAVECIERRA EXECUTE LLAVEABRE Sec LLAVECIERRA 	{ $$ = new bloque($7); table.exit_scope(); }
			| Create LLAVEABRE LLAVECIERRA EXECUTE LLAVEABRE Sec LLAVECIERRA  			{ $$ = new bloque($6); table.exit_scope(); }
			| EXECUTE LLAVEABRE Sec LLAVECIERRA  										{ $$ = new bloque($3); }
			;

Create:  	CREATE { table.new_scope(); } 
			;

Typedef		: BOOL  											{ $$ = new tipedec(0); }
			| LCHAR 											{ $$ = new tipedec(1); }
			| LSTRING  											{ $$ = new tipedec(2); }
			| LINT  											{ $$ = new tipedec(3); }
			| LFLOAT  											{ $$ = new tipedec(4); }
			| ARRAY Typedef CORCHETEABRE Exp CORCHETECIERRA 	{ $$ = new tipedec(5,$2,NULL,$4); }
			| LIST Typedef 										{ $$ = new tipedec(6,$2); }
			| TUPLE PARABRE Typedef COMA Typedef PARCIERRA  	{ $$ = new tipedec(7,$3,$5); }
			| IDENTIFIER										{ $$ = new identificador($1); }
			| POINTER Typedef  									{ $$ = new tipedec(9,$2); }
			| UNIT 												{ $$ = new tipedec(10); }
			;

Varlist 	: Typedef IDENTIFIER COMA Varlist 					{ declarar_variable($2, yylloc.first_column); $$ = new parametros($4,$1,new identificador($2),false); }
			| Typedef IDENTIFIER 								{ declarar_variable($2, yylloc.first_column); $$ = new parametros($1,new identificador($2),false); }
			| Typedef REFERENCE IDENTIFIER COMA Varlist			{ declarar_variable($3, yylloc.first_column); $$ = new parametros($5,$1,new identificador($3),true); }
			| Typedef REFERENCE IDENTIFIER						{ declarar_variable($3, yylloc.first_column); $$ = new parametros($1,new identificador($3),true);}
			|													{ $$ = (ArbolSintactico*)(NULL);}
			;

Declist 	: Typedef IDENTIFIER PUNTOCOMA Declist				{ declarar_variable($2, yylloc.first_column); $$ = new declaracion($4,$1, new identificador($2),(ArbolSintactico*)(NULL)); }
			| Typedef IDENTIFIER IGUAL Exp PUNTOCOMA Declist	{ declarar_variable($2, yylloc.first_column); $$ = new asignacion(new identificador($2), $4, $6); }
			| Typedef IDENTIFIER PUNTOCOMA 						{ declarar_variable($2, yylloc.first_column); $$ = new declaracion($1,new identificador($2)); }
			| Typedef IDENTIFIER IGUAL Exp PUNTOCOMA 			{ declarar_variable($2, yylloc.first_column); $$ = new asignacion(new identificador($2), $4, NULL);}
			;

Sec 		: Inst PUNTOCOMA Sec  								{ $$ = new instruccion($3,$1); }
			| Inst PUNTOCOMA									{ $$ = new instruccion($1); }
			;

Inst		: Scope					 							{ $$ = $1; }
			| Ids IGUAL Exp										{ $$ = new asignacion($1,$3, NULL); }
			| IF PARABRE Exp PARCIERRA LLAVEABRE Sec LLAVECIERRA  									{ $$ = new inst_guardia($3,$6,0); }
			| IF PARABRE Exp PARCIERRA LLAVEABRE Sec LLAVECIERRA ELSE LLAVEABRE Sec LLAVECIERRA		{ $$ = new inst_guardia($3,$6,$10,1); }
			| WHILE PARABRE Exp PARCIERRA LLAVEABRE Sec LLAVECIERRA  		{ $$ = new inst_guardia($3,$6,2); }
			| For Typedef Identifier COMA CORCHETEABRE Exp COMA Exp CORCHETECIERRA COMA Exp PARCIERRA LLAVEABRE Sec LLAVECIERRA 	{ table.exit_scope(); $$ = new it_determinada(new declaracion($2, $3),$6,$8,$11,$14); }
			| NEW PARABRE IDENTIFIER PARCIERRA					{ usar_variable($3, yylloc.first_column); $$ = new memoria(new identificador($3),true); }
			| FREE PARABRE IDENTIFIER PARCIERRA					{ usar_variable($3, yylloc.first_column); $$ = new memoria(new identificador($3),false); }
			| SALIDA Exp 										{ $$ = new entrada_salida($2,false); }
			| ENTRADA Exp  										{ $$ = new entrada_salida($2,true); }
			| IDENTIFIER PARABRE List PARCIERRA 				{ usar_variable($1, yylloc.first_column); $$ = new llamada(new identificador($1),$3); }
			| RETURN Exp										{ $$ = new ret_brk(true, $2); }
			| BREAK												{ $$ = new ret_brk(false, (ArbolSintactico*)(NULL)); }
			;

Identifier 	: IDENTIFIER {declarar_variable($1, yylloc.first_column); $$ = new identificador($1); }
			;

For  		: FOR PARABRE { table.new_scope(); }
			;

Exp	 		: Exp SUMA Exp										{ $$ = new exp_aritmetica($1,$3,0); }
			| Exp RESTA Exp		 								{ $$ = new exp_aritmetica($1,$3,1); }
			| Exp MULT Exp										{ $$ = new exp_aritmetica($1,$3,2); }
			| Exp DIV Exp										{ $$ = new exp_aritmetica($1,$3,3); }
			| Exp MOD Exp										{ $$ = new exp_aritmetica($1,$3,4); }
			| Exp POW Exp 										{ $$ = new exp_aritmetica($1,$3,5); }
			| PARABRE Exp PARCIERRA								{ $$ = $2; }
			| RESTA Exp	 										{ $$ = new exp_aritmetica($2,1); }
			
			| Exp IGUALA Exp									{ $$ = new exp_booleana($1,$3,0); }
			| Exp DISTINTOA Exp									{ $$ = new exp_booleana($1,$3,1); }
			| Exp MENOR Exp										{ $$ = new exp_booleana($1,$3,2); }
			| Exp MAYOR Exp										{ $$ = new exp_booleana($1,$3,3); }
			| Exp MENORIGUAL Exp								{ $$ = new exp_booleana($1,$3,4); }
			| Exp MAYORIGUAL Exp								{ $$ = new exp_booleana($1,$3,5); }
			| Exp DISYUNCION Exp								{ $$ = new exp_booleana($1,$3,6); }
			| Exp CONJUNCION Exp								{ $$ = new exp_booleana($1,$3,7); }
			| NEGACION Exp										{ $$ = new exp_booleana($2,8); }

			| IDENTIFIER PARABRE List PARCIERRA 				{ usar_variable($1, yylloc.first_column); $$ = new llamada(new identificador($1),$3); }

			| OPTR Ids	 										{ $$ = new exp_point(new ids($2)); }
			| Literals											{ $$ = $1; }
			;
			

Literals	: Ids												{ $$ = $1; }
			| CHAR 												{ $$ = new character($1); }
			| FLOAT 											{ $$ = new flotante($1); }
			| INT 												{ $$ = new entero($1); }
			| STRING 											{ $$ = new str($1); }
			| LLAVEABRE List LLAVECIERRA 						{ $$ = new lista($2); }
			| CORCHETEABRE List CORCHETECIERRA 					{ $$ = new arreglo($2); }
			| PARABRE Exp COMA Exp PARCIERRA 					{ $$ = new tupla($2,$4); }
			| TRUE 												{ $$ = new booleano($1); }
			| FALSE 											{ $$ = new booleano($1); }
			;

Ids 		: IDENTIFIER PUNTO Ids 								{ usar_variable($1, yylloc.first_column); $$ = new ids(new identificador($1),$3); }
			| IDENTIFIER Corchetes 								{ usar_variable($1, yylloc.first_column); $$ = new ids(new identificador($1),(ArbolSintactico*)(NULL),$2); }
			| IDENTIFIER 										{ usar_variable($1, yylloc.first_column); $$ = new ids(new identificador($1)); }
			;

Corchetes	: CORCHETEABRE Exp CORCHETECIERRA Corchetes 		{ $$ = new exp_index($2,$4); }
			| CORCHETEABRE Exp CORCHETECIERRA 					{ $$ = new exp_index($2); }
			;

List		: Exp COMA List 									{ $$ = new elementos($3,$1); }
			| Exp 												{ $$ = new elementos($1); }
			;
