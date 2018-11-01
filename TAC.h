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

class TAC {
	TACNode* first;
	TACNode* last;
	TAC* data;
	public:
		TAC(): first(NULL), last(0){};
		~TAC(){
			TACNode * tmp = first;
			while (tmp){
				TACNode * next = tmp->next;
				delete(tmp); 
				tmp = next;
			}
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

		void output_code(){
			TACNode * tmp = first;
			while (tmp){
				tmp->output_code();
				tmp = tmp->next;
			}
		};
};
#endif