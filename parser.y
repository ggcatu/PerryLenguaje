%error-verbose
%{
#include <iostream>
#include <map>
#include "definiciones.h"
#include "ast.h"
#include "table.h"
#include "tipos.h"
#include <sstream>

using namespace std;

extern int yylex(void);
extern int yycolumn;
extern int yylineno;
extern char * yytext;
extern vector<Token *> errors;

//Map que contiene los alias
map<string,ArbolSintactico*> alias;

ArbolSintactico * root_ast;
sym_table table;
bool error_sintactico = 0;
int last_uuid = 1;

void yyerror (char const *s) {
	error_sintactico = 1;
	if (errors.empty()){
		cout << "\nError de Parseo:\nCaracter Inesperado: " << yytext << "en la fila " << yylineno << ", columna " << yycolumn-1-strlen(yytext) << "\n" ; 
	} else {
		print_errors(errors);
	}
}

void open_scope(ArbolSintactico * arb) {
	identificador * b = (identificador *)arb;
	table_element * elemento = table.lookup(b->valor, -1);
	if(elemento != NULL){
		elemento->set_child_scope(table.new_scope());
	} else {
		cout << b->valor << " no encontrado";
	}
}

void usar_variable(string identificador, int columna){
	if (table.lookup(identificador, -1) == NULL ){
		errors.push_back(new TokenError(1,yylineno, columna,identificador, NODEFINICION));
		error_sintactico = 1;
	}
}

void asignar_tipo(ArbolSintactico * tipo, string variable){
	if (tipo->is_type){
		bool value = ((tipedec *)tipo)->asignar_tipo(variable, &table);
		if (!value){
			cout << "Error: " << ((identificador *)tipo)->valor << " no es un tipo, en la fila " << yylineno << ", columna " <<  yycolumn-1-strlen(yytext) << endl;
			error_sintactico = 1;			
		}
	} 
}

string uuid(){
	stringstream sstm;
	sstm << "$anonim_" << last_uuid;
	last_uuid++;
	return sstm.str();
}

void usar_variable_top(string identificador, int columna){
	if (table.lookup_top(identificador) == NULL ){
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
%token TYPE_ERROR 

%token <num> INT
%token <flot> FLOAT
%token <str> IDENTIFIER
%token <ch> CHAR
%token <str> STRING 
%token <boolean> TRUE FALSE
%type <arb> S Create Includelist Start Typedef Scope Varlist Declist Ids Sec Inst Exp List Literals Corchetes Parabre Ids_plox Llaveabre LlaveabreSp Llavecierra IdentifierSp
%type <str> Identifier


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
	 		| MAIN LLAVEABRE error LLAVECIERRA					{ $$ = (ArbolSintactico*)(NULL); }
	 		| MAIN LLAVEABRE error LLAVECIERRA Start			{ $$ = (ArbolSintactico*)(NULL); }
			
			| IdentifierSp Parabre Varlist PARCIERRA	LLAVEABRE Sec Llavecierra Start		{ $$ = new funcion($1, $6, $8); }
			| IdentifierSp Parabre Varlist PARCIERRA 	LLAVEABRE Sec Llavecierra			{ $$ = new funcion($1, $6); }
			| IdentifierSp Parabre error PARCIERRA		LLAVEABRE Sec Llavecierra			{ $$ = (ArbolSintactico*)(NULL); }
			| IdentifierSp Parabre error PARCIERRA		LLAVEABRE Sec Llavecierra Start		{ $$ = (ArbolSintactico*)(NULL); }
			| IdentifierSp Parabre Varlist PARCIERRA	LLAVEABRE error Llavecierra			{ $$ = (ArbolSintactico*)(NULL); }
			| IdentifierSp Parabre Varlist PARCIERRA	LLAVEABRE error Llavecierra	Start	{ $$ = (ArbolSintactico*)(NULL); }

			| TYPE STRUCT Llaveabre Declist Llavecierra Start 	{ $$ = new skip($6); }
			| TYPE STRUCT Llaveabre Declist Llavecierra			{ $$ = (ArbolSintactico*)(NULL); }
			| TYPE STRUCT Llaveabre error Llavecierra Start 	{ $$ = (ArbolSintactico*)(NULL); }
			| TYPE STRUCT Llaveabre error Llavecierra			{ $$ = (ArbolSintactico*)(NULL); }
			
			| TYPE UNION Llaveabre Declist Llavecierra Start  	{ $$ = new skip($6); }
			| TYPE UNION Llaveabre Declist Llavecierra 			{ $$ = (ArbolSintactico*)(NULL); }
			| TYPE UNION Llaveabre error Llavecierra Start  	{ $$ = (ArbolSintactico*)(NULL); }
			| TYPE UNION Llaveabre error Llavecierra 			{ $$ = (ArbolSintactico*)(NULL); }
			
			| TYPE Identifier IGUAL Typedef PUNTOCOMA Start		{ $$ = new skip($6); alias[$2] = $4;}
			| TYPE Identifier IGUAL Typedef	PUNTOCOMA			{ $$ = (ArbolSintactico*)(NULL); alias[$2] = $4; }
			; 

Llaveabre 	: IDENTIFIER LLAVEABRE 	 							{ declarar_variable($1, yylloc.first_column); asignar_tipo(new tipedec(* new tipo_tipo()), $1); $$ = new identificador($1); open_scope($$); }
			;

Llavecierra : LLAVECIERRA 										{ table.exit_scope(); }
			;

Parabre 	: PARABRE 											{ table.new_scope(); }
			;

Scope 		: Create LLAVEABRE Declist LLAVECIERRA EXECUTE LLAVEABRE Sec LLAVECIERRA 	{ $$ = new bloque($3,$7); table.exit_scope(); }
			| Create LLAVEABRE LLAVECIERRA EXECUTE LLAVEABRE Sec LLAVECIERRA  			{ $$ = new bloque($6); table.exit_scope(); }
			| EXECUTE LLAVEABRE Sec LLAVECIERRA  										{ $$ = new bloque($3); }
			;

Create:  	CREATE { table.new_scope(); } 
			;

Typedef		: BOOL  											{ $$ = new tipedec(tipo_bool::instance()); }
			| LCHAR 											{ $$ = new tipedec(tipo_char::instance()); }
			| LSTRING  											{ $$ = new tipedec(tipo_string::instance()); }
			| LINT  											{ $$ = new tipedec(tipo_int::instance()); }
			| LFLOAT  											{ $$ = new tipedec(tipo_float::instance()); }
			| ARRAY Typedef CORCHETEABRE Exp CORCHETECIERRA 	{ $$ = new tipedec(*(new tipo_array(((tipedec *)$2)->tipo)),$2,NULL,$4); }
			| LIST Typedef 										{ $$ = new tipedec(*new tipo_list(((tipedec *)$2)->tipo),$2); }
			| TUPLE PARABRE Typedef COMA Typedef PARCIERRA  	{ $$ = new tipedec(*new tipo_tuple(((tipedec *)$3)->tipo, ((tipedec *)$5)->tipo),$3,$5);}
			| IDENTIFIER										{ $$ = alias[$1]; }
			| TYPE STRUCT LlaveabreSp Declist Llavecierra		{ $$ = new tipedec(tipo_identifier::instance(), $3); }
			| TYPE STRUCT LlaveabreSp error Llavecierra			{ $$ = (ArbolSintactico*)(NULL); }
			
			| TYPE UNION LlaveabreSp Declist Llavecierra 		{ $$ = new tipedec(tipo_identifier::instance(), $3); }
			| TYPE UNION LlaveabreSp error Llavecierra 			{ $$ = (ArbolSintactico*)(NULL); }

			| POINTER Typedef  									{ $$ = new tipedec(*new tipo_pointer(((tipedec *)$2)->tipo),$2); }
			| UNIT 												{ $$ = new tipedec(tipo_unit::instance()); }
			;

LlaveabreSp : LLAVEABRE 										{  string u = uuid(); $$ = new identificador(u); 
																   declarar_variable(u, yylloc.first_column); open_scope(new identificador(u));}
			; 


Varlist 	: IdentifierSp COMA Varlist 						{}
			| IdentifierSp 										{}
			| Typedef REFERENCE IDENTIFIER COMA Varlist			{ declarar_variable($3, yylloc.first_column); asignar_tipo($1, $3);}
			| Typedef REFERENCE IDENTIFIER						{ declarar_variable($3, yylloc.first_column); asignar_tipo($1, $3);}
			|													{ $$ = (ArbolSintactico*)(NULL);}
			;

Declist 	: IdentifierSp PUNTOCOMA Declist					{$$ = new skip($3); }
			| IdentifierSp IGUAL Exp PUNTOCOMA Declist			{$$ = new asignacion($1, $3, $5); }
			| IdentifierSp PUNTOCOMA 							{$$ = new skip(); }
			| IdentifierSp IGUAL Exp PUNTOCOMA 					{$$ = new asignacion($1, $3, NULL);}
			;

Sec 		: Inst PUNTOCOMA Sec  								{ $$ = new instruccion($3,$1); }
			| Inst PUNTOCOMA									{ $$ = new instruccion($1); }
			;

Inst		: Scope					 							{ $$ = $1; }
			| Ids IGUAL Exp										{ $$ = new asignacion($1,$3, NULL); }
			| IF PARABRE Exp PARCIERRA LLAVEABRE Sec LLAVECIERRA  									{ $$ = new inst_guardia($3,$6,0); }
			| IF PARABRE Exp PARCIERRA LLAVEABRE Sec LLAVECIERRA ELSE LLAVEABRE Sec LLAVECIERRA		{ $$ = new inst_guardia($3,$6,$10,1); }
			| WHILE PARABRE Exp PARCIERRA LLAVEABRE Sec LLAVECIERRA  								{ $$ = new inst_guardia($3,$6,2); }
			| For IdentifierSp COMA CORCHETEABRE Exp COMA Exp CORCHETECIERRA COMA Exp PARCIERRA LLAVEABRE Sec LLAVECIERRA 	{ table.exit_scope(); $$ = new it_determinada($5,$7,$10,$13); }
			| NEW PARABRE IDENTIFIER PARCIERRA					{ usar_variable($3, yylloc.first_column); $$ = new memoria(new identificador($3),true); }
			| FREE PARABRE IDENTIFIER PARCIERRA					{ usar_variable($3, yylloc.first_column); $$ = new memoria(new identificador($3),false); }
			| SALIDA Exp 										{ $$ = new entrada_salida($2,false); }
			| ENTRADA Exp  										{ $$ = new entrada_salida($2,true); }
			| IDENTIFIER PARABRE List PARCIERRA 				{ usar_variable($1, yylloc.first_column); $$ = new llamada(new identificador($1),$3); }
			| RETURN Exp										{ $$ = new ret_brk(true, $2); }
			| BREAK												{ $$ = new ret_brk(false, (ArbolSintactico*)(NULL)); }
			;

Identifier 	: IDENTIFIER 										{declarar_variable($1, yylloc.first_column); $$ = $1; }
			;

IdentifierSp: Typedef IDENTIFIER 								{declarar_variable($2, yylloc.first_column); asignar_tipo($1, $2); $$ = new identificador($2); }
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

Ids 		: Ids_plox Ids										{ $$ = new ids($1,$2); table.exit_scope();}
			| IDENTIFIER Corchetes 								{ usar_variable($1, yylloc.first_column); $$ = new ids(new identificador($1),(ArbolSintactico*)(NULL),$2); }
			| IDENTIFIER 										{ usar_variable($1, yylloc.first_column); $$ = new ids(new identificador($1)); }
			;

Ids_plox 	: IDENTIFIER PUNTO 									{  usar_variable($1, yylloc.first_column); table.open_scope($1); $$ = new identificador($1); }
			;

Corchetes	: CORCHETEABRE Exp CORCHETECIERRA Corchetes 		{ $$ = new exp_index($2,$4); }
			| CORCHETEABRE Exp CORCHETECIERRA 					{ $$ = new exp_index($2); }
			;

List		: Exp COMA List 									{ $$ = new elementos($3,$1); }
			| Exp 												{ $$ = new elementos($1); }
			;
