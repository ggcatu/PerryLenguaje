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

		std::vector<std::string> getReg(std::string v1, std::string v2, std::string v3);

	private:
		std::map<std::string, std::vector<std::string> > m;
		std::map<std::string, std::string> vars;
		bool spill[];
		std::vector<std::string> v;
		std::string registers[];
};

#endif