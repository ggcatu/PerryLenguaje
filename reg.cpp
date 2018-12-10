#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <ctype.h>
#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <functional>

//Elimina un substring de un string
void remove_sub(std::string & str, const std::string & sub){
	size_t pos = std::string::npos;
	while ((pos  = str.find(sub) )!= std::string::npos){
		str.erase(pos, sub.length());
	}
}

void split(const std::string& s, char c,std::vector<std::string>& v) {
	std::string::size_type i = 0;
	std::string::size_type j = s.find(c);
	while (j != std::string::npos) {
		v.push_back(s.substr(i, j-i));
		i = ++j;
		j = s.find(c, j);
		if (j == std::string::npos)
			v.push_back(s.substr(i, s.length()));
	}
}

int getreg(std::string var_n, std::string file, int spll){
    int lines = 0;
	std::ifstream infile("prueba");
	std::string line;
	int i = 0;
	int j = 0;
	//std::string keywrd[6];
	//keywrd = ["+","*","-","=","/","print"];
	std::vector<std::string> v;
	while (std::getline(infile, line)){
		lines = lines + 1;
	}
	std::ifstream newfile("prueba");
	std::string m[lines][3];
	while(j<lines){
		for (int y = 0; y < 3; ++y) {
			m[j][y] = "";
		}
		j = j + 1;
	}
	while (std::getline(newfile, line)){
		remove_sub(line,"*");
		remove_sub(line,"/");
		remove_sub(line,"=");
		remove_sub(line,"+");
		remove_sub(line,"-");
		remove_sub(line,"print");
		split(line,' ',v);
		j = 0;
		for (int y = 0; y < v.size(); ++y) {
			remove_sub(line," ");
			if(v[y] != ""){
				m[i][j] = v[y];
				j = j + 1;
				if(j == 3){
					j = 0;
				}
			}
		}
		j = 0;
		v.clear();
		i = i + 1;
	}
	j = 0;
	while(j<lines){
		for (int y = 0; y < 3; ++y) {
			if(std::find(v.begin(), v.end(), m[j][y]) == v.end()){
				v.push_back(m[j][y]);
			}
		}
		j = j + 1;
	}
	int n_vars = v.size() - 1;
	std::vector<std::string> alives[lines];
	bool current_alive[n_vars];
	int next_use[n_vars][lines];
	int current_nu[n_vars];
	i = 0;
	while(i<n_vars){
		current_alive[i] = false;
		current_nu[i] = -1;
		i = i + 1;
	}
	i = lines - 1;
	//Recorremos el codigo de abajo hacia arriba
	while(i >= 0){
		current_alive[std::distance(v.begin(),std::find(v.begin(), v.end(), m[i][0]))] = false; //La variable a la que se le asigna un valor ya no esta viva
		//Se colocan como vivas las variables usadas en la operacion
		if(m[i][1] != ""){ 
			alives[i].push_back(m[i][1]);
			current_alive[std::distance(v.begin(),std::find(v.begin(), v.end(), m[i][1]))] = true;
		}
		if(m[i][2] != ""){ 
			alives[i].push_back(m[i][2]);
			current_alive[std::distance(v.begin(),std::find(v.begin(), v.end(), m[i][2]))] = true;
		}
		j = 0;
		//Se agregan al vector las variables que estan vivas actualmente
		while(j < n_vars){
			if(current_alive[j]){
				alives[i].push_back(v[j]);
			}
			j = j + 1;
		}
		//Se asigna esta instruccion como proximo uso actual de las variables usadas
		current_nu[std::distance(v.begin(),std::find(v.begin(), v.end(), m[i][1]))] = i;
		current_nu[std::distance(v.begin(),std::find(v.begin(), v.end(), m[i][2]))] = i;
		//Se guardan los proximos usos de las variables utilizadas en la operacion
		next_use[std::distance(v.begin(),std::find(v.begin(), v.end(), m[i][0]))][i] = current_alive[std::distance(v.begin(),std::find(v.begin(), v.end(), m[i][0]))];
		next_use[std::distance(v.begin(),std::find(v.begin(), v.end(), m[i][1]))][i] = current_alive[std::distance(v.begin(),std::find(v.begin(), v.end(), m[i][1]))];
		next_use[std::distance(v.begin(),std::find(v.begin(), v.end(), m[i][2]))][i] = current_alive[std::distance(v.begin(),std::find(v.begin(), v.end(), m[i][2]))];
		i = i - 1;
	}
	i = 0;
	j = 0;
	int grafo[n_vars][n_vars];
	while(i<n_vars){
		j = 0;
		while(j<n_vars){
			grafo[i][j] = 0;

			j = j + 1;
		}
		i = i + 1;
	}
	//Determinamos las aristas de el grafo basado en el algoritmo de variables vivas
	i = 0;
	j = 0;
	//Vamos linea por linea buscando que variables estan vivas
	while(i < lines){
		for (int y = 0; y < alives[i].size(); ++y) {
			for (int z = 0; z < alives[i].size(); ++z) {
				grafo[std::distance(v.begin(),std::find(v.begin(), v.end(), alives[i][y]))][std::distance(v.begin(),std::find(v.begin(), v.end(), alives[i][z]))] = 1;
				grafo[std::distance(v.begin(),std::find(v.begin(), v.end(), alives[i][z]))][std::distance(v.begin(),std::find(v.begin(), v.end(), alives[i][y]))] = 1;
			}
		}
		i = i + 1;
	}
	//Chequeamos si el grafo es 19-coloreable
	std::vector< std::vector<int> > grafo_v;
	std::vector<int> dummy;
	i = 0;
	while(i<n_vars){
		j = 0;
		while(j<n_vars){
			dummy.push_back(grafo[i][j]);
			j = j + 1;
		}
		grafo_v.push_back(dummy);
		dummy.clear();
		i = i + 1;
	}
	i = 0;
	j = 0;
	int k = 0;
	int x = 0;
	int aristas;
	int n_col = 6;
	bool kcolor = false;
	while(kcolor){
		//Hay maximo n_var iteraciones en el ciclo
		while(i<n_vars){
			j = 0;
			//Para cada nodo
			while(j<grafo_v.size()){
				k = 0;
				aristas = 0;
				//Contamos las aristas del nodo
				while(k<grafo_v[j].size()){
					if(grafo_v[j][k] == 1){
						aristas = aristas + 1;
					}
					k = k + 1;
				}
				if(aristas<n_col){
					//Eliminamos el nodo
					for (int y = 0; y < grafo_v.size(); ++y) {
						grafo_v[y].erase(grafo_v[y].begin()+j);
					}
					grafo_v.erase(grafo_v.begin()+j);
				}
				j = j + 1;
			}
			i = i + 1;
		}
		kcolor = grafo_v.empty();
		if(!kcolor){
			n_col = n_col - 1;
		}
	}
	i = 0;
	bool spill[n_vars];
	while(i<n_vars){
		if(i<n_col){
			spill[i] = true;
		}
		else{
			spill[i] = false;
		}
		i = i + 1;
	}
	if(spll == 0){
		return std::distance(v.begin(),std::find(v.begin(), v.end(), var_n));
	}
	else{
		return spill[std::distance(v.begin(),std::find(v.begin(), v.end(), var_n))];
	}
}