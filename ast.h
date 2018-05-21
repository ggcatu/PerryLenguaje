/*
ast.cpp:

Contiene la definicion del arbol sintactico asi como la de la tabla de simbolos utilizadas
en el proyecto
*/

#include <stdio.h>
#include <iostream>
#include <stdexcept>
#include <map>
#include <ctype.h>
#include <cstdlib>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include "ast_def.h"
using namespace std;
#define ENTEROS 1
#define FLOTANTES 3
#define BOOLEANOS 4
#define CHARACTERES 5
#define CONDICION 6
#define STR 7
#define PTR 8
#define UNITS 9

/* Definiciones externas (parser.y) que permiten compartir el codigo. */
extern int yylineno;
extern char error_strp[1000];
extern int yyparse();

/* Definicion de la clase raiz */
class raiz : public ArbolSintactico {
	public:
		bool module;
		ArbolSintactico * includes;
		ArbolSintactico * programa;
		raiz() {}
		raiz(ArbolSintactico * p, bool m) : programa(p), module(m) {}
		raiz(ArbolSintactico * i, ArbolSintactico * p, bool m) : includes(i), programa(p), module(m) {}
		virtual void imprimir(int tab){
			if (module){
				cout << "MODULE:" << endl;
				if (programa != NULL){
					programa->imprimir(tab+1);
				}
			} else {
				if(includes != NULL){
					includes->imprimir(tab);
				}
				if(programa != NULL){
					programa->imprimir(tab);
				}
			}
		}
};


/* Definicion de la clase para una lista de funciones declaradas */
class funcion : public ArbolSintactico {
	public:
		ArbolSintactico * tipo;
		ArbolSintactico * parametros;
		ArbolSintactico * id;
		ArbolSintactico * instrucciones;
		ArbolSintactico * funciones;
		funcion(ArbolSintactico * t, ArbolSintactico * i, ArbolSintactico * p, ArbolSintactico * is) : tipo(t), id(i), parametros(p), instrucciones(is) {}
		funcion(ArbolSintactico * t, ArbolSintactico * i, ArbolSintactico * p, ArbolSintactico * is, ArbolSintactico * fs) : tipo(t), id(i), parametros(p), instrucciones(is), funciones(fs) {}
		virtual void imprimir(int tab){
			cout << "FUNCION:" << endl;
			for (int j = 0; j < tab+1; j++) cout << " ";
			cout << "TIPO RETORNO:" << endl;
			tipo -> imprimir(tab+2);
			for (int j = 0; j < tab+1; j++) cout << " ";
			cout << "IDENTIFICADOR:" << endl;
			id -> imprimir(tab+2);
			for (int j = 0; j < tab+1; j++) cout << " ";
			cout << "PARAMETROS:" << endl;
			parametros -> imprimir(tab+2);
			for (int j = 0; j < tab+1; j++) cout << " ";
			cout << "INSTRUCCIONES:" << endl;
			instrucciones -> imprimir(tab+2);
			if (funciones != NULL){
				funciones -> imprimir(tab+2);
			}
		}
};


/* Definicion de la clase para una lista de importaciones */
class include : public ArbolSintactico {
	public:
		ArbolSintactico * archivo;
		ArbolSintactico * includes;
		include(ArbolSintactico * i, ArbolSintactico * a) : includes(i), archivo(a) {}
		include(ArbolSintactico * a) : archivo(a) {}
		virtual void imprimir(int tab) {
			cout << "INCLUDE:" << endl;
			if (archivo != NULL){
				archivo -> imprimir(tab+1);
			}
			if (includes != NULL){
				includes -> imprimir(tab);
			}
		}
};


/* Definicion de la clase para los structs, union */
class estructura : public ArbolSintactico {
	public:
		ArbolSintactico * id;
		ArbolSintactico * campos;
		ArbolSintactico * estructuras;
		bool st;
		estructura(ArbolSintactico * i, ArbolSintactico * c, bool t) : id(i), campos(c), st(t) {}
		estructura(ArbolSintactico * i, ArbolSintactico * c, ArbolSintactico * es, bool t) : id(i), campos(c), estructuras(es), st(t) {}
		virtual void imprimir(int tab){
			//if (estructuras != NULL){
			//		estructuras -> imprimir(tab);
			//}
			if (st){
				cout << "STRUCT:" << endl;
			}
			else {
				cout << "UNION:" << endl;
			}
			if (campos != NULL){
				id -> imprimir(tab+1);
				campos -> imprimir(tab+1);
			}
			if (estructuras != NULL){
				estructuras -> imprimir(tab+1);
			}
		}
};


/* Definicion de la clase para los type */
class tipo : public ArbolSintactico {
	public:
		ArbolSintactico * ti;
		ArbolSintactico * id;
		ArbolSintactico * tis;
		tipo(ArbolSintactico * i, ArbolSintactico * t) : id(i), ti(t) {}
		tipo(ArbolSintactico * i, ArbolSintactico * t, ArbolSintactico * ts) : id(i), ti(t), tis(ts) {}
		virtual void imprimir(int tab){
			//if (tis != NULL){
			//	tis -> imprimir(tab);
			//}
			cout << "TYPE" << endl;
			for (int j = 0; j < tab+1; j++) cout << " ";
			cout << "IDENTIFICADOR:" << endl;
			id -> imprimir(tab+2);
			for (int j = 0; j < tab+1; j++) cout << " ";
			cout << "TIPO:" << endl;
			ti -> imprimir(tab+2);
			if (tis != NULL){
				tis -> imprimir(tab);
			}
		}
};


/* Definicion de la clase para el main */
class programa : public ArbolSintactico {
	public:
		ArbolSintactico * program;
		ArbolSintactico * sec;
		programa(ArbolSintactico * s, ArbolSintactico * p) : program(p), sec(s) {}
		programa(ArbolSintactico * s) : sec(s) {}
		virtual void imprimir(int tab){
			if (program != NULL){
				program -> imprimir(tab);
			}
			if (sec != NULL){
				cout << "MAIN:" << endl;
				sec -> imprimir(tab+1);
			}
		}
};


/* Definicion de la clase para un bloque create-execute */
class bloque : public ArbolSintactico {
	public:
		ArbolSintactico * declaraciones;
		ArbolSintactico * instrucciones;
		bloque(ArbolSintactico * d, ArbolSintactico * i) : declaraciones(d), instrucciones(i) {}
		bloque(ArbolSintactico * i) : instrucciones(i) {}
		virtual void imprimir(int tab){
			for (int j = 0; j < tab; j++) cout << " ";
			cout << "BLOQUE:" << endl;
			if (declaraciones != NULL){
				for (int j = 0; j < tab+1; j++) cout << " ";
				cout << "DECLARACIONES:" << endl;
				declaraciones -> imprimir(tab+2);
			}
			if (instrucciones != NULL){
				for (int j = 0; j < tab+1; j++) cout << " ";
				cout << "INSTRUCCIONES:" << endl;
				instrucciones -> imprimir(tab+2);
			}
		}
};


/* Definicion de la clase para la lista de declaraciones */
class declaracion : public ArbolSintactico {
	public:
		ArbolSintactico * tipo;
		ArbolSintactico * id;
		ArbolSintactico * exp;
		ArbolSintactico * declaraciones;
		declaracion(ArbolSintactico * d, ArbolSintactico * t, ArbolSintactico * i, ArbolSintactico * e) : declaraciones(d), tipo(t), id(i), exp(e) {}
		declaracion(ArbolSintactico * d, ArbolSintactico * t, ArbolSintactico * i, bool aux) : declaraciones(d), tipo(t), id(i) {}
		declaracion(ArbolSintactico * t, ArbolSintactico * i, ArbolSintactico * e) : tipo(t), id(i), exp(e) {}
		declaracion(ArbolSintactico * t, ArbolSintactico * i) : tipo(t), id(i) {}
		virtual void imprimir(int tab){
			if (declaraciones != NULL){
				declaraciones -> imprimir(tab);
			}
			for (int j = 0; j < tab; j++) cout << " ";
			cout << "TIPO:" << endl;
			tipo -> imprimir(tab+1);
			for (int j = 0; j < tab; j++) cout << " ";
			cout << "IDENTIFICADOR:" << endl;
			id -> imprimir(tab+2);
			if (exp != NULL){
				for (int j = 0; j < tab; j++) cout << " ";
				cout << "VALOR:" << endl;
				exp -> imprimir(tab+1); 
			}
		}
};


/* Definicion de la clase para la lista de declaraciones */
class tipedec : public ArbolSintactico {
	enum tipes {BOOL,CHAR,STRING,INT,FLOAT,ARRAY,LIST,TUPLE,ID,POINTER,UNIT};
	public:
		tipes names;
		ArbolSintactico * subtipo1;
		ArbolSintactico * subtipo2;
		ArbolSintactico * tam;
		tipedec( int n, ArbolSintactico * s1, ArbolSintactico * s2, ArbolSintactico * t) : names(static_cast<tipes>(n)), subtipo1(s1), tam(t) {}
		tipedec( int n, ArbolSintactico * s1, ArbolSintactico * s2) : names(static_cast<tipes>(n)), subtipo1(s1), subtipo2(s2) {}
		tipedec( int n, ArbolSintactico * s1) : names(static_cast<tipes>(n)), subtipo1(s1) {}
		tipedec( int n) : names(static_cast<tipes>(n)) {}
		virtual void imprimir(int tab){
			for (int j = 0; j < tab; j++) cout << " ";
			switch(names){
				case BOOL:
					cout << "BOOL" << endl;
					break;
				case CHAR:
					cout << "CHAR" << endl;
					break;
				case STRING:
					cout << "STRING" << endl;
					break;
				case INT:
					cout << "ENTERO" << endl;
					break;
				case FLOAT:
					cout << "FLOTANTE" << endl;
					break;
				case ARRAY:
					cout << "ARREGLO" << endl;
					break;
				case LIST:
					cout << "LISTA" << endl;
					break;
				case TUPLE:
					cout << "TUPLA" << endl;
					break;
				case ID:
					cout << "IDENTIFICADOR" << endl;
					break;
				case POINTER:
					cout << "APUNTADOR" << endl;
					break;
				case UNIT:
					cout << "UNIT" << endl;
					break;
			}
			if (subtipo1 != NULL){
				for (int j = 0; j < tab; j++) cout << " ";
				cout << "DE:" << endl;
				subtipo1 -> imprimir(tab+1); 
			}
			if (subtipo2 != NULL){
				for (int j = 0; j < tab; j++) cout << " ";
				cout << "Y DE:" << endl;
				subtipo2 -> imprimir(tab+1); 
			}
			if (tam != NULL){
				for (int j = 0; j < tab; j++) cout << " ";
				cout << "DE TAMAÑO:" << endl;
				tam -> imprimir(tab+1); 
			}
		}
};


/* Definicion de la clase para la lista de instrucciones */
class instruccion : public ArbolSintactico {
	public:
		ArbolSintactico * instrucciones;
		ArbolSintactico * inst;
		instruccion(ArbolSintactico * is, ArbolSintactico * i) : instrucciones(is), inst(i) {}
		instruccion(ArbolSintactico * i) : inst(i) {}
		virtual void imprimir(int tab){
			if (inst != NULL){
				inst -> imprimir(tab);
			}
			if (instrucciones != NULL){
				instrucciones -> imprimir(tab);
			}
		}
};


/* Definicion de la clase para la lista de parametros de una funcion */
class parametros : public ArbolSintactico {
	public:
		ArbolSintactico * params;
		bool ref;
		ArbolSintactico * id;
		ArbolSintactico * tipo;
		parametros(ArbolSintactico * t, ArbolSintactico * i, bool r) : tipo(t), id(i), ref(r) {}
		parametros(ArbolSintactico * p, ArbolSintactico * t, ArbolSintactico * i, bool r) : params(p), tipo(t), id(i), ref(r) {}
		virtual void imprimir(int tab){
			if (params != NULL){
				params -> imprimir(tab);
			}
			for (int j = 0; j < tab; j++) cout << " ";
			if (ref){
				cout << "PARAMETRO POR REFERENCIA:" << endl;
			} else {
				cout << "PARAMETRO POR VALOR:" << endl;
			}
			for (int j = 0; j < tab+1; j++) cout << " ";
			cout << "TIPO:" << endl;
			tipo -> imprimir(tab+2);
			for (int j = 0; j < tab+1; j++) cout << " ";
			cout << "IDENTIFICADOR:" << endl;
			id -> imprimir(tab+2);
		}
};


/* Definicion de la clase para la llamada a una funcion */
class llamada : public ArbolSintactico {
	public:
		ArbolSintactico * parametros;
		ArbolSintactico * id;
		llamada(ArbolSintactico * i, ArbolSintactico * p) : id(i), parametros(p) {}
		virtual void imprimir(int tab){
			for (int j = 0; j < tab; j++) cout << " ";
			cout << "LLAMADA A FUNCION:" << endl;
			for (int j = 0; j < tab+1; j++) cout << " ";
			cout << "IDENTIFICADOR:" << endl;
			id -> imprimir(tab+2);
			for (int j = 0; j < tab+1; j++) cout << " ";
			cout << "PARAMETROS:" << endl;
			parametros -> imprimir(tab+2);
		}
};


/* Definicion de la clase para new y free */
class memoria : public ArbolSintactico {
	public:
		ArbolSintactico * id;
		bool is_new;
		memoria(ArbolSintactico * i, bool in) : id(i), is_new(in) {}
		virtual void imprimir(int tab){
			for (int j = 0; j < tab; j++) cout << " ";
			if (is_new){
				cout << "NEW:" << endl;
					
			} else {
				cout << "FREE:" << endl;
			}
			for (int j = 0; j < tab+1; j++) cout << " ";
			cout << "IDENTIFICADOR:" << endl;
			id -> imprimir(tab+2);
		}
};


/* Definicion de la clase para la entrada y salida */
class entrada_salida : public ArbolSintactico {
	public:
		ArbolSintactico * exp;
		bool entrada;
		entrada_salida(ArbolSintactico * s, bool e) : exp(s), entrada(e) {}
		virtual void imprimir(int tab){
			for (int j = 0; j < tab; j++) cout << " ";
			if (entrada){
				cout << "ENTRADA:" << endl;
					
			} else {
				cout << "SALIDA:" << endl;
			}
			for (int j = 0; j < tab+1; j++) cout << " ";
			cout << "EXPRESION:" << endl;
			exp -> imprimir(tab+2);
		}
};


//COMO LA ITERACION ES DETERMINADA POSIBLEMENTE NO SEA NECESARIO HACER UN ARBOL PARA EL RANGO
/* Definicion de la clase para el for */
class it_determinada : public ArbolSintactico {
	public:
		ArbolSintactico * variable;
		ArbolSintactico * inicio;
		ArbolSintactico * fin;
		ArbolSintactico * paso;
		ArbolSintactico * inst;
		it_determinada(ArbolSintactico * v, ArbolSintactico * i, ArbolSintactico * f, ArbolSintactico * p, ArbolSintactico * is) : variable(v), inicio(i), fin(f), paso(p), inst(is) {}
		virtual void imprimir(int tab){
			for (int j = 0; j < tab; j++) cout << " ";
			cout << "FOR:" << endl;
			for (int j = 0; j < tab+1; j++) cout << " ";
			cout << "VARIABLE:" << endl;
			variable -> imprimir(tab+2);
			for (int j = 0; j < tab+1; j++) cout << " ";
			cout << "INICIO:" << endl;
			inicio -> imprimir(tab+2);
			for (int j = 0; j < tab+1; j++) cout << " ";
			cout << "FIN:" << endl;
			fin -> imprimir(tab+2);
			for (int j = 0; j < tab+1; j++) cout << " ";
			cout << "PASO:" << endl;
			paso -> imprimir(tab+2);
		}
};


/* Definicion de la clase para el while, if */
class inst_guardia : public ArbolSintactico {
	enum inst {IF, ELSE, WHILE};
	public:
		ArbolSintactico * condicion;
		ArbolSintactico * cuerpo;
		ArbolSintactico * cuerpo_else;
		inst instruccion;
		inst_guardia(ArbolSintactico * c, ArbolSintactico * is, int i) : condicion(c), cuerpo(is), instruccion(static_cast<inst>(i)) {}
		inst_guardia(ArbolSintactico * c, ArbolSintactico * is, ArbolSintactico * ie, int i) : condicion(c), cuerpo(is), cuerpo_else(ie), instruccion(static_cast<inst>(i)) {}
		virtual void imprimir(int tab){
			for (int j = 0; j < tab; j++) cout << " ";
			switch(instruccion){
				case IF:
					cout << "IF:" << endl;
					break;
				case ELSE:
					cout << "IF:" << endl;
					break;
				case WHILE:
					cout << "WHILE:" << endl;
					break;
			}
			for (int j = 0; j < tab+1; j++) cout << " ";
			cout << "GUARDIA:" << endl;
			condicion -> imprimir(tab+2);
			for (int j = 0; j < tab+1; j++) cout << " ";
			cout << "CUERPO:" << endl;
			cuerpo -> imprimir(tab+2);
			if (cuerpo_else != NULL){
				for (int j = 0; j < tab+1; j++) cout << " ";
				cout << "ELSE: " << endl;
				cuerpo_else -> imprimir(tab+2);
			}
		}
};


/* Definicion de la clase return y el break */
class ret_brk : public ArbolSintactico {
	public:
		bool ret;
		ArbolSintactico * valor;
		ret_brk(bool r, ArbolSintactico * v) : ret(r), valor(v) {}
		virtual void imprimir(int tab){
			for (int j = 0; j < tab+1; j++) cout << " ";
			if (ret){
				cout << "RETURN: " << endl;
				valor -> imprimir(tab+2);
			}else{
				cout << "BREAK" << endl;
			}
		}
};


/* Definicion de la asignacion */
class asignacion : public ArbolSintactico {
	public:
		ArbolSintactico * variable;
		ArbolSintactico * valor;
		asignacion(ArbolSintactico * v, ArbolSintactico * b) : variable(v), valor(b) {}
		virtual void imprimir(int tab){
			for (int j = 0; j < tab; j++) cout << " ";
			cout << "ASIGNACION: " << endl;
			for (int j = 0; j < tab+1; j++) cout << " ";
			cout << "VARIABLE: " << endl;
			variable->imprimir(tab+2);
			for (int j = 0; j < tab+1; j++) cout << " ";
			cout << "VALOR: " << endl;
			valor->imprimir(tab+2);
		}
};


/* Definicion de la clase para las expresiones que retornan int o float */
class exp_aritmetica : public ArbolSintactico {
	enum inst {SUMA, RESTA, MULT, DIV, MOD, POW, PARENTESIS, CORCHETE, LLAVE, INDEX, FUNCION, CONSTANTE};
	public:
		ArbolSintactico * der;
		ArbolSintactico * izq;
		inst instruccion;
		exp_aritmetica(ArbolSintactico * d, ArbolSintactico * i, int is) : der(d), izq(i), instruccion(static_cast<inst>(is)) {}
		exp_aritmetica(ArbolSintactico * d, int is) : der(d), instruccion(static_cast<inst>(is)) {}
		virtual void imprimir(int tab){
			
			for (int j = 0; j < tab; j++) cout << " ";
			switch(instruccion){
				case SUMA:
					cout << "SUMA:" << endl;
					break;
				case RESTA:
					cout << "RESTA:" << endl;
					break;
				case MULT:
					cout << "MULT:" << endl;
					break;
				case DIV:
					cout << "DIV:" << endl;
					break;
				case MOD:
					cout << "MOD:" << endl;
					break;
				case POW:
					cout << "POW:" << endl;
					break;
				case PARENTESIS:
					cout << "PARENTESIS:" << endl;
					break;
				case CORCHETE:
					cout << "CORCHETE:" << endl;
					break;
				case LLAVE:
					cout << "LLAVE:" << endl;
					break;
				case INDEX:
					cout << "INDEXACION:" << endl;
					break;
				case FUNCION:
					cout << "LLAMADA A FUNCION:" << endl;
					break;
				case CONSTANTE:
					cout << "VALOR:" << endl;
					break;
			}
			if (izq != NULL){
				for (int j = 0; j < tab+1; j++) cout << " ";
				cout << "DERECHA: " << endl;
				izq -> imprimir(tab+2);
			}
			if (der != NULL){
				for (int j = 0; j < tab+1; j++) cout << " ";
				cout << "IZQUIERDA: " << endl;
				der -> imprimir(tab+2);
			}
		}

};


/* Definicion de la clase para las expresiones que retornan booleandos */
class exp_booleana : public ArbolSintactico {
	enum inst {IGUALA, DISTINTO, MENOR, MAYOR, MENORIGUAL, MAYORIGUAL, DISYUNCION, CONJUNCION, NEGACION, PARENTESIS, CORCHETE, LLAVE, CONSTANTE};
	public:
		ArbolSintactico * der;
		ArbolSintactico * izq;
		inst instruccion;
		exp_booleana(ArbolSintactico * d, ArbolSintactico * i, int is) : der(d), izq(i), instruccion(static_cast<inst>(is)) {}
		exp_booleana(ArbolSintactico * d, int is) : der(d), instruccion(static_cast<inst>(is)) {}
		virtual void imprimir(int tab){
			for (int j = 0; j < tab; j++) cout << " ";
			switch(instruccion){
				case IGUALA:
					cout << "IGUALA:" << endl;
					break;
				case MENOR:
					cout << "MENOR:" << endl;
					break;
				case MAYOR:
					cout << "MAYOR:" << endl;
					break;
				case MENORIGUAL:
					cout << "MENORIGUAL:" << endl;
					break;
				case MAYORIGUAL:
					cout << "MAYORIGUAL:" << endl;
					break;
				case PARENTESIS:
					cout << "PARENTESIS:" << endl;
					break;
				case CORCHETE:
					cout << "CORCHETE:" << endl;
					break;
				case LLAVE:
					cout << "LLAVE:" << endl;
					break;
				case DISYUNCION:
					cout << "DISYUNCION:" << endl;
					break;
				case CONJUNCION:
					cout << "CONJUNCION:" << endl;
					break;
				case NEGACION:
					cout << "NEGACION:" << endl;
					break;
				case CONSTANTE:
					cout << "CONSTANTE:" << endl;
					break;
			}
			
			if (izq != NULL){
				for (int j = 0; j < tab+1; j++) cout << " ";
				cout << "DERECHA: " << endl;
				izq -> imprimir(tab+2);
			}
			if (der != NULL){
				for (int j = 0; j < tab+1; j++) cout << " ";
				cout << "IZQUIERDA: " << endl;
				der -> imprimir(tab+2);
			}
		}
};


/* Definicion de la clase para las expresiones para apuntadores*/
class exp_point : public ArbolSintactico {
	public:
		ArbolSintactico * der;
		exp_point(ArbolSintactico * d) : der(d) {}
		virtual void imprimir(int tab) {
			for (int j = 0; j < tab; j++) cout << " ";
			cout << "DESREFERENCIACION: " << endl;
			der -> imprimir(tab+1);
		}
};


/* Definicion de la clase para las expresiones para indexaciones*/
class exp_index : public ArbolSintactico {
	public:
		ArbolSintactico * var;
		ArbolSintactico * ind;
		exp_index(ArbolSintactico * v,ArbolSintactico * i) : var(v), ind(i) {}
		virtual void imprimir(int tab) {
			for (int j = 0; j < tab; j++) cout << " ";
			cout << "INDEXACION: " << endl;
			for (int j = 0; j < tab+1; j++) cout << " ";
			cout << "DE: " << endl;
			var -> imprimir(tab+2);
			for (int j = 0; j < tab+1; j++) cout << " ";
			cout << "EN: " << endl;
			ind -> imprimir(tab+2);
		}
};


/* Definicion de la clase para los identificadores */
class identificador : public ArbolSintactico {
	public:
		string valor;
		identificador(string v) : valor(v) {}
		virtual void imprimir(int tab) {
			for (int j = 0; j < tab; j++) cout << " ";
			cout << "identificador: " << valor << endl;
		}
};


/* Definicion de la clase para int */
class entero : public ArbolSintactico {
	public:
		int valor;
		entero(): ArbolSintactico(ENTEROS) {is_type = 1;}
		entero(int v) : valor(v), ArbolSintactico(ENTEROS){}
		virtual void imprimir(int tab) {
			if (!is_type){
				for (int j = 0; j < tab; j++) cout << " ";
				cout << "entero: " << valor << endl;
			}
		}
};


/* Definicion de la clase para float */
class flotante : public ArbolSintactico {
	public:
		float valor;
		flotante(): ArbolSintactico(FLOTANTES) {is_type = 1;}
		flotante(float v) : valor(v), ArbolSintactico(FLOTANTES){}
		virtual void imprimir(int tab) {
			if (!is_type){
				for (int j = 0; j < tab; j++) cout << " ";
				cout << "flotante: " << valor << endl;
			}
		}
};


/* Definicion de la clase para bool */
class booleano : public ArbolSintactico {
	public:
		bool valor;
		booleano(): ArbolSintactico(BOOLEANOS) {is_type = 1;}
		booleano(bool v) : valor(v), ArbolSintactico(BOOLEANOS) {}
		virtual void imprimir(int tab) {
			if (!is_type){
				for (int j = 0; j < tab; j++) cout << " ";
				cout << "booleano: " << valor << endl;
			}
		}
};


/* Definicion de la clase para char */
class character : public ArbolSintactico {
	public:
		char valor;
		character() : ArbolSintactico(CHARACTERES) {is_type = 1;}
		character(char v) : valor(v), ArbolSintactico(CHARACTERES) {}
		virtual void imprimir(int tab) {
			if (!is_type){
				for (int j = 0; j < tab; j++) cout << " ";
				cout << "character: " << valor << endl;
			}
		}
};


/* Definicion de la clase para unit (tipo que no retorna nada) */
class unit : public ArbolSintactico {
	public:
		unit() : ArbolSintactico(UNITS) {}
		virtual void imprimir(int tab){
			for (int j = 0; j < tab; j++) cout << " ";
			cout << "unit" << endl;
		}
};


/* Definicion de la clase para string */
class str : public ArbolSintactico {
	public:
		string valor;
		str() : ArbolSintactico(STR) {is_type = 1;}
		str(string v) : valor(v) {}
		virtual void imprimir(int tab){
			for (int j = 0; j < tab; j++) cout << " ";
			cout << "string: " << valor << endl;
		}
};


/* Definicion de la clase para ptr (apuntadores) */
class ptr : public ArbolSintactico {
	public:
		ArbolSintactico * tipo;
		ArbolSintactico * id;
		ArbolSintactico * elem; // atributo para lo que sea que apunta?
		ptr() {}
		ptr(ArbolSintactico * t, ArbolSintactico * i, ArbolSintactico * e): tipo(t), id(i), elem(e) {}
		virtual void imprimir(int tab){
			for (int j = 0; j < tab; j++) cout << " ";
			cout << "APUNTADOR: " << endl;
			if (tipo != NULL && id != NULL && elem != NULL){
				for (int j = 0; j < tab+1; j++) cout << " ";
				cout << "TIPO: " << endl;
				tipo -> imprimir(tab+2);
				for (int j = 0; j < tab+1; j++) cout << " ";
				cout << "IDENTIFICADOR: " << endl;
				id -> imprimir(tab+2);
				for (int j = 0; j < tab+1; j++) cout << " ";
				cout << "TIPO: " << endl;
				elem -> imprimir(tab+2);
			}
		}
};


/* Definicion de la clase para la lista de elementos de una lista o arreglo */
class elementos : public ArbolSintactico {
	public:
		ArbolSintactico * elems;
		ArbolSintactico * val;
		elementos( ArbolSintactico * v) : val(v) {}
		elementos(ArbolSintactico * e, ArbolSintactico * v) : elems(e), val(v) {}
		virtual void imprimir(int tab){
			if (elems != NULL){
				elems -> imprimir(tab);
			}
			for (int j = 0; j < tab; j++) cout << " ";
			cout << "ELEMENTO:" << endl;
			val -> imprimir(tab+1);
		}
};


/* Definicion de la clase para list */
class lista : public ArbolSintactico {
	public:
		ArbolSintactico * valor;
		lista( ArbolSintactico * v) :  valor(v) {}
		virtual void imprimir(int tab){
			for (int j = 0; j < tab; j++) cout << " ";
			cout << "LISTA:" << endl;
			if (valor != NULL){		
				for (int j = 0; j < tab+1; j++) cout << " ";
				cout << "CON:" << endl;
				valor -> imprimir(tab+2);
			}
		}
};


/* Definicion de la clase para array */
class arreglo : public ArbolSintactico {
	public:
		ArbolSintactico * valor;
		arreglo(ArbolSintactico * v) : valor(v) {}
		virtual void imprimir(int tab){
			for (int j = 0; j < tab; j++) cout << " ";
			cout << "ARREGLO:" << endl;	
			if (valor != NULL){		
				for (int j = 0; j < tab+1; j++) cout << " ";
				cout << "CON:" << endl;
				valor -> imprimir(tab+2);
			}
		}
};


/* Definicion de la clase para tuple */
class tupla : public ArbolSintactico {
	public:
		ArbolSintactico * valor1;
		ArbolSintactico * valor2;
		tupla(ArbolSintactico * v1, ArbolSintactico * v2) : valor1(v1), valor2(v2) {}
		virtual void imprimir(int tab){
			for (int j = 0; j < tab; j++) cout << " ";
			cout << "TUPLA:" << endl;	
			if (valor1 != NULL && valor2 != NULL){ 
				for (int j = 0; j < tab+1; j++) cout << " ";
				cout << "VALOR1:" << endl;	
				valor1 ->	imprimir(tab+2);
				for (int j = 0; j < tab+1; j++) cout << " ";
				cout << "VALOR2:" << endl;	
				valor2 -> imprimir(tab+2);
			}
		}
};