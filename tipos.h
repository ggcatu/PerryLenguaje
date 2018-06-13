#ifndef TIPOS_H
#define TIPOS_H

#include <iostream>
#include "parser.tab.h"

using namespace std;

class type {
	public:
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
		}	
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

class tipo_array: public CRTP_type<tipo_array>{
	friend class CRTP_type<tipo_array>;
	tipo_array() : CRTP_type(ARRAY) {}; 
};

class tipo_list: public CRTP_type<tipo_list>{
	friend class CRTP_type<tipo_list>;
	tipo_list() : CRTP_type(LIST) {}; 
};

class tipo_tuple: public CRTP_type<tipo_tuple>{
	friend class CRTP_type<tipo_tuple>;
	tipo_tuple() : CRTP_type(TUPLE) {}; 
};

class tipo_identifier: public CRTP_type<tipo_identifier>{
	friend class CRTP_type<tipo_identifier>;
	tipo_identifier() : CRTP_type(IDENTIFIER) {}; 
};

class tipo_pointer: public CRTP_type<tipo_pointer>{
	friend class CRTP_type<tipo_pointer>;
	tipo_pointer() : CRTP_type(POINTER) {}; 
};

class tipo_unit: public CRTP_type<tipo_unit>{
	friend class CRTP_type<tipo_unit>;
	tipo_unit() : CRTP_type(UNIT) {}; 
};

class tipo_funcion: public CRTP_type<tipo_funcion>{
	friend class CRTP_type<tipo_funcion>;
	tipo_funcion() : CRTP_type(UNIT) {}; 

};

#endif