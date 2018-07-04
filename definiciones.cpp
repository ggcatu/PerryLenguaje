/*
Lenguajes de Programación II (CI-4721)


definiciones.cpp:
Contiene las definiciones de diversas funciones utilizadas en el código principal (scanner.cpp)
Así mismo contiene la definicion de las clases que se utilizaron para guardar los tokens.
*/

#include <string>
#include <iostream>
#include <map>
#include <sstream>
#include <stdio.h>
#include <stdlib.h> 
#include <cstring>
#include "definiciones.h"
#include "ast.h"
#include "parser.tab.h"

using namespace std;

/*

Este arreglo nos permitió diversas faciilidades durante el proceso de creación del código,
así como nos ayuda a detectar algunos caracteres, que queremos obviar Ej: Space, Tab, enter.
Por otro lado facilita bastante modificar el to_string de los tokens.

*/

string nToWord[300];
string tipo2word[100];

void init_strings(){
	nToWord[LLAVEABRE] = "LLAVEABRE"; 
	nToWord[LLAVECIERRA] = "LLAVECIERRA"; 
	nToWord[CORCHETEABRE] = "CORCHETEABRE"; 
	nToWord[CORCHETECIERRA] = "CORCHETECIERRA"; 
	nToWord[PARABRE] = "PARABRE"; 
	nToWord[PARCIERRA] = "PARCIERRA"; 
	nToWord[MAIN] = "MAIN"; 
	nToWord[INCLUDE] = "INCLUDE"; 
	nToWord[CREATE] = "CREATE"; 
	nToWord[EXECUTE] = "EXECUTE"; 
	nToWord[MODULE] = "MODULE"; 
	nToWord[PUNTOCOMA] = "PUNTOCOMA"; 
	nToWord[WHILE] = "WHILE"; 
	nToWord[FOR] = "FOR"; 
	nToWord[BREAK] = "BREAK"; 
	nToWord[IF] = "IF"; 
	nToWord[ELSE] = "ELSE"; 
	nToWord[RETURN] = "RETURN"; 
	nToWord[STRUCT] = "STRUCT"; 
	nToWord[UNION] = "UNION"; 
	nToWord[ARRAY] = "ARRAY"; 
	nToWord[LIST] = "LIST"; 
	nToWord[TUPLE] = "TUPLE"; 
	nToWord[LSTRING] = "LSTRING"; 
	nToWord[LFLOAT] = "LFLOAT"; 
	nToWord[LCHAR] = "LCHAR"; 
	nToWord[BOOL] = "BOOL"; 
	nToWord[LINT] = "LINT"; 
	nToWord[POINTER] = "POINTER"; 
	nToWord[UNIT] = "UNIT";
	nToWord[TRUE] = "TRUE"; 
	nToWord[FALSE] = "FALSE"; 
	nToWord[IGUALA] = "IGUALA"; 
	nToWord[DISTINTOA] = "DISTINTOA"; 
	nToWord[SUMA] = "SUMA"; 
	nToWord[RESTA] = "RESTA"; 
	nToWord[MOD] = "MOD"; 
	nToWord[DIV] = "DIV"; 
	nToWord[POW] = "POW"; 
	nToWord[MULT] = "MULT"; 
	nToWord[IGUAL] = "IGUAL"; 
	nToWord[MENORIGUAL] = "MENORIGUAL"; 
	nToWord[MENOR] = "MENOR"; 
	nToWord[MAYORIGUAL] = "MAYORIGUAL"; 
	nToWord[MAYOR] = "MAYOR"; 
	nToWord[NEGACION] = "NEGACION"; 
	nToWord[CONJUNCION] = "CONJUNCION"; 
	nToWord[DISYUNCION] = "DISYUNCION"; 
	nToWord[SPACE] = "SPACE"; 
	nToWord[TAB] = "TAB"; 
	nToWord[COMA] = "COMA"; 
	nToWord[SALIDA] = "SALIDA"; 
	nToWord[ENTRADA] = "ENTRADA"; 
	nToWord[IDENTIFIER] = "IDENTIFIER"; 
	nToWord[CHAR] = "CHAR"; 
	nToWord[FLOAT] = "FLOAT"; 
	nToWord[PUNTO] = "PUNTO"; 
	nToWord[INT] = "INT"; 
	nToWord[STRING] = "STRING"; 
	nToWord[ERROR] = "ERROR"; 
	nToWord[ENTER] = "ENTER"; 
	nToWord[COMENTARIO] = "COMENTARIO"; 
	nToWord[TYPE] = "TYPE"; 
	nToWord[NEW] = "NEW"; 
	nToWord[FREE] = "FREE"; 
	nToWord[REFERENCE] = "REFERENCE"; 
	nToWord[OPTR] = "OPTR"; 

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

// Magia negra sacada de internet, permite pasar de int a string
std::string IntToString (int number) {
  std::ostringstream oss;
  // Works just like cout
  oss << number;
  // Return the underlying string
  return oss.str();
}

std::string FloatToString (float number) {
  std::ostringstream oss;
  // Works just like cout
  oss << number;
  // Return the underlying string
  return oss.str();
}

// Clase base Token
Token::Token (int id, int y, int x) { 
		identificador = id;
		fila = y;
		columna = x;
	}

// Implementación de to_string para la clase base Token
string Token::to_string() {
		if (identificador == ENTER)
			return "\n";
		return "Tk" + nToWord[identificador] + " " + IntToString(fila) + " " + IntToString(columna);
}

// Implementación de clase TokenIdentificador
TokenIdentificador::TokenIdentificador (int x, int y, int z, string v) : Token(x,y,z) {
	valor = v;
}

// Implementación de to_string para la clase TokenIdentificador
string TokenIdentificador::to_string() {
	return "Tk" +  nToWord[identificador] + "(\"" + valor + "\")" + " " + IntToString(fila) + " " + IntToString(columna);
}

// Implementación de clase TokenInteger
TokenInteger::TokenInteger (int x, int y, int z, int v) : Token(x,y,z) {
	valor = v;
}

// Implementación de to_string para la clase TokenInteger
string TokenInteger::to_string() {
	return "Tk" +  nToWord[identificador] + "(" + IntToString(valor) + ")" + " " + IntToString(fila) + " " + IntToString(columna);
}

// Implementación de clase TokenCharacter
TokenCharacter::TokenCharacter (int x, int y, int z, char v) : Token(x,y,z) {
	valor = v;
}

// Implementación de to_string para la clase TokenCharacter
string TokenCharacter::to_string() {
	return "Tk" +  nToWord[identificador] + "(\'" + valor + "\')" + " " + IntToString(fila) + " " + IntToString(columna);
}


// Implementación de clase TokenCharacter
TokenFloat::TokenFloat (int x, int y, int z, float v) : Token(x,y,z) {
	valor = v;
}

// Implementación de to_string para la clase TokenFloat
string TokenFloat::to_string() {
	return "Tk" +  nToWord[identificador] + "(\'" + FloatToString(valor) + "\')" + " " + IntToString(fila) + " " + IntToString(columna);
}

// Implementación de clase TokenError
TokenError::TokenError (int x, int y, int z, string v, ErrorEnum t) : Token(x,y,z) {
	tipo = t;
	valor = v;
}

// Implementación de to_string para la clase TokenError
string TokenError::to_string() {
	switch(tipo){
		case LEXER:
			return "Error: Expresión inesperada \"" + valor + "\" en la fila " + IntToString(fila) + ", columna " + IntToString(columna); 
		case NODEFINICION:
			return "Error: Variable no definida \"" + valor + "\" en la fila " + IntToString(fila) + ", columna " + IntToString(columna); 
		case REDEFINICION:
			return "Error: Redefinicion de variable \"" + valor + "\" en la fila " + IntToString(fila) + ", columna " + IntToString(columna); 
		case PARSER:
			return "Error: Parseo \"" + valor + "\" en la fila " + IntToString(fila) + ", columna " + IntToString(columna); 
		case VERIFICACION:
			return "Error: " + valor + ". En la fila " + IntToString(fila) + ", columna " + IntToString(columna);
		case EXPARITMETICA:
			return "Error: En las expresiones aritméticas se espera el tipo INT o el tipo FLOAT y se tiene el tipo " + valor + ". En la fila " + IntToString(fila) + ", columna " + IntToString(columna);
		case EXPBOOLEANA:
			return "Error: En las expresiones booleanas se espera el tipo BOOL y se tiene el tipo " + valor + ". En la fila " + IntToString(fila) + ", columna " + IntToString(columna);
		case ASIGNACION:
			return "Error: Los tipos de la asignación no son iguales. " + valor + ". En la fila " + IntToString(fila) + ", columna " + IntToString(columna);
		case ELEMENTOS:
			return "Error: Los arreglos y listas son homogéneos. " + valor + ". En la fila " + IntToString(fila) + ", columna " + IntToString(columna);
	}
}

// Función encontrar el índice de la última ocurrencia de un caracter en un arreglo de caracteres.
int lastOcurrence(char* s, char x) {
	for (int i = 0 ; i < strlen(s); i++) {
		if (s[strlen(s)-(i+1)] == x) {
			return strlen(s)-(i+1);
		}
	}
	return -1;
}

// Función que cuenta el número ocurrencias de un caracter, en un arreglo de caracteres, dada una posicion inicial.
int numOcurrence(char* s, char x, int start) {
	if (start < strlen(s)) {
		int counter = 0;
		for (int i = 0 ; i < strlen(s); i++) {
			if (s[i] == x) {
				counter++;
			}
		}
		return counter;
	}
	return -1;
}

// Función que permite imprimir rápidamente un vector de Tokens
void print_vector(vector<Token *> vector){
	for(int i=0; i < vector.size(); i++){
		if (i + 1 < vector.size()) {
			if (vector[i]->identificador == ENTER){
				cout << vector[i] -> to_string();
			} else {
				cout << vector[i] -> to_string() << ", ";
			}
		} else {
			cout << vector[i] -> to_string();
		}
   		
	}
	cout << endl;
}

// Función que permite imprimir rápidamente un vector de TokensError
void print_errors(vector<Token *> vector) {
	for(int i=0; i < vector.size(); i++){
		cout << vector[i] -> to_string() << endl;
	}
}
