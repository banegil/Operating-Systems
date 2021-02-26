#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mitar.h"

extern char *uso;

int copynFile(FILE *origen, FILE *destino, int nBytes);
int readHeader(FILE *tarFile, stHeaderEntry **header, int *nFiles);
int loadstr(FILE *file, char** buf);
int appendTar(int nFiles, char *fileNames[], char tarName[],char newTarName[]);
int listTar(char tarName[]);



/** Copia el nBytes del fichero origen al fichero destino.
 *
 * origen: puntero al descriptor FILE del fichero origen
 * destino: puntero al descriptor FILE del fichero destino
 * nBytes: numero de bytes a copiar
 *
 * Devuelve el numero de bytes copiados o -1 en caso de error. 
 */
int copynFile(FILE *origen, FILE *destino, int nBytes) {
  // Completar la funci�n
   int numBytes = 0;
   int c; 

   /* Si el fichero origen y destin no se abren correctamente, error. En contrario, se copia el fichero origen en el fichero destino. */
   if(origen == NULL || destino == NULL) {
      return -1;
   }

   /* Mientras no se llegue al final de fichero y el n�mero de bytes le�dos sean distinto de el nBytes m�ximos a copiar, se lee caracter 
   a caracter el fichero origen y se copia en el de destino. */
   do {
         c = getc(origen); 
         if(c != EOF) { 
            putc(c, destino);
            numBytes++;
         }
   }while((c != EOF) && (numBytes != nBytes));

   return numBytes;
   
}

/** Carga en memoria la cabecera del tarball.
 *
 * tarFile: puntero al descriptor FILE del tarball
 * header: direcci�n de un puntero que se inicializar� con la direcci�n de un
 * buffer que contenga los descriptores de los ficheros cargados.
 * nFiles: direcci�n de un entero (puntero a entero) que se inicializar� con el
 * n�mero de ficheros contenido en el tarball (primeros bytes de la cabecera).
 *
 * Devuelve EXIT_SUCCESS en caso de exito o EXIT_FAILURE en caso de error
 * (macros definidas en stdlib.h).
 */
int readHeader(FILE *tarFile, stHeaderEntry **header, int *nFiles) {
   // Completar la funci�n 
   stHeaderEntry *array = NULL;
   int nr_files = 0;
   char c;
   int i;

   /* Si el fichero tarFile no se abre correctamente, error. En contrario, se contin�a creando el fichero comprimido. */
   if(tarFile == NULL)
      return (EXIT_FAILURE);
  
   /* Leemos el n�mero de ficheros (N) del tarFile y lo volcamos en nr_files */
   fread(&nr_files, sizeof(int), 1, tarFile);
   fread(&c, sizeof(char), 1, tarFile);

   /* Reservamos memoria para el array */
   array = (stHeaderEntry*) malloc(sizeof(stHeaderEntry) * nr_files);
   
   /* Leemos la metainformaci�n del tarFile y lo volcamos en el array */
   for(i= 0; i < nr_files; i++) {
      array[i].name = malloc(PATH_MAX);
      loadstr(tarFile, &array[i].name);
      fread(&array[i].size, sizeof(int), 1, tarFile);
      fread(&c, sizeof(char), 1, tarFile);
   }

   /* Devolvemos los valores le�dos a la funci�n invocadora */
   (*nFiles) = nr_files;
   (*header) = array;

   return (EXIT_SUCCESS);
 
}

/** Carga una cadena de caracteres de un fichero.
 *
 * file: direcci�n de la estructura FILE del fichero
 * buf: direcci�n del puntero en que se inicializar� con la direcci�n donde se
 * copiar� la cadena. Ser� una direcci�n del heap obtenida con malloc.
 *
 * Devuelve: 0 si tuvo exito, -1 en caso de error.
 */

int loadstr(FILE *file, char** buf) {
   // Completar la funci�n
   char *buffer = NULL;
   int len;
   int ch;
   
   /* Si el fichero file no se abre correctamente, error. En contrario, se carga una cadena de caracteres del fichero file en el buffer. */
   if(file == NULL)
      return -1;

   /* Se reserva memoria din�mica para el puntero buffer y se inicializa la variable len que contiene la longitud del buffer */
   buffer = malloc(PATH_MAX);
   len = 0;

  /* Se lee caracter a caracter del fichero file y se a�ade el caracter al buffer */
   do {
      ch = getc(file);
      if(ch != '\n')  {
         buffer[len++] = ch;
      }
   } while(ch != '\n');

   /*Finalmente, se a�ade el caracter de terminaci�n de la cadena*/
   buffer[len] = '\0';

   /* Devolvemos lo valores le�dos a la funci�n invocadora */
   (*buf) = buffer;

   return 0;
}

/** crea un tarball a partir de unos ficheros de entrada.
 *
 * nfiles:    numero de ficheros de entrada
 * filenames: array con los nombres de los ficheros de entrada (rutas)
 * tarname:   nombre del tarball
 * 
 * devuelve exit_success en caso de exito y exit_failure en caso de error
 * (macros definidas en stdlib.h).
 *
 * ayuda: calcular primero el espacio necesario para la cabecera del tarball,
 * saltar dicho espacio desde el comienzo del archivo e ir copiando uno a uno
 * los ficheros, rellenando a la vez una representaci�n de la cabecera del
 * tarball en memoria. al final copiar la cabecera al comienzo del tarball.
 * recordar que  que no vale un simple sizeof de stheaderentry para saber el
 * tama�o necesario en disco para el descriptor de un fichero puesto que sizeof
 * no calcula el tama�o de la cadena apuntada por name, sino el tama�o del
 * propio puntero. para calcular el tama�o de una cadena c puede usarse la
 * funci�n strlen.
 */
int createTar(int nFiles, char *fileNames[], char tarName[]) {
   // Completar la funci�n 
   FILE *inputFile = NULL;
   FILE *tarFile = NULL;
   stHeaderEntry *array= NULL;

   int nBytes = 0;
   int i;
   
   /* Si el fichero tarFile no se abre correctamente, error. En contrario, se contin�a creando el fichero comprimido. */
   if((tarFile = fopen(tarName, "w")) == NULL)
      return (EXIT_FAILURE);
   
   /* Reservamos memor�a dinamica para el puntero array */ 
   array = malloc(sizeof(stHeaderEntry) * nFiles);
   
   /* El n�mero de bytes a desplazar son los de la cabecera del fichero comprimido m�s los bytes de salto de linea (expresados como (nFiles + 1)) */
   nBytes = (nFiles + 1) + sizeof(int) + nFiles * sizeof(unsigned int); 
   
   /* Se copia la cabecera del fichero en array*/
   for(i = 0; i < nFiles; i++) {
      array[i].name = malloc(strlen(fileNames[i]) + 1);
      strcpy(array[i].name,fileNames[i]);
      nBytes += strlen(fileNames[i]) + 1;
   }
   
   /* Desplazamos el indicador de fichero tantos bytes como los que tenga la cabecera*/
   fseek(tarFile, nBytes, SEEK_SET);

   /* Copiamos el contenido de los ficheros en el fichero comprimido */
   for(i = 0; i < nFiles; i++) {
      inputFile = fopen(fileNames[i], "r");
      if(inputFile == NULL)
         return (EXIT_FAILURE);
      nBytes = copynFile(inputFile, tarFile, INT_MAX);
      fclose(inputFile);
      array[i].size = nBytes;
   }
   /* Nos posicionamos en el byte 0 en el fichero tar*/   
   rewind(tarFile);

   /* Escribir el n�mero de ficheros en fichero comprimido*/  
   fwrite(&nFiles, sizeof(int), 1, tarFile);
   fwrite("\n", sizeof(char), 1, tarFile);

   /* Para cada estructura stdHeaderEntry, escribir la ruta del fichero y el n�mero de bytes que ocupa el fichero 
      Posteriormente, liberamos la memor�a dinamica que ocupa cada ruta del archivo i correspondiente a la estructura stHeaderEntry */
   for(i = 0; i < nFiles;i++) {
      fwrite(array[i].name,strlen(fileNames[i]), 1, tarFile);
      fwrite("\n", sizeof(char), 1, tarFile);
      fwrite(&array[i].size, sizeof(unsigned int), 1, tarFile);
      fwrite("\n", sizeof(char), 1, tarFile);
      free(array[i].name);
   }

   printf("Fichero mitar creado con exito\n");
   

   free(array);
   fclose(tarFile);

   return (EXIT_SUCCESS);
  
}

/** Extrae todos los ficheros de un tarball.
 *
 * tarName: cadena C con el nombre del tarball
 *
 * Devuelve EXIT_SUCCESS en caso de exito y EXIT_FAILURE en caso de error (macros
 * definidas en stdlib.h).
 *
 * AYUDA: debemos cargar primero la cabecera, con lo que nos situaremos al
 * comienzo del primer fichero almacenado. Entonces recorriendo la cabecera
 * sabremos el nombre que debemos ir dando a los ficheros y la cantidad de bytes
 * que debemos leer del fichero para cargar su contenido.
 */
int extractTar(char tarName[]) {
   // Completar la funci�n
   FILE *outputFile = NULL;
   FILE *tarFile = NULL;
   stHeaderEntry *array= NULL;  

   int nr_files = 0;
   int i;

   /* Si el fichero tarFile no se abre correctamente, error. En contrario, se contin�a creando el fichero comprimido. */
   if((tarFile = fopen(tarName, "r")) == NULL)
      return (EXIT_FAILURE);

   /* Se lee la cabecera del fichero comprimido */
   readHeader(tarFile, &array, &nr_files);
   
   /* Se descomprime el fichero comprimido, es decir, se cop�a tanto bytes como tengan los ficheros a comprimir con el nombre indicado. 
   Toda est� informaci�n viene en la cabecera del fichero. */
   for(i = 0; i < nr_files; i++) {
      outputFile = fopen(array[i].name, "w");
      copynFile(tarFile, outputFile, array[i].size);
      printf("[%d]: Creando fichero %s, tamano %d Bytes...Ok\n",i,array[i].name,array[i].size); 
      fclose(outputFile);
   }

   free(array);
   fclose(tarFile);

   return (EXIT_SUCCESS);
}


int appendTar(int nFiles, char *fileNames[], char tarName[],char newTarName[]) {
   FILE *inputFile = NULL;
   FILE *tarFile = NULL;
   FILE *newtarFile = NULL;
   stHeaderEntry *array= NULL;
   stHeaderEntry *newArray= NULL;

   int nr_files = 0;
   int newNFiles = 0;
   int i;
   int nBytes = 0;

   /* Si el fichero tarFile no se abre correctamente, error. En contrario, se contin�a creando el fichero comprimido. */
   if((tarFile = fopen(tarName, "r")) == NULL)
      return (EXIT_FAILURE);

   if((newtarFile = fopen(newTarName, "w")) == NULL)
      return (EXIT_FAILURE);

   fread(&nr_files, sizeof(int), 1, tarFile);
   rewind(tarFile);

   array = (stHeaderEntry*) malloc(sizeof(stHeaderEntry) * nr_files);

   /* Se lee la cabecera del fichero comprimido */
   readHeader(tarFile, &array, &nr_files);

   newNFiles = nFiles + nr_files;

   newArray = (stHeaderEntry*) malloc(sizeof(stHeaderEntry) * (newNFiles));

   for(i = 0; i < nr_files; i++) {
      newArray[i].name = malloc(strlen(array[i].name) + 1);
      strcpy(newArray[i].name,array[i].name);
      newArray[i].size = array[i].size;
   }

   for(i = 0; i < nFiles; i++) {
      newArray[i+nr_files].name = malloc(strlen(fileNames[i])+1);
      strcpy(newArray[i+nr_files].name,fileNames[i]);
   }

   

   /* Se reserva espacio para el nuevo n�mero de ficheros y la nueva cabecera, es decir, la cabecera antigua y los pares 
     (nombre, tama�o) de los nuevos ficheros */
   for(i = 0; i < nr_files; i++) {
      nBytes += strlen(array[i].name) + 1;
   }
   
   for(i = 0; i < nFiles; i++) {
      nBytes += strlen(fileNames[i]) + 1;
   }

   
   nBytes += (nFiles + nr_files + 1) + sizeof(int) + (newNFiles) * sizeof(unsigned int);

   /* Desplazamos el indicador de fichero tantos bytes como los que tenga la nueva cabecera*/
   fseek(newtarFile, nBytes, SEEK_SET);   

   /* Copiamos el contenido de los ficheros del mtar existente y los nuevos ficheros en el nuevo fichero mtar que contendr�
      el contenido de los ficheros del anterior mtar m�s los nuevos ficheros*/
   for(i = 0; i < nr_files; i++) {  
      copynFile(tarFile, newtarFile, newArray[i].size);
   }
   
   for(i = 0; i < nFiles; i++) {
      inputFile = fopen(fileNames[i], "r");
      if(inputFile == NULL)
         return (EXIT_FAILURE);
      nBytes = copynFile(inputFile, newtarFile, INT_MAX);
      newArray[i+nr_files].size = nBytes;
      fclose(inputFile);
   }

   /* Nos posicionamos en el byte 0 en el fichero tar nuevo*/  
   rewind(newtarFile);

   /* Escribir el n�mero de ficheros en fichero comprimido*/  
   fwrite(&newNFiles, sizeof(int), 1, newtarFile);
   fwrite("\n", sizeof(char), 1, newtarFile);

   /* Para cada estructura stdHeaderEntry, escribir la ruta del fichero y el n�mero de bytes que ocupa el fichero 
      Posteriormente, liberamos la memor�a dinamica que ocupa cada ruta del archivo i correspondiente a la estructura stHeaderEntry */
   for(i = 0; i < newNFiles;i++) {
      fwrite(newArray[i].name,strlen(newArray[i].name), 1, newtarFile);
      fwrite("\n", sizeof(char), 1, newtarFile);
      fwrite(&newArray[i].size, sizeof(unsigned int), 1, newtarFile);
      fwrite("\n", sizeof(char), 1, newtarFile);
      free(newArray[i].name);
   }
   
   for(i = 0; i < nr_files; i++) {
      free(array[i].name);
   }

   printf("Fichero mitar creado con exito\n");

   free(array);
   free(newArray);
   fclose(newtarFile);
   fclose(tarFile);

   return (EXIT_SUCCESS);

}

int listTar(char tarName[]) {
   FILE *tarFile = NULL;
   stHeaderEntry *array= NULL;

   int nr_files = 0;
   int i, j;

   /* Si el fichero tarFile no se abre correctamente, error. En contrario, se contin�a creando el fichero comprimido. */
   if((tarFile = fopen(tarName, "r")) == NULL)
      return (EXIT_FAILURE);

   array = (stHeaderEntry*) malloc(sizeof(stHeaderEntry) * nr_files);

   /* Se lee la cabecera del fichero comprimido */
   readHeader(tarFile, &array, &nr_files);

   /* Ordena los pares (nombre, tama�o) de los ficheros en funci�n del tama�o de los ficheros a trav�s del m�todo de la burbuja */
   for (i = 0; i < nr_files -1 ; i++) {
      for (j = i + 1; j < nr_files ; j++) {
         if (array[i].size < array[j].size) {
            stHeaderEntry t = array[i];
            array[i] = array[j];
            array[j] = t;
         }
      }
   }

   for(i = 0; i < nr_files; i++) {
      printf("Nombre del fichero %s, tamano %i Bytes\n", array[i].name, array[i].size);
   }

   return (EXIT_SUCCESS);

}
