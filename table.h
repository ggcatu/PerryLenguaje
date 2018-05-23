#ifndef TABLE_H
#define TABLE_H

#include <vector>
#include <deque>
#include <map>
#include <string>
#include <iostream>
#include <algorithm>

enum elem_tipes {VARIABLE, FUNCTION};

/* Elementos de la tabla de simbolos, despues se expandira */
class table_element {
	public:
		std::string id;
		int scope;
		elem_tipes type;

		table_element(std::string i, int s, elem_tipes e): id(i), scope(s), type(e){};

		bool operator==(const table_element & rhs) const { return (this->scope == rhs.scope && this->id == rhs.id);}

		void print(){
			std::cout << "ID: " << id << " , SCOPE: " << scope << " , TYPE: " << type << std::endl; 
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

		void new_scope(){
			last_scope++;
			stack.push_back(last_scope);
		}

		void exit_scope(){
			stack.pop_back();
		}

		int lookup(std::string x, int scope){

			if ( tabla.find(x) == tabla.end() ) {
				return -1;
			} 

			for (std::deque<table_element>::iterator vit = tabla[x].begin() ; vit != tabla[x].end(); vit++){
				if ( scope == -1  && (std::find(stack.begin(), stack.end(), vit->scope) != stack.end() ) ){
					return vit->scope;
				} 
				if ( scope != -1 && vit->scope == scope ){
					return scope;
				}
			}
			return -1;
		}

		bool insert(std::string identifier){
			if(tabla.find(identifier) == tabla.end()){
				tabla[identifier];
			}
			if (lookup(identifier, stack.back()) != -1){
				std::cout << "La variable " << identifier << " ya esta declarada en el scope: " << stack.back() << std::endl;
				return false;	
			}  

			tabla[identifier].push_back(table_element(identifier, stack.back(), VARIABLE));
			return true;
		}

		void print(){		
			std::cout << "Imprimiendo tabla de simbolos." << std::endl << std::endl; 
		    for(std::map<std::string, std::deque<table_element> >::iterator it = tabla.begin(); it != tabla.end(); ++it) {
		    	std::cout << "Variable " << it->first << std::endl;
		    	for (std::deque<table_element>::iterator vit = it->second.begin() ; vit != it->second.end(); ++vit)
					vit->print();
		    }
		}
};

#endif