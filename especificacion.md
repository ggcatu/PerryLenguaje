# Perry

Especificación del lenguaje de programación Perry, cuyo compilador está implementado en C/C++.  

## Estructura

### Programa

Todo programa en Perry tiene la siguiente estructura:  

```
main{
  is;
}
```

Donde `is` es una secuencia de instrucciones.  

### Bloque y Declaración

Es posible formar una instrucción de bloque `create{ ds } do{ is };`, donde `ds` es una secuencia declaraciones, la cual es opcional, y `is` es una secuencia de instrucciones separadas por `;`. Es posible indicar, en una misma linea, varias declaraciones a un mismo tipo; sólo si no se inicializa explicitamente (asignando algún valor) alguna de las variables. Un bloque con declaraciones asociadas se puede observar de la siguiente forma:  

```
create{
  tipo id;
  tipo id = valor;
}
execute{
  is;
}
```

Donde `tipo` indica el tipo de dato al que pertenece la variable y `id` es el identificador de la variable. De forma opcional se puede asignar un valor a la variable, poniendo un igual seguido del valor al que se quiere igualar.  

La secuencia de declaraciones genera un contexto donde hace disponible cada variable del tipo declarado asociado a un identificador y no estarán disponibles fuera de ese contexto. Dicho contexto es alcanzable para toda instrucción o expresión dentro de la secuencia de instrucciones. En el caso de que haya una instrucción de bloque dentro de la secuencia de instrucciones de otro bloque y exista una declaración con un mismo identificador, la asociación interna va a sustituir a la externa.  

Es importante resaltar que sería un error tener dos declaraciones para un mismo identificador dentro de un mismo contexto.  

### Asignación

Una asignación `id = e` se usa para evaluar la expresión `e`  y almacenar el resultado en la variable con identificador `id`. La variable `id` debe haber sido declarada; en caso contrario dará un error. Análogamente, si se usan variables en `e` estas deben haber sido declaradas y asignadas con un valor previamente; en caso contrario dará un error. Además, `e` y `id` deben de ser del mismo tipo.  

### Secuencia

La composición secuencial de las intrucciones `inst_0` e `inst_1` es la instrucción compuesta `inst_0; inst_1;`. Esta corresponde a ejecutar la instrucción `inst_0` y luego la instrucción `inst_1`. Es importante destacar que la secuencia es una sola instrucción compuesta, que permite combinar varias instrucciones en una sola.  

### Condicional

La instrucción condicional `if (condición){ is_0 } else{ is_1 }` verifica una `condición` y ejecuta la intrucción `is_0` si la condición es `true`. En caso contrario ejecuta la instrucción `is_1`. Además `condición` es una expresion booleana. La expresión evaluada dentro de los paréntesis requieren del uso de algún operador de comparación.  

### Iteración

#### Iteración determinada

La repetición determinada se declara con la instrucción `for` y se utiliza para repetir un bloque de código.  

```
for (inicialización, inicio, fin, paso){
  is;
};
```

En `inicialización` se declara la variable, en `inicio` se establece el valor de inicio del rango de la repetición del bloque de instrucciones `is`, `fin` es el valor final de dicho rango y `paso` es el incremento de la variable declarada en `inicialización`.  

#### Iteración indeterminada

Un ciclo indeterminado se expresa con la instrucción `while` y ciclará de manera continua e infinita hasta que no se cumpla una condición expresada con condicion dentro de paréntesis. Algo debe cambiar a la variable dentro de la condición, o el ciclo nunca se detendrá. Esto podráa ser una variable incremental.  

```
while (condición){
  is;
}
```

Donde la `condición` es una expresión boolena. Y `is` es una secuencia de instrucciones.  

### Break

`break` Se usa para salir de un ciclo (determinado o indeterminado), pasando por alto la condición normal del ciclo. 

## Instrucciones

### Funciones

Segmentar el código en funciones permite crear piezas modulares de código que realizan una tarea determinada y luego retornan al area del código en donde la función fue “llamada”. El uso típico de una función es cuando se necesita realizar la misma acción múltiples veces en un programa.  

Perry tiene la función requerida main. Otras funciones deben ser creadas fuera de las llaves de esta función.  

La sintaxis para declarar una función es:

```
tipo_retorno nombre( lista_de_parámetros ){
  is;
};
```

Donde `tipo_retorno` es el tipo de valor de retorno, `nombre` es el identificador de la función (el nombre por el que sera “llamada”), `lista_de_parámetros` es una lista de la forma `[tipo_1 var_1, tipo_2 var_2, ...]` en el cual `tipo_i` es el tipo de la variable `var_i`. Opcionalmente se puede declarar un valor “default” para las variables, que será tomado en el caso de que el parámetro no sea suministrado en la llamada a la función. 

Dentro de las llaves va el bloque de instrucciones que ejecutará la función. Entre estas instrucciones tenemos a `return valor_retorno;`, la cual retorna `valor_retorno` del tipo `tipo_retorno` y termina la ejecución de las intrucciones en la función. Por último, una función puede tener como tipo de valor de retorno a `void` y como instrucción de retorno `return;` cuando se quiere retornar nada.   

Las variables que se declaren dentro del bloque `is` sólo tendrán alcance dentro del mismo bloque mediante el uso de la instrucción de bloque y alcance, y no pueden ser utilizadas fuera de las llaves. Si la función no recibe parametros se espera que se escriban los parentesis `()` y el punto y coma `;`.  

En cuanto al pasaje de parámetros, por defecto se utiliza por valor, sin embargo también se pueden pasar valores por referencia mediante el símbolo @. Por ejemplo:

```
tipo_retorno nombre( int @a, boolean b, float @b) {
  is;
};
```

Donde las variables a y b son pasadas por referencia y b por valor.

### Entrada/Salida

La entrada y salida del programa se maneja con los símbolos `>>` y `<<`, usando el primero para la salida y el segundo para la entrada. Con la sintaxis: `<< id` para la entrada, donde `id` es el identificador de la variable donde se almacenarán los datos que entrena. Mientras que la salida se expresa con la sintaxis: `>> id`.  

### new y free

En Perry se puede reservar o liberar memoria dinámicamente, es decir, según se necesite. Para ello existen las instrucciones `new` y `free`, las cuales reservan un bloque en memoria o liberan el espacio ocupado respectivamente.

En el caso de new se tiene la siguiente estructura: `new(id);`. Por otra parte, `free` tiene la siguiente estructura:  `free(id);`.

## Expresiones

Las expresiones están constituidas por variables, enteros, flotantes, literales, booleanos y operadores. En el caso de una variable, al momento de acceder a su valor, independiente de su tipo, la misma debe haber sido declarada. Partiendo de una expresión `e`, de tipo cualquiera, se puede construir la expresión `(e)` y ambas van a producir el mismo valor, es decir, evaluar `(e)` conlleva a evaluar a `e`.  

### Caracteres 

Los literales son valores fijos que no cambian en el curso del programa. Por ejemplo, si se escribe el numero 3 o 500 en el programa, estos son literales. Los literales para los caracteres se epresan entres comillas, al igual que cualquier caracter ASCII, como ‘h’. De esta forma, un caracter se expresa de la forma `char`.  

### Enteros

Las constantes enteras son números en base 10 (decimal) que se usan directamente en el programa, como 123 o -123, lo cuales se expresan de la forma `int`.

### Flotantes

Las constantes de punto flotante son cambiadas a tiempo de compilación por el valor al que evalua la expresión. Se puede expresar como `float`. 

### Booleanos

La constantes boolenas seran `true` y `false`, teniendo el tipo `bool`.  

### Apuntador

Un apuntador es una variable cuyo valor es la dirección de memoria de otra variable. Se dice que un apuntador “apunta” a la variable cuyo valor se almacena a partir de la dirección de memoria que contiene el apuntador. Por ejemplo, si un apuntador p almacena la dirección de una variable x, se dice que “p apunta a x”.

Un apuntador se incializa de la siguiente forma:

```
  ptr tipo id;
``` 

El símbolo `&` se utiliza para acceder al valor de `id` de la siguiente forma: `&id`.


### Arreglos

Al igual que las variables que se han utilizado hasta este momento, un arreglo debe tener un tipo (como int, char o float) y un nombre. Además, habrá que especificar el número de valores que almacenará. Un arreglo se declara de la siguiente forma:  

```
  array[tamaño] tipo;
```

Donde `tipo` se refiere al tipo de dato de todos los elementos del arreglo, `id` es el identificador del arreglo y `tamaño` es el número de elementos que contiene el arreglo.  

Los índices de un arreglo se encuentran en el intervalo [0,tamaño-1]. Además, se asigna valores a las posiciones del arreglo de la siguiente forma `id[índice] = valor`, donde `índice` es un entero menor al tamaño del arreglo y `valor` debe ser del tipo de dato del arreglo. En caso contrario se tendrá un error.  

Un arreglo se puede inicializar también de la siguiente forma:  

```
  array[tamaño] tipo id = [valo_0, valor_1, ... , valor_(tamaño-1)];
```

### Listas

Al igual que las variables que se han utilizado hasta este momento, una lista debe tener un tipo de dato asociado (como int, char o float) y un nombre. Una lista se declara de la siguiente forma:  

```
  list tipo id;
```

Donde `tipo` se refiere al tipo de dato de todos los elementos del arreglo, `id` es el identificador del arreglo y `tamaño` es el número de elementos que contiene el arreglo.  

Una lista se puede inicializar también de la siguiente forma:  

```
  list tipo id = {valor_0, valor_1, ... };
```

### Tuplas

Una tupla es una estructura de datos inmutable, la cual no puede modificarse de ningún modo después de su creación. Contiene dos tipos de datos iguales o distinos y se declara de la siguiente forma:

```
  tuple (tipo_1, tipo_2) id;
```

Y puede asignarse un valor a la tupla de la siguiente forma:

```
  id = (valor_1, valor_2);
```

### Strings

Un string es una cadena de caracteres encerrada en comillas dobles `"` y se puede expresar como `string`. Además, en dicha secuencia no es posible que exista un salto de linea, comillas dobles o backslashes `\` a no ser de que estén escapados, es decir, `\\n`, `\\` y `\"`. 


### Variables

Las variables en Perry tienen un alcance definido por el bloque en el cual son declaradas.  

Una variable global es aquella que puede ser vista por cualquier función en el programa. Las variables locales sólo son visibles en la función en la que son declaradas. En Perry, cualquier variable declarada fuera de una función (incluyendo el programa principal main) es una variable global.  

## Sintaxis

### Comentarios

Los comentarios son lı́neas en el programa que se usan para informar al programador acerca de la forma que trabajan el mismo.  

Se pueden tener comentarios de una sola línea, definidos por dos slashes consecutivos `//`, que indican al compilador que ignore el resto de la lı́nea despues de estos, o bloques de comentarios, que comienzan por un slash seguido de un asterisco `/*` y terminan con los mismos símbolos al contrario `*/`.  

### Operadores

#### Operadores aritmeticos:  
- Modulo `mod`  
- Multiplicación `*`
- Potencia `**`
- Suma `+`  
- Resta `-`  
- División `/`  

#### Operadores booleanos:  
- no lógico `not`  
- conjunción lógica `and`  
- disjunción lógica `or`  

#### Operadores de comparación:  
- distinto a `!=`  
- menor que `< `  
- menor o igual que `<=`  
- igual a `==`  
- mayor que `> `  
- mayor o igual que `>=`  

### include

Se usa para incluir librerias externas. Esto le da al programador acceso a módulos especificados especialmente para Perry. `include`, no tiene punto y coma al final y se usa de la siguiente forma:

```include "archivo.extensión"```

### module

Se usa para incluir módulos que no contienen estructura principal. `module` no tiene punto y coma al final. Se utiliza de la siguiente forma:

```
module
is;
```

### structs

Un `struct` es una estructura de datos que se utiliza para organizar y almacenar distintos tipos de datos. La estructura general es la siguiente:

``` 
type struct id{
	tipo_1 id_1;
	...
	tipo_n id_n;
};
```

Donde `id` es el identificador de la estructura y `tipo_i id_i` se refiere al miembro de la estructura con su tipo y id asociado.

### union

Un `union` es una estructura de datos que se utiliza para almacenar uno de los tipos de datos presentes en la misma. La estructura general es la siguiente:

``` 
type union id{
	tipo_1 id_1;
	...
	tipo_n id_n;
};
```

Donde `id` es el identificador de la estructura y `tipo_i id_i` se refiere al miembro de la estructura con su tipo y id asociado.

### type

En perry se puede declarar un tipo de datos personalizado de la siguiente forma: `type id = constructor_tipo`, donde `id` es el nombre de tipo de datos declarado y en `constructor_tipo`  se especifica la estructura del mismo.