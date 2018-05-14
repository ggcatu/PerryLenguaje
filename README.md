# Perry

Lenguaje de programación cuyo compilador está implementado en C/C++ usando Flex (v.2.6.0-11) y Bison (v.2:3.0.4-dfsg.1).

Para compilar un programa en el lenguaje Perry debe ejecutar:

```
./perry programa flag
```

Donde:

* `programa` es un archivo.prr, en caso contrario el compilador retornará un error.
* `flag` imprime alguna de las fases del desarrollo del compilador. Puede ser alguna de las siguientes opciones:
    * -l para el lexer (utilizado por defecto).
    * -p para el parser.
