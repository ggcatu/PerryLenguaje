%{
#include "definiciones.h"
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

\/\/.*							return COMENTARIOLINEA;
\/\*(\*[^\/]|[^\*])*\*\/				return COMENTARIO;

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

"True"							return TRUE;
"False"							return FALSE;

==								return IGUALA;
!=								return DISTINTOA;

\+								return SUMA;
\-								return RESTA;
%								return MOD;
\/								return DIV;
\*\*							return POW;
\*								return MULT;
=								return IGUAL;
\<=								return MENORIGUAL;
\<								return MENOR;
>=								return MAYORIGUAL;
>								return MAYOR;

!								return NEGACION;
\&\&							return CONJUNCION;
\|\|							return DISYUNCION;

[ ] 							return SPACE;
\t								return TAB;
\n								return ENTER;
,								return COMA;

"inout"							return INOUT;
\<<								return SALIDA;
>> 								return ENTRADA;


[a-zA-Z][a-zA-Z0-9_]*			return IDENTIFIER;
'(.|\\n|\\t)' 					return CHAR;
[0-9]+\.[0-9]+					return FLOAT;
\.								return PUNTO;
[0-9]+|0+						return INT;
\"([^"])*\"						return LSTRING;

.								return ERROR;

%%

int yywrap(void){
	return 1;
}