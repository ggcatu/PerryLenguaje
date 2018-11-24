#ifndef RMANAGER_H
#define RMANAGER_H
#include <string>
#include <map>
#include <vector>
#include <iostream>

class TACObject;

class RegisterManager {	
	public:
		RegisterManager();
		~RegisterManager();
		std::string getFreeRegister();

		std::string getReg(TACObject * obj);
		std::string getReg(std::string var);

	private:
		std::map<std::string, std::vector<std::string> > m;
		std::map<std::string, std::string> vars;
};

#endif