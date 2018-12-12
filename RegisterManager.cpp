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

#include "RegisterManager.h"
#include "TAC.h"

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

RegisterManager::RegisterManager(){
	std::string registers[] = { 
		"$t0", "$t1", "$t2",
		"$t3", "$t4", "$t5",
		"$t6", "$t7", "$t8",
		"$s0", "$s1", "$s2",
		"$s3", "$s4", "$s5",
		"$s6", "$s7", "$a0",
		"$a1"
	};
	for( unsigned int i = 0; i < sizeof(registers)/sizeof(registers[0]); i++)
	    m[registers[i]] = std::vector<std::string>();

	vars["false"] = "$0";
	vars["0"] = "$0";
    int lines = 0;
	std::ifstream infile("PruebasNovich/prueba");
	std::string line;
	int i = 0;
	int j = 0;
	std::string keywrd[] = {
		"+","*","-",":=","/","print",
		"#","main","id:","!","fall",
		"&","[","]",
	};
	std::vector<std::string> v;
	while (std::getline(infile, line)){
		lines = lines + 1;
	}
	std::ifstream newfile("PruebasNovich/prueba");
	std::string m[lines][3];
	while(j<lines){
		for (int y = 0; y < 3; ++y) {
			m[j][y] = "";
		}
		j = j + 1;
	}
	while (std::getline(newfile, line)){
		j = 0;
		while(j < 6){
			remove_sub(line,keywrd[j]);
			j = j + 1;
		}
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
	//Chequeamos si el grafo es k-coloreable
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
	int n_col = 19;
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
		if(i<n_col-1){
			spill[i] = false;
		}
		else{
			spill[i] = true;
		}
		i = i + 1;
	}
};

RegisterManager::~RegisterManager(){};

std::string RegisterManager::getFreeRegister(){
			for ( std::map<std::string, std::vector<std::string> >::iterator it = m.begin(); it != m.end(); it++ ) {
			    if (it->second.empty()) return it->first;
			}
			std::cout << "ME HE QUEDADO SIN REGISTROS" << std::endl;
		}

std::vector<std::string> RegisterManager::getReg(std::string v1, std::string v2, std::string v3){

	int iv1;
	int iv2;
	int iv3;
	bool sp1;
	bool sp2;
	bool sp3;

	iv1 = std::distance(v.begin(),std::find(v.begin(), v.end(), v1));
	if(v2 != ""){
		iv2 = std::distance(v.begin(),std::find(v.begin(), v.end(), v2));
	}else{
		iv2=-1;
	}
	if(v3 != ""){
		iv3 = std::distance(v.begin(),std::find(v.begin(), v.end(), v3));
	}else{
		iv3=-1;
	}
	sp1 = spill[iv1];
	sp2 = spill[iv2];
	sp3 = spill[iv3];
	while(iv3 == iv2 || iv1 == iv3){
		iv3 = iv3 + 1;
		sp3 = true;
	}
	while(iv2 == iv1 || iv2 == iv3){
		iv2 = iv2 + 1;
		sp2 = true;
	}
	while(iv1 == iv2 || iv1 == iv3){
		iv1 = iv1 + 1;
		sp1 = true;
	}
	std::vector<std::string> ret;
	ret.push_back(registers[iv1]);
	if(iv2 != -1){
		ret.push_back(registers[iv2]);
	}else{
		ret.push_back("");
	}
	if(iv3 != -1){
		ret.push_back(registers[iv3]);	
	}else{
		ret.push_back("");
	}
	if(sp1){
		ret.push_back("1");
	}else{
		ret.push_back("0");
	}
	if(sp2){
		ret.push_back("1");
	}else{
		ret.push_back("0");
	}
	if(sp3){
		ret.push_back("1");
	}else{
		ret.push_back("0");
	}
	return ret;
}
