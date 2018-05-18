#ifndef ARB_H
#define ARB_H
#include <stdio.h>
#include <iostream>

/* Definicion de la clase base ArbolSintactico */
class ArbolSintactico {
	ArbolSintactico * first;
	public:
		int ident;
		int linea;
		bool is_type;
		ArbolSintactico(): is_type(0){};
		ArbolSintactico(int i): ident(i),is_type(0) {};
		ArbolSintactico(ArbolSintactico * l): first(l),is_type(0) {};
		virtual void imprimir(int i){ if(first != NULL) first->imprimir(i); }; 
		virtual int get_ident(){ return ident; }
		virtual void ejecutar(){if(first != NULL) first->ejecutar();}
		virtual void check(){;}
		virtual int * get_value(){;}
		virtual bool * get_bool(){;}
		virtual char * get_character(){;}
};

#endif