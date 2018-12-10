#ifndef TABLE_H
#define TABLE_H

#include <vector>
#include <deque>
#include <map>
#include <string>
#include <iostream>
#include <algorithm>

class type;

extern bool error_sintactico;

/* Elementos de la tabla de simbolos, despues se expandira */
class table_element {
	private:
		std::string tipo2word[300];
	public:
		std::string id;
		int scope;
		int offset;
		type * tipo;
		int child_scope;

		table_element(std::string i, int s);
		table_element(std::string i, int s, type * t);
		bool operator==(const table_element & rhs) const; 
		void set_type(type& t);
		void set_scope(int s);
		void set_child_scope(int s);
		void init_tipo2string();
		void print();
		void print_tipo(type * tipo);
};

class offset_table {
	private:
		std::map<int, std::vector<int> > table;	
	public:
		offset_table();
		~offset_table();
		void increase_offset(int scope, int value);
		void copy_value(); 
		void subs_value();
		void print();
		int get_offset(int scope);
		int get_malloc(int scope);
};


/* Definicion de la tabla de simbolos */
class sym_table {
	private:
		std::map<std::string, std::deque<table_element> > tabla;
		std::vector<int> stack;
		int last_scope;
	public:
		offset_table * off;
		sym_table();
		~sym_table();
		int new_scope();
		int current_scope();
		void open_scope(std::string x);
		void exit_scope();
		table_element * lookup(std::string x, int scope);
		table_element * lookup_top(std::string x);
		bool insert(table_element * ele);
		bool insert(std::string identifier, int scope, type * tipo);
		bool insert(std::string identifier);
		void print();
};


#endif