%{
#include "ast.h"
#include "parser.tab.h"

using namespace std;
int yycolumn = 1;

#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; yylloc.last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng; \
    yylval.str = strdup(yytext);
%}

%option yylineno

%%

\{								return LLAVEABRE;
\}								return LLAVECIERRA;
\[								return CORCHETEABRE;
\]								return CORCHETECIERRA;
\(								return PARABRE;
\)								return PARCIERRA;

"main"							return MAIN;

\/\/.*							;
\/\*(\*[^\/]|[^\*])*\*\/		;

"include"						return INCLUDE;
"create"						return CREATE;
"execute"						return EXECUTE;
"module"						return MODULE;

;								return PUNTOCOMA;

"while"							return WHILE;
"for"							return FOR;
"break"							return BREAK;
"if"							return IF;
"else"							return ELSE;
"return"						return RETURN;

"new"							return NEW;
"free"							return FREE;
"type"							return TYPE;
"struct"						return STRUCT;
"union"							return UNION;
"array"							return ARRAY;
"list"							return LIST;
"tuple"							return TUPLE;
"string"						return LSTRING;
"float"							return LFLOAT;
"char"							return LCHAR;
"bool"							return BOOL;
"int"							return LINT;
"ptr"							return POINTER;
"unit"							return UNIT;

"true"							return TRUE;
"false"							return FALSE;

==								return IGUALA;
!=								return DISTINTOA;

\+								return SUMA;
\-								return RESTA;
mod								return MOD;
\/								return DIV;
\*\*							return POW;
\*								return MULT;
=								return IGUAL;
\<=								return MENORIGUAL;
\<								return MENOR;
>=								return MAYORIGUAL;
>								return MAYOR;
\@								return REFERENCE;
\^								return OPTR;

"not"							return NEGACION;
"and"							return CONJUNCION;
"or"							return DISYUNCION;

[ ] 							{;}
\t								{yycolumn+= 3;}
\n								{yycolumn = 1;}
,								return COMA;

\<<								return SALIDA;
>> 								return ENTRADA;


[a-zA-Z][a-zA-Z0-9_]*			return IDENTIFIER;
'(.|\\n|\\t)' 					return CHAR;
[0-9]+\.[0-9]+					return FLOAT;
\.								return PUNTO;
[0-9]+|0+						return INT;
\"([^"])*\"						return STRING;

.								return ERROR;

%%

int yywrap(void){
	return 1;
}