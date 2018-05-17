/*
ast.cpp:

Contiene la definicion del arbol sintactico asi como la de la tabla de simbolos utilizadas
en el proyecto
*/

#include <stdio.h>
#include <iostream>
#include <stdexcept>
#include <map>
#include <vector>
#include <ctype.h>
#include <cstdlib>
#include <string>
#include "arb.h"
using namespace std;
#define ENTEROS 1
#define FLOTANTES 3
#define BOOLEANOS 4
#define CHARACTERES 5
#define CONDICION 6

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
};

/* Definicion de la clase para una lista de funciones declaradas */
class funcion : public ArbolSintactico {
	public:
		ArbolSintactico * tipo;
		ArbolSintactico * 
		string id;
		ArbolSintactico * funciones;
};

class include : public ArbolSintactico {
	public:
		ArbolSintactico * includes;
		string archivo;
};

/* Definicion de la clase para los structs, union, type */
class estructura : public ArbolSintactico {
	public:
		ArbolSintactico * estructuras;
		string id;
		ArbolSintactico * campos;
};

/* Definicion de la clase para el main */
class programa : public ArbolSintactico {
	public:
		ArbolSintactico * programa;
		ArbolSintactico * sec;
};

/* Definicion de la clase para un bloque create-execute */
class bloque : public ArbolSintactico {
	public:
		ArbolSintactico * declaraciones;
		ArbolSintactico * instrucciones;
};

/* Definicion de la clase para la lista de declaraciones */
class declaracion : public ArbolSintactico {
	public:
		ArbolSintactico * tipo;
		string id;
		ArbolSintactico * exp;
		ArbolSintactico * declaraciones;
};

/* Definicion de la clase para la lista de instrucciones */
class instruccion : public ArbolSintactico {
	public:
		ArbolSintactico * instrucciones;
		ArbolSintactico * inst;
};

/* Definicion de la clase para la lista de parametros de una funcion */
class parametros : public ArbolSintactico {
	public:
		ArbolSintactico * parametros;
		bool ref;
		string id;
		ArbolSintactico * tipo;
};

/* Definicion de la clase para la llamada a una funcion */
class llamada_funcion : public ArbolSintactico {
	public:
		ArbolSintactico * parametros;
		ArbolSintactico * tipo;
		ArbolSintactico * cuerpo;
		string id;
};

/* Definicion de la clase para new y free */
class memoria : public ArbolSintactico {
	public:
		bool es_new;
		string id;
};

/* Definicion de la clase para la entrada y salida */
class entrada_salida : public ArbolSintactico {
	public:
		ArbolSintactico * str;
		bool * es_in;
};

/* Definicion de la clase para el for */
class it_determinada : public ArbolSintactico {
	public:
		ArbolSintactico * variable;
		ArbolSintactico * inicio;
		ArbolSintactico * fin;
		ArbolSintactico * paso;
};

/* Definicion de la clase para el while, if */
class inst_guardia : public ArbolSintactico {
	public:
		ArbolSintactico * condicion;
		ArbolSintactico * cuerpo;
		ArbolSintactico * cuerpo_else;
		bool iffi;
};

/* Definicion de la clase para las expresiones que retornan int o float */
class exp_aritmetica : public ArbolSintactico {
	public:
		ArbolSintactico * der;
		ArbolSintactico * izq;
};

/* Definicion de la clase para las expresiones que retornan booleandos */
class exp_booleana : public ArbolSintactico {
	public:
		ArbolSintactico * der;
		ArbolSintactico * izq;
};

/* Definicion de la clase para los identificadores */
class identificador : public ArbolSintactico {
	public:
		string valor;
};

/* Definicion de la clase para int */
class entero : public ArbolSintactico {
	public:
		int valor;
};

/* Definicion de la clase para float */
class flotante : public ArbolSintactico {
	public:
		float valor;
};

/* Definicion de la clase para bool */
class booleano : public ArbolSintactico {
	public:
		bool valor;
};

/* Definicion de la clase para char */
class character : public ArbolSintactico {
	public:
		char valor;
};

/* Definicion de la clase para string */
class str : public ArbolSintactico {
	public:
		string valor;
};

/* Definicion de la clase para ptr (apuntadores) */
class ptr : public ArbolSintactico {
	public:
		ArbolSintactico * tipo;
		string id;
		ArbolSintactico * elem; // atributo para lo que sea que apunta?
};

/* Definicion de la clase para list */
class lista : public ArbolSintactico {
	public:
		ArbolSintactico * tipo;
		string id;
		// falta atributo para guardar elementos de la lista
};

/* Definicion de la clase para array */
class arreglo : public ArbolSintactico {
	public:
		ArbolSintactico * tipo
		int * tam;
		string id;
		// falta atributo para guardar elementos del arreglo
};

/* Definicion de la clase para tuple */
class tupla : public ArbolSintactico {
	public:
		ArbolSintactico * tipo1;
		ArbolSintactico * tipo2;
		string id;
		ArbolSintactico * valor1;
		ArbolSintactico * valor2;
};