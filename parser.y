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
%token TYPE 64 NEW 65 FREE 66
%token REFERENCE 67 OPTR 68 UNIT 69
%token <num> number
//%token <str> IDENTIFIER
%token <ch> CHARACTER
%token <arb> arbol
//%token <boolean> TRUE FALSE

%type <void> S Includelist Start Scope Typedef Varlist Declist Inst Sec Exp Literals Ids List

%%

S			: Includelist Start 										{cout << "Includelist Start \n";}
			| MODULE Start												{cout << "MODULE Start\n";}
			| Start      												{cout << "Start      \n";}
			|															{cout << "\n";}
			;

Includelist : INCLUDE Exp Includelist									{cout << "INCLUDE Exp Includelist\n";}
			| INCLUDE Exp 												{cout << "INCLUDE Exp \n";}
			;

Start 		: MAIN LLAVEABRE Sec LLAVECIERRA Start 									{cout << "MAIN LLAVEABRE Sec LLAVECIERRA PUNTOCOMA Start\n";}
	 		| MAIN LLAVEABRE Sec LLAVECIERRA										{cout << "MAIN LLAVEABRE Sec LLAVECIERRA\n";}
			
			| Typedef IDENTIFIER PARABRE Varlist PARCIERRA	LLAVEABRE Sec LLAVECIERRA Start				{cout << "Typedef IDENTIFIER PARABRE Varlist PARCIERRA LLAVEABRE Sec LLAVECIERRA Start\n";}
			| Typedef IDENTIFIER PARABRE Varlist PARCIERRA	LLAVEABRE Sec LLAVECIERRA					{cout << "Typedef IDENTIFIER PARABRE Varlist PARCIERRA\n";}

			| TYPE STRUCT IDENTIFIER LLAVEABRE Subdeclist LLAVECIERRA Start 		{cout << "TYPE STRUCT IDENTIFIER LLAVEABRE Subdeclist LLAVECIERRA Start\n";}
			| TYPE STRUCT IDENTIFIER LLAVEABRE Subdeclist LLAVECIERRA				{cout << "TYPE STRUCT IDENTIFIER LLAVEABRE Subdeclist LLAVECIERRA \n";}
			
			| TYPE UNION IDENTIFIER LLAVEABRE Subdeclist LLAVECIERRA Start  		{cout << "TYPE UNION IDENTIFIER LLAVEABRE Subdeclist LLAVECIERRA Start\n";}
			| TYPE UNION IDENTIFIER LLAVEABRE Subdeclist LLAVECIERRA 				{cout << "TYPE UNION IDENTIFIER LLAVEABRE Subdeclist LLAVECIERRA \n";}
			
			| TYPE IDENTIFIER IGUAL Typedef Start								{cout << "TYPE IDENTIFIER IGUAL Typedef Start\n";}
			| TYPE IDENTIFIER IGUAL Typedef										{cout << "TYPE IDENTIFIER IGUAL Typedef\n";}
			; 

Scope 		: CREATE LLAVEABRE Declist LLAVECIERRA EXECUTE LLAVEABRE Sec LLAVECIERRA  			{cout << "CREATE LLAVEABRE Declist LLAVECIERRA EXECUTE LLAVEABRE Sec LLAVECIERRA  \n";}
			| CREATE LLAVEABRE LLAVECIERRA EXECUTE LLAVEABRE Sec LLAVECIERRA  					{cout << "CREATE LLAVEABRE LLAVECIERRA EXECUTE LLAVEABRE Sec LLAVECIERRA  \n";}
			| EXECUTE LLAVEABRE Sec LLAVECIERRA  												{cout << "EXECUTE LLAVEABRE Sec LLAVECIERRA  \n";}
			;

Typedef		: LCHAR 											{cout << "LCHAR \n";}
			| BOOL  											{cout << "BOOL  \n";}
			| LINT  											{cout << "LINT  \n";}
			| LFLOAT  											{cout << "LFLOAT  \n";}
			| LSTRING  											{cout << "LSTRING  \n";}
			| ARRAY Typedef CORCHETEABRE Exp CORCHETECIERRA 	{cout << "ARRAY Typedef CORCHETEABRE Exp CORCHETECIERRA \n";}
			| LIST Typedef 										{cout << "LIST Typedef \n";}
			| TUPLE PARABRE Typedef COMA Typedef PARCIERRA  	{cout << "TUPLE PARABRE Typedef COMA Typedef PARCIERRA  \n";}
			| POINTER Typedef  									{cout << "POINTER  \n";}
			| IDENTIFIER										{cout << "IDENTIFIER  \n";}
			| UNIT 												{cout << "UNIT  \n";}
			;

Varlist 	: Typedef IDENTIFIER COMA Varlist 					{cout << "Typedef IDENTIFIER COMA Varlist \n";}
			| Typedef REFERENCE IDENTIFIER COMA Varlist			{cout << "Typedef Typedef REFERENCE IDENTIFIER COMA Varlist \n";}
			| Typedef IDENTIFIER 								{cout << "Typedef IDENTIFIER \n";}
			| Typedef REFERENCE IDENTIFIER						{cout << "Typedef Typedef REFERENCE IDENTIFIER \n";}
			;

Declist 	: Typedef IDENTIFIER PUNTOCOMA Declist					{cout << "Typedef IDENTIFIER PUNTOCOMA Declist\n";}
			| Typedef IDENTIFIER IGUAL Exp PUNTOCOMA Declist		{cout << "Typedef IDENTIFIER IGUAL Exp PUNTOCOMA Declist\n";}
			| Typedef IDENTIFIER PUNTOCOMA 							{cout << "Typedef IDENTIFIER PUNTOCOMA \n";}
			| Typedef IDENTIFIER IGUAL Exp PUNTOCOMA 				{cout << "Typedef IDENTIFIER IGUAL Exp PUNTOCOMA \n";}
			| TYPE IDENTIFIER IGUAL Typedef PUNTOCOMA				{cout << "TYPE IDENTIFIER IGUAL Typedef PUNTOCOMA \n";}
			| TYPE IDENTIFIER IGUAL Typedef PUNTOCOMA Declist		{cout << "TYPE IDENTIFIER IGUAL Typedef PUNTOCOMA Declist \n";}
			;

Subdeclist 	: Typedef IDENTIFIER PUNTOCOMA Declist					{cout << "Typedef IDENTIFIER PUNTOCOMA Declist\n";}
			| Typedef IDENTIFIER IGUAL Exp PUNTOCOMA Declist		{cout << "Typedef IDENTIFIER IGUAL Exp PUNTOCOMA Declist\n";}
			| Typedef IDENTIFIER PUNTOCOMA 							{cout << "Typedef IDENTIFIER PUNTOCOMA \n";}
			| Typedef IDENTIFIER IGUAL Exp PUNTOCOMA 				{cout << "Typedef IDENTIFIER IGUAL Exp PUNTOCOMA \n";}
			;

Sec 		: Inst PUNTOCOMA Sec  								{cout << "Inst PUNTOCOMA Sec  \n";}
			| Inst PUNTOCOMA									{cout << "Inst PUNTOCOMA\n";}
			;

Inst		: Scope					 							{cout << "Scope\n";}
			| Ids IGUAL Exp										{cout << "Ids IGUAL Exp\n";}
			| SALIDA Exp 										{cout << "SALIDA Exp \n";}
			| ENTRADA Exp  										{cout << "ENTRADA Exp  \n";}
			| IF PARABRE Exp PARCIERRA LLAVEABRE Sec LLAVECIERRA ELSE LLAVEABRE Sec LLAVECIERRA  										{cout << "IF PARABRE Exp PARCIERRA LLAVEABRE Sec LLAVECIERRA ELSE LLAVEABRE Sec LLAVECIERRA  \n";}
			| IF PARABRE Exp PARCIERRA LLAVEABRE Sec LLAVECIERRA  																		{cout << "IF PARABRE Exp PARCIERRA LLAVEABRE Sec LLAVECIERRA  \n";}
			| FOR PARABRE Typedef IDENTIFIER IGUAL Exp COMA CORCHETEABRE Exp COMA Exp CORCHETECIERRA COMA Exp PARCIERRA LLAVEABRE Sec LLAVECIERRA 	{cout << "FOR PARABRE Typedef IDENTIFIER COMA CORCHETEABRE Exp COMA Exp CORCHETECIERRA COMA Exp PARCIERRA LLAVEABRE Sec LLAVECIERRA \n";}
			| WHILE PARABRE Exp PARCIERRA LLAVEABRE Sec LLAVECIERRA  																	{cout << "WHILE PARABRE Exp PARCIERRA LLAVEABRE Sec LLAVECIERRA  \n";}
			| NEW PARABRE IDENTIFIER PARCIERRA					{cout << "NEW PARABRE IDENTIFIER PARCIERRA\n";}
			| FREE PARABRE IDENTIFIER PARCIERRA					{cout << "FREE PARABRE IDENTIFIER PARCIERRA\n";}
			| IDENTIFIER PARABRE List PARCIERRA 				{cout << "IDENTIFIER PARABRE List PARCIERRA\n";}
			| RETURN Exp										{cout << "RETURN Exp\n";}
			| BREAK												{cout << "BREAK\n";}
			| 													{cout << "\n";}
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

			| IDENTIFIER PARABRE List PARCIERRA 				{cout << "IDENTIFIER PARABRE List PARCIERRA\n";}
			
			| Exp DISYUNCION Exp								{cout << "Exp DISYUNCION Exp\n";}
			| Exp CONJUNCION Exp								{cout << "Exp CONJUNCION Exp\n";}
			| NEGACION Exp										{cout << "NEGACION Exp\n";}	
			| Literals											{cout << "Literals\n";}
			| OPTR IDENTIFIER	 								{cout << "OPTR IDENTIFIER\n";}
			| OPTR PARABRE IDENTIFIER PARCIERRA 				{cout << "OPTR PARABRE IDENTIFIER PARCIERRA\n";}
			;

Literals 	: Ids												{cout << "Ids\n";}
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

Ids 		: IDENTIFIER PUNTO Ids 								{cout << "IDENTIFIER PUNTO Ids \n";}
			| IDENTIFIER 										{cout << "IDENTIFIER \n";}
			;

List		: Exp COMA List 									{cout << "Exp COMA List \n";}
			| Exp 												{cout << "Exp \n";}
			;
