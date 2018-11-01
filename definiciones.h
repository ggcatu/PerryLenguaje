/*
Lenguajes de Programación II (CI-4721)

definiciones.h:
Contiene un mapeo palabra -> integer para identificar los tipos de tokens, así como las firmas 
de las clases y funciones implementadas en definiciones.cpp
*/

#ifndef DEFINICIONES_H
#define DEFINICIONES_H

#include <string>
#include <iostream>
#include <map>
#include <sstream>
#include <stdio.h>
#include <stdlib.h> 
#include <cstring>
#include <vector>

using namespace std;

enum ErrorEnum {LEXER, REDEFINICION, NODEFINICION, PARSER, VERIFICACION, EXPARITMETICA, EXPBOOLEANA, ASIGNACION, ELEMENTOS};

extern string nToWord[];
extern string tipos2word[];
string IntToString ( int number );
string FloatToString (float number);
string new_uuid();
string new_label();

struct Token{
	int identificador,fila,columna;
	Token (int x, int y, int z);
	virtual string to_string();
};

struct TokenIdentificador:Token {
	string valor;
	TokenIdentificador (int x, int y, int z, string v);
	string to_string();
};

struct TokenInteger:Token {
	int valor;
	TokenInteger (int x, int y, int z, int v);
	string to_string();
};

struct TokenFloat:Token {
	float valor;
	TokenFloat (int x, int y, int z, float v);
	string to_string();
};

struct TokenCharacter:Token {
	char valor;
	TokenCharacter (int x, int y, int z, char v);
	string to_string();
};

struct TokenError:Token{
	string valor;
	ErrorEnum tipo;
	TokenError(int x, int y, int z, string v, ErrorEnum t);
	string to_string();
};

int lastOcurrence(char* s, char x);
int numOcurrence(char* s, char x, int start);
void print_vector(vector<Token *> vector);
void print_errors(vector<Token *> vector);
void init_strings();
#endif