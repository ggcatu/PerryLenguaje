#ifndef TAC_H
#define TAC_H

#include <stdio.h>
#include <iostream>
#include <string>
#include "RegisterManager.h"
#include "Classes/TACObject.h"
#include "Classes/Sym_table.h"

extern RegisterManager regs;
extern sym_table table;
extern sym_table tac_simbolos;
extern vector<table_element *> vector_parametros;
extern vector<table_element *> vector_declaraciones;

using namespace std;

class TACNode {
	public:
		TACNode(): next(NULL) {};
		TACNode * next;
		virtual void output_code(){};	
		virtual void output_mips(){ return output_code(); };	
		
		void push(string reg, int w){
			cout << "\tsubu \t$sp, $sp, " << w << endl;
			if (reg != "")
				cout << "\tsw \t" << reg << ", ($sp)" << endl;
		}

		void pop(string reg, int w){
			if (reg != "")
				top(reg);
			cout << "\t" << "addu\t" "$sp, $sp, " << w << endl;
		}

		void top(string reg){
			cout << "\t" << "lw\t" << reg << ", ($sp)" << endl;
		}
};

class node_op: public TACNode{
	public:
		TACObject * left;
		TACObject * right;
		int op;
		node_op(TACObject * l): TACNode(), left(l), right(NULL){};
		node_op(TACObject * l, int o, TACObject * r): TACNode(), left(l), op(o), right(r){};
		~node_op(){free(left); free(right);};
};

class node_save_registers: public TACNode{
	public:
		node_save_registers(){};
		~node_save_registers(){};
		void output_mips(){
			cout << "# Saving used registers" << endl;
		}
};

class node_restore_registers: public TACNode{
	public:
		node_restore_registers(){};
		~node_restore_registers(){};

		void output_mips(){
			cout << "# Restoring used registers" << endl;
		}
};

class node_assign: public TACNode{
	string left; 
	// String right deberia desaparecer
	string right;
	node_op * right_node;
	public:
		node_assign(string l, string r): TACNode(), left(l), right(r), right_node(NULL){};
		node_assign(string l, node_op * r): TACNode(), left(l), right_node(r){};
		~node_assign(){free(right_node);};
		// void output_code(){ cout << left << " := " << right << endl; };
		void output_code(){
		if (right_node == NULL)
			 cout << left << " := " << right << endl;
		else
			if (right_node->right == NULL)
				cout << left << " := " << right_node->left->valor << endl; 
			else
				cout << left << " := " << right_node->left->valor << " " << right_node->op << " " << right_node->right->valor << endl; };


		string load_value(TACObject * r){
			if(r->t->tipo == IDENTIFIER){
				string x; regs.getReg(left);
			} else if (r->t->tipo == LINT) {

			}; 
		};
		void output_mips(){ 
			enum inst {SUMA, RESTA, MULT, DIV, MOD, POW};

			// Pide registros de mas
			// Codigo cancer para chequeos

			if (right_node != NULL ){
				string x = regs.getReg(left);
				string y = regs.getReg(right_node->left);
				string z = "$0";
				if (right_node->right == NULL){
					if (right_node->left->t->tipo == LINT){
						cout << "\t" << "li " << "\t" << x << ", " << y	 << endl;
					}
					else if (right_node->left->t->tipo == LFLOAT){
						cout << "\t" << "add.s " << "\t" << x << ", " << y << ", $0" << endl;
					} else if (right_node->left->t->tipo == FUNC){
						pop(x, 4);
					} else {
						// cout << "IDETIFICADOR" << right_node->left->valor << endl;
						cout << "\t" << "add " << "\t" <<  x << ", " << y << ", $0" << endl;
					}
				} else {
					z = regs.getReg(right_node->right->valor);
					switch(right_node->op){
						case SUMA:
							if (right_node->left->t->tipo == LINT && right_node->right->t->tipo == LINT){
								cout << "\t" << "add " << "\t" << x << ", " << right_node->left->valor << ", " << right_node->right->valor << endl;
							} else {
								cout << "\t" << "add " << "\t" << x << ", " << y << ", " << z << endl;
							}
							break;
						case RESTA:
							cout << "\t" << "sub " << "\t" << x << ", " << y << ", " << z << endl;
							break;
						case MULT:
							cout << "\t" << "mult " << "\t" << x << ", " << y << ", " << z << endl;
							break;
						case DIV:
							cout << "\t" << "div " << "\t" << x << ", " << y << ", " << z << endl;
							break;
						default:
							break;
					}
				}
			} else {
				cout << left << " := " << right << endl;
			}
	    }
};

class node_if: public TACNode{
	string condition;
	string label;
	public:
		node_if(string c, string l): TACNode(), condition(c), label(l){};
		~node_if(){};	
		void output_code(){ cout << "if (" << condition << ") go to " << label << endl; };
		void output_mips(){ 
			string x = regs.getReg(condition);
			cout << "\tbgtz \t" << x << ", " << label << endl; };
};

class node_for: public TACNode{
	string val1;
	string val2;
	string label;
	public:
		node_for(string v1, string v2, string l): TACNode(), val1(v1), val2(v2), label(l){};
		~node_for(){};	
		void output_code(){ cout << "if (" << val1 << " == " << val2 << ") go to " << label << endl; };
};

class node_label: public TACNode{
	string label;
	public:
		node_label(string l): TACNode(), label(l){};
		~node_label(){};	
		void output_code(){ cout << label << ": " << endl; };
};

class node_goto: public TACNode{
	string label;
	public:
		node_goto(string l): TACNode(), label(l){};
		~node_goto(){};	
		void output_code(){ cout << "goto " << label << endl; };
		void output_mips(){ cout << "\t" << "j " << label << endl; };
};

class node_call: public TACNode{
	string label;
	public:
		node_call(string l): TACNode(), label(l){};
		~node_call(){};	
		void output_code(){ cout << "call " << label << endl; };
		void output_mips(){ 
			cout << "\tjal \t" << label << endl; 
		};
};

class node_param: public TACNode{
	TACObject * obj;
	bool type_param;
	public:
		node_param(TACObject * o, bool t): TACNode(), obj(o), type_param(t) {};
		~node_param(){free(obj);};	
		void output_code(){ cout << "param " << obj->valor << endl; };
		void output_mips(){
			string x = regs.getReg(obj);
			// No es label, debe ser el registro asociado.
			push(x, 4);
		 };
};

class node_unparam: public TACNode{
	TACObject * obj;
	public:
		node_unparam(TACObject * o): TACNode(), obj(o){};
		~node_unparam(){free(obj);};	
		void output_code(){ cout << "unparam " << obj->valor << endl; };
		void output_mips(){
			// string x = regs.getReg(obj->valor);
			// No es label, debe ser el registro asociado.
			// cout << "\tlw   \t" << x << ", "<< obj->offset <<"($sp)" << endl;
		 };
};

class node_pop: public TACNode {
	TACObject * obj;
	public:
		node_pop(TACObject * o): TACNode(), obj(o){};
		~node_pop(){free(obj);};	
		void output_code(){ cout << "pop " << obj->valor << endl; };
		void output_mips(){
			pop("", 4);
		 };	
};

class node_elem: public TACNode{
	string val;
	public:
		node_elem(string v): TACNode(), val(v){};
		~node_elem(){};
		void output_code(){ cout << val; };
};

class node_store: public TACNode{
	string label;
	string value;
	public:
		node_store(string l, string v): TACNode(), label(l), value(v){};
		~node_store(){};	
		void output_code(){ cout << "store " << value << " as " << label << endl; };
		void output_mips(){ cout << "\t" <<  label << ": .asciiz  " << value << endl; };
};

class node_return: public TACNode{
	string val;
	int offset;
	public:
		node_return(string v, int o): TACNode(), val(v), offset(o){};
		~node_return(){};	
		void output_code(){ cout << "return " << val << endl; };
		void output_mips(){ 
			string x = regs.getReg(val);
			cout << "\tsw \t" << x << ", ($fp)" << endl;
			cout << "\tjr \t$ra" << endl;
		};
};

class node_break: public TACNode{
	//string label;
	public:
		//node_break(string v): TACNode(), val(v){};
		node_break(): TACNode() {};
		~node_break(){};	
		void output_code(){ cout << "break " << endl; };
};

class node_in: public TACNode{
	string val;
	public:
		node_in(string v): TACNode(), val(v) {};
		~node_in(){};	
		void output_code(){ 
			cout << "read " << val << endl; 
		};
};

class node_out: public TACNode{
	string val;
	public:
		node_out(string v): TACNode(), val(v) {};
		~node_out(){};	
		void output_code(){ 
			cout << "write " << val << endl; 
		};
		void output_mips(){
			cout << "\t" << "li	$v0, 4" << endl;
			cout << "\t" << "la \t$a0, " << val << endl;
			cout << "\t" << "syscall" << endl;
		}
};

class node_new: public TACNode{
	string val;
	public:
		node_new(string v): TACNode(), val(v) {};
		~node_new(){};	
		void output_code(){ 
			cout << "new " << val << endl; 
		};
};

class node_free: public TACNode{
	string val;
	public:
		node_free(string v): TACNode(), val(v) {};
		~node_free(){};	
		void output_code(){ 
			cout << "free " << val << endl; 
		};
};

class node_malloc: public TACNode{
	int val;
	public:
		node_malloc(int v): val(v){};
		~node_malloc();
		void output_mips(){
			int v = tac_simbolos.off->get_malloc(val);
			if (v != 0){
				cout << "# Reservando espacio de memoria de scope " << endl;
				cout << "sub $sp, $sp, " << v << endl;
			}
		}
};

class node_free_stack: public TACNode{
	int val;
	public:
		node_free_stack(int v): val(v){};
		~node_free_stack();
		void output_mips(){
			int v = tac_simbolos.off->get_malloc(val);
			if ( v != 0){
				cout << "# Liberando espacio de memoria de scope " << endl;
				cout << "add $sp, $sp, " << v << endl;
			}
		}
};

class node_array: public TACNode{
	vector<string> elems;
	string left;
	public:
		node_array(vector<string> e, string v): elems(e), left(v) {};
		~node_array();
		void output_code(){
			string x = regs.getReg(left);
			string y = regs.getFreeRegister(); // necesito un registro bien
			cout << "la " << x << " " << left << endl;
			for (unsigned i=0; i<elems.size(); i++){
				cout << "li " << y << ", " << elems[i] << endl;
				cout << "sw " << y << ", " << IntToString((i+1)*4) << "(" << x << ")" << endl;
			}
			cout << "mul " << x << ", " << x << ", " << "4" << endl; // deberia ir el length de la cosa
			cout << "move $a0, " << x << endl;
			cout << "li $v0, 9" << endl;
			cout << "syscall" << endl;
			cout << "sw $v0, " << left;
		}
		void output_mips(){
			output_code();
		}
};

class TAC {
	TACNode* first;
	TACNode* last;
	TAC* data;
	public:
		TAC(): first(NULL), last(NULL), data(NULL) {};
		~TAC(){
			TACNode * tmp = first;
			while (tmp){
				TACNode * next = tmp->next;
				delete(tmp); 
				tmp = next;
			}
			delete(data);
		};
		void add(TACNode * node){ 
			if (!first){
				first = node;
				last = node;
			} else {
				last->next = node;
				last = node;
			}
		}

		void add_data(TACNode * node){
			if (!data)
				data = new TAC();
			data->add(node);
		}

		void calculate_offsets(){
			int last_scope = -1;
			int father;
			for (std::vector<table_element *>::iterator i = vector_parametros.begin(); i != vector_parametros.end(); ++i) {
				tac_simbolos.insert( (*i) );
			}
			tac_simbolos.off->copy_value();

			std::sort(vector_declaraciones.begin(), vector_declaraciones.end(), compare_table_element);
			
			for (std::vector<table_element *>::iterator i = vector_declaraciones.begin(); i != vector_declaraciones.end(); ++i) {
				if ( (*i)->scope != last_scope ){
					father = table.scope_relation[(*i)->scope];
					if (father != 0){
						tac_simbolos.off->dual_increase_offset((*i)->scope, tac_simbolos.off->get_offset(father));
					}
				}
				last_scope = (*i)->scope;
				tac_simbolos.insert( (*i) );
			}
			tac_simbolos.off->subs_value();
			// tac_simbolos.off->print();
			// table.print_scope_relation();
		}	

		void output_mips(){
			// Calcular tabla de offsets
			calculate_offsets();
			cout << ".data" << endl;
			output_data();
			cout << ".text" << endl;
			output_text();			
		}

		void output_data(){
			if (data){
				data->output_mips();
			}
		}

		void output_text(){
			TACNode * tmp = first;
			while (tmp){
				tmp->output_mips();
				tmp = tmp->next;
			}			
			cout << "\tli \t$v0, 10\t\t # system call code for exit" << endl;
			cout << "\tsyscall" << endl;
			return;
		}

		void output_code(){
			TACNode * tmp = first;
			if (data){
				data->output_code();
			}
			while (tmp){
				tmp->output_code();
				tmp = tmp->next;
			}
		};
};
#endif