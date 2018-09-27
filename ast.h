/*
ast.cpp:

Contiene la definicion del arbol sintactico asi como la de la tabla de simbolos utilizadas
en el proyecto
*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <ctype.h>
#include <cstdlib>
#include <string>
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
extern char * yytext;
extern char error_strp[1000];
extern int yyparse();
extern bool error_sintactico;
extern vector<Token *> errors;
extern string tipo2word[300];

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
				errors.push_back(new TokenError(1,yylineno, yycolumn-1-strlen(yytext),"Los nombres de los archivos importados son cadenas de caracteres. STRING != "+tipo2word[tipo_archivo->tipo],VERIFICACION));
				error_sintactico = 1;
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
		}
		string get_nombre(){
			return valor;
		}
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

		bool asignar_tipo(string variable, sym_table * table){
			table_element * instancia = table->lookup(variable, -1);
			switch(tipo.tipo){
				case IDENTIFIER: {
					table_element * elemento = table->lookup(((identificador *)subtipo1)->valor, -1);
					if (elemento != NULL){
						instancia->set_child_scope(elemento->child_scope);
					} else {
						errors.push_back(new TokenError(1,yylineno, yycolumn-1-strlen(yytext),((identificador *)subtipo1)->valor+" no es un tipo",VERIFICACION));
						error_sintactico = 1;
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
		instruccion() : inst(NULL), instrucciones(NULL) {}
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
			string valor = ((identificador *)id)->valor;
			table_element * instancia = table.lookup(valor, -1);
			if (instancia){
				if (instancia->tipo->tipo == FUNC){
					vector<type *> params = instancia->tipo->parametros;
					parametros->verificar_llamada(params, 0);
				} else {
					string s = "\"" + valor + "\" no es una función";
					errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),s,VERIFICACION));
					error_sintactico = 1;
				}
			} else {
				tipo = &tipo_error::instance();
			}
		}
		type * get_tipo(){
			string valor = ((identificador *)id)->valor;
			table_element * instancia = table.lookup(valor, -1);
			if (instancia){
				if (instancia->tipo->tipo == FUNC){
					if (instancia->tipo->tipo == FUNC){
						tipo = &((tipo_funcion *)instancia->tipo)->p1;
					} else {
						string s = "\"" + valor + "\" no es una función";
						errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),s,VERIFICACION));
						error_sintactico = 1;
						tipo = &tipo_error::instance();
					}
				}
			} else {
				tipo = &tipo_error::instance();
			}
			return tipo;
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
			if (tipo_inicio != &tipo_int::instance() && tipo_inicio != &tipo_float::instance() && tipo_inicio != &tipo_error::instance()){
				errors.push_back(new TokenError(1,yylineno, yycolumn-1-strlen(yytext),"El inicio del rango del for debe ser de tipo entero o flotante. Se esperaba el tipo INT o el tipo float FLOAT y se tiene el tipo != "+tipo2word[tipo_inicio->tipo],VERIFICACION));
				tipo = &tipo_error::instance();
				error_sintactico = 1;
			}
			if (tipo_fin != &tipo_int::instance() && tipo_fin != &tipo_float::instance() && tipo_fin != &tipo_error::instance()){
				errors.push_back(new TokenError(1,yylineno, yycolumn-1-strlen(yytext),"El fin del rango del for debe ser de tipo entero o flotante. Se esperaba el tipo INT o el tipo float FLOAT y se tiene el tipo "+tipo2word[tipo_fin->tipo],VERIFICACION));
				tipo = &tipo_error::instance();
				error_sintactico = 1;
			}
			if (tipo_paso != &tipo_int::instance() && tipo_paso != &tipo_float::instance() && tipo_paso != &tipo_error::instance()){
				errors.push_back(new TokenError(1,yylineno, yycolumn-1-strlen(yytext),"El paso del for debe ser de tipo entero o flotante. Se esperaba el tipo INT o el tipo float FLOAT y se tiene el tipo "+tipo2word[tipo_paso->tipo],VERIFICACION));
				tipo = &tipo_error::instance();
				error_sintactico = 1;
			}
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
			if (condicion->get_tipo() != &tipo_bool::instance() && condicion->get_tipo() != &tipo_error::instance()){
				if (instruccion == IF){
					errors.push_back(new TokenError(1,yylineno, yycolumn-1-strlen(yytext),"La condición del if debe ser de tipo booleano. Se esperaba el tipo BOOL y se tiene el tipo "+tipo2word[condicion->get_tipo()->tipo],VERIFICACION));
				} 
				else if (instruccion == WHILE){
					errors.push_back(new TokenError(1,yylineno, yycolumn-1-strlen(yytext),"La condición del while debe ser de tipo booleano. Se esperaba el tipo BOOL y se tiene el tipo "+tipo2word[condicion->get_tipo()->tipo],VERIFICACION));
				}
				error_sintactico = 1;
				tipo = &tipo_error::instance();
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

			if (tipo_var != &tipo_error::instance() && tipo_val != &tipo_error::instance()){
				switch(tipo_var->tipo) {
					case IDENTIFIER:
						if (tipo_val->tipo != IDENTIFIER){
							string s = tipo2s("",tipo_var);
							s += " != ";
							s += tipo2s("",tipo_val);
							errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),s,ASIGNACION));
							error_sintactico = 1;
							tipo = &tipo_error::instance();
						} else {
							if (((tipo_identifier *)tipo_var)->p1 != ((tipo_identifier *)tipo_val)->p1){
								errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),((tipo_identifier *)tipo_var)->p1+" != "+((tipo_identifier *)tipo_val)->p1,ASIGNACION));
								error_sintactico = 1;
								tipo = &tipo_error::instance();
							}
						}
						break;
					case TYPE:
						if (tipo_val->tipo != TYPE){
							string s = tipo2s("",tipo_var);
							s += " != ";
							s += tipo2s("",tipo_val);
							errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),s,ASIGNACION));
							error_sintactico = 1;
							tipo = &tipo_error::instance();
						} else {
							if (((tipo_tipo *)tipo_var)->p1 != ((tipo_tipo *)tipo_val)->p1){
								errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),((tipo_tipo *)tipo_var)->p1+" != "+((tipo_tipo *)tipo_val)->p1,ASIGNACION));
								error_sintactico = 1;
								tipo = &tipo_error::instance();
							}
						}
						break;
					case TUPLE:
						/*
						if (tipo_val->tipo != TUPLE){
							string s = tipo2s("",tipo_var);
							s += " != ";
							s += tipo2s("",tipo_val);
							errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),s,ASIGNACION));
							error_sintactico = 1;
							tipo = &tipo_error::instance();
						} else {
							if (&((tipo_tuple *)tipo_var)->p1 != &((tipo_tuple *)tipo_val)->p1) {
								if (verificar_aux(&((tipo_tuple *)tipo_var)->p1,&((tipo_tuple *)tipo_val)->p1)){
									string s = tipo2s("",&((tipo_tuple *)tipo_var)->p1);
									s += " != ";
									s += tipo2s("",&((tipo_tuple *)tipo_val)->p1);
									errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),s,ASIGNACION));
									error_sintactico = 1;
									tipo = &tipo_error::instance();
								}
							}
							if (&((tipo_tuple *)tipo_var)->p2 != &((tipo_tuple *)tipo_val)->p2) {
								if (verificar_aux(&((tipo_tuple *)tipo_var)->p2,&((tipo_tuple *)tipo_val)->p2)){
									string s = tipo2s("",&((tipo_tuple *)tipo_var)->p2);
									s += " != ";
									s += tipo2s("",&((tipo_tuple *)tipo_val)->p2);
									errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),s,ASIGNACION));
									error_sintactico = 1;
									tipo = &tipo_error::instance();
								}
							}
						}
						*/
						break;
					case ARRAY:
						if (tipo_val->tipo != ARRAY){
							string s = tipo2s("",tipo_var);
							s += " != ";
							s += tipo2s("",tipo_val);
							errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),s,ASIGNACION));
							error_sintactico = 1;
						} else {
							if (&((tipo_array *)tipo_var)->p1 != &((tipo_array *)tipo_val)->p1) {
								if (verificar_aux(&((tipo_array *)tipo_var)->p1,&((tipo_array *)tipo_val)->p1)){
									string s = tipo2s("",&((tipo_array *)tipo_var)->p1);
									s += " != ";
									s += tipo2s("",&((tipo_array *)tipo_val)->p1);
									errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),s,ASIGNACION));
									error_sintactico = 1;
									tipo = &tipo_error::instance();
								}
							}
						}
						break;
					case LIST:
						if (tipo_val->tipo != LIST){
							string s = tipo2s("",tipo_var);
							s += " != ";
							s += tipo2s("",tipo_val);
							errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),s,ASIGNACION));
							error_sintactico = 1;
							tipo = &tipo_error::instance();
						} else {
							if (&((tipo_list *)tipo_var)->p1 != &((tipo_list *)tipo_val)->p1) {
								if (verificar_aux(&((tipo_list *)tipo_var)->p1,&((tipo_list *)tipo_val)->p1)){
									string s = tipo2s("",&((tipo_list *)tipo_var)->p1);
									s += " != ";
									s += tipo2s("",&((tipo_list *)tipo_val)->p1);
									errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),s,ASIGNACION));
									error_sintactico = 1;
									tipo = &tipo_error::instance();
								}
							}
						}
						break;
					default:
						if (tipo_val != tipo_var && tipo_val->tipo != TUPLE){
							if ((tipo_var != &tipo_float::instance() || tipo_val != &tipo_int::instance()) && (tipo_var != &tipo_string::instance() || tipo_val != &tipo_char::instance())){
								errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),tipo2word[tipo_var->tipo]+" != "+tipo2word[tipo_val->tipo],ASIGNACION));
								error_sintactico = 1;
								tipo = &tipo_error::instance();
							}
						}
					}
			}
		}
		virtual bool verificar_aux(type * tipo_var, type * tipo_val){
			if (tipo_val != 0){
				if (tipo_val->tipo == FUNC){
					cout << "hola soy una funcion" << endl;
					return true;
				}
				switch(tipo_var->tipo){
					case IDENTIFIER:
						if (tipo_val->tipo != IDENTIFIER){
							return true;
						} else {
							if (((tipo_identifier *)tipo_var)->p1 != ((tipo_identifier *)tipo_val)->p1){
								return true;
							}
						}
						return false;
					case TYPE:
						if (tipo_val->tipo != TYPE){
							return true;
						} else {
							if (((tipo_tipo *)tipo_var)->p1 != ((tipo_tipo *)tipo_val)->p1){
								return true;
							}
						}
						return false;
					case TUPLE:
						/*
						if (tipo_val->tipo != TUPLE){
							return true;
						} else {
							if (&((tipo_tuple *)tipo_var)->p1 != &((tipo_tuple *)tipo_val)->p1) {
								if (verificar_aux(&((tipo_tuple *)tipo_var)->p1,&((tipo_tuple *)tipo_val)->p1)){
									return true;
								}
							}
							if (&((tipo_tuple *)tipo_var)->p2 != &((tipo_tuple *)tipo_val)->p2) {
								if (verificar_aux(&((tipo_tuple *)tipo_var)->p2,&((tipo_tuple *)tipo_val)->p2)){
									return true;
								}
							}
						}*/
						return false;
					case ARRAY:
						if (tipo_val->tipo != ARRAY){
							return true;
						} else {
							if (&((tipo_array *)tipo_var)->p1 != &((tipo_array *)tipo_val)->p1) {
								if (verificar_aux(&((tipo_array *)tipo_var)->p1,&((tipo_array *)tipo_val)->p1)){
									return  true;
								}
							}
						}
						return false;
					case LIST:
						if (tipo_val->tipo != LIST){
							return true;
						} else {
							if (&((tipo_list *)tipo_var)->p1 != &((tipo_list *)tipo_val)->p1) {
								if (verificar_aux(&((tipo_list *)tipo_var)->p1,&((tipo_list *)tipo_val)->p1)){
									return true;
								}
							}
						}
						return false;
					default:
						if (tipo_val != tipo_var && tipo_val->tipo != TUPLE){
							if ((tipo_var != &tipo_float::instance() || tipo_val != &tipo_int::instance()) && (tipo_var != &tipo_string::instance() || tipo_val != &tipo_char::instance()) && tipo_val != &tipo_unit::instance()){
								return true;
							}
						}
						return false;
				}
			}
			return true;
		}
		string tipo2s(string s, type * tipo){
			s += tipo2word[tipo->tipo];
			switch(tipo->tipo){
				case TUPLE:
					s += "<";
					if ((((tipo_tuple *)tipo)->p1).tipo == 30 || (((tipo_tuple *)tipo)->p1).tipo == 29 || (((tipo_tuple *)tipo)->p1).tipo == 28 || (((tipo_tuple *)tipo)->p1).tipo == 27 || (((tipo_tuple *)tipo)->p1).tipo == 26){
						s += tipo2word[(((tipo_tuple *)tipo)->p1).tipo];
					} else {
						s += tipo2s(s,&((tipo_tuple *)tipo)->p1);
					}
					s += ",";
					if ((((tipo_tuple *)tipo)->p2).tipo == 30 || (((tipo_tuple *)tipo)->p2).tipo == 29 || (((tipo_tuple *)tipo)->p2).tipo == 28 || (((tipo_tuple *)tipo)->p2).tipo == 27 || (((tipo_tuple *)tipo)->p2).tipo == 26){
						s += tipo2word[(((tipo_tuple *)tipo)->p2).tipo];
					} else {
						s += tipo2s(s,&((tipo_tuple *)tipo)->p2);
					}
					s += ">";
					break;
				case LIST:
					s += "<";
					if ((((tipo_list *)tipo)->p1).tipo == 30 || (((tipo_list *)tipo)->p1).tipo == 29 || (((tipo_list *)tipo)->p1).tipo == 28 || (((tipo_list *)tipo)->p1).tipo == 27 || (((tipo_list *)tipo)->p1).tipo == 26){
						s += tipo2word[(((tipo_list *)tipo)->p1).tipo];
					} else {
						s += tipo2s(s,&((tipo_list *)tipo)->p1);
					}
					s += ">";
					break;
				case ARRAY:
					s += "<";
					if ((((tipo_array *)tipo)->p1).tipo == 30 || (((tipo_array *)tipo)->p1).tipo == 29 || (((tipo_array *)tipo)->p1).tipo == 28 || (((tipo_array *)tipo)->p1).tipo == 27 || (((tipo_array *)tipo)->p1).tipo == 26){
						s += tipo2word[(((tipo_array *)tipo)->p1).tipo];
					} else {
						s += tipo2s(s,&((tipo_array *)tipo)->p1);
					}
					s += ">";
					break;
				case POINTER:
					s += "<";
					if ((((tipo_pointer *)tipo)->p1).tipo == 30 || (((tipo_pointer *)tipo)->p1).tipo == 29 || (((tipo_pointer *)tipo)->p1).tipo == 28 || (((tipo_pointer *)tipo)->p1).tipo == 27 || (((tipo_pointer *)tipo)->p1).tipo == 26){
						s += tipo2word[(((tipo_pointer *)tipo)->p1).tipo];
					} else {
						s += tipo2s(s,&((tipo_pointer *)tipo)->p1);
					}
					s += ">";
					break;
				case IDENTIFIER:
				default:
					break;
			}
			return s;
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
			type * tipo_izq = (izq!=NULL) ? izq->get_tipo() : &tipo_unit::instance();
			if (tipo_der != &tipo_error::instance() && tipo_izq != &tipo_error::instance()){
				switch(instruccion){
					case SUMA:
					case MULT:
					case DIV:
						if (tipo_izq != &tipo_int::instance() && tipo_izq != &tipo_float::instance()){
							tipo = &tipo_error::instance();
							errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),tipo2word[tipo_izq->tipo],EXPARITMETICA));
							error_sintactico = 1;
						}
						if (tipo_der != &tipo_int::instance() && tipo_der != &tipo_float::instance() && tipo != &tipo_error::instance()){
							tipo = &tipo_error::instance();
							errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),tipo2word[tipo_der->tipo],EXPARITMETICA));
							error_sintactico = 1;
						}
						break;
					case RESTA:
						if (izq == NULL){
							if (tipo_der != &tipo_int::instance() && tipo_der != &tipo_float::instance()){
								tipo = &tipo_error::instance();
								errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),tipo2word[tipo_der->tipo],EXPARITMETICA));
								error_sintactico = 1;
							}
						} else{
							if (tipo_izq != &tipo_int::instance() && tipo_izq != &tipo_float::instance()){
								tipo = &tipo_error::instance();
								errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),tipo2word[tipo_izq->tipo],EXPARITMETICA));
								error_sintactico = 1;
							}
							if (tipo_der != &tipo_int::instance() && tipo_der != &tipo_float::instance() && tipo != &tipo_error::instance()){
								tipo = &tipo_error::instance();
								errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),tipo2word[tipo_der->tipo],EXPARITMETICA));
								error_sintactico = 1;
							}
						}
						break;
					case MOD:
						if (tipo_izq != &tipo_int::instance()){
							tipo = &tipo_error::instance();
							errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),"En la operación \"mod\" se espera el tipo INT y se tiene el tipo "+tipo2word[tipo_der->tipo],VERIFICACION));
							error_sintactico = 1;
						}
						if (tipo_der != &tipo_int::instance() && tipo != &tipo_error::instance()){
							tipo = &tipo_error::instance();
							errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),"En la operación \"mod\" se espera el tipo INT y se tiene el tipo "+tipo2word[tipo_der->tipo],VERIFICACION));
							error_sintactico = 1;
						}
						break;
					case POW:
						if (tipo_izq != &tipo_int::instance() && tipo_izq != &tipo_float::instance()){
							tipo = &tipo_error::instance();
							errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),tipo2word[tipo_izq->tipo],EXPARITMETICA));
							error_sintactico = 1;
						}
						if (tipo_der != &tipo_int::instance() && tipo_der != &tipo_float::instance() && tipo != &tipo_error::instance()){
							errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),tipo2word[tipo_der->tipo],EXPARITMETICA));
							tipo = &tipo_error::instance();
							error_sintactico = 1;
						}
						break;
				}
				if (tipo != &tipo_error::instance()){
					if (tipo_der == &tipo_int::instance() && (tipo_izq == &tipo_int::instance() || izq == NULL)) {
						tipo = &tipo_int::instance();
					}
					else {
						tipo = &tipo_float::instance();
					}
				}
			} else {
				tipo = &tipo_error::instance();
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
			if (tipo_der != &tipo_error::instance() && tipo_izq != &tipo_error::instance()){
				switch(instruccion){
					case NEGACION:
						if (tipo_der != &tipo_bool::instance()){
							tipo = &tipo_error::instance();
							errors.push_back(new TokenError(1,yylineno, yycolumn-1-strlen(yytext),tipo2word[tipo_der->tipo],EXPBOOLEANA));
							error_sintactico = 1;
						}
						break;
					case DISYUNCION:
					case CONJUNCION:
						if (tipo_izq != &tipo_bool::instance()){
							tipo = &tipo_error::instance();
							errors.push_back(new TokenError(1,yylineno, yycolumn-1-strlen(yytext),tipo2word[tipo_izq->tipo],EXPBOOLEANA));
							error_sintactico = 1;
						}
						if (tipo_der != &tipo_bool::instance() && tipo != &tipo_error::instance()){
							tipo = &tipo_error::instance();
							errors.push_back(new TokenError(1,yylineno, yycolumn-1-strlen(yytext),tipo2word[tipo_der->tipo],EXPBOOLEANA));
							error_sintactico = 1;
						}
						break;
					case IGUALA:
					case DISTINTO:
						if (verificar_aux(tipo_izq,tipo_der)){
							tipo = &tipo_error::instance();
							string s = tipo2s("",tipo_der);
							s += " != ";
							s += tipo2s("",tipo_izq);
							errors.push_back(new TokenError(1,yylineno, yycolumn-1-strlen(yytext),"Los tipos en los operadores \"==\" y \"!=\" deben ser iguales. "+s,VERIFICACION));
							error_sintactico = 1;
						}
						break;
					case MAYOR:
					case MENORIGUAL:
					case MAYORIGUAL:
					case MENOR:
						if (tipo_izq != &tipo_int::instance() && tipo_izq != &tipo_float::instance()){
							errors.push_back(new TokenError(1,yylineno, yycolumn-1-strlen(yytext),tipo2word[tipo_izq->tipo],EXPARITMETICA));
							tipo = &tipo_error::instance();
							error_sintactico = 1;
						}
						if (tipo_der != &tipo_int::instance() && tipo_der != &tipo_float::instance() && tipo != &tipo_error::instance()){
							tipo = &tipo_error::instance();
							errors.push_back(new TokenError(1,yylineno, yycolumn-1-strlen(yytext),tipo2word[tipo_der->tipo],EXPARITMETICA));
							error_sintactico = 1;
						}
						break;
				}
				if (tipo != &tipo_error::instance()){
					tipo = &tipo_bool::instance();
				}
			} else {
				tipo = &tipo_error::instance();
			}
		}
		virtual bool verificar_aux(type * tipo_var, type * tipo_val){
			if (tipo_val != 0){
				switch(tipo_var->tipo){
					case IDENTIFIER:
						if (tipo_val->tipo != IDENTIFIER){
							return true;
						} else {
							if (((tipo_identifier *)tipo_var)->p1 != ((tipo_identifier *)tipo_val)->p1){
								return true;
							}
						}
						return false;
					case TYPE:
						if (tipo_val->tipo != TYPE){
							return true;
						} else {
							if (((tipo_tipo *)tipo_var)->p1 != ((tipo_tipo *)tipo_val)->p1){
								return true;
							}
						}
						return false;
					case TUPLE:
						/*
						if (tipo_val->tipo != TUPLE){
							return true;
						} else {
							if (&((tipo_tuple *)tipo_var)->p1 != &((tipo_tuple *)tipo_val)->p1) {
								if (verificar_aux(&((tipo_tuple *)tipo_var)->p1,&((tipo_tuple *)tipo_val)->p1)){
									return true;
								}
							}
							if (&((tipo_tuple *)tipo_var)->p2 != &((tipo_tuple *)tipo_val)->p2) {
								if (verificar_aux(&((tipo_tuple *)tipo_var)->p2,&((tipo_tuple *)tipo_val)->p2)){
									return true;
								}
							}
						}
						*/
						return false;
					case ARRAY:
						if (tipo_val->tipo != ARRAY){
							return true;
						} else {
							if (&((tipo_array *)tipo_var)->p1 != &((tipo_array *)tipo_val)->p1) {
								if (verificar_aux(&((tipo_array *)tipo_var)->p1,&((tipo_array *)tipo_val)->p1)){
									return  true;
								}
							}
						}
						return false;
					case LIST:
						if (tipo_val->tipo != LIST){
							return true;
						} else {
							if (&((tipo_list *)tipo_var)->p1 != &((tipo_list *)tipo_val)->p1) {
								if (verificar_aux(&((tipo_list *)tipo_var)->p1,&((tipo_list *)tipo_val)->p1)){
									return true;
								}
							}
						}
						return false;
					default:
						if (tipo_val != tipo_var && tipo_val->tipo != TUPLE){
							if ((tipo_var != &tipo_float::instance() || tipo_val != &tipo_int::instance()) && (tipo_var != &tipo_string::instance() || tipo_val != &tipo_char::instance()) && tipo_val != &tipo_unit::instance()){
								return true;
							}
						}
						return false;
				}
			}
			return true;
		}
		string tipo2s(string s, type * tipo){
			s += tipo2word[tipo->tipo];
			switch(tipo->tipo){
				case TUPLE:
					s += "<";
					if ((((tipo_tuple *)tipo)->p1).tipo == 30 || (((tipo_tuple *)tipo)->p1).tipo == 29 || (((tipo_tuple *)tipo)->p1).tipo == 28 || (((tipo_tuple *)tipo)->p1).tipo == 27 || (((tipo_tuple *)tipo)->p1).tipo == 26){
						s += tipo2word[(((tipo_tuple *)tipo)->p1).tipo];
					} else {
						s += tipo2s(s,&((tipo_tuple *)tipo)->p1);
					}
					s += ",";
					if ((((tipo_tuple *)tipo)->p2).tipo == 30 || (((tipo_tuple *)tipo)->p2).tipo == 29 || (((tipo_tuple *)tipo)->p2).tipo == 28 || (((tipo_tuple *)tipo)->p2).tipo == 27 || (((tipo_tuple *)tipo)->p2).tipo == 26){
						s += tipo2word[(((tipo_tuple *)tipo)->p2).tipo];
					} else {
						s += tipo2s(s,&((tipo_tuple *)tipo)->p2);
					}
					s += ">";
					break;
				case LIST:
					s += "<";
					if ((((tipo_list *)tipo)->p1).tipo == 30 || (((tipo_list *)tipo)->p1).tipo == 29 || (((tipo_list *)tipo)->p1).tipo == 28 || (((tipo_list *)tipo)->p1).tipo == 27 || (((tipo_list *)tipo)->p1).tipo == 26){
						s += tipo2word[(((tipo_list *)tipo)->p1).tipo];
					} else {
						s += tipo2s(s,&((tipo_list *)tipo)->p1);
					}
					s += ">";
					break;
				case ARRAY:
					s += "<";
					if ((((tipo_array *)tipo)->p1).tipo == 30 || (((tipo_array *)tipo)->p1).tipo == 29 || (((tipo_array *)tipo)->p1).tipo == 28 || (((tipo_array *)tipo)->p1).tipo == 27 || (((tipo_array *)tipo)->p1).tipo == 26){
						s += tipo2word[(((tipo_array *)tipo)->p1).tipo];
					} else {
						s += tipo2s(s,&((tipo_array *)tipo)->p1);
					}
					s += ">";
					break;
				case POINTER:
					s += "<";
					if ((((tipo_pointer *)tipo)->p1).tipo == 30 || (((tipo_pointer *)tipo)->p1).tipo == 29 || (((tipo_pointer *)tipo)->p1).tipo == 28 || (((tipo_pointer *)tipo)->p1).tipo == 27 || (((tipo_pointer *)tipo)->p1).tipo == 26){
						s += tipo2word[(((tipo_pointer *)tipo)->p1).tipo];
					} else {
						s += tipo2s(s,&((tipo_pointer *)tipo)->p1);
					}
					s += ">";
					break;
				case IDENTIFIER:
				default:
					break;
			}
			return s;
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
		type * get_tipo(){
			type * tipo_der = der->get_tipo();
			if (tipo_der->tipo != POINTER){
				errors.push_back(new TokenError(1,yylineno, yycolumn-1-strlen(yytext),"Sólo se puede desreferenciar sobre apuntadores. Se esperaba el tipo POINTER y se tiene el tipo "+tipo2word[tipo_der->tipo],VERIFICACION));
				error_sintactico = 1;
				return &tipo_error::instance();
			}
			return &((tipo_pointer *)tipo_der)->p1;
		}
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
		type * get_tipo_index(type * t){
			if (indexaciones != NULL){
				switch(t->tipo){
					case ARRAY:
						return indexaciones->get_tipo_index(&((tipo_array *)t)->p1);
					case LIST:
						return indexaciones->get_tipo_index(&((tipo_list *)t)->p1);
					case TUPLE: {
						if (ind->get_tipo() != &tipo_int::instance()){
							errors.push_back(new TokenError(1,yylineno, yycolumn-1-strlen(yytext),"Sólo se puede indexar con enteros. Se esperaba el tipo INT y se tiene el tipo "+tipo2word[ind->get_tipo()->tipo],VERIFICACION));
							error_sintactico = 1;
							return &tipo_error::instance();
						}
					}
					default:
						return t;
				}
		}
		switch(t->tipo){
			case ARRAY:
				return &((tipo_array *)t)->p1;
			case LIST:
				return &((tipo_list *)t)->p1;
			case TUPLE: {
				if (ind->get_tipo() != &tipo_int::instance()){
					errors.push_back(new TokenError(1,yylineno, yycolumn-1-strlen(yytext),"Sólo se puede indexar con enteros. Se esperaba el tipo INT y se tiene el tipo "+tipo2word[ind->get_tipo()->tipo],VERIFICACION));
					error_sintactico = 1;
					return &tipo_error::instance();
				}
			}
			default:
				return t;
			}
		}
		virtual void verificar(){
			if (ind->get_tipo() != &tipo_int::instance()) {
				errors.push_back(new TokenError(1,yylineno, yycolumn-1-strlen(yytext),"Sólo se puede indexar con enteros. Se esperaba el tipo INT y se tiene el tipo "+tipo2word[ind->get_tipo()->tipo],VERIFICACION));
				error_sintactico = 1;
				tipo = &tipo_error::instance();
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
					tipo = indx->get_tipo_index(tipo);
				}
			} else {
				tipo = idr->get_tipo();
			}
			return tipo;
		}
		string get_nombre(){
			return id->get_nombre();
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
					  t->tipo == POINTER ||
					  t == &tipo_unit::instance() ||
					  t == &tipo_error::instance() 
					  )) {
				error_sintactico = 1;
				errors.push_back(new TokenError(1,yylineno, yycolumn-1-strlen(yytext),"El tipo "+ tipo2word[t->tipo]+" no es indexable",VERIFICACION));
				tipo = &tipo_error::instance();
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
		type * get_tipo(){
			return tipo;
		}
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
};


/* Definicion de la clase para la lista de parametros de una llamada a funcion */
class parametros : public ArbolSintactico {
	public:
		ArbolSintactico * elems;
		ArbolSintactico * val;
		parametros(ArbolSintactico * v) : val(v), elems(NULL) {}
		parametros(ArbolSintactico * v, ArbolSintactico * e) : val(e), elems(v) {verificar();}
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
		type * get_tipo(){
			return val->get_tipo();
		}
		virtual void verificar_llamada(vector<type *> parametros, int actual){
			int tam = parametros.size();
			if (val != NULL){
				// cout << 10 << "actual " << IntToString(actual) << "tam " << tam << endl;
				type * tipo_val = val->get_tipo();
				// cout << 11 << " tipo_param_llamada " << tipo_val->tipo << " tipo_param_vector " << &parametros[actual]->tipo << endl;
				if (verificar_aux(tipo_val,parametros[actual])){
					// cout << 12 << endl;
					string s = "Se esperaba que el argumento "+IntToString(actual)+" sea del tipo ";
					s += tipo2s("",parametros[actual]);
					s += " pero se tiene el tipo ";
					s += tipo2s("",tipo_val);
					errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),s,VERIFICACION));
					error_sintactico = 1;
				} else {
					if (elems != NULL){
						actual++;
						elems->verificar_llamada(parametros,actual);
					} else {
						if (actual != parametros.size()-1){
							string s = "Se esperaban "+IntToString(actual)+" argumentos y se tienen "+IntToString(tam);
							errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),s,VERIFICACION));
							error_sintactico = 1;
						}
					}
				}
			} else {
				if (tam != parametros.size()-1){
					string s = "Se esperaban "+IntToString(actual)+" argumentos y se tienen "+IntToString(tam);
					errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),s,VERIFICACION));
					error_sintactico = 1;
				}
			}			
		}
		virtual bool verificar_aux(type * tipo_var, type * tipo_val){
			// cout << "comparacion: tipo_param_llamada " << tipo_var->tipo << " tipo_param_vector " << tipo_val->tipo << endl;
			if (tipo_val != 0 && tipo_var != 0){
				switch(tipo_var->tipo){

					case IDENTIFIER:
						if (tipo_val->tipo != IDENTIFIER){
							return true;
						} else {
							if (((tipo_identifier *)tipo_var)->p1 != ((tipo_identifier *)tipo_val)->p1){
								return true;
							}
						}
						return false;
					case TYPE:
						if (tipo_val->tipo != TYPE){
							return true;
						} else {
							if (((tipo_tipo *)tipo_var)->p1 != ((tipo_tipo *)tipo_val)->p1){
								return true;
							}
						}
						return false;
					case TUPLE:
						/*
						if (tipo_val->tipo != TUPLE){
							return true;
						} else {
							if (&((tipo_tuple *)tipo_var)->p1 != &((tipo_tuple *)tipo_val)->p1) {
								if (verificar_aux(&((tipo_tuple *)tipo_var)->p1,&((tipo_tuple *)tipo_val)->p1)){
									return true;
								}
							}
							if (&((tipo_tuple *)tipo_var)->p2 != &((tipo_tuple *)tipo_val)->p2) {
								if (verificar_aux(&((tipo_tuple *)tipo_var)->p2,&((tipo_tuple *)tipo_val)->p2)){
									return true;
								}
							}
						}
						*/
						return false;
					case ARRAY:
						if (tipo_val->tipo != ARRAY){
							return true;
						} else {
							if (&((tipo_array *)tipo_var)->p1 != &((tipo_array *)tipo_val)->p1) {
								if (verificar_aux(&((tipo_array *)tipo_var)->p1,&((tipo_array *)tipo_val)->p1)){
									return  true;
								}
							}
						}
						return false;
					case LIST:
						if (tipo_val->tipo != LIST){
							return true;
						} else {
							if (&((tipo_list *)tipo_var)->p1 != &((tipo_list *)tipo_val)->p1) {
								if (verificar_aux(&((tipo_list *)tipo_var)->p1,&((tipo_list *)tipo_val)->p1)){
									return true;
								}
							}
						}
						return false;
					default:
						if (tipo_val->tipo != tipo_var->tipo && tipo_val->tipo != TUPLE){
							if ((tipo_var != &tipo_float::instance() || tipo_val != &tipo_int::instance()) && (tipo_var != &tipo_string::instance() || tipo_val != &tipo_char::instance()) && tipo_val != &tipo_unit::instance()){
								return true;
							}
						}
						return false;
				}
			}
			return true;
		}
		string tipo2s(string s, type * tipo){
			s += tipo2word[tipo->tipo];
			switch(tipo->tipo){
				case TUPLE:
					s += "<";
					if ((((tipo_tuple *)tipo)->p1).tipo == 30 || (((tipo_tuple *)tipo)->p1).tipo == 29 || (((tipo_tuple *)tipo)->p1).tipo == 28 || (((tipo_tuple *)tipo)->p1).tipo == 27 || (((tipo_tuple *)tipo)->p1).tipo == 26){
						s += tipo2word[(((tipo_tuple *)tipo)->p1).tipo];
					} else {
						s += tipo2s(s,&((tipo_tuple *)tipo)->p1);
					}
					s += ",";
					if ((((tipo_tuple *)tipo)->p2).tipo == 30 || (((tipo_tuple *)tipo)->p2).tipo == 29 || (((tipo_tuple *)tipo)->p2).tipo == 28 || (((tipo_tuple *)tipo)->p2).tipo == 27 || (((tipo_tuple *)tipo)->p2).tipo == 26){
						s += tipo2word[(((tipo_tuple *)tipo)->p2).tipo];
					} else {
						s += tipo2s(s,&((tipo_tuple *)tipo)->p2);
					}
					s += ">";
					break;
				case LIST:
					s += "<";
					if ((((tipo_list *)tipo)->p1).tipo == 30 || (((tipo_list *)tipo)->p1).tipo == 29 || (((tipo_list *)tipo)->p1).tipo == 28 || (((tipo_list *)tipo)->p1).tipo == 27 || (((tipo_list *)tipo)->p1).tipo == 26){
						s += tipo2word[(((tipo_list *)tipo)->p1).tipo];
					} else {
						s += tipo2s(s,&((tipo_list *)tipo)->p1);
					}
					s += ">";
					break;
				case ARRAY:
					s += "<";
					if ((((tipo_array *)tipo)->p1).tipo == 30 || (((tipo_array *)tipo)->p1).tipo == 29 || (((tipo_array *)tipo)->p1).tipo == 28 || (((tipo_array *)tipo)->p1).tipo == 27 || (((tipo_array *)tipo)->p1).tipo == 26){
						s += tipo2word[(((tipo_array *)tipo)->p1).tipo];
					} else {
						s += tipo2s(s,&((tipo_array *)tipo)->p1);
					}
					s += ">";
					break;
				case POINTER:
					s += "<";
					if ((((tipo_pointer *)tipo)->p1).tipo == 30 || (((tipo_pointer *)tipo)->p1).tipo == 29 || (((tipo_pointer *)tipo)->p1).tipo == 28 || (((tipo_pointer *)tipo)->p1).tipo == 27 || (((tipo_pointer *)tipo)->p1).tipo == 26){
						s += tipo2word[(((tipo_pointer *)tipo)->p1).tipo];
					} else {
						s += tipo2s(s,&((tipo_pointer *)tipo)->p1);
					}
					s += ">";
					break;
				case IDENTIFIER:
				default:
					break;
			}
			return s;
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
			type * tipo_var = val->get_tipo();
			type * tipo_val = elems->get_tipo();
			if (tipo_var != &tipo_error::instance() && tipo_val != &tipo_error::instance()){
				switch(tipo_var->tipo) {
					case IDENTIFIER:
						if (tipo_val->tipo != IDENTIFIER){
							string s = tipo2s("",tipo_var);
							s += " != ";
							s += tipo2s("",tipo_val);
							errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),s,ASIGNACION));
							error_sintactico = 1;
							tipo = &tipo_error::instance();
						} else {
							if (((tipo_identifier *)tipo_var)->p1 != ((tipo_identifier *)tipo_val)->p1){
								errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),((tipo_identifier *)tipo_var)->p1+" != "+((tipo_identifier *)tipo_val)->p1,ASIGNACION));
								error_sintactico = 1;
								tipo = &tipo_error::instance();
							}
						}
						break;
					case TYPE:
						if (tipo_val->tipo != TYPE){
							string s = tipo2s("",tipo_var);
							s += " != ";
							s += tipo2s("",tipo_val);
							errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),s,ASIGNACION));
							error_sintactico = 1;
							tipo = &tipo_error::instance();
						} else {
							if (((tipo_tipo *)tipo_var)->p1 != ((tipo_tipo *)tipo_val)->p1){
								errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),((tipo_tipo *)tipo_var)->p1+" != "+((tipo_tipo *)tipo_val)->p1,ASIGNACION));
								error_sintactico = 1;
								tipo = &tipo_error::instance();
							}
						}
						break;
					case TUPLE:
						/*
						if (tipo_val->tipo != TUPLE){
							string s = tipo2s("",tipo_var);
							s += " != ";
							s += tipo2s("",tipo_val);
							errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),s,ASIGNACION));
							error_sintactico = 1;
							tipo = &tipo_error::instance();
						} else {
							if (&((tipo_tuple *)tipo_var)->p1 != &((tipo_tuple *)tipo_val)->p1) {
								if (verificar_aux(&((tipo_tuple *)tipo_var)->p1,&((tipo_tuple *)tipo_val)->p1)){
									string s = tipo2s("",&((tipo_tuple *)tipo_var)->p1);
									s += " != ";
									s += tipo2s("",&((tipo_tuple *)tipo_val)->p1);
									errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),s,ASIGNACION));
									error_sintactico = 1;
									tipo = &tipo_error::instance();
								}
							}
							if (&((tipo_tuple *)tipo_var)->p2 != &((tipo_tuple *)tipo_val)->p2) {
								if (verificar_aux(&((tipo_tuple *)tipo_var)->p2,&((tipo_tuple *)tipo_val)->p2)){
									string s = tipo2s("",&((tipo_tuple *)tipo_var)->p2);
									s += " != ";
									s += tipo2s("",&((tipo_tuple *)tipo_val)->p2);
									errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),s,ASIGNACION));
									error_sintactico = 1;
									tipo = &tipo_error::instance();
								}
							}
						}
						*/
						break;
					case ARRAY:
						if (tipo_val->tipo != ARRAY){
							string s = tipo2s("",tipo_var);
							s += " != ";
							s += tipo2s("",tipo_val);
							errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),s,ASIGNACION));
							error_sintactico = 1;
						} else {
							if (&((tipo_array *)tipo_var)->p1 != &((tipo_array *)tipo_val)->p1) {
								if (verificar_aux(&((tipo_array *)tipo_var)->p1,&((tipo_array *)tipo_val)->p1)){
									string s = tipo2s("",&((tipo_array *)tipo_var)->p1);
									s += " != ";
									s += tipo2s("",&((tipo_array *)tipo_val)->p1);
									errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),s,ASIGNACION));
									error_sintactico = 1;
									tipo = &tipo_error::instance();
								}
							}
						}
						break;
					case LIST:
						if (tipo_val->tipo != LIST){
							string s = tipo2s("",tipo_var);
							s += " != ";
							s += tipo2s("",tipo_val);
							errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),s,ASIGNACION));
							error_sintactico = 1;
							tipo = &tipo_error::instance();
						} else {
							if (&((tipo_list *)tipo_var)->p1 != &((tipo_list *)tipo_val)->p1) {
								if (verificar_aux(&((tipo_list *)tipo_var)->p1,&((tipo_list *)tipo_val)->p1)){
									string s = tipo2s("",&((tipo_list *)tipo_var)->p1);
									s += " != ";
									s += tipo2s("",&((tipo_list *)tipo_val)->p1);
									errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),s,ASIGNACION));
									error_sintactico = 1;
									tipo = &tipo_error::instance();
								}
							}
						}
						break;
					default:
						if (tipo_val != tipo_var && tipo_val->tipo != TUPLE){
							if ((tipo_var != &tipo_float::instance() || tipo_val != &tipo_int::instance()) && (tipo_var != &tipo_string::instance() || tipo_val != &tipo_char::instance())){
								errors.push_back(new TokenError(1,yylineno,yycolumn-1-strlen(yytext),tipo2word[tipo_var->tipo]+" != "+tipo2word[tipo_val->tipo],ASIGNACION));
								error_sintactico = 1;
								tipo = &tipo_error::instance();
							}
						}
					}
			}
		}
		virtual bool verificar_aux(type * tipo_var, type * tipo_val){
			if (tipo_val != 0){
				switch(tipo_var->tipo){
					case IDENTIFIER:
						if (tipo_val->tipo != IDENTIFIER){
							return true;
						} else {
							if (((tipo_identifier *)tipo_var)->p1 != ((tipo_identifier *)tipo_val)->p1){
								return true;
							}
						}
						return false;
					case TYPE:
						if (tipo_val->tipo != TYPE){
							return true;
						} else {
							if (((tipo_tipo *)tipo_var)->p1 != ((tipo_tipo *)tipo_val)->p1){
								return true;
							}
						}
						return false;
					case TUPLE:
						/*
						if (tipo_val->tipo != TUPLE){
							return true;
						} else {
							if (&((tipo_tuple *)tipo_var)->p1 != &((tipo_tuple *)tipo_val)->p1) {
								if (verificar_aux(&((tipo_tuple *)tipo_var)->p1,&((tipo_tuple *)tipo_val)->p1)){
									return true;
								}
							}
							if (&((tipo_tuple *)tipo_var)->p2 != &((tipo_tuple *)tipo_val)->p2) {
								if (verificar_aux(&((tipo_tuple *)tipo_var)->p2,&((tipo_tuple *)tipo_val)->p2)){
									return true;
								}
							}
						}
						*/
						return false;
					case ARRAY:
						if (tipo_val->tipo != ARRAY){
							return true;
						} else {
							if (&((tipo_array *)tipo_var)->p1 != &((tipo_array *)tipo_val)->p1) {
								if (verificar_aux(&((tipo_array *)tipo_var)->p1,&((tipo_array *)tipo_val)->p1)){
									return  true;
								}
							}
						}
						return false;
					case LIST:
						if (tipo_val->tipo != LIST){
							return true;
						} else {
							if (&((tipo_list *)tipo_var)->p1 != &((tipo_list *)tipo_val)->p1) {
								if (verificar_aux(&((tipo_list *)tipo_var)->p1,&((tipo_list *)tipo_val)->p1)){
									return true;
								}
							}
						}
						return false;
					default:
						if (tipo_val != tipo_var && tipo_val->tipo != TUPLE){
							if ((tipo_var != &tipo_float::instance() || tipo_val != &tipo_int::instance()) && (tipo_var != &tipo_string::instance() || tipo_val != &tipo_char::instance()) && tipo_val != &tipo_unit::instance()){
								return true;
							}
						}
						return false;
				}
			}
			return true;
		}
		string tipo2s(string s, type * tipo){
			s += tipo2word[tipo->tipo];
			switch(tipo->tipo){
				case TUPLE:
					s += "<";
					if ((((tipo_tuple *)tipo)->p1).tipo == 30 || (((tipo_tuple *)tipo)->p1).tipo == 29 || (((tipo_tuple *)tipo)->p1).tipo == 28 || (((tipo_tuple *)tipo)->p1).tipo == 27 || (((tipo_tuple *)tipo)->p1).tipo == 26){
						s += tipo2word[(((tipo_tuple *)tipo)->p1).tipo];
					} else {
						s += tipo2s(s,&((tipo_tuple *)tipo)->p1);
					}
					s += ",";
					if ((((tipo_tuple *)tipo)->p2).tipo == 30 || (((tipo_tuple *)tipo)->p2).tipo == 29 || (((tipo_tuple *)tipo)->p2).tipo == 28 || (((tipo_tuple *)tipo)->p2).tipo == 27 || (((tipo_tuple *)tipo)->p2).tipo == 26){
						s += tipo2word[(((tipo_tuple *)tipo)->p2).tipo];
					} else {
						s += tipo2s(s,&((tipo_tuple *)tipo)->p2);
					}
					s += ">";
					break;
				case LIST:
					s += "<";
					if ((((tipo_list *)tipo)->p1).tipo == 30 || (((tipo_list *)tipo)->p1).tipo == 29 || (((tipo_list *)tipo)->p1).tipo == 28 || (((tipo_list *)tipo)->p1).tipo == 27 || (((tipo_list *)tipo)->p1).tipo == 26){
						s += tipo2word[(((tipo_list *)tipo)->p1).tipo];
					} else {
						s += tipo2s(s,&((tipo_list *)tipo)->p1);
					}
					s += ">";
					break;
				case ARRAY:
					s += "<";
					if ((((tipo_array *)tipo)->p1).tipo == 30 || (((tipo_array *)tipo)->p1).tipo == 29 || (((tipo_array *)tipo)->p1).tipo == 28 || (((tipo_array *)tipo)->p1).tipo == 27 || (((tipo_array *)tipo)->p1).tipo == 26){
						s += tipo2word[(((tipo_array *)tipo)->p1).tipo];
					} else {
						s += tipo2s(s,&((tipo_array *)tipo)->p1);
					}
					s += ">";
					break;
				case POINTER:
					s += "<";
					if ((((tipo_pointer *)tipo)->p1).tipo == 30 || (((tipo_pointer *)tipo)->p1).tipo == 29 || (((tipo_pointer *)tipo)->p1).tipo == 28 || (((tipo_pointer *)tipo)->p1).tipo == 27 || (((tipo_pointer *)tipo)->p1).tipo == 26){
						s += tipo2word[(((tipo_pointer *)tipo)->p1).tipo];
					} else {
						s += tipo2s(s,&((tipo_pointer *)tipo)->p1);
					}
					s += ">";
					break;
				case IDENTIFIER:
				default:
					break;
			}
			return s;
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
			if (valor != NULL){
				type * t = valor->get_tipo();
				if (t == &tipo_unit::instance()){
					tipo = &tipo_list::instance();
				} 
				else if (t == &tipo_error::instance()){
					tipo = &tipo_error::instance();
				}
				else {
					tipo = new tipo_list(*t);
				}
				return tipo;
			} else{
				tipo = &tipo_list::instance();
				return tipo;
			}
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
			if (valor != NULL){
				type * t = valor->get_tipo();
				if (t == &tipo_unit::instance()){
					tipo = &tipo_array::instance();
				} 
				else if (t == &tipo_error::instance()){
					tipo = &tipo_error::instance();
				}
				else {
					tipo = new tipo_array(*t);
				}
				return tipo;
			} else{
				tipo = &tipo_array::instance();
				return tipo;
			}
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
		type * get_tipo(){
			type * t1 = valor1->get_tipo();
			type * t2 = valor2->get_tipo();
			if (t1 != &tipo_error::instance() && t2 != &tipo_error::instance()){
				tipo = new tipo_tuple(*t1,*t2);	
			} else {
				tipo =  &tipo_error::instance();
			}
			return tipo;
		}
};
