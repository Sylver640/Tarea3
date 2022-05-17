#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>
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

bool esComun (char* palabra)
{
    char* commonWords[100] = {"the", "be", "to", "of", "and", "a", "in", "that", "have", "I", "it", "for", "not", "on", 
                             "with", "he", "as", "you", "do", "at", "this", "but", "his", "by", "from", "they", "we", 
                             "say", "her", "she", "or", "an", "will", "my", "one", "all", "would", "there", "their", "what", 
                             "so", "up", "out", "if", "about", "who", "get", "which", "go", "me", "when", "make", "can", "like", 
                             "no", "just", "him", "know", "take", "into", "your", "good", "some", "could", "us"};
}

void cargarDocumentos (char* idLibros, Map* mapaLibrosPorID, TreeMap* mapaLibrosPorTitulo)
{
    char limit[2] = " ";
    char path[100];
    char linea[1024];
    char titulo[100];
    char* token = strtok(idLibros, limit);
    if (token != NULL)
    {
        while (token != NULL)
        {
            snprintf(path, sizeof(path), "books/%s.txt", token);
            FILE* texto = fopen(path, "rt");
            if (texto == NULL) 
            {
                printf("Texto %s no encontrado!\n", path);
                token = strtok(NULL, limit);
                continue;
            }
            else 
                printf("Archivo abierto con exito!\n");
            
            tipoLibro* nuevoTexto = (tipoLibro*) malloc (sizeof(tipoLibro));
            nuevoTexto->id = atoi(token);
            fgets(linea, 1023, texto);
            //sscanf(linea, "%")
            printf("%s\n", linea);

            token = strtok(NULL, limit);
        }
    }
}
void BuscarPorPalabra(char* palabra, Map* MapaLibros)
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
    Map* mapaLibrosPorID = createMap(lower_than_int);
    TreeMap* mapaLibrosPorTitulo = createTreeMap(lower_than_string);
    char* idLibros = (char*) malloc (50*sizeof(char));
    char* titulo = (char*) malloc (100*sizeof(char));
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
            case 1: getchar();
                    printf("Ingrese el nombre de su archivo (si es mas de uno, separe con espacios): ");
                    scanf("%50[^\n]s", idLibros);
                    cargarDocumentos(idLibros, mapaLibrosPorID, mapaLibrosPorTitulo);
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
                    BuscarPorPalabra(palabra, mapaLibrosPorID);
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