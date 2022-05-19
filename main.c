#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>
#include <conio.h>
#include "Map.h"
#include "list.h"
#include "treemap.h"

typedef struct
{
    char id[50];
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
    List* libroAsociado; //Al final será útil esto??
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
    char* commonWords[100] = {"the", "be", "to", "of", "and", "a", "in", "that", "have", "i", "it", "for", "not", "on", 
                             "with", "he", "as", "you", "do", "at", "this", "but", "his", "by", "from", "they", "we", 
                             "say", "her", "she", "or", "an", "will", "my", "one", "all", "would", "there", "their", "what", 
                             "so", "up", "out", "if", "about", "who", "get", "which", "go", "me", "when", "make", "can", "like", 
                             "no", "just", "him", "know", "take", "into", "your", "good", "some", "could", "them", "than", "then", 
                             "now", "come", "its", "i'ts", "also", "how", "our", "well", "even", "want", "because", "any", "this", 
                             "most", "us", "are", "is"};
    for (int i = 0; i < 100; i++)
    {
        if (commonWords[i] == NULL) break;
        if (strcmp(palabra, commonWords[i]) == 0) 
            return true;
    }

    return false;
}

char* next_word (FILE *f) {
    char x[1024];
    if (fscanf(f, " %1023s", x) == 1)
        return strdup(x);
    else
        return NULL;
}

int hayQueEliminar (char c, char* string_chars)
{
    for (int i = 0; i < strlen(string_chars); i++)
        if (string_chars[i] == c) return 1;
    return 0;
}

char* quitarCaracteres(char* string, char* c)
{
    int i, k;
    for (i = 0; i < strlen(string); i++)
    {
        if (hayQueEliminar(string[i], c))
        {
            for (k = i; k < strlen(string)-1; k++)
                string[k] = string[k+1];
            string[k] = '\0';
            i--;
        }
    }

    return string;
}

void contarPalabrasYCaracteres(FILE* texto, tipoLibro* nuevoLibro)
{
    tipoPalabra* palabraAuxiliar = (tipoPalabra*) malloc (sizeof(tipoPalabra));
    char* palabra = (char*) malloc (100*sizeof(char));
    palabra = next_word(texto);
    for (int i = 0; palabra[i]; i++)
        palabra[i] = tolower(palabra[i]);
    int k = 0;
    int repetidas = 1;
    while (palabra)
    {
        for (int i = 0; palabra[i]; i++)
            palabra[i] = tolower(palabra[i]);
        palabra = quitarCaracteres(palabra, "!?,;""':.1234567890/()-*¨{}[]<>|$%&_^°¬¿¡ÔÇ£ÔÇØ#&=~+’“”");

        nuevoLibro->cantPalabras++;
        for (int i = 0; i < strlen(palabra); i++)
            nuevoLibro->cantCaracteres++;
        
        
        if (!esComun(palabra) && palabra[0] != '\0')
        {
            if (searchMap(nuevoLibro->mapaPalabras, palabra) == NULL)
            {
                tipoPalabra* nuevaPalabra = (tipoPalabra*) malloc (sizeof(tipoPalabra));
                nuevaPalabra->lineaPalabra = createList();
                ftell(texto);
                nuevaPalabra->apariciones = 1;
                strcpy(nuevaPalabra->palabra, palabra);
                nuevaPalabra->relevancia = 0;
                insertMap(nuevoLibro->mapaPalabras, palabra, nuevaPalabra);
            }

            if (searchMap(nuevoLibro->mapaPalabras, palabra) != NULL)
            {
                palabraAuxiliar = searchMap(nuevoLibro->mapaPalabras, palabra);
                palabraAuxiliar->apariciones++;
            }
        }
        palabra = next_word(texto);
    }
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
                printf("Texto %s abierto con exito!\n", path);
            
            tipoLibro* nuevoTexto = (tipoLibro*) malloc (sizeof(tipoLibro));
            strcpy(nuevoTexto->id, token);
            fgets(linea, 1023, texto);
            
            titulo[0] = '\0';
            const char *start = strchr(linea, 'f') + 2;
            strncat(titulo, start, strcspn(start, "\n"));
            strcpy(nuevoTexto->titulo, titulo);
            nuevoTexto->cantCaracteres = 0;
            nuevoTexto->cantPalabras = 0;
            nuevoTexto->mapaPalabras = createMap(is_equal_string);
            contarPalabrasYCaracteres(texto, nuevoTexto);

            /*tipoPalabra* prueba = firstMap(nuevoTexto->mapaPalabras);
            while (prueba != NULL)
            {
                printf("palabra: %s\n", prueba->palabra);
                printf("apariciones de %s: %d\n", prueba->palabra, prueba->apariciones);
                prueba = nextMap(nuevoTexto->mapaPalabras);
            }*/

            insertMap(mapaLibrosPorID, nuevoTexto->id, nuevoTexto);
            insertTreeMap(mapaLibrosPorTitulo, nuevoTexto->titulo, nuevoTexto);

            fclose(texto);
            token = strtok(NULL, limit);
        }
    }
}

void BuscarPorPalabra(char* palabra, Map* MapaLibros, List* listaPrioridad)
{
    tipoLibro* libro = firstMap(MapaLibros);
    int cont_libros = 0;
    int total_libros = 0;
    while(libro != NULL)
    {
        tipoPalabra* buscador_palabra = searchMap(libro->mapaPalabras, palabra);
        if(buscador_palabra != NULL )
        {
            printf("%s \n", libro->titulo);
            cont_libros++;
        }
        total_libros++;
        libro = nextMap(MapaLibros);
    }
    if(cont_libros == 0)
    {
        printf("La palabra ingresada no se encuentra en ningún libro\n");
    }
}

void mostrarDocsOrdenados(TreeMap* mapaLibrosPorTitulo){
    tipoLibro* IDActual = (tipoLibro*)malloc(sizeof(tipoLibro));
    IDActual = firstTreeMap(mapaLibrosPorTitulo); //Variable que ayudará a recorrer los documentos.

    //Si la primera posición es vacía, no hay documentos cargados.
    if(IDActual==NULL){
        printf("\nNo existen documentos cargados!\n"); 
        return;
    }

    //De lo contrario, se procede a mostrar los datos de todos los documentos.
    while(IDActual != NULL){
        printf("ID: %s\nTitulo: %s\nNumero de palabras: %d\nNumero de caracteres: %d\n\n", IDActual->id, IDActual->titulo, IDActual->cantPalabras,IDActual->cantCaracteres);
        IDActual=nextTreeMap(mapaLibrosPorTitulo);
    } 

    //Al finalizar de recorrer todos los documentos, se avisa al usuario y se retorna al menú.
    printf("Todos los documentos han sido procesados correctamente!\n");

}

int main()
{
    Map* mapaLibrosPorID = createMap(is_equal_string);
    TreeMap* mapaLibrosPorTitulo = createTreeMap(is_equal_string);
    char* idLibros = (char*) malloc (50*sizeof(char));
    char* titulo = (char*) malloc (100*sizeof(char));
    char* palabra = (char*) malloc (100*sizeof(char));
    int option;
    List* listaPrioridadPorPalabra = createList();

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
                    //getch();
                    break;
            case 2: mostrarDocsOrdenados(mapaLibrosPorTitulo);
                    break;
            case 3: printf("FUNCION NO IMPLEMENTADA!\n");
                    break;
            case 4: printf("FUNCION NO IMPLEMENTADA!\n");
                    break;
            case 5: printf("FUNCION NO IMPLEMENTADA!\n");
                    break;
            case 6: getchar();
                    printf("Ingrese la palabra que desea buscar\n");
                    scanf("%100[^\n]s", palabra);
                    BuscarPorPalabra(palabra, mapaLibrosPorID, listaPrioridadPorPalabra);
                    break;
            case 7: printf("FUNCION NO IMPLEMENTADA!\n");
                    break;
            case 0: break;
        }
        printf("\n");
    }
    return 0;
}