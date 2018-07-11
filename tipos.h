#ifndef TIPOS_H
#define TIPOS_H

#include <string>
#include <iostream>
#include <map>
#include <sstream>
#include <stdio.h>
#include <stdlib.h> 
#include <cstring>
#include <string>
#include "tokentype.h"

using namespace std;

class type {
	public:
		std::vector<type *> parametros;
		yytokentype tipo;
		type(yytokentype t){
			tipo = t;
		};
};

template <class T> class CRTP_type : public type {
	public:
		CRTP_type(yytokentype t) : type(t) {};

		static T&  instance(){
			static T instance = *(new T()); 
			return instance;
		};
};

class tipo_bool: public CRTP_type<tipo_bool>{
	friend class CRTP_type<tipo_bool>;
	tipo_bool() : CRTP_type(BOOL) {}; 
};

class tipo_char: public CRTP_type<tipo_char>{
	friend class CRTP_type<tipo_char>;
	tipo_char() : CRTP_type(LCHAR) {}; 
};

class tipo_string: public CRTP_type<tipo_string>{
	friend class CRTP_type<tipo_string>;
	tipo_string() : CRTP_type(LSTRING) {}; 
};

class tipo_int: public CRTP_type<tipo_int>{
	friend class CRTP_type<tipo_int>;
	tipo_int() : CRTP_type(LINT) {};
};

class tipo_float: public CRTP_type<tipo_float>{
	friend class CRTP_type<tipo_float>;
	tipo_float() : CRTP_type(LFLOAT) {}; 
};

class tipo_tuple: public CRTP_type<tipo_tuple>{
	public:
		type &p1;
		type &p2;
		tipo_tuple(type &p, type &r) : CRTP_type(TUPLE), p1(p), p2(r) {}; 
};

class tipo_identifier: public CRTP_type<tipo_identifier>{
	friend class CRTP_type<tipo_identifier>;
	public:	
		string p1;
		tipo_identifier(string p): CRTP_type(IDENTIFIER), p1(p) {};
		tipo_identifier() : CRTP_type(IDENTIFIER) {};
};

class tipo_pointer: public CRTP_type<tipo_pointer>{
	public:
		type &p1;
		tipo_pointer(type &p) : CRTP_type(POINTER), p1(p) {}; 
};

class tipo_tipo: public CRTP_type<tipo_tipo>{
	public:
		string p1;
		tipo_tipo(string p): CRTP_type(TYPE), p1(p) {};
		tipo_tipo() : CRTP_type(TYPE), p1("") {}; 
};

class tipo_unit: public CRTP_type<tipo_unit>{
	friend class CRTP_type<tipo_unit>;
	tipo_unit() : CRTP_type(UNIT) {}; 
};

class tipo_array: public CRTP_type<tipo_array>{
	public:
		type &p1;
		tipo_array(type &p) : CRTP_type(ARRAY), p1(p) {}; 
		tipo_array() : CRTP_type(ARRAY), p1(tipo_unit::instance()) {};
};

class tipo_list: public CRTP_type<tipo_list>{
	public:
		type &p1;
		tipo_list(type &p) : CRTP_type(LIST), p1(p) {};
		tipo_list() : CRTP_type(LIST), p1(tipo_unit::instance()) {};
};

class tipo_funcion: public CRTP_type<tipo_funcion>{
	public:
		type &p1;
		std::vector<type> parametros;
		tipo_funcion(type &p) : CRTP_type(FUNC), p1(p) {}; 
};

class tipo_error: public CRTP_type<tipo_error>{
	friend class CRTP_type<tipo_error>;
	tipo_error() : CRTP_type(TYPE_ERROR) {}; 

};

#endif