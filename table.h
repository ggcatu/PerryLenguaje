#ifndef TABLE_H
#define TABLE_H

#include <vector>
#include <deque>
#include <map>
#include <string>
#include <iostream>
#include <algorithm>
#include "parser.tab.h"
#include "tipos.h"

extern bool error_sintactico;

/* Elementos de la tabla de simbolos, despues se expandira */
class table_element {
	private:
		string tipo2word[100];
	public:
		std::string id;
		int scope;
		type * tipo;
		int child_scope;

		table_element(std::string i, int s): id(i), scope(s), child_scope(-1){};

		bool operator==(const table_element & rhs) const { return (this->scope == rhs.scope && this->id == rhs.id);}

		void set_type(type& t){
			tipo = &t;
		}

		void set_scope(int s){
			scope = s;
		}

		void set_child_scope(int s){
			child_scope = s;
		}

		void init_tipo2string(){
			tipo2word[STRUCT] = "STRUCT"; 
			tipo2word[UNION] = "UNION"; 
			tipo2word[ARRAY] = "ARRAY"; 
			tipo2word[LIST] = "LIST"; 
			tipo2word[TUPLE] = "TUPLE"; 
			tipo2word[LSTRING] = "STRING"; 
			tipo2word[LFLOAT] = "FLOAT"; 
			tipo2word[LCHAR] = "CHAR"; 
			tipo2word[BOOL] = "BOOL"; 
			tipo2word[LINT] = "INT"; 
			tipo2word[POINTER] = "POINTER"; 
			tipo2word[UNIT] = "UNIT";
			tipo2word[TYPE] = "TYPE";
		}

		void print(){
			init_tipo2string();
			std::cout << "SCOPE: " << scope; 
			if (tipo != NULL){
				std::cout << " TYPE: "; 
				print_tipo(tipo);
			}
			if (child_scope != -1)
				std::cout << ", CHILD SCOPE: " << child_scope;
		}

		void print_tipo(type * tipo){ 
			std::cout << tipo2word[tipo->tipo];
			switch(tipo->tipo){
				case TUPLE:
					std::cout << "<";
					if ((((tipo_tuple *)tipo)->p1).tipo == 30 || (((tipo_tuple *)tipo)->p1).tipo == 29 || (((tipo_tuple *)tipo)->p1).tipo == 28 || (((tipo_tuple *)tipo)->p1).tipo == 27 || (((tipo_tuple *)tipo)->p1).tipo == 26){
						std::cout << tipo2word[(((tipo_tuple *)tipo)->p1).tipo];
					} else {
						print_tipo(&((tipo_tuple *)tipo)->p1);
					}
					std:: cout << ",";
					if ((((tipo_tuple *)tipo)->p2).tipo == 30 || (((tipo_tuple *)tipo)->p2).tipo == 29 || (((tipo_tuple *)tipo)->p2).tipo == 28 || (((tipo_tuple *)tipo)->p2).tipo == 27 || (((tipo_tuple *)tipo)->p2).tipo == 26){
						std::cout << tipo2word[(((tipo_tuple *)tipo)->p2).tipo];
					} else {
						print_tipo(&((tipo_tuple *)tipo)->p2);
					}
					std::cout << ">";
					break;
				case LIST:
					std::cout << "<";
					if ((((tipo_list *)tipo)->p1).tipo == 30 || (((tipo_list *)tipo)->p1).tipo == 29 || (((tipo_list *)tipo)->p1).tipo == 28 || (((tipo_list *)tipo)->p1).tipo == 27 || (((tipo_list *)tipo)->p1).tipo == 26){
						std::cout << tipo2word[(((tipo_list *)tipo)->p1).tipo];
					} else {
						print_tipo(&((tipo_list *)tipo)->p1);
					}
					std::cout << ">";
					break;
				case ARRAY:
					std::cout << "<";
					if ((((tipo_array *)tipo)->p1).tipo == 30 || (((tipo_array *)tipo)->p1).tipo == 29 || (((tipo_array *)tipo)->p1).tipo == 28 || (((tipo_array *)tipo)->p1).tipo == 27 || (((tipo_array *)tipo)->p1).tipo == 26){
						std::cout << tipo2word[(((tipo_array *)tipo)->p1).tipo];
					} else {
						print_tipo(&((tipo_array *)tipo)->p1);
					}
					std::cout << ">";
					break;
				case POINTER:
					std::cout << "<";
					if ((((tipo_pointer *)tipo)->p1).tipo == 30 || (((tipo_pointer *)tipo)->p1).tipo == 29 || (((tipo_pointer *)tipo)->p1).tipo == 28 || (((tipo_pointer *)tipo)->p1).tipo == 27 || (((tipo_pointer *)tipo)->p1).tipo == 26){
						std::cout << tipo2word[(((tipo_pointer *)tipo)->p1).tipo];
					} else {
						print_tipo(&((tipo_pointer *)tipo)->p1);
					}
					std::cout << ">";
					break;
				case IDENTIFIER:
				default:
					break;
			}
		}
};


/* Definicion de la tabla de simbolos */
class sym_table {
	private:
		std::map<std::string, std::deque<table_element> > tabla;
		std::vector<int> stack;
		int last_scope;
	public:
		sym_table() : last_scope(0) {stack.push_back(last_scope);}

		int new_scope(){
			last_scope++;
			stack.push_back(last_scope);
			return last_scope;
		}

		void open_scope(std::string x){
			table_element * scope = lookup(x, -1);
			if (scope == NULL){
				std::cout << x << " esta variable no esta definida." << std::endl;
				stack.push_back(-1);
			}
			else if (scope->child_scope == -1){
				std::cout << x << " no es de un tipo complejo." << std::endl;
				error_sintactico = 1;
				stack.push_back(scope->child_scope);
			} else {	
				stack.push_back(scope->child_scope);
			}
		}

		void exit_scope(){
			if (!stack.empty())
			stack.pop_back();
		}

		table_element * lookup(std::string x, int scope){

			if ( tabla.find(x) == tabla.end() ) {
				return NULL;
			} 

			for (std::deque<table_element>::iterator vit = tabla[x].begin() ; vit != tabla[x].end(); vit++){
				if ( scope == -1  && (std::find(stack.begin(), stack.end(), vit->scope) != stack.end() ) ){
					return &(*vit);
				} 
				if ( scope != -1 && vit->scope == scope ){
					return &(*vit);
				}
			}
			return NULL;
		}

		table_element * lookup_top(std::string x){

			if ( tabla.find(x) == tabla.end() ) {
				return NULL;
			} 

			for (std::deque<table_element>::iterator vit = tabla[x].begin() ; vit != tabla[x].end(); vit++){
				if (vit->scope == stack.back()){
					return &(*vit);
				}
			}
			return NULL;
		}

		bool insert(std::string identifier){
			if(tabla.find(identifier) == tabla.end()){
				tabla[identifier];
			}
			if (lookup(identifier, stack.back()) != NULL){
				// std::cout << "La variable " << identifier << " ya esta declarada en el scope: " << stack.back() << std::endl;
				return false;	
			}  

			tabla[identifier].push_front(table_element(identifier, stack.back()));
			return true;
		}

		void print(){
			std::cout << std::endl << "Imprimiendo tabla de simbolos:" << std::endl; 
		    for(std::map<std::string, std::deque<table_element> >::iterator it = tabla.begin(); it != tabla.end(); ++it) {
		    	std::cout << "Variable: " << it->first << " [";
		    	for (std::deque<table_element>::iterator vit = it->second.begin() ; vit != it->second.end(); ++vit) {
					vit->print();
					if (vit+1 != it->second.end())
						cout << ", ";
		    	}
			    std::cout << "]" << std::endl;
		    }
		    /*
		    std::cout << std::endl << std::endl << "Imprimiendo tipos " << std::endl;
		    std::cout << "INT: " << &tipo_int::instance() << std::endl;
		    std::cout << "BOOL: " << &tipo_bool::instance() << std::endl;
		    std::cout << "FLOAT: " << &tipo_float::instance() << std::endl;
		    std::cout << "UNIT: " << &tipo_unit::instance() << std::endl;
		    std::cout << "CHAR: " << &tipo_char::instance() << std::endl;
		    std::cout << "STR: " << &tipo_string::instance() << std::endl;
		    std::cout << "ERROR: " << &tipo_error::instance() << std::endl;
		    */
		}
};

#endif