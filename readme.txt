Proyexto BOT
Etapa 1


Gabriel Giménez, carnet 12-11006

- Detalles de implementación:

Nuestro proyecto consta de tres códigos principales, lexer.l, definiciones.h,
definiciones.cpp, scanner.cpp. Básicamente, lexer.l contiene todas las reglas
para reconocer todas las expresiones definidas en el lenguaje BOT, definiciones.h
tiene definidas ciertas struct que nos permitirán almacenar los tokens reconocidos
por lexer.l en el texto de entrada, al igual que definiciones para cada una de las
palabras reservadas, operadores y demás expresiones pertinentes en nuestro lenguaje.
Luego, definiciones.cpp tiene la implementación de ciertas funciones útiles para
almacenar los tokens reconocidos por lexer.l y otras funciones que nos facilitan la
implementación de nuestro analizador lexicográfico.

En scanner.cpp abrimos el archivo que tengamos como argumento al ejecutar el programa
y se empieza a reconocer todos los patrones suministrados en lexer.l en dicho archivo
con la función yylex() correspondiente a Flex, luego se van guardando todos los
reconocimientos de los patrones de lexer.l en un vector que almacena elementos tipo
Token, que tienen tres atributos, el nombre del token, la fila y la columna donde
comienza el token en el texto del archivo. Igualmente, si se consigue alguna expresión
no válida en el lenguaje BOT, se almacena dicha expresión en un vector de errores.

Finalmente, se imprime por salida estándar cada uno de los tokens del primer vector si
no se ha conseguido ningún error, de lo contrario se imprimen los errores.

- Ejecución del analizador lexicográfico

Una vez en el directorio, ejecutar los siguientes comandos 

$ flex lexer.l
$ g++ scanner.cpp definiciones.cpp lex.yy.c -o LexBot
$ ./LexBot <File>

Donde <File> es el nombre del archivo que contiene el texto en lenguaje BOT.
