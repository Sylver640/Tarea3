#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "Map.h"
#include "list.h"
#include "treemap.h"

typedef struct
{
    int id;
    char titulo[100];
    long cantPalabras;
    long cantCaracteres;
    Map* mapaPalabras;
} tipoLibro;

typedef struct
{
    int apariciones;
    char palabra[100];
    int relevancia;
    List* lineaPalabra;
    List* libroAsociado;
    
} tipoPalabra;

/*
  función para comparar claves de tipo string
  retorna 1 si son iguales
*/
int is_equal_string(void * key1, void * key2) {
    if(strcmp((char*)key1, (char*)key2)==0) return 1;
    return 0;
}

/*
  función para comparar claves de tipo string
  retorna 1 si son key1<key2
*/
int lower_than_string(void * key1, void * key2) {
    if(strcmp((char*)key1, (char*)key2) < 0) return 1;
    return 0;
}

/*
  función para comparar claves de tipo int
  retorna 1 si son iguales
*/
int is_equal_int(void * key1, void * key2) {
    if(*(int*)key1 == *(int*)key2) return 1;
    return 0;
}

/*
  función para comparar claves de tipo int
  retorna 1 si son key1<key2
*/
int lower_than_int(void * key1, void * key2) {
    if(*(int*)key1 < *(int*)key2) return 1;
    return 0;
}

void cargarDocumentos (char* nombreLibros)
{

}
void BuscarPorPalabra(char* palabra)
{
    tipoLibro* libro = firstMap(MapaLibros);
    int cont_libros = 0;
    int aux_relevancia;
    int total_libros = 0;
    //primero debo calcular en cuantos documentos se encuentra la palabra
    while(libro != NULL)
    {
        tipoPalabra* buscador_palabra = searchMap(libro->mapaPalabras, palabra);
        if(buscador_palabra != NULL)
        {
            cont_libros++;
        }
        total_libros++;
        libro = nextMap(MapaLibros);
    }
    
    while(libro != NULL)
    {
        tipoPalabra* buscador_palabra = searchMap(libro->mapaPalabras, palabra);
        
        if(buscador_palabra->relevancia == 0)
        {
            aux_relevancia = ((buscador_palabra->apariciones) / (libro->cantPalabras)) * (log(total_libros/cont_libros));
            buscador_palabra->relevancia = aux_relevancia;
        }
        libro = nextMap(MapaLibros);
    }

}





int main()
{
    char* nombreLibros = (char*) malloc (sizeof(char));
    char* palabra = (char*) malloc (100*sizeof(char));
    int option;

    //Creación de menú
    while (option != 0)
    {
        printf("******************************************\n");
        printf("1. Cargar documentos\n");
        printf("2. Mostrar documentos ordenados\n");
        printf("3. Buscar un libro por titulo\n");
        printf("4. Mostrar palabras con mayor frecuencia en un documento\n");
        printf("5. Mostrar palabras mas relevantes en un documento\n");
        printf("6. Buscar por palabra\n");
        printf("7. Mostrar palabras en el contexto del documento\n");
        printf("0. Salir\n");
        printf("******************************************\n");

        printf("Indique su opcion: ");
        scanf("%d", &option);

        switch(option)
        {
            case 1: printf("FUNCION NO IMPLEMENTADA!\n");
                    break;
            case 2: printf("FUNCION NO IMPLEMENTADA!\n");
                    break;
            case 3: printf("FUNCION NO IMPLEMENTADA!\n");
                    break;
            case 4: printf("FUNCION NO IMPLEMENTADA!\n");
                    break;
            case 5: getchar();
                    printf("Ingrese la palabra que desea buscar\n");
                    scanf("%100[^\n]s", palabra);
                    BuscarPorPalabra(palabra, );

                    break;
            case 6: printf("FUNCION NO IMPLEMENTADA!\n");
                    break;
            case 7: printf("FUNCION NO IMPLEMENTADA!\n");
                    break;
            case 0: break;
        }
        printf("\n");
    }
    return 0;
}