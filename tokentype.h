#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    CONJUNCION = 46,
    DISYUNCION = 47,
    IGUALA = 32,
    DISTINTOA = 33,
    MENOR = 42,
    MAYOR = 44,
    MAYORIGUAL = 43,
    MENORIGUAL = 41,
    SUMA = 34,
    RESTA = 35,
    POW = 38,
    MULT = 39,
    DIV = 37,
    MOD = 36,
    NEGACION = 45,
    ELSE = 19,
    LLAVEABRE = 1,
    LLAVECIERRA = 2,
    CORCHETEABRE = 3,
    CORCHETECIERRA = 4,
    PARABRE = 5,
    PARCIERRA = 6,
    MAIN = 7,
    INCLUDE = 10,
    CREATE = 11,
    EXECUTE = 12,
    MODULE = 13,
    PUNTOCOMA = 14,
    WHILE = 15,
    FOR = 16,
    BREAK = 17,
    IF = 18,
    RETURN = 20,
    STRUCT = 21,
    UNION = 22,
    ARRAY = 23,
    LIST = 24,
    TUPLE = 25,
    LSTRING = 26,
    LFLOAT = 27,
    LCHAR = 28,
    BOOL = 29,
    LINT = 30,
    POINTER = 31,
    IGUAL = 40,
    SPACE = 48,
    TAB = 49,
    COMA = 50,
    SALIDA = 51,
    ENTRADA = 52,
    PUNTO = 53,
    ERROR = 54,
    ENTER = 55,
    COMENTARIO = 56,
    TYPE = 57,
    NEW = 58,
    FREE = 59,
    REFERENCE = 60,
    OPTR = 61,
    UNIT = 62,
    TYPE_ERROR = 258,
    INT = 259,
    FLOAT = 260,
    IDENTIFIER = 261,
    CHAR = 262,
    STRING = 263,
    TRUE = 264,
    FALSE = 265
  };
#endif