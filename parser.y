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

%type <void> S Includelist Start Scope Typedef Varlist Declist Inst Sec Exp Literals Ids List

%%

S			: Includelist Start 								{$$ = new raiz($1,$2); root_ast = new ArbolSintactico($$); }
			| MODULE Start										{$$ = new raiz($2); root_ast = new ArbolSintactico($$); }
			| Start      										{$$ = new raiz($1); root_ast = new ArbolSintactico($$); }
			|													{$$ = new raiz(); root_ast = new ArbolSintactico($$); }
			;

Includelist : INCLUDE Exp Includelist							{$$ = new }
			| INCLUDE Exp 										{$$ = new }
			;

Start 		: MAIN LLAVEABRE Sec LLAVECIERRA Start 				{$$ = new }		
	 		| MAIN LLAVEABRE Sec LLAVECIERRA					{$$ = new }
			
			| Typedef IDENTIFIER PARABRE Varlist PARCIERRA	LLAVEABRE Sec LLAVECIERRA Start		{$$ = new }
			| Typedef IDENTIFIER PARABRE Varlist PARCIERRA	LLAVEABRE Sec LLAVECIERRA			{$$ = new }

			| TYPE STRUCT IDENTIFIER LLAVEABRE Declist LLAVECIERRA Start 	{$$ = new }
			| TYPE STRUCT IDENTIFIER LLAVEABRE Declist LLAVECIERRA			{$$ = new }
			
			| TYPE UNION IDENTIFIER LLAVEABRE Declist LLAVECIERRA Start  	{$$ = new }
			| TYPE UNION IDENTIFIER LLAVEABRE Declist LLAVECIERRA 			{$$ = new }
			
			| TYPE IDENTIFIER IGUAL Typedef PUNTOCOMA Start					{$$ = new }
			| TYPE IDENTIFIER IGUAL Typedef	PUNTOCOMA						{$$ = new }
			; 

Scope 		: CREATE LLAVEABRE Declist LLAVECIERRA EXECUTE LLAVEABRE Sec LLAVECIERRA 	{$$ = new }
			| CREATE LLAVEABRE LLAVECIERRA EXECUTE LLAVEABRE Sec LLAVECIERRA  			{$$ = new }
			| EXECUTE LLAVEABRE Sec LLAVECIERRA  										{$$ = new }
			;

Typedef		: LCHAR 											{$$ = new }
			| BOOL  											{$$ = new }
			| LINT  											{$$ = new }
			| LFLOAT  											{$$ = new }
			| LSTRING  											{$$ = new }
			| ARRAY Typedef CORCHETEABRE Exp CORCHETECIERRA 	{$$ = new }
			| LIST Typedef 										{$$ = new }
			| TUPLE PARABRE Typedef COMA Typedef PARCIERRA  	{$$ = new }
			| POINTER Typedef  									{$$ = new }
			| IDENTIFIER										{$$ = new }
			| UNIT 												{$$ = new }
			;

Varlist 	: Typedef IDENTIFIER COMA Varlist 					{$$ = new }
			| Typedef REFERENCE IDENTIFIER COMA Varlist			{$$ = new }
			| Typedef IDENTIFIER 								{$$ = new }
			| Typedef REFERENCE IDENTIFIER						{$$ = new }
			;

Declist 	: Typedef IDENTIFIER PUNTOCOMA Declist				{$$ = new }
			| Typedef IDENTIFIER IGUAL Exp PUNTOCOMA Declist	{$$ = new }
			| Typedef IDENTIFIER PUNTOCOMA 						{$$ = new }
			| Typedef IDENTIFIER IGUAL Exp PUNTOCOMA 			{$$ = new }
			;

Sec 		: Inst PUNTOCOMA Sec  								{$$ = new }
			| Inst PUNTOCOMA									{$$ = new }
			;

Inst		: Scope					 							{$$ = new }
			| Ids IGUAL Exp										{$$ = new }
			| SALIDA Exp 										{$$ = new }
			| ENTRADA Exp  										{$$ = new }

			| IF PARABRE Exp PARCIERRA LLAVEABRE Sec LLAVECIERRA ELSE LLAVEABRE Sec LLAVECIERRA		{$$ = new }
			| IF PARABRE Exp PARCIERRA LLAVEABRE Sec LLAVECIERRA  									{$$ = new }

			| FOR PARABRE Typedef IDENTIFIER IGUAL Exp COMA CORCHETEABRE Exp COMA Exp CORCHETECIERRA COMA Exp PARCIERRA LLAVEABRE Sec LLAVECIERRA 	{$$ = new }

			| WHILE PARABRE Exp PARCIERRA LLAVEABRE Sec LLAVECIERRA  		{$$ = new }
			| NEW PARABRE IDENTIFIER PARCIERRA					{$$ = new }
			| FREE PARABRE IDENTIFIER PARCIERRA					{$$ = new }
			| IDENTIFIER PARABRE List PARCIERRA 				{$$ = new }
			| RETURN Exp										{$$ = new }
			| BREAK												{$$ = new }
			| 													{$$ = new }
			;

Exp	 		: Exp SUMA Exp										{$$ = new }
			| Exp RESTA Exp		 								{$$ = new }
			| Exp MULT Exp										{$$ = new }
			| Exp DIV Exp										{$$ = new }
			| Exp MOD Exp										{$$ = new }
			| PARABRE Exp PARCIERRA								{$$ = new }
			| RESTA Exp	 										{$$ = new }
			| Exp POW Exp 										{$$ = new }
			
			| Exp IGUALA Exp									{$$ = new }
			| Exp DISTINTOA Exp									{$$ = new }
			| Exp MENOR Exp										{$$ = new }
			| Exp MAYOR Exp										{$$ = new }
			| Exp MENORIGUAL Exp								{$$ = new }
			| Exp MAYORIGUAL Exp								{$$ = new }
			| Exp DISYUNCION Exp								{$$ = new }
			| Exp CONJUNCION Exp								{$$ = new }
			| NEGACION Exp										{$$ = new }

			| Exp Exp 											{$$ = new }
			| Literals											{$$ = new }
			| OPTR IDENTIFIER	 								{$$ = new }
			;

Literals 	: Ids												{$$ = new }
			| CHAR 												{$$ = new }
			| FLOAT 											{$$ = new }
			| INT 												{$$ = new }
			| STRING 											{$$ = new }
			| LLAVEABRE List LLAVECIERRA 						{$$ = new }
			| CORCHETEABRE List CORCHETECIERRA 					{$$ = new }
			| PARABRE Exp COMA Exp PARCIERRA 					{$$ = new }
			| TRUE 												{$$ = new }
			| FALSE 											{$$ = new }
			;

Ids 		: IDENTIFIER PUNTO Ids 								{$$ = new }
			| IDENTIFIER 										{$$ = new }
			;

List		: Exp COMA List 									{$$ = new }
			| Exp 												{$$ = new }
			;
