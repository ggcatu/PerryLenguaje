














/* Definicion de la clase para las expresiones que retornan booleandos */
class exp_booleana : public ArbolSintactico {
	enum inst {IGUAL, DISTINTO, MENOR, MAYOR, MENORIGUAL, MAYORIGUAL, DISYUNCION, CONJUNCION, NEGACION, PARENTESIS, CORCHETE, LLAVE, CONSTANTE};
	public:
		ArbolSintactico * der;
		ArbolSintactico * izq;
		inst instruccion;
		exp_aritmetica(ArbolSintactico * d, ArbolSintactico * i, int is) : der(d), izq(i), instruccion(static_cast<inst>(is)), ArbolSintactico(BOOLEANOS) {}
		exp_aritmetica(ArbolSintactico * d, int is) : der(d), instruccion(static_cast<inst>(is)), ArbolSintactico(BOOLEANOS) {}
		virtual void imprimir(int i){
			if (izq != NULL){
				izq -> imprimir(tab + 1);
			}
			for (int j = 0; j < tab; j++) cout << "	";
			switch(instruccion){
				case IGUAL:
					cout << "IGUAL:" << endl;
					break;
				case MENOR:
					cout << "MENOR:" << endl;
					break;
				case MAYOR:
					cout << "MAYOR:" << endl;
					break;
				case MENORIGUAL:
					cout << "MENORIGUAL:" << endl;
					break;
				case MAYORIGUAL:
					cout << "MAYORIGUAL:" << endl;
					break;
				case PARENTESIS:
					cout << "PARENTESIS:" << endl;
					break;
				case CORCHETE:
					cout << "CORCHETE:" << endl;
					break;
				case LLAVE:
					cout << "LLAVE:" << endl;
					break;
				case DISYUNCION:
					cout << "DISYUNCION:" << endl;
					break;
				case CONJUNCION:
					cout << "CONJUNCION:" << endl;
					break;
				case NEGACION:
					cout << "NEGACION:" << endl;
					break;
				case CONSTANTE:
					cout << "CONSTANTE:" << endl;
					break;
			}
			if (der != NULL){
				der -> imprimir(tab+1);
			}
		}
};

/* Definicion de la clase para las expresiones para apuntadores, arreglos, listas, tuplas, llamada de funcion, etc */
class exp_otras : public ArbolSintactico {
	public:
		ArbolSintactico * der;
		ArbolSintactico * izq;
}

/* Definicion de la clase para los identificadores */
class identificador : public ArbolSintactico {
	public:
		string valor;
		identificador(string v) : valor(v) {}
		virtual void imprimir(int i) {
			for (int j = 0; j < tab; j++) cout << "	";
			cout << "identificador: " << valor.c_str() << endl;
		}
};

/* Definicion de la clase para int */
class entero : public ArbolSintactico {
	public:
		int valor;
		entero(): ArbolSintactico(ENTEROS) {is_type = 1;}
		entero(int v) : valor(v), ArbolSintactico(ENTEROS){}
		virtual void imprimir(int tab) {
			if (!is_type){
				for (int j = 0; j < tab; j++) cout << "	";
				cout << "entero: " << valor << endl;
			}
		}
};

/* Definicion de la clase para float */
class flotante : public ArbolSintactico {
	public:
		float valor;
		flotante(): ArbolSintactico(FLOTANTES) {is_type = 1;}
		flotante(float v) : valor(v), ArbolSintactico(FLOTANTES){}
		virtual void imprimir(int tab) {
			if (!is_type){
				for (int j = 0; j < tab; j++) cout << "	";
				cout << "flotante: " << valor << endl;
			}
		}
};

/* Definicion de la clase para bool */
class booleano : public ArbolSintactico {
	public:
		bool valor;
		booleano(): ArbolSintactico(BOOLEANOS) {is_type = 1;}
		booleano(bool v) : valor(v), ArbolSintactico(BOOLEANOS) {}
		virtual void imprimir(int tab) {
			if (!is_type){
				for (int j = 0; j < tab; j++) cout << "	";
				cout << "booleano: " << valor << endl;
			}
		}
};

/* Definicion de la clase para char */
class character : public ArbolSintactico {
	public:
		char valor;
		character() : ArbolSintactico(CHARACTERES) {is_type = 1;}
		character(char v) : valor(v), ArbolSintactico(CHARACTERES) {}
		virtual void imprimir(int tab) {
			if (!is_type){
				for (int j = 0; j < tab; j++) cout << "	";
				cout << "character: " << valor << endl;
			}
		}
};

/* Definicion de la clase para string */
class str : public ArbolSintactico {
	public:
		string valor;
};

/* Definicion de la clase para ptr (apuntadores) */
class ptr : public ArbolSintactico {
	public:
		ArbolSintactico * tipo;
		string id;
		ArbolSintactico * elem; // atributo para lo que sea que apunta?
};

/* Definicion de la clase para list */
class lista : public ArbolSintactico {
	public:
		ArbolSintactico * tipo;
		string id;
		// falta atributo para guardar elementos de la lista
};

/* Definicion de la clase para array */
class arreglo : public ArbolSintactico {
	public:
		ArbolSintactico * tipo
		int * tam;
		string id;
		// falta atributo para guardar elementos del arreglo
};

/* Definicion de la clase para tuple */
class tupla : public ArbolSintactico {
	public:
		ArbolSintactico * tipo1;
		ArbolSintactico * tipo2;
		string id;
		ArbolSintactico * valor1;
		ArbolSintactico * valor2;
};