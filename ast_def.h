#ifndef ARB_H
#define ARB_H
#include <stdio.h>
#include <iostream>
#include <string>
#include "tipos.h"

/* Definicion de la clase base ArbolSintactico */
class ArbolSintactico {
	ArbolSintactico * first;
	public:
		// int ident;
		int linea;
		bool is_type;
		type * tipo;
		ArbolSintactico(): tipo(NULL), is_type(0){};
		ArbolSintactico(type * t): tipo(t), is_type(0) {};
		ArbolSintactico(ArbolSintactico * l): tipo(NULL), first(l), is_type(0) {};
		virtual void imprimir(int i){ if(first != NULL) first->imprimir(i); }
		virtual void verificar() {}; 
		virtual type * get_tipo(){};
		virtual int get_valor(){};
		virtual string get_nombre(){};
};

#endif