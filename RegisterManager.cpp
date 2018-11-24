#include <string>
#include <map>
#include <vector>
#include <iostream>
#include "RegisterManager.h"
#include "TAC.h"

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
};

RegisterManager::~RegisterManager(){};

std::string RegisterManager::getFreeRegister(){
			for ( std::map<std::string, std::vector<std::string> >::iterator it = m.begin(); it != m.end(); it++ ) {
			    if (it->second.empty()) return it->first;
			}
			std::cout << "ME HE QUEDADO SIN REGISTROS" << std::endl;
		}

std::string RegisterManager::getReg(TACObject * obj){
	std::string var = obj->valor;
	if (obj->t->tipo == LINT || var == ""){
		return var;
	}
	return getReg(var);
}

std::string RegisterManager::getReg(std::string var){
	if (vars.count(var)){
		// std::cout << "# " << var << " := " << vars[var] << std::endl; 
		return vars[var];
	}

	std::string temp = getFreeRegister();
	
	vars[var] = temp;
	m[temp].push_back(var);
	// std::cout << "# " << var << " := " << temp << std::endl; 
	return temp;
}