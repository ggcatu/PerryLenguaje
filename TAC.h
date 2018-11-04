#ifndef TAC_H
#define TAC_H
#include <stdio.h>
#include <iostream>
#include <string>
#include "ast.h"


using namespace std;

class TACNode {
	public:
		TACNode(): next(NULL) {};
		TACNode * next;
		virtual void output_code(){};	
};

class node_assign: public TACNode{
	string left; 
	string right;
	public:
		node_assign(string l, string r): TACNode(), left(l), right(r){};
		~node_assign(){};
		void output_code(){ cout << left << " := " << right << endl; };
};

class node_if: public TACNode{
	string condition;
	string label;
	public:
		node_if(string c, string l): TACNode(), condition(c), label(l){};
		~node_if(){};	
		void output_code(){ cout << "if (" << condition << ") go to " << label << endl; };
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
		void output_code(){ cout << "label: " << label << endl; };
};

class node_goto: public TACNode{
	string label;
	public:
		node_goto(string l): TACNode(), label(l){};
		~node_goto(){};	
		void output_code(){ cout << "goto " << label << endl; };
};

class node_call: public TACNode{
	string label;
	public:
		node_call(string l): TACNode(), label(l){};
		~node_call(){};	
		void output_code(){ cout << "call " << label << endl; };
};

class node_param: public TACNode{
	string label;
	public:
		node_param(string l): TACNode(), label(l){};
		~node_param(){};	
		void output_code(){ cout << "param " << label << endl; };
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
};

class node_return: public TACNode{
	string val;
	public:
		node_return(string v): TACNode(), val(v){};
		~node_return(){};	
		void output_code(){ cout << "return " << val << endl; };
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