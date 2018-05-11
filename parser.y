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

bool error_sintactico = 0; 

void yyerror (char const *s) {
	error_sintactico = 1;
	cout << "Parse error:" << s << "\nFila: " << yylineno << "\n" << "Columna: " << yycolumn-1-strlen(yytext) << "\n" ; 
}

%}

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
%token <num> number
//%token <str> IDENTIFIER
%token <ch> CHARACTER
%token <arb> arbol
//%token <boolean> TRUE FALSE

%type <void> S Includelist Start Scope Type Varlist Declist Inst Sec Exp Literals List

%%

S			: Includelist Start 										{cout << "Includelist Start \n";}
			| MODULE Start												{cout << "MODULE Start\n";}
			| Start      												{cout << "Start      \n";}
			;

Includelist : INCLUDE Exp Includelist									{cout << "INCLUDE Exp Includelist\n";}
			| INCLUDE Exp 												{cout << "INCLUDE Exp \n";}

Start 		: MAIN LLAVEABRE Scope LLAVECIERRA PUNTOCOMA Start			{cout << "MAIN LLAVEABRE Scope LLAVECIERRA PUNTOCOMA Start\n";}
	 		| MAIN LLAVEABRE Scope LLAVECIERRA PUNTOCOMA				{cout << "MAIN LLAVEABRE Scope LLAVECIERRA PUNTOCOMA\n";}
			| Type IDENTIFIER PARABRE Varlist PARCIERRA	LLAVEABRE Scope LLAVECIERRA PUNTOCOMA Start										{cout << "Type IDENTIFIER PARABRE Varlist PARCIERRA\n";}
			| Type IDENTIFIER PARABRE Varlist PARCIERRA	LLAVEABRE Scope LLAVECIERRA PUNTOCOMA											{cout << "Type IDENTIFIER PARABRE Varlist PARCIERRA\n";}
			;

Scope 		: CREATE LLAVEABRE Declist LLAVECIERRA EXECUTE LLAVEABRE Sec LLAVECIERRA  													{cout << "CREATE LLAVEABRE Declist LLAVECIERRA EXECUTE LLAVEABRE Sec LLAVECIERRA  \n";}
			| CREATE LLAVEABRE LLAVECIERRA EXECUTE LLAVEABRE Sec LLAVECIERRA  													{cout << "CREATE LLAVEABRE LLAVECIERRA EXECUTE LLAVEABRE Sec LLAVECIERRA  \n";}
			;

Type		: LCHAR 											{cout << "LCHAR \n";}
			| BOOL  											{cout << "BOOL  \n";}
			| LINT  											{cout << "LINT  \n";}
			| LFLOAT  											{cout << "LFLOAT  \n";}
			| LSTRING  											{cout << "LSTRING  \n";}
			| ARRAY Type CORCHETEABRE Exp CORCHETECIERRA 		{cout << "ARRAY Type CORCHETEABRE Exp CORCHETECIERRA \n";}
			| LIST Type 										{cout << "LIST Type \n";}
			| TUPLE PARABRE Type COMA Type PARCIERRA  			{cout << "TUPLE PARABRE Type COMA Type PARCIERRA  \n";}
			| POINTER  											{cout << "POINTER  \n";}
			;

Varlist 	: Type IDENTIFIER COMA Varlist 						{cout << "Type IDENTIFIER COMA Varlist \n";}
			| Type IDENTIFIER 									{cout << "Type IDENTIFIER \n";}
			;

Declist 	: Type IDENTIFIER PUNTOCOMA Declist					{cout << "Type IDENTIFIER PUNTOCOMA Declist\n";}
			| Type IDENTIFIER IGUAL Exp PUNTOCOMA Declist		{cout << "Type IDENTIFIER IGUAL Exp PUNTOCOMA Declist\n";}
			| Type IDENTIFIER PUNTOCOMA 						{cout << "Type IDENTIFIER PUNTOCOMA \n";}
			| Type IDENTIFIER IGUAL Exp PUNTOCOMA 				{cout << "Type IDENTIFIER IGUAL Exp PUNTOCOMA \n";}
			;

Sec 		: Inst PUNTOCOMA Sec  								{cout << "Inst PUNTOCOMA Sec  \n";}
			| Inst PUNTOCOMA									{cout << "Inst PUNTOCOMA\n";}

Inst		: Scope					 							{cout << "Scope\n";}
			| IDENTIFIER IGUAL Exp								{cout << "IDENTIFIER IGUAL Exp\n";}
			| SALIDA Exp 										{cout << "SALIDA Exp \n";}
			| ENTRADA Exp  										{cout << "ENTRADA Exp  \n";}
			| IF PARABRE Exp PARCIERRA LLAVEABRE Inst LLAVECIERRA ELSE LLAVEABRE Inst LLAVECIERRA  										{cout << "IF PARABRE Exp PARCIERRA LLAVEABRE Inst LLAVECIERRA ELSE LLAVEABRE Inst LLAVECIERRA  \n";}
			| IF PARABRE Exp PARCIERRA LLAVEABRE Inst LLAVECIERRA  																		{cout << "IF PARABRE Exp PARCIERRA LLAVEABRE Inst LLAVECIERRA  \n";}
			| FOR PARABRE Type IDENTIFIER COMA CORCHETEABRE Exp COMA Exp CORCHETECIERRA COMA Exp PARCIERRA LLAVEABRE Inst LLAVECIERRA 	{cout << "FOR PARABRE Type IDENTIFIER COMA CORCHETEABRE Exp COMA Exp CORCHETECIERRA COMA Exp PARCIERRA LLAVEABRE Inst LLAVECIERRA \n";}
			| WHILE PARABRE Exp PARCIERRA LLAVEABRE Inst LLAVECIERRA  																	{cout << "WHILE PARABRE Exp PARCIERRA LLAVEABRE Inst LLAVECIERRA  \n";}
			| RETURN Exp								{cout << "RETURN Exp\n";}
			;

Exp	 		: Exp SUMA Exp										{cout << "Exp SUMA Exp\n";}
			| Exp RESTA Exp		 								{cout << "Exp RESTA Exp\n";}
			| Exp MULT Exp										{cout << "Exp MULT Exp\n";}
			| Exp DIV Exp										{cout << "Exp DIV Exp\n";}
			| Exp MOD Exp										{cout << "Exp MOD Exp\n";}
			| PARABRE Exp PARCIERRA								{cout << "PARABRE Exp PARCIERRA\n";}
			| RESTA Exp	 										{cout << "RESTA Exp\n";}
			| Exp POW	 										{cout << "Exp POW\n";}
			
			| Exp IGUALA Exp									{cout << "Exp IGUALA Exp\n";}
			| Exp DISTINTOA Exp									{cout << "Exp DISTINTOA Exp\n";}
			| Exp MENOR Exp										{cout << "Exp MENOR Exp\n";}
			| Exp MAYOR Exp										{cout << "Exp MAYOR Exp\n";}
			| Exp MENORIGUAL Exp								{cout << "Exp MENORIGUAL Exp\n";}
			| Exp MAYORIGUAL Exp								{cout << "Exp MAYORIGUAL Exp\n";}
			
			| Exp DISYUNCION Exp								{cout << "Exp DISYUNCION Exp\n";}
			| Exp CONJUNCION Exp								{cout << "Exp CONJUNCION Exp\n";}
			| NEGACION Exp										{cout << "NEGACION Exp\n";}	
			| Literals											{cout << "Literals\n";}	
			;

Literals 	: IDENTIFIER										{cout << "IDENTIFIER\n";}
			| CHAR 												{cout << "CHAR \n";}
			| FLOAT 											{cout << "FLOAT \n";}
			| INT 												{cout << "INT \n";}
			| STRING 											{cout << "STRING \n";}
			| LLAVEABRE List LLAVECIERRA 						{cout << "LLAVEABRE List LLAVECIERRA \n";}
			| CORCHETEABRE List CORCHETECIERRA 					{cout << "CORCHETEABRE List CORCHETECIERRA \n";}
			| PARABRE Exp COMA Exp PARCIERRA 					{cout << "PARABRE Exp COMA Exp PARCIERRA \n";}
			| TRUE 												{cout << "TRUE \n";}
			| FALSE 											{cout << "FALSE \n";}
			;

List		: Exp COMA List 									{cout << "Exp COMA List \n";}
			| Exp 												{cout << "Exp \n";}
			;