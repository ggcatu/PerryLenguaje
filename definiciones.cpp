/*
Traductores e Interpretadores (CI-3725)

Autores:
Guillermo Betancourt, carnet 11-10103
Gabriel Giménez, carnet 12-11006

definiciones.cpp:
Contiene las definiciones de diversas funciones utilizadas en el código principal (scanner.cpp)
Así mismo contiene la definicion de las clases que se utilizaron para guardar los tokens.
*/

#include <string>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h> 
#include <cstring>
#include "definiciones.h"

using namespace std;

/*

Este arreglo nos permitió diversas faciilidades durante el proceso de creación del código,
así como nos ayuda a detectar algunos caracteres, que queremos obviar Ej: Space, Tab, enter.
Por otro lado facilita bastante modificar el to_string de los tokens.

*/

string nToWord[] = {  
	"NuLL", "{", "}", "[", "]", "(", ")", 
	"main", "", "", "include", "create", "execute", "module", 
	";", "while", "for", "break", "if", "else", "struct", 
	"union", "array", "list", "tuple", "string", "float", 
	"char", "bool", "int", "ptr", "True", "False", "==", 
	"!=", "+", "-", "MOD", "/", "**", "*", "=", "<=", "<", 
	">=", ">", "!", "&&", "||", "", "", "ENTER", ",", "inout", "<<", ">>", ".",
	"CHAR", "FLOAT", "INT", "IDENTIFIER", "STRING", "ERROR" 
	};

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
TokenError::TokenError (int x, int y, int z, string v) : Token(x,y,z) {
	valor = v;
}

// Implementación de to_string para la clase TokenError
string TokenError::to_string() {
	return "Error: Expresión inesperada \"" + valor + "\" en la fila " + IntToString(fila) + ", columna " + IntToString(columna); 
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
