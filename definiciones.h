/*
Traductores e Interpretadores (CI-3725)

Autores:
Guillermo Betancourt, carnet 11-10103
Gabriel Giménez, carnet 12-11006

definiciones.h:
Contiene un mapeo palabra -> integer para identificar los tipos de tokens, así como las firmas 
de las clases y funciones implementadas en definiciones.cpp
*/

#ifndef DEFINICIONES_H
#define DEFINICIONES_H

#include <string>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h> 
#include <cstring>
#include <vector>

using namespace std;

#define LLAVEABRE 1
#define LLAVECIERRA 2
#define CORCHETEABRE 3
#define CORCHETECIERRA 4
#define PARABRE 5
#define PARCIERRA 6

#define MAIN 7

#define COMENTARIOLINEA 8
#define COMENTARIO 9

#define INCLUDE 10
#define CREATE 11
#define EXECUTE 12
#define MODULE 13

#define PUNTOCOMA 14

#define WHILE 15
#define FOR 16
#define BREAK 17
#define IF 18
#define ELSE 19

#define STRUCT 20
#define UNION 21
#define ARRAY 22
#define LIST 23
#define TUPLE 24
#define LSTRING 25
#define LFLOAT 26
#define LCHAR 27
#define BOOL 28
#define LINT 29
#define POINTER 30

#define TRUE 31
#define FALSE 32

#define IGUALA 33
#define DISTINTOA 34

#define SUMA 35
#define RESTA 36
#define MOD 37
#define DIV 38
#define POW 39
#define MULT 40
#define IGUAL 41
#define MENORIGUAL 42
#define MENOR 43
#define MAYORIGUAL 44
#define MAYOR 45

#define NEGACION 46
#define CONJUNCION 47
#define DISYUNCION 48

#define SPACE 49
#define TAB 50
#define ENTER 51
#define COMA 52

#define INOUT 53
#define SALIDA 54
#define ENTRADA 55

#define PUNTO 56
#define CHAR 57
#define FLOAT 58
#define INT 59
#define IDENTIFIER 60
#define STRING 61

#define ERROR 62


extern string nToWord[];
string IntToString ( int number );

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
	TokenError(int x, int y, int z, string v);
	string to_string();
};

int lastOcurrence(char* s, char x);
int numOcurrence(char* s, char x, int start);
void print_vector(vector<Token *> vector);
void print_errors(vector<Token *> vector);

#endif