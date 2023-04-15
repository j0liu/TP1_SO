# TP1 SO
## Tabla de contenidos
* [Información general](#información-general)
* [Instrucciones de compilación y ejecución](#instrucciones-de-compilación-y-ejecución)
* [Autores](#autores)
* [Acerca de](#acerca-de)

## Información general
Este proyecto permite obtener el md5 de un conjunto de archivos, guardando el nombre del archivo, el md5 y qué proceso se encargó de él, en un archivo de salida y una memoria compartida.
<br>Además, permite leer dicha memoria con un programa vista. 


## Instrucciones de compilación y ejecución
Una vez descargado el proyecto, se debe correr el comando make all para que se creen los archivos ejecutables.
Luego, para poder ver el programa en funcionamiento se puede proceder de dos maneras diferentes:
Crear un pipe que permite conectar al proceso aplicación con el proceso vista con   

*Caso 1*: 

```
$ ./md5 [archivos] | ./view
```  


*Caso 2*:  

En una terminal, ejecutar:  
```
$ ./md5 [archivos]
```  
este imprimirá en pantalla los valores [pid] [cant archivos], que se deberán pasar como argumentos en otra terminal al otro programa:  
```
$ ./view [pid] [cant archivos]
```  

En ambos casos, se creará un archivo `file_[pid]` que tendrá los resultados, y el proceso vista los imprimirá.

**Ejemplos:**  

*Caso 1*: 
```
$ ./md5 ./files/* | ./view
```

*Caso 2*:  
```
$ ./md5 hola.txt chau.*
```

Si el resultado es:
```
666 3
```

Hay que escribir:
```
$ ./view 666 3
```
  
## Autores
* Liu, Jonathan Daniel
* Vilamowski, Abril
* Wischñevsky, David
## Acerca de
Este trabajo fue realizado como primer Trabajo Práctico de la materia Sistemas Operativos, en el tercer año de la carrera de Ingeniería Informática en el ITBA.


**Profesores:**
- Godio, Ariel
- Aquili, Alejo Ezequiel
- Mogni, Guido Matías
