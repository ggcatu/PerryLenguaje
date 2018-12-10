#include <vector>
#include <deque>
#include <map>
#include <string>
#include <iostream>
#include <algorithm>
#include "Sym_table.h"
// #include "../parser.tab.h"
#include "../tipos.h"

extern bool error_sintactico;

std::vector<table_element *> vector_parametros;
std::vector<table_element *> vector_declaraciones;

table_element::table_element(std::string i, int s): id(i), scope(s), tipo(NULL), child_scope(-1), offset(0){};
table_element::table_element(std::string i, int s, type * t): id(i), scope(s), tipo(t), child_scope(-1), offset(0){};

bool table_element::operator==(const table_element & rhs) const { return (this->scope == rhs.scope && this->id == rhs.id);}

void table_element::set_type(type& t){
	tipo = &t;
}

void table_element::set_scope(int s){
	scope = s;
}

void table_element::set_child_scope(int s){
	child_scope = s;
}

void table_element::init_tipo2string(){
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
	tipo2word[IDENTIFIER] = "TYPE";
	tipo2word[FUNC] = "FUNCION";
	tipo2word[TYPE_ERROR] = "ERROR";
}

void table_element::print(){
	init_tipo2string();
	std::cout << "SCOPE: " << scope; 
	if (tipo != NULL){
		std::cout << " TYPE: "; 
		print_tipo(tipo);
	}
	if (child_scope != -1)
		std::cout << ", CHILD SCOPE: " << child_scope;
	
	std::cout << ", OFFSET: " << offset;
}

void table_element::print_tipo(type * tipo){ 
	std::cout << tipo2word[tipo->tipo];
	switch(tipo->tipo){
		case FUNC:
			std::cout << "<";
			for (std::vector<type *>::iterator i = tipo->parametros.begin(); i != tipo->parametros.end(); ++i){
				print_tipo(*i);
				std::vector<type *>::iterator j = i;
				if (++j != tipo->parametros.end()){
					std::cout << ",";
				}
			}
			std::cout << ">";
			break;
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
		default:
			break;
	}
};


/* Definicion de la tabla de simbolos */
sym_table::sym_table() : last_scope(0), off(new offset_table()) {stack.push_back(last_scope);}
sym_table::~sym_table(){delete(off);}

int sym_table::new_scope(){
	last_scope++;
	stack.push_back(last_scope);
	return last_scope;
}

int sym_table::current_scope(){
	return stack.back();
}

void sym_table::open_scope(std::string x){
	table_element * scope = lookup(x, -1);
	if (scope == NULL){
		std::cout << "La variable \"" << x << "\" no está definida." << std::endl;
		error_sintactico = 1;
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

void sym_table::exit_scope(){
	if (!stack.empty())
		stack.pop_back();
}

table_element * sym_table::lookup(std::string x, int scope){

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

table_element * sym_table::lookup_top(std::string x){

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

bool sym_table::insert(table_element * ele){
	stringstream ss;
	ss << ele->id << "#" << ele->scope;
	string id = ss.str();
	if(tabla.find(id) == tabla.end()){
		tabla[id];
	}
	table_element tmp = table_element(id, ele->scope, ele->tipo);

	tmp.offset = off->get_offset(ele->scope);
	off->increase_offset(ele->scope, ele->tipo->size());
	
 	tabla[id].push_front(tmp);
	return true;
}

bool sym_table::insert(std::string identifier, int scope, type * tipo){
	if(tabla.find(identifier) == tabla.end()){
		tabla[identifier];
	}
	if (lookup(identifier, scope) != NULL){
		// std::cout << "La variable " << identifier << " ya esta declarada en el scope: " << scope << std::endl;
		return false;	
	}  
	table_element tmp = table_element(identifier, scope, tipo);

	tmp.offset = off->get_offset(scope);
	off->increase_offset(scope, tipo->size());
	
 	tabla[identifier].push_front(tmp);
	return true;
}

bool sym_table::insert(std::string identifier){
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

void sym_table::print(){
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
    
}

offset_table::offset_table(){};
offset_table::~offset_table(){};

void offset_table::increase_offset(int scope, int value){
	table[scope][0] += value;
	return;
};

int offset_table::get_offset(int scope){
	if(table.find(scope) == table.end()){
		table[scope];
		table[scope].push_back(0);
		table[scope].push_back(0);
	}
	return table[scope][0];
};

void offset_table::copy_value(){
	std::map<int, std::vector<int> >::iterator it;	
	for ( it = table.begin(); it != table.end(); it++ ) {
		it->second[1] = it->second[0];
	}
	return;
};

void offset_table::print(){

	std::map<int, std::vector<int> >::iterator it;	
	for ( it = table.begin(); it != table.end(); it++ ) {
		cout << it->first << " : (" << it->second[0] << ", " << it->second[1]<<")"<< endl;
	}
	return;
};

void offset_table::subs_value(){
	std::map<int, std::vector<int> >::iterator it;	
	for ( it = table.begin(); it != table.end(); it++ ) {
		it->second[1] = it->second[0] - it->second[1];
	}
	return;
};

int offset_table::get_malloc(int scope){
	if(table.find(scope) == table.end()){
		table[scope];
		table[scope].push_back(0);
		table[scope].push_back(0);
	}
	return table[scope][1];
};