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

/* Definiciones externas (parser.y) que permiten compartir el codigo. */
extern sym_table table;
extern int yylineno;
extern int yycolumn;
extern char error_strp[1000];
extern int yyparse();

/* Definicion de la clase raiz */
class raiz : public ArbolSintactico {
	public:
		bool module;
		ArbolSintactico * includes;
		ArbolSintactico * programa;
		raiz(): programa(NULL), module(NULL), includes(NULL) {}
		raiz(ArbolSintactico * p, bool m) : programa(p), module(m), includes(NULL) {verificar();}
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
		ArbolSintactico * id;
		ArbolSintactico * instrucciones;
		ArbolSintactico * funciones;
		funcion(ArbolSintactico * i, ArbolSintactico * is) :  id(i), instrucciones(is), funciones(NULL) {verificar();}
		funcion(ArbolSintactico * i, ArbolSintactico * is, ArbolSintactico * fs) : id(i), instrucciones(is), funciones(fs) {verificar();}

		virtual void imprimir(int tab){
			cout << "FUNCION:" << endl;
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
		include(ArbolSintactico * a) : includes(NULL), archivo(a) {verificar();}

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
			type * tipo_archivo = archivo->get_tipo();
			if (tipo_archivo != &tipo_string::instance()){
				cout << "El nombre del archivo importado debe ser de tipo string." << endl;
			}
			if (includes != NULL){
				includes -> verificar();
			} 
		}
};


/* Definicion de la clase para el main */
class programa : public ArbolSintactico {
	public:
		ArbolSintactico * program;
		ArbolSintactico * sec;
		programa(ArbolSintactico * s, ArbolSintactico * p) : program(p), sec(s) {verificar();}
		programa(ArbolSintactico * s) : sec(s), program(NULL) {verificar();}

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
		bloque(ArbolSintactico * i) : instrucciones(i), declaraciones(NULL) {verificar();}

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


/* Definicion de la clase para los identificadores */
class identificador : public ArbolSintactico {
	public:
		string valor;
		identificador(string v) : valor(v), ArbolSintactico() { verificar(); }

		void verificar(){
			get_tipo();
		}
		type * get_tipo(){
			if (tipo != NULL){
				return tipo;
			}

			table_element * instancia = table.lookup(valor, -1);
			// cout << "Busqueda de " << valor <<endl;
			// table.print();
			if (instancia){
				tipo = instancia->tipo;
				// cout << "#" << valor << " " << instancia->tipo << endl;
				return tipo;
			}
			return &tipo_error::instance();
		};
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
		tipedec( type& n, ArbolSintactico * s1, ArbolSintactico * s2) : tipo(n), subtipo1(s1), subtipo2(s2), tam(NULL) {is_type = 1;}
		tipedec( type& n, ArbolSintactico * s1) : tipo(n), subtipo1(s1), subtipo2(NULL), tam(NULL) {is_type = 1;}
		tipedec( type& n) : tipo(n), subtipo1(NULL), subtipo2(NULL), tam(NULL) {is_type = 1;}

		bool asignar_tipo(string variable, sym_table * table ){
			table_element * instancia = table->lookup(variable, -1);
			// if (instancia == NULL || subtipo1 == NULL){
				// return false;
			// }
			switch(tipo.tipo){
				case IDENTIFIER: {
					table_element * elemento = table->lookup(((identificador *)subtipo1)->valor, -1);
					if (elemento != NULL){
						instancia->set_child_scope(elemento->child_scope);
					} else {
						cout << "Error: " << ((identificador *)subtipo1)->valor << " no es un tipo" << endl;
						return false;
					}
				}
				default:
					instancia->set_type(tipo);
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
		instruccion(ArbolSintactico * i) : instrucciones(NULL), inst(i) {verificar();}

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
			type * tipo_id = id -> get_tipo();
			//if (tipo_id != &tipo_pointer::instance()){
			//	cout << "Solo se puede reservar o liberar memoria para apuntadores." << endl;
			//}
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
			exp -> verificar();
		}
};


/* Definicion de la clase para el for */
class it_determinada : public ArbolSintactico {
	public:
		ArbolSintactico * inicio;
		ArbolSintactico * fin;
		ArbolSintactico * paso;
		ArbolSintactico * inst;
		it_determinada(ArbolSintactico * i, ArbolSintactico * f, ArbolSintactico * p, ArbolSintactico * is) : inicio(i), fin(f), paso(p), inst(is) {verificar();}
		
		virtual void imprimir(int tab){
			for (int j = 0; j < tab; j++) cout << " ";
			cout << "FOR:" << endl;
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
			type * tipo_inicio = inicio->get_tipo();
			type * tipo_fin = fin->get_tipo();
			type * tipo_paso = paso->get_tipo();
			if (tipo_inicio != &tipo_int::instance() && tipo_inicio != &tipo_float::instance()){
				cout << "El inicio del rango del for debe ser de tipo entero o flotante." << endl;
				tipo = &tipo_error::instance();
			}
			if (tipo_fin != &tipo_int::instance() && tipo_fin != &tipo_float::instance()){
				cout << "El fin del rango del for debe ser de tipo entero o flotante." << endl;
				tipo = &tipo_error::instance();
			}
			if (tipo_paso != &tipo_int::instance() && tipo_paso != &tipo_float::instance()){
				cout << "El paso del for debe ser de tipo entero o flotante." << endl;
				tipo = &tipo_error::instance();
			}
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
		inst_guardia(ArbolSintactico * c, ArbolSintactico * is, int i) : condicion(c), cuerpo(is), cuerpo_else(NULL), instruccion(static_cast<inst>(i)) {verificar();}
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
			if (condicion->get_tipo() != &tipo_bool::instance()){
				if (instruccion == IF){
					cout << "La condición del if debe ser de tipo booleano." << endl;	
				} 
				else if (instruccion == WHILE){
					cout << "La condición del while debe ser de tipo booleano." << endl;
				}
				tipo = &tipo_error::instance();
			}
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
			if (ret){
				valor -> verificar();
			}
		}
};


class skip : public ArbolSintactico {
	public:
		ArbolSintactico * siguiente;
		skip(ArbolSintactico * s ) : siguiente(s) {verificar();}
		skip(): siguiente(NULL) {}

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
			type * tipo_var = variable->get_tipo();
			type * tipo_val = valor->get_tipo();
			//cout << variable->get_tipo() << "  " << valor->get_tipo() << endl;
			if (tipo_val != tipo_var){
				if ((tipo_var != &tipo_float::instance() || tipo_val != &tipo_int::instance()) && tipo_val != &tipo_unit::instance()){
					cout << "Los tipos de la asignacion no son iguales." << endl;
				}

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
		exp_aritmetica(ArbolSintactico * d, ArbolSintactico * i, int is) : der(i), izq(d), instruccion(static_cast<inst>(is)) {verificar();}
		exp_aritmetica(ArbolSintactico * d, int is) : der(d), izq(NULL), instruccion(static_cast<inst>(is)) {verificar();}

		type * get_tipo(){
			return tipo;
		};
		virtual void verificar(){
			type * tipo_der = der->get_tipo();
			type * tipo_izq = (izq!=NULL) ? izq->get_tipo(): &tipo_unit::instance();
			switch(instruccion){
				case SUMA:
				case MULT:
				case DIV:
					if (tipo_izq != &tipo_int::instance() && tipo_izq != &tipo_float::instance() ||
						tipo_der != &tipo_int::instance() && tipo_der != &tipo_float::instance()){
						tipo = &tipo_error::instance();
						cout << "Las expresiones aritmeticas deben tener tipo entero o flotante." << endl;
					}
					break;
				case RESTA:
					if (izq == NULL){
						if (tipo_der != &tipo_int::instance() && tipo_der != &tipo_float::instance()){
							tipo = &tipo_error::instance();
							cout << "Las expresiones aritmeticas deben tener tipo entero o flotante." << endl;
						}
					} else{
						if (tipo_izq != &tipo_int::instance() && tipo_izq != &tipo_float::instance() ||
							tipo_der != &tipo_int::instance() && tipo_der != &tipo_float::instance()){
							tipo = &tipo_error::instance();
							cout << "Las expresiones aritmeticas deben tener tipo entero o flotante." << endl;
						}
					}
					break;
				case MOD:
					if (tipo_izq != &tipo_int::instance()){
						tipo = &tipo_error::instance();
						cout << "Las expresiones aritmeticas deben tener tipo entero o flotante." << endl;
					}
					if (tipo_der != &tipo_int::instance() && tipo_der != &tipo_int::instance()){
						tipo = &tipo_error::instance();
						cout << "Las expresiones aritmeticas deben tener tipo entero o flotante." << endl;
					}
					break;
				case POW:
					if (tipo_izq != &tipo_int::instance() && tipo_izq != &tipo_float::instance() ||
						tipo_der != &tipo_int::instance() && tipo_der != &tipo_float::instance()){
						tipo = &tipo_error::instance();
						cout << "Las expresiones aritmeticas deben tener tipo entero." << endl;
					}
					if (tipo_der != &tipo_int::instance()){
						cout << tipo_der << " " << &tipo_int::instance() << " " << &tipo_float::instance() << endl;
						cout << "El exponente debe ser de tipo entero." << endl;
						tipo = &tipo_error::instance();
					}
					break;
			}
			if (tipo_der == &tipo_int::instance() && (tipo_izq == &tipo_int::instance() || izq == NULL)) {
				tipo = &tipo_int::instance();
			}
			else {
				tipo = &tipo_float::instance();
			}
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
				cout << "IZQUIERDA: " << endl;
				izq -> imprimir(tab+2);
			}
			if (der != NULL){
				for (int j = 0; j < tab+1; j++) cout << " ";
				cout << "DERECHA: " << endl;
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
		exp_booleana(ArbolSintactico * d, int is) : der(d), izq(NULL), instruccion(static_cast<inst>(is)) {verificar();}

		type * get_tipo(){
			return tipo;
		}
		void verificar(){
			type * tipo_der = der->get_tipo();
			type * tipo_izq = (izq!=NULL) ? izq->get_tipo(): &tipo_unit::instance();
			switch(instruccion){
				case NEGACION:
					if (!(tipo_der == &tipo_bool::instance())){
						tipo = &tipo_error::instance();
						cout << "Las expresiones boolenas deben ser de tipo booleno." << endl;
					}
					break;
				case DISYUNCION:
				case CONJUNCION:
					if (tipo_izq != &tipo_bool::instance() || tipo_der != &tipo_bool::instance()){
						tipo = &tipo_error::instance();
						cout << "Las expresiones boolenas deben ser de tipo booleno." << endl;
					}
					break;
				case IGUALA:
				case DISTINTO:
					if (tipo_izq != tipo_der){
						tipo = &tipo_error::instance();
						cout << "Las expresiones boolenas deben ser de tipo booleno." << endl;
					}
					break;
				case MAYOR:
				case MENORIGUAL:
				case MAYORIGUAL:
				case MENOR:
					if (tipo_izq != &tipo_int::instance() && tipo_izq != &tipo_float::instance()){
						cout << "Las expresiones aritmeticas debe ser de tipo entero o flotante." << endl;
						tipo = &tipo_error::instance();
					}
					if (tipo_der != &tipo_int::instance() && tipo_der != &tipo_float::instance()){
						tipo = &tipo_error::instance();
						cout << "Las expresiones aritmeticas debe ser de tipo entero o flotante." << endl;
					}
					break;
			}
			tipo = &tipo_bool::instance();
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
		virtual void verificar(){}
};


/* Definicion de la clase para las expresiones para indexaciones*/
class exp_index : public ArbolSintactico {
	public:
		ArbolSintactico * ind;
		ArbolSintactico * indexaciones;
		exp_index(ArbolSintactico * i,ArbolSintactico * ids) : ind(i), indexaciones(ids)  {verificar();}
		exp_index(ArbolSintactico * i) : ind(i), indexaciones(NULL) {verificar();}

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
		type * get_tipo(){
			return ind->get_tipo();
		}
		int get_valor(){
			return ind -> get_valor();
		}
		virtual void verificar(){
			if (ind->get_tipo() != &tipo_int::instance()) {
				cout << "La indexacion es de tipo entero." << endl;
			}
			if (indexaciones != NULL){
				indexaciones->verificar();
			}
		}
};


/* Definicion de la clase para los accesos a campos e identificadores en expresiones */
class ids : public ArbolSintactico {
	public:
		ArbolSintactico * id;
		ArbolSintactico * idr;
		ArbolSintactico * indx;
		ids(ArbolSintactico * i) : id(i), idr(NULL), indx(NULL), ArbolSintactico() {}
		ids(ArbolSintactico * i, ArbolSintactico * is) : id(i), idr(is), indx(NULL), ArbolSintactico() {}
		ids(ArbolSintactico * i, ArbolSintactico * is, ArbolSintactico * ix) : id(i), idr(is), indx(ix), ArbolSintactico() {verificar();}

		virtual void imprimir(int tab){
			if (idr != NULL){
				for (int j = 0; j < tab; j++) cout << " ";
				cout << "ACCESO: " << endl;
			}
			if (id != NULL){
				for (int j = 0; j < tab+1; j++) cout << " ";
				cout << "IDENTIFICADOR: " << endl;
				id -> imprimir(tab+2);
			}
			if (idr != NULL){
				for (int j = 0; j < tab+1; j++) cout << " ";
				cout << "CAMPO: " << endl;
				idr -> imprimir(tab+2);
			}
			if (indx != NULL){
				indx -> imprimir(tab+2);
			}
		}
		type * get_tipo(){
			if (tipo != NULL){
				return tipo;
			} 
			if (idr == NULL){
				tipo = id->get_tipo();
				if (indx != NULL){
					// Hay que sacar el tipo interno
					switch(tipo->tipo){
						case ARRAY:
							tipo = &((tipo_array *)tipo)->p1;
							break;
						case LIST:
							tipo = &((tipo_list *)tipo)->p1;
							break;
						case TUPLE: {
							int indice = indx->get_valor();
							if (indice == 0){
								tipo = &((tipo_tuple *)tipo)->p1;
							}
							else if (indice == 1) {
								tipo = &((tipo_tuple *)tipo)->p2;
							}
							else {
								cout << "Fuera del rango del tipo tupla." << endl;
								tipo = &tipo_error::instance();
							}
							break;
						}
					}
				} else {
					switch(tipo->tipo){
						case ARRAY:
							tipo = &((tipo_array *)tipo)->p1;
							break;
						case LIST:
							tipo = &((tipo_list *)tipo)->p1;
							break;
						case TUPLE:
							break;
					}
				}
			} else {
				tipo = idr->get_tipo();
			}
			return tipo;
		}
		virtual void verificar(){
			type * t;
			if (idr == NULL){
				t = id->get_tipo();
			} else {
				t = idr->get_tipo();
			}

			// La condicion no esta completa
			if (indx != NULL && 
					( t == &tipo_int::instance() ||
					  t == &tipo_bool::instance() || 
					  t == &tipo_float::instance() ||
					  t == &tipo_char::instance() ||
					  t == &tipo_tipo::instance() ||
					  // t == &tipo_pointer::instance() ||
					  // t == &tipo_unit::instance() ||
					  t == &tipo_error::instance() 
					  ) ){

				cout << "Este tipo no es indexable" << endl;
			}
		}
};


/* Definicion de la clase para int */
class entero : public ArbolSintactico {
	public:
		int valor;
		entero(): ArbolSintactico(&tipo_int::instance()) {is_type = 1;}
		entero(int v) : valor(v), ArbolSintactico(&tipo_int::instance()) {}
		
		virtual void imprimir(int tab) {
			if (!is_type){
				for (int j = 0; j < tab; j++) cout << " ";
				cout << "int: " << valor << endl;
			}
		}
		int get_valor(){
			return valor;
		}
		type * get_tipo(){
			return tipo;
		}
		virtual void verificar(){}
};


/* Definicion de la clase para float */
class flotante : public ArbolSintactico {
	public:
		float valor;
		flotante(): ArbolSintactico(&tipo_float::instance()) {is_type = 1;}
		flotante(float v) : valor(v), ArbolSintactico(&tipo_float::instance()) {}
		virtual void imprimir(int tab) {
			if (!is_type){
				for (int j = 0; j < tab; j++) cout << " ";
				cout << "float: " << valor << endl;
			}
		}
		type * get_tipo(){
			return tipo;
		}
		virtual void verificar(){}
};


/* Definicion de la clase para bool */
class booleano : public ArbolSintactico {
	public:
		bool valor;
		booleano(): ArbolSintactico(&tipo_bool::instance()) {is_type = 1;}
		booleano(bool v) : valor(v), ArbolSintactico(&tipo_bool::instance()) {}
		
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
		type * get_tipo(){
			return tipo;
		}
		virtual void verificar(){}
};


/* Definicion de la clase para char */
class character : public ArbolSintactico {
	public:
		char valor;
		character() : ArbolSintactico(&tipo_char::instance()) {is_type = 1;}
		character(char v) : valor(v), ArbolSintactico(&tipo_char::instance()) {}
		
		virtual void imprimir(int tab) {
			if (!is_type){
				for (int j = 0; j < tab; j++) cout << " ";
				cout << "char: " << valor << endl;
			}
		}
		type * get_tipo(){
			return tipo;
		}
		virtual void verificar(){}
};


/* Definicion de la clase para unit (tipo que no retorna nada) */
class unit : public ArbolSintactico {
	public:
		unit() : ArbolSintactico(&tipo_unit::instance()) {}
		
		virtual void imprimir(int tab){
			for (int j = 0; j < tab; j++) cout << " ";
			cout << "unit" << endl;
		}
		type * get_tipo(){
			return tipo;
		}
		virtual void verificar(){}
};


/* Definicion de la clase para string */
class str : public ArbolSintactico {
	public:
		string valor;
		str() : ArbolSintactico(&tipo_string::instance()) {is_type = 1;}
		str(string v) : valor(v), ArbolSintactico(&tipo_string::instance()) {}
		
		virtual void imprimir(int tab){
			for (int j = 0; j < tab; j++) cout << " ";
			cout << "string: " << valor << endl;
		}
		type * get_tipo(){
			return tipo;
		}
		virtual void verificar(){}
};


/* Definicion de la clase para la lista de parametros de una llamada a funcion */
class parametros : public ArbolSintactico {
	public:
		ArbolSintactico * elems;
		ArbolSintactico * val;
		parametros( ArbolSintactico * v) : val(v), elems(NULL) {}
		parametros(ArbolSintactico * e, ArbolSintactico * v) : elems(e), val(v) {verificar();}
		parametros() : elems(NULL), val(NULL) {}
	
		virtual void imprimir(int tab){
			if (elems != NULL){
				elems -> imprimir(tab);
			}
			if (val != NULL){
				for (int j = 0; j < tab; j++) cout << " ";
				cout << "PARAMETRO:" << endl;
				val -> imprimir(tab+1);
			}
		}
		int get_valor(){
			if (val != NULL){
				return val->get_valor();
			}
			return -1;	
		}
		type * get_tipo(){
			return val->get_tipo();
		}
		virtual void verificar(){
			if (val != NULL){
				val -> verificar();
			}
			if (elems != NULL){
				elems -> verificar();
			}
		}
};

/* Definicion de la clase para la lista de elementos de una lista o arreglo */
class elementos : public ArbolSintactico {
	public:
		ArbolSintactico * elems;
		ArbolSintactico * val;
		elementos( ArbolSintactico * v) : val(v), elems(NULL) {}
		elementos(ArbolSintactico * e, ArbolSintactico * v) : elems(e), val(v) {verificar();}
		elementos() : elems(NULL), val(NULL) {}
		
		virtual void imprimir(int tab){
			if (elems != NULL){
				elems -> imprimir(tab);
			}
			if (val != NULL){
				for (int j = 0; j < tab; j++) cout << " ";
				cout << "PARAMETRO:" << endl;
				val -> imprimir(tab+1);
			}
		}
		int get_valor(){
			if (val != NULL){
				return val->get_valor();
			}
			return -1;	
		}
		type * get_tipo(){
			if (tipo != NULL){
				return tipo;
			}
			if (elems == NULL){
				if (val != NULL){
					tipo = val->get_tipo();
				} else {
					tipo = &tipo_unit::instance();
				}
			} else{
				tipo = elems->get_tipo();
			}
			return tipo;
		}
		virtual void verificar(){
			if (val != NULL && elems != NULL){ 	
				type * tipo_val = val->get_tipo();
				type * tipo_elems = elems->get_tipo();
				if (tipo_val != tipo_elems) {
					cout << "Las listas y arreglos son homogeneos." << endl;
				}
			}
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
		type * get_tipo() {
			return valor->get_tipo();
		}
		virtual void verificar(){
			valor -> verificar();
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
		type * get_tipo() {
			return valor->get_tipo();
		}
		virtual void verificar(){
			valor -> verificar();
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
		virtual void verificar(){}
};