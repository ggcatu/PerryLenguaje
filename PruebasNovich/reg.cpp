//Funcion que determina el numero de regitros

#include <vector>
#include <string>
#include <stdio.h>
#include <iostream>

//Cuenta las lineas de un archivo
int count_lines(FILE f);

lines = 10;

//Toma un archivo de codigo de 3 direcciones y retorna una matriz que guarda las variables usadas en cada 
void get_mat();

string[3][lines] m;

m = [["a","b","r"],["r","t1","c"],["a","c","t2"],["t2","b","d"],["a","d","m"],["c","b","n"],["m","b","a"],["p","m","t3"],["t3","t4","f"],["f","g","h"]]

//Toma una matriz resultado del codigo de 3 direcciones y cuenta sus variables
int count_vars();

n_vars = 15;

//toma unel nombre de una variable y devuelve su indice
int get_ind(string var); 

//toma unel indice de una variable y devuelve su nombre
string get_name(int ind); 

//Guarda que variables estan vivas en cada linea
vector<string>[ln] alives;
//Guarda cuando se usara proximamente una variable
int  [vars][ln] next_use;

int i = 0;
int j = 0;

//Se establecen los valores base de la matriz
while(i < n_vars){
	while(j < lines){
		next_use[i][j] = -1;
		j = j + 1
	}
	j = 0;
	i = i + 1;
}

//Funcion que dice si una variable esta lista en una linea
void find_alives(int ln, int vars){
	bool[vars] current_alive;
	int[vars] current_nu;
	i = 0;
	while(i<vars){
		current_alive[i] = false
		current_nu[i] = -1
	}
	i = ln - 1;
	int j;
	//Recorremos el codigo de abajo hacia arriba
	while(i >= 0){
		current_alive[get_ind(m[i][0])] = false; //La variable a la que se le asigna un valor ya no esta viva
		//Se colocan como vivas las variables usadas en la operacion
		if(m[i][1] != ""){ 
			alive[i].push_back(m[i][1]);
			current_alive[get_ind(m[i][1])] = true;
		}
		if(m[i][2] != ""){ 
			alive[i].push_back(m[i][2]);
			current_alive[get_ind(m[i][2])] = true;
		}
		j = 0;
		//Se agregan al vector las variables que estan vivas actualmente
		while(j < vars){
			if(current_alive[j]){
				alive[i].push_back(get_name(j));
			}
			j = j + 1
		}
		//Se asigna esta instruccion como proximo uso actual de las variables usadas
		current_nu[get_ind(m[i][1])] = i;
		current_nu[get_ind(m[i][2])] = i;
		//Se guardan los proximos usos de las variables utilizadas en la operacion
		next_use[get_ind(m[i][0])][i] = current_alive[get_ind(m[i][0])]
		next_use[get_ind(m[i][1])][i] = current_alive[get_ind(m[i][1])]
		next_use[get_ind(m[i][2])][i] = current_alive[get_ind(m[i][2])]
		i = i - 1;
	}
}

//Funcion que determina las aristas de el grafo basado en el algoritmo de variables vivas
void set_aristas(int ln){
	i = 0;
	//Vamos linea por linea buscando que variables estan vivas
	while(i < ln){
		//Para cada variable en la lista de variables vivas creamos una arista con las otras variables 
			//que estan en la lista
		for (vector<int>::iterator it = alive[i].begin() ; it != alive[i].end(); ++it){
			for (vector<int>::iterator it2 = alive[i].begin() ; it2 != alive[i].end(); ++it){
    			arista(*it,*it2);
			}
		}
	}
}