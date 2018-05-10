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

%token MAIN
%token LLAVEABRE LLAVECIERRA CORCHETEABRE CORCHETECIERRA PARABRE PARCIERRA
%token INCLUDE CREATE EXECUTE MODULE
%token PUNTOCOMA
%token WHILE FOR BREAK IF ELSE
%token STRUCT UNION ARRAY LIST TUPLE LSTRING LFLOAT LCHAR BOOL LINT POINTER
%token TRUE FALSE
%token IGUALA DISTINTOA
%token SUMA RESTA MOD DIV POW MULT IGUAL MENORIGUAL MENOR MAYORIGUAL MAYOR
%token NEGACION CONJUNCION DISYUNCION
%token SPACE TAB ENTER COMA
%token SALIDA ENTRADA
%token IDENTIFIER CHAR FLOAT PUNTO INT STRING

%token <num> number
//%token <str> IDENTIFIER
%token <ch> CHARACTER
%token <arb> arbol
//%token <boolean> TRUE FALSE

%type <void> S Includelist Start Scope Type Varlist Declist Inst Exp Literals List

%%

S			: Includelist Start 								{;}
			| MODULE Start										{;}
			| Start      										{;}
			;

Includelist : INCLUDE Exp Includelist{;}
			| INCLUDE Exp {;}

Start 		: MAIN LLAVEABRE Scope LLAVECIERRA PUNTOCOMA Start	{;}
	 		| MAIN LLAVEABRE Scope LLAVECIERRA PUNTOCOMA	{;}
			| Type IDENTIFIER PARABRE Varlist PARCIERRA	LLAVEABRE Scope LLAVECIERRA PUNTOCOMA Start								{;}
			| Type IDENTIFIER PARABRE Varlist PARCIERRA	LLAVEABRE Scope LLAVECIERRA PUNTOCOMA								{;}
			;

Scope 		: CREATE LLAVEABRE Declist LLAVECIERRA EXECUTE LLAVEABRE Inst LLAVECIERRA  								{;}
			;

Type		: LCHAR 			{;}
			| BOOL  			{;}
			| LINT  			{;}
			| LFLOAT  	{;}
			| LSTRING  	{;}
			| ARRAY Type CORCHETEABRE Exp CORCHETECIERRA {;}
			| LIST Type {;}
			| TUPLE PARABRE Type COMA Type PARCIERRA  	{;}
			| POINTER  	{;}
			;

Varlist 	: Type IDENTIFIER COMA Varlist 										{;}
			| Type IDENTIFIER 							{;}
			;

Declist 	: Type IDENTIFIER PUNTOCOMA Declist										{;}
			| Type IDENTIFIER IGUAL Exp PUNTOCOMA Declist								{;}
			| Type IDENTIFIER PUNTOCOMA 										{;}
			| Type IDENTIFIER IGUAL Exp PUNTOCOMA 								{;}
			;

Inst		: Inst PUNTOCOMA Inst										{;}
			| Scope					 								{;}
			| IDENTIFIER IGUAL Exp								{;}
			| IF PARABRE Exp PARCIERRA LLAVEABRE Inst LLAVECIERRA ELSE LLAVEABRE Inst LLAVECIERRA  								{;}
			| IF PARABRE Exp PARCIERRA LLAVEABRE Inst LLAVECIERRA  								{;}
			| FOR PARABRE Type IDENTIFIER COMA CORCHETEABRE Exp COMA Exp CORCHETECIERRA COMA Exp PARCIERRA LLAVEABRE Inst LLAVECIERRA 								{;}
			| WHILE PARABRE Exp PARCIERRA LLAVEABRE Inst LLAVECIERRA  								{;}
			| SALIDA Exp 								{;}
			| ENTRADA Exp  								{;}
			;

Exp	 		: Exp SUMA Exp									{;}
			| Exp RESTA Exp		 							{;}
			| Exp MULT Exp									{;}
			| Exp DIV Exp										{;}
			| Exp MOD Exp										{;}
			| PARABRE Exp PARCIERRA							{;}
			| RESTA Exp	 									{;}
			| Exp POW	 									{;}
			
			| Exp IGUALA Exp									{;}
			| Exp DISTINTOA Exp									{;}
			| Exp MENOR Exp									{;}
			| Exp MAYOR Exp									{;}
			| Exp MENORIGUAL Exp								{;}
			| Exp MAYORIGUAL Exp								{;}
			
			| Exp DISYUNCION Exp								{;}
			| Exp CONJUNCION Exp								{;}
			| NEGACION Exp										{;}	
			| Literals										{;}	
			;

Literals 	: IDENTIFIER		{;}
			| CHAR 				{;}
			| FLOAT 				{;}
			| INT 				{;}
			| STRING 				{;}
			| LLAVEABRE List LLAVECIERRA {;}
			| CORCHETEABRE List CORCHETECIERRA {;}
			| PARABRE Exp COMA Exp PARCIERRA {;}
			| TRUE {;}
			| FALSE {;}
			;

List		: Exp COMA List {;}
			| Exp 				{;}
			;