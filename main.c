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
    char* titulo = (char*) malloc (100*sizeof(char));
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
            titulo = quitarCaracteres(titulo, ";""'/()-*¨{}[]<>|$%&_^°¬¿¡ÔÇ£ÔÇØ#&=~+’“”");
            strcpy(nuevoTexto->titulo, titulo);
            nuevoTexto->cantCaracteres = 0;
            nuevoTexto->cantPalabras = 0;
            nuevoTexto->mapaPalabras = createMap(is_equal_string);
            contarPalabrasYCaracteres(texto, nuevoTexto);

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

void mostrarDocsOrdenados(TreeMap* LibrosPorTitulo){
    Pair* IDActual = firstTreeMap(LibrosPorTitulo); //Variable que ayudará a recorrer los documentos.
    //Si la primera posición es vacía, no hay documentos cargados.
    if(IDActual==NULL){
        printf("\nNo existen documentos cargados!\n"); 
        return;
    }

    tipoLibro* libroAuxiliar = IDActual->value; //Variable que ayudará a ingresar a los datos de cada documento.

    //De lo contrario, se procede a mostrar los datos de todos los documentos en orden alfabético por título.
    while(IDActual != NULL){
        libroAuxiliar = IDActual->value;
        printf("Titulo: %s\nCantidad de palabras: %ld\nCantidad de caracteres: %ld\n\n", libroAuxiliar->titulo, libroAuxiliar->cantPalabras, libroAuxiliar->cantCaracteres);
        IDActual = nextTreeMap(LibrosPorTitulo);
    } 

    //Al finalizar de recorrer todos los documentos, se avisa al usuario y se retorna al menú.
    printf("Todos los documentos han sido procesados correctamente!\n");

}

void buscarLibroPorTitulo(TreeMap* LibrosPorTitulo, char* palabrasTitulo){
    Pair* TituloActual = firstTreeMap(LibrosPorTitulo); //Se comienza a analizar el primer documento.
    char limit[2] = " ";
    char* token = strtok(palabrasTitulo, limit);
    
    if(!TituloActual){ //Si no hay un primer documento, es porque no hay cargados en el programa.
        printf("No existen documentados cargados.\n\n");
        return;
    }

    

}

void mostrarContexto(char* titulo, char* palabra, TreeMap* mapaLibrosPorTitulo)
{
    
}

int main()
{
    Map* mapaLibrosPorID = createMap(is_equal_string);
    TreeMap* mapaLibrosPorTitulo = createTreeMap(lower_than_string);
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
                    break;
            case 2: mostrarDocsOrdenados(mapaLibrosPorTitulo);
                    break;
            case 3: getchar();
                    printf("Ingrese palabras separadas por espacios, el algoritmo buscar titulos que las contengan: \n");
                    scanf("%100[^\n]s", titulo);
                    getchar();
                    buscarLibroPorTitulo(mapaLibrosPorTitulo, titulo);
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