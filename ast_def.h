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
		string True;
		string False;
		ArbolSintactico(): first(NULL), tipo(NULL), is_type(0){};
		ArbolSintactico(type * t): first(NULL), tipo(t), is_type(0) {};
		ArbolSintactico(ArbolSintactico * l): tipo(NULL), first(l), is_type(0) {};
		virtual void imprimir(int i){ if(first != NULL) first->imprimir(i); };
		virtual void verificar(){};
		virtual type * get_tipo(){};
		virtual int get_valor(){};
		virtual string get_nombre(){};
		virtual type * get_tipo_index(type * t){};
		virtual void verificar_llamada(vector<type *> parametros, int actual) {};
		virtual string output_code(){ if (first) first->output_code(); return "";};
		virtual string jumping_code(ArbolSintactico * B){ return output_code(); };
		virtual string rvalue(){ return output_code(); };
		virtual string lvalue(){ return output_code(); };
		virtual std::vector<string> values(){ std::vector<string> val; return val; };
};

#endif