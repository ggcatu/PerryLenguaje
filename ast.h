/*
ast.cpp:

Contiene la definicion del arbol sintactico asi como la de la tabla de simbolos utilizadas
en el proyecto
*/

#include <stdio.h>
#include <stdlib.h>
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
#include "definiciones.h"
#include "ast_def.h"
#include "table.h"

using namespace std;
#define ENTEROS 1
#define FLOTANTES 3
#define BOOLEANOS 4
#define CHARACTERES 5
#define STR 6
#define UNITS 7

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
		raiz(ArbolSintactico * p, bool m) : programa(p), module(m) {verificar();}
		raiz(ArbolSintactico * i, ArbolSintactico * p, bool m) : includes(i), programa(p), module(m) {verificar();}
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
		virtual void verificar(){
			if (programa != NULL){
				programa -> verificar();
			}
			if (includes != NULL){
				programa -> verificar();
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
		funcion(ArbolSintactico * t, ArbolSintactico * i, ArbolSintactico * p, ArbolSintactico * is) : tipo(t), id(i), parametros(p), instrucciones(is) {verificar();}
		funcion(ArbolSintactico * t, ArbolSintactico * i, ArbolSintactico * p, ArbolSintactico * is, ArbolSintactico * fs) : tipo(t), id(i), parametros(p), instrucciones(is), funciones(fs) {verificar();}
		virtual void imprimir(int tab){
			cout << "FUNCION:" << endl;
			for (int j = 0; j < tab+1; j++) cout << " ";
			cout << "TIPO RETORNO:" << endl;
			tipo -> imprimir(tab+2);
			for (int j = 0; j < tab+1; j++) cout << " ";
			cout << "IDENTIFICADOR:" << endl;
			id -> imprimir(tab+2);
			for (int j = 0; j < tab+1; j++) cout << " ";
			cout << "INSTRUCCIONES:" << endl;
			if (instrucciones != NULL)
				instrucciones -> imprimir(tab+2);
			if (funciones != NULL){
				funciones -> imprimir(tab);
			}
		}
		virtual void verificar(){
			if (instrucciones != NULL){
				instrucciones -> verificar();
			}
			if (funciones != NULL){
				funciones -> verificar();
			}

		}
};


/* Definicion de la clase para una lista de importaciones */
class include : public ArbolSintactico {
	public:
		ArbolSintactico * archivo;
		ArbolSintactico * includes;
		include(ArbolSintactico * i, ArbolSintactico * a) : includes(i), archivo(a) {verificar();}
		include(ArbolSintactico * a) : archivo(a) {verificar();}
		virtual void imprimir(int tab) {
			cout << "INCLUDE:" << endl;
			if (archivo != NULL){
				archivo -> imprimir(tab+1);
			}
			if (includes != NULL){
				includes -> imprimir(tab);
			}
		}
		virtual void verificar(){
			// verificar que archivo es de tipo string
			// aun no
		}
};


/* Definicion de la clase para los structs, union */
class estructura : public ArbolSintactico {
	public:
		ArbolSintactico * id;
		ArbolSintactico * campos;
		ArbolSintactico * estructuras;
		bool st;
		estructura(ArbolSintactico * i, ArbolSintactico * c, bool t) : id(i), campos(c), st(t) {verificar();}
		estructura(ArbolSintactico * i, ArbolSintactico * c, ArbolSintactico * es, bool t) : id(i), campos(c), estructuras(es), st(t) {verificar();}
		virtual void imprimir(int tab){
			if (st){
				cout << "STRUCT:" << endl;
			}
			else {
				cout << "UNION:" << endl;
			}
			if (campos != NULL){
				for (int j = 0; j < tab+1; j++) cout << " ";
				cout << "INDENTIFICADOR:" << endl;
				id -> imprimir(tab+2);
				for (int j = 0; j < tab+1; j++) cout << " ";
				cout << "CAMPOS:" << endl;
				campos -> imprimir(tab+1);
			}
			if (estructuras != NULL){
				estructuras -> imprimir(tab);
			}
		}
		virtual void verificar(){
			// verificar?
		}
};


/* Definicion de la clase para los type */
class tipo : public ArbolSintactico {
	public:
		ArbolSintactico * ti;
		ArbolSintactico * id;
		ArbolSintactico * tis;
		tipo(ArbolSintactico * i, ArbolSintactico * t) : id(i), ti(t) {verificar();}
		tipo(ArbolSintactico * i, ArbolSintactico * t, ArbolSintactico * ts) : id(i), ti(t), tis(ts) {verificar();}
		virtual void imprimir(int tab){
			cout << "TYPE" << endl;
			for (int j = 0; j < tab+1; j++) cout << " ";
			cout << "IDENTIFICADOR:" << endl;
			id -> imprimir(tab+2);
			for (int j = 0; j < tab+1; j++) cout << " ";
			cout << "TIPO:" << endl;
			if (ti != NULL)
				ti -> imprimir(tab+2);
			if (tis != NULL){
				tis -> imprimir(tab);
			}
		}
		virtual void verificar(){
			// verificar?
		}
};


/* Definicion de la clase para el main */
class programa : public ArbolSintactico {
	public:
		ArbolSintactico * program;
		ArbolSintactico * sec;
		programa(ArbolSintactico * s, ArbolSintactico * p) : program(p), sec(s) {verificar();}
		programa(ArbolSintactico * s) : sec(s) {verificar();}
		virtual void imprimir(int tab){
			if (program != NULL){
				program -> imprimir(tab);
			}
			if (sec != NULL){
				cout << "MAIN:" << endl;
				sec -> imprimir(tab+1);
			}
		}
		virtual void verificar(){
			if (program != NULL){
				program -> verificar();
			} 
			if (sec != NULL){
				sec -> verificar();
			}
		}
};


/* Definicion de la clase para un bloque create-execute */
class bloque : public ArbolSintactico {
	public:
		ArbolSintactico * declaraciones;
		ArbolSintactico * instrucciones;
		bloque(ArbolSintactico * d, ArbolSintactico * i) : declaraciones(d), instrucciones(i) {verificar();}
		bloque(ArbolSintactico * i) : instrucciones(i) {verificar();}
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
		virtual void verificar(){
			if (declaraciones != NULL){
				declaraciones -> verificar();
			}
			if (instrucciones != NULL){
				instrucciones -> verificar();
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
		declaracion(ArbolSintactico * d, ArbolSintactico * t, ArbolSintactico * i, ArbolSintactico * e) : declaraciones(d), tipo(t), id(i), exp(e) {verificar();}
		declaracion(ArbolSintactico * d, ArbolSintactico * t, ArbolSintactico * i, bool aux) : declaraciones(d), tipo(t), id(i) {verificar();}
		declaracion(ArbolSintactico * t, ArbolSintactico * i, ArbolSintactico * e) : tipo(t), id(i), exp(e) {verificar();}
		declaracion(ArbolSintactico * t, ArbolSintactico * i) : tipo(t), id(i) {verificar();}
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
		virtual void verificar(){
			// verificar?
		}
};

/* Definicion de la clase para los identificadores */
class identificador : public ArbolSintactico {
	public:
		string valor;
		identificador(string v) : valor(v) {}
		virtual void imprimir(int tab) {
			for (int j = 0; j < tab; j++) cout << " ";
			cout << "id: " << valor << endl;
		}
};

/* Definicion de la clase para la lista de declaraciones */
class tipedec : public ArbolSintactico {

	public:
		type& tipo;
		ArbolSintactico * subtipo1;
		ArbolSintactico * subtipo2;
		ArbolSintactico * tam;
		tipedec( type& n, ArbolSintactico * s1, ArbolSintactico * s2, ArbolSintactico * t) : tipo(n), subtipo1(s1), tam(t) {is_type = 1;}
		tipedec( type& n, ArbolSintactico * s1, ArbolSintactico * s2) : tipo(n), subtipo1(s1), subtipo2(s2) {is_type = 1;}
		tipedec( type& n, ArbolSintactico * s1) : tipo(n), subtipo1(s1) {is_type = 1;}
		tipedec( type& n) : tipo(n) {is_type = 1;}

		bool asignar_tipo(string variable, sym_table * table ){
			table_element * instancia = table->lookup(variable, -1);
			switch(tipo.tipo){
				case IDENTIFIER: {
					table_element * elemento = table->lookup(((identificador *)subtipo1)->valor, -1);
					if (elemento != NULL){
						instancia->set_child_scope(elemento->child_scope);
					} else {
						cout << "Error: " << ((identificador *)subtipo1)->valor << " no es un tipo" << endl;
						// , en la fila " << yylineno << ", columna " <<  yycolumn-1-strlen(yytext)
						return false;
					}
				}
				default:
					instancia->set_type(tipo);
					instancia->print();
			}
			return true;
		}

		virtual void imprimir(int tab){
			for (int j = 0; j < tab; j++) cout << " ";
			switch(tipo.tipo){
				case BOOL:
					cout << "BOOL:" << endl;
					break;
				case LCHAR:
					cout << "CHAR:" << endl;
					break;
				case LSTRING:
					cout << "STRING:" << endl;
					break;
				case LINT:
					cout << "ENTERO:" << endl;
					break;
				case LFLOAT:
					cout << "FLOTANTE:" << endl;
					break;
				case ARRAY:
					cout << "ARREGLO:" << endl;
					break;
				case LIST:
					cout << "LISTA:" << endl;
					break;
				case TUPLE:
					cout << "TUPLA:" << endl;
					break;
				case IDENTIFIER:
					cout << "IDENTIFICADOR:" << endl;
					break;
				case POINTER:
					cout << "APUNTADOR:" << endl;
					break;
				case UNIT:
					cout << "UNIT:" << endl;
					break;
			}
			if (subtipo1 != NULL){
				for (int j = 0; j < tab; j++) cout << " ";
				cout << "TIPO:" << endl;
				subtipo1 -> imprimir(tab+1); 
			}
			if (subtipo2 != NULL){
				for (int j = 0; j < tab; j++) cout << " ";
				cout << "TIPO:" << endl;
				subtipo2 -> imprimir(tab+1); 
			}
			if (tam != NULL){
				for (int j = 0; j < tab; j++) cout << " ";
				cout << "TAMAÑO:" << endl;
				tam -> imprimir(tab+1); 
			}
		}
};


/* Definicion de la clase para la lista de instrucciones */
class instruccion : public ArbolSintactico {
	public:
		ArbolSintactico * instrucciones;
		ArbolSintactico * inst;
		instruccion() {}
		instruccion(ArbolSintactico * is, ArbolSintactico * i) : instrucciones(is), inst(i) {verificar();}
		instruccion(ArbolSintactico * i) : inst(i) {verificar();}
		virtual void imprimir(int tab){
			if (inst != NULL){
				inst -> imprimir(tab);
			}
			if (instrucciones != NULL){
				instrucciones -> imprimir(tab);
			}
		}
		virtual void verificar(){
			if (inst != NULL){
				inst -> verificar();
			}
			if (instrucciones != NULL){
				instrucciones -> verificar();
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
		parametros(ArbolSintactico * t, ArbolSintactico * i, bool r) : tipo(t), id(i), ref(r) {verificar();}
		parametros(ArbolSintactico * p, ArbolSintactico * t, ArbolSintactico * i, bool r) : params(p), tipo(t), id(i), ref(r) {verificar();}
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
		virtual void verificar(){
			// verificar?
		}
};


/* Definicion de la clase para la llamada a una funcion */
class llamada : public ArbolSintactico {
	public:
		ArbolSintactico * parametros;
		ArbolSintactico * id;
		llamada(ArbolSintactico * i, ArbolSintactico * p) : id(i), parametros(p) {verificar();}
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
		virtual void verificar(){
			// verificar que cada tipo es igual a los parametros asociados a id, incluso el numero de parametros
			// la tabla tiene los parametros?
		}
};


/* Definicion de la clase para new y free */
class memoria : public ArbolSintactico {
	public:
		ArbolSintactico * id;
		bool is_new;
		memoria(ArbolSintactico * i, bool in) : id(i), is_new(in) {verificar();}
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
		virtual void verificar(){
			// que se debe verificar aqui?
		}
};


/* Definicion de la clase para la entrada y salida */
class entrada_salida : public ArbolSintactico {
	public:
		ArbolSintactico * exp;
		bool entrada;
		entrada_salida(ArbolSintactico * s, bool e) : exp(s), entrada(e) {verificar();}
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
		virtual void verificar(){
			// aqui que se debe verificar
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
		it_determinada(ArbolSintactico * v, ArbolSintactico * i, ArbolSintactico * f, ArbolSintactico * p, ArbolSintactico * is) : variable(v), inicio(i), fin(f), paso(p), inst(is) {verificar();}
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
			for (int j = 0; j < tab+1; j++) cout << " ";
			cout << "CUERPO:" << endl;
			inst -> imprimir(tab+2);
		}
		virtual void verificar(){
			/*
			// que verificar con la variable?
			// verificar que inicio, fin, paso son numeros
			table_element * tipo_variable = table->lookup((variable)->valor, -1);
			if (tipo_variable != LINT && tipo_variable != LFLOAT){
				cout << "La variable " << variable. << " debe ser un entero o un flotante" endl;
			}
			table_element * tipo_inicio = table->lookup((inicio)->valor, -1);
			if (tipo_inicio != LINT && tipo_inicio != LFLOAT){
				cout << "El inicio del rango del for debe ser un entero o un flotante." << endl;
			}
			table_element * tipo_fin = table->lookup((fin)->valor, -1);
			if (tipo_fin != LINT && tipo_fin != LFLOAT){
				cout << "El fin del rango del for debe ser un entero o un flotante." << endl;
			}
			table_element * tipo_paso = table->lookup((paso)->valor, -1);
			if (tipo_paso != LINT && tipo_paso != LFLOAT){
				cout << "El paso  en el for debe ser un entero o flotante." << endl;
			}
			*/
			inst -> verificar();
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
		inst_guardia(ArbolSintactico * c, ArbolSintactico * is, int i) : condicion(c), cuerpo(is), instruccion(static_cast<inst>(i)) {verificar();}
		inst_guardia(ArbolSintactico * c, ArbolSintactico * is, ArbolSintactico * ie, int i) : condicion(c), cuerpo(is), cuerpo_else(ie), instruccion(static_cast<inst>(i)) {verificar();}
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
		virtual void verificar(){
			// verificar que condicion es booleano
			/*
			table_element * tipo_condicion = table->lookup((condicion)->valor, -1);
			if (tipo_condicion != LBOOL){
				cout << "La condicion debe ser de tipo booleano." << endl;
			}
			*/
			cuerpo -> verificar();
			if (cuerpo_else != NULL){ 
				cuerpo_else -> verificar();
			}
		}
};


/* Definicion de la clase return y el break */
class ret_brk : public ArbolSintactico {
	public:
		bool ret;
		ArbolSintactico * valor;
		ret_brk(bool r, ArbolSintactico * v) : ret(r), valor(v) {verificar();}
		virtual void imprimir(int tab){
			for (int j = 0; j < tab+1; j++) cout << " ";
			if (ret){
				cout << "RETURN: " << endl;
				valor -> imprimir(tab+2);
			}else{
				cout << "BREAK" << endl;
			}
		}
		virtual void verificar(){
			// el libro dice que se debe verificar que pertenece a un alcance de for, while = break
			// verificar que pertenece a una funcion y que el tipo corresponde con su tipo salida
		}
};

class skip : public ArbolSintactico {
	public:
		ArbolSintactico * siguiente;
		skip(ArbolSintactico * s ) : siguiente(s) {verificar();}
		skip(){}
		virtual void imprimir(int tab){
			if (siguiente != NULL)
				siguiente->imprimir(tab);
		}
		virtual void verificar(){
			if (siguiente != NULL){
				siguiente -> verificar();
			} 
		}
};

/* Definicion de la asignacion */
class asignacion : public ArbolSintactico {
	public:
		ArbolSintactico * variable;
		ArbolSintactico * valor;
		ArbolSintactico * siguiente;
		asignacion(ArbolSintactico * v, ArbolSintactico * b, ArbolSintactico * s) : variable(v), valor(b), siguiente(s) {verificar();}
		virtual void imprimir(int tab){
			for (int j = 0; j < tab; j++) cout << " ";
				cout << "ASIGNACION: " << endl;

			if (variable != NULL){
				for (int j = 0; j < tab+1; j++) cout << " ";
				cout << "VARIABLE: " << endl;
				variable->imprimir(tab+2);
			}

			if (valor != NULL){
				for (int j = 0; j < tab+1; j++) cout << " ";
				cout << "VALOR: " << endl;
				valor->imprimir(tab+2);
			}
			
			if (siguiente != NULL) 
				siguiente->imprimir(tab+2);
		}
		virtual void verificar(){
			/*
			// verificar que variable y valor son del mismo tipo
			table_element * tipo_var = table->lookup(variable->valor, -1);
			table_el-ement * tipo_val = table->lookup(valor->valor, -1);
			if (tipovar != tipo_val){
				cout << "Los tipos de la asignacion no son iguales." << endl;
			}*/
			if (siguiente != NULL){
				siguiente -> verificar();
			} 
		}
};


/* Definicion de la clase para las expresiones que retornan int o float */
class exp_aritmetica : public ArbolSintactico {
	enum inst {SUMA, RESTA, MULT, DIV, MOD, POW};
	public:
		ArbolSintactico * der;
		ArbolSintactico * izq;
		inst instruccion;
		exp_aritmetica(ArbolSintactico * d, ArbolSintactico * i, int is) : der(d), izq(i), instruccion(static_cast<inst>(is)) {verificar();}
		exp_aritmetica(ArbolSintactico * d, int is) : der(d), instruccion(static_cast<inst>(is)) {verificar();}

		virtual void verificar(){
			/*
			table_element * tipo_der = table->lookup(der->valor, -1);
			table_element * tipo_izq = table->lookup(izq->valor, -1);
				
			switch(instruccion){
				case SUMA:
					if (tipo_izq != LINT && tipo_izq != LFLOAT){
						cout << "Las expresiones aritmeticas deben tener tipo entero o flotante." << endl;
					}
					if (tipo_der != LINT && tipo_der != LFLOAT){
						cout << "Las expresiones aritmeticas deben tener tipo entero o flotante." << endl;
					}
					break;
				case RESTA:
					if (tipo_der != LINT && tipo_der != LFLOAT){
						cout << "Las expresiones aritmeticas deben tener tipo entero o flotante." << endl;
					}
					break;
				case MULT:
					if (tipo_izq != LINT && tipo_izq != LFLOAT){
						cout << "Las expresiones aritmeticas deben tener tipo entero o flotante." << endl;
					}
					if (tipo_der != LINT && tipo_der != LFLOAT){
						cout << "Las expresiones aritmeticas deben tener tipo entero o flotante." << endl;
					}
					break;
				case DIV:
					if (tipo_izq != LINT && tipo_izq != LFLOAT){
						cout << "Las expresiones aritmeticas deben tener tipo entero o flotante." << endl;
					}
					if (tipo_der != LINT && tipo_der != LFLOAT){
						cout << "Las expresiones aritmeticas deben tener tipo entero o flotante." << endl;
					}
					break;
				case MOD:
					if (tipo_izq != LINT){
						cout << "Las expresiones aritmeticas deben tener tipo entero o flotante." << endl;
					}
					if (tipo_der != LINT){
						cout << "Las expresiones aritmeticas deben tener tipo entero o flotante." << endl;
					}
					break;
				case POW:
					if (tipo_izq != LINT && tipo_izq != LFLOAT){
						cout << "Las expresiones aritmeticas deben tener tipo entero." << endl;
					}
					if (tipo_der != LINT){
						cout << "El exponente debe ser de tipo entero." << endl;
					}
					break;
			}
			*/
		}

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
	enum inst {IGUALA, DISTINTO, MENOR, MAYOR, MENORIGUAL, MAYORIGUAL, DISYUNCION, CONJUNCION, NEGACION};
	public:
		ArbolSintactico * der;
		ArbolSintactico * izq;
		inst instruccion;
		exp_booleana(ArbolSintactico * d, ArbolSintactico * i, int is) : der(d), izq(i), instruccion(static_cast<inst>(is)) {verificar();}
		exp_booleana(ArbolSintactico * d, int is) : der(d), instruccion(static_cast<inst>(is)) {verificar();}

		void verificar(){
			/*
			table_element * tipo_der = table->lookup(der->valor, -1);
			table_element * tipo_izq = table->lookup(izq->valor, -1);
				
			switch(instruccion){
				case IGUALA:
					if !(tipo_izq == LBOOL){
						cout << "Las expresiones boolenas deben ser de tipo booleno." << endl;
					}
					if !(tipo_der == LBOOL){
						cout << "Las expresiones boolenas deben ser de tipo booleno." << endl;
					}
					break;
				case DISTINTO:
					if !(tipo_izq == LBOOL){
						cout << "Las expresiones boolenas deben ser de tipo booleno." << endl;
					}
					if !(tipo_der == LBOOL){
						cout << "Las expresiones boolenas deben ser de tipo booleno." << endl;
					}
					break;
				case MENOR:
					if (tipo_izq != LINT && tipo_izq != LFLOAT){
						cout << "Las expresiones aritmeticas debe ser de tipo entero o flotante." << endl;
					}
					if (tipo_der != LINT && tipo_der != LFLOAT){
						cout << "Las expresiones aritmeticas debe ser de tipo entero o flotante." << endl;
					}
					break;
				case MAYOR:
					if (tipo_izq != LINT && tipo_izq != LFLOAT){
						cout << "Las expresiones aritmeticas debe ser de tipo entero o flotante." << endl;
					}
					if (tipo_der != LINT && tipo_der != LFLOAT){
						cout << "Las expresiones aritmeticas debe ser de tipo entero o flotante." << endl;
					}
					break;
				case MENORIGUAL:
					if (tipo_izq != LINT && tipo_izq != LFLOAT){
						cout << "Las expresiones aritmeticas debe ser de tipo entero o flotante." << endl;
					}
					if (tipo_der != LINT && tipo_der != LFLOAT){
						cout << "Las expresiones aritmeticas debe ser de tipo entero o flotante." << endl;
					}
					break;
				case MAYORIGUAL:
					if (tipo_izq != LINT && tipo_izq != LFLOAT){
						cout << "Las expresiones aritmeticas debe ser de tipo entero o flotante." << endl;
					}
					if (tipo_der != LINT && tipo_der != LFLOAT){
						cout << "Las expresiones aritmeticas debe ser de tipo entero o flotante." << endl;
					}
					break;
				case DISYUNCION:
					if (!(tipo_izq == LBOOL)){
						cout << "Las expresiones boolenas deben ser de tipo booleno." << endl;
					}
					if (!(tipo_der == LBOOL)){
						cout << "Las expresiones boolenas deben ser de tipo booleno." << endl;
					}
					break;
				case CONJUNCION:
					if (!(tipo_izq == LBOOL)){
						cout << "Las expresiones boolenas deben ser de tipo booleno." << endl;
					}
					if (!(tipo_der == LBOOL)){
						cout << "Las expresiones boolenas deben ser de tipo booleno." << endl;
					}
					break;
				case NEGACION:
					if (!(tipo_der == LBOOL)){
						cout << "Las expresiones boolenas deben ser de tipo booleno." << endl;
					}
					break;
			}
			*/
		}
		
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
				case DISYUNCION:
					cout << "DISYUNCION:" << endl;
					break;
				case CONJUNCION:
					cout << "CONJUNCION:" << endl;
					break;
				case NEGACION:
					cout << "NEGACION:" << endl;
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
		exp_point(ArbolSintactico * d) : der(d) {verificar();}
		virtual void imprimir(int tab) {
			for (int j = 0; j < tab; j++) cout << " ";
			cout << "DESREFERENCIACION: " << endl;
			der -> imprimir(tab+1);
		}
		virtual void verificar(){
			// verificar?
		}
};


/* Definicion de la clase para las expresiones para indexaciones*/
class exp_index : public ArbolSintactico {
	public:
		ArbolSintactico * ind;
		ArbolSintactico * indexaciones;
		exp_index(ArbolSintactico * i,ArbolSintactico * ids) : indexaciones(ids), ind(i) {verificar();}
		exp_index(ArbolSintactico * i) : ind(i) {verificar();}
		virtual void imprimir(int tab) {
			for (int j = 0; j < tab; j++) cout << " ";
			cout << "INDEXACION: " << endl;
			for (int j = 0; j < tab+1; j++) cout << " ";
			cout << "ELEMENTO: " << endl;
			ind -> imprimir(tab+2);
			if (indexaciones != NULL){
				indexaciones -> imprimir(tab+2);
			}
		}
		virtual void verificar(){
			/*
			table_element * tipo_ind = table->lookup(ind->valor, -1);
			if (tipo_ind != LINT) {
				cout << "La indexacion es de tipo entero." << endl;
			}
			*/
			if (indexaciones != NULL){
				indexaciones -> verificar();
			} 
		}
};


/* Definicion de la clase para los accesos a campos e identificadores en expresiones */
class ids : public ArbolSintactico {
	public:
		ArbolSintactico * id;
		ArbolSintactico * idr;
		ArbolSintactico * indx;
		ids(ArbolSintactico * i) : id(i) {}
		ids(ArbolSintactico * i, ArbolSintactico * is) : id(i), idr(is) {}
		ids(ArbolSintactico * i, ArbolSintactico * is, ArbolSintactico * ix) :indx(ix), id(i), idr(is) {verificar();}
		virtual void imprimir(int tab){
			if (idr != NULL){
				for (int j = 0; j < tab; j++) cout << " ";
				cout << "ACCESO: " << endl;
			}
			for (int j = 0; j < tab+1; j++) cout << " ";
			cout << "IDENTIFICADOR: " << endl;
			id -> imprimir(tab+2);
			if (idr != NULL){
				for (int j = 0; j < tab+1; j++) cout << " ";
				cout << "CAMPO: " << endl;
				idr -> imprimir(tab+2);
			}
			if (indx != NULL){
				indx -> imprimir(tab+2);
			}
		}
		virtual string str(){
			// if (idr != NULL)  
				// return id << "." << idr.str();
		}
		virtual void verificar(){
			// que se debe verificar?
		}
};


/* Definicion de la clase para int */
class entero : public ArbolSintactico {
	public:
		int valor;
		entero(): ArbolSintactico(ENTEROS) {is_type = 1;}
		entero(int v) : valor(v), ArbolSintactico(ENTEROS) {}
		virtual void imprimir(int tab) {
			if (!is_type){
				for (int j = 0; j < tab; j++) cout << " ";
				cout << "int: " << valor << endl;
			}
		}
		virtual void verificar(){
			//
		}
};


/* Definicion de la clase para float */
class flotante : public ArbolSintactico {
	public:
		float valor;
		flotante(): ArbolSintactico(FLOTANTES) {is_type = 1;}
		flotante(float v) : valor(v), ArbolSintactico(FLOTANTES) {}
		virtual void imprimir(int tab) {
			if (!is_type){
				for (int j = 0; j < tab; j++) cout << " ";
				cout << "float: " << valor << endl;
			}
		}
		virtual void verificar(){
			//
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
				if (valor){
					cout << "bool: true" << endl;
				} else{
					cout << "bool: false" << endl;
				}
			}
		}
		virtual void verificar(){
			//
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
				cout << "char: " << valor << endl;
			}
		}
		virtual void verificar(){
			//
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
		virtual void verificar(){
			//
		}
};


/* Definicion de la clase para string */
class str : public ArbolSintactico {
	public:
		string valor;
		str() : ArbolSintactico(STR) {is_type = 1;}
		str(string v) : valor(v), ArbolSintactico(STR) {}
		virtual void imprimir(int tab){
			for (int j = 0; j < tab; j++) cout << " ";
			cout << "string: " << valor << endl;
		}
		virtual void verificar(){
			//
		}
};


/* Definicion de la clase para ptr (apuntadores) */
class ptr : public ArbolSintactico {
	public:
		ArbolSintactico * tipo;
		ArbolSintactico * id;
		ArbolSintactico * elem; // atributo para lo que sea que apunta?
		ptr() {}
		ptr(ArbolSintactico * t, ArbolSintactico * i, ArbolSintactico * e): tipo(t), id(i), elem(e) {verificar();}
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
		virtual void verificar(){

		}
};


/* Definicion de la clase para la lista de elementos de una lista o arreglo */
class elementos : public ArbolSintactico {
	public:
		ArbolSintactico * elems;
		ArbolSintactico * val;
		elementos( ArbolSintactico * v) : val(v) {}
		elementos(ArbolSintactico * e, ArbolSintactico * v) : elems(e), val(v) {verificar();}
		virtual void imprimir(int tab){
			if (elems != NULL){
				elems -> imprimir(tab);
			}
			for (int j = 0; j < tab; j++) cout << " ";
			cout << "PARAMETRO:" << endl;
			val -> imprimir(tab+1);
		}
		virtual void verificar(){
			// que debemos verificar?
		}
};


/* Definicion de la clase para list */
class lista : public ArbolSintactico {
	public:
		ArbolSintactico * valor;
		lista( ArbolSintactico * v) :  valor(v) {verificar();}
		virtual void imprimir(int tab){
			for (int j = 0; j < tab; j++) cout << " ";
			cout << "LISTA:" << endl;
			if (valor != NULL){		
				for (int j = 0; j < tab+1; j++) cout << " ";
				cout << "CON:" << endl;
				valor -> imprimir(tab+2);
			}
		}
		virtual void verificar(){
			// que verificar aqui?
		}
};


/* Definicion de la clase para array */
class arreglo : public ArbolSintactico {
	public:
		ArbolSintactico * valor;
		arreglo(ArbolSintactico * v) : valor(v) {verificar();}
		virtual void imprimir(int tab){
			for (int j = 0; j < tab; j++) cout << " ";
			cout << "ARREGLO:" << endl;	
			if (valor != NULL){		
				for (int j = 0; j < tab+1; j++) cout << " ";
				cout << "CON:" << endl;
				valor -> imprimir(tab+2);
			}
		}
		virtual void verificar(){
			// verificamos que los elementos del arreglo son del tipo del arreglo
		}
};


/* Definicion de la clase para tuple */
class tupla : public ArbolSintactico {
	public:
		ArbolSintactico * valor1;
		ArbolSintactico * valor2;
		tupla(ArbolSintactico * v1, ArbolSintactico * v2) : valor1(v1), valor2(v2) {verificar();}
		virtual void imprimir(int tab){
			for (int j = 0; j < tab; j++) cout << " ";
			cout << "TUPLA:" << endl;	
			if (valor1 != NULL && valor2 != NULL){ 
				for (int j = 0; j < tab+1; j++) cout << " ";
				cout << "PRIMER VALOR:" << endl;	
				valor1 ->	imprimir(tab+2);
				for (int j = 0; j < tab+1; j++) cout << " ";
				cout << "SEGUNDO VALOR:" << endl;	
				valor2 -> imprimir(tab+2);
			}
		}
		virtual void verificar(){
			// verificar que?
		}
};