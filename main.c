#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>
#include "Map.h"
#include "list.h"
#include "treemap.h"
#include "heap.h"

//Struct que guarda los datos de los libros
typedef struct
{
    char id[50];
    char titulo[100];
    double cantPalabras;
    long cantCaracteres;
    Map* mapaPalabras; //Mapa que guarda datos de tipoPalabra
} tipoLibro;

//Struct que guarda todo lo necesario para procesar palabras
typedef struct
{
    double apariciones;
    char palabra[100];
    double relevancia;
    List* posicionPalabra; //Lista que guarda las posiciones de la palabra en distintos puntos del texto
    List* libroAsociado; //Lista que guarda los libros asociados a esta palabra
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

//Función booleana que verifica si la palabra pertenece a la lista de las palabras más usadas en los textos de habla inglesa
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


//Función que permite avanzar por el texto palabra por palabra
char* next_word (FILE *f) {
    char x[1024];
    if (fscanf(f, " %1023s", x) == 1)
        return strdup(x);
    else
        return NULL;
}

//Función que detecta si es necesario eliminar un caracter de un string
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
            string[k] = '\0'; //Se elimina la posición, siendo el nuevo fin del string
            i--;
        }
    }

    return string;
}

void contarPalabrasYCaracteres(FILE* texto, tipoLibro* nuevoLibro)
{
    tipoPalabra* palabraAuxiliar = (tipoPalabra*) malloc (sizeof(tipoPalabra));
    char* palabra = (char*) malloc (100*sizeof(char));
    palabra = next_word(texto); //Se empieza a buscar en la primera palabra del texto
    for (int i = 0; palabra[i]; i++)
        palabra[i] = tolower(palabra[i]); //se hace minúscula la palabra para un procesamiento más rápido
    int k = 0;
    int repetidas = 1;
    while (palabra) //Se entra a un ciclo while hasta que llegar al fin del texto
    {
        for (int i = 0; palabra[i]; i++)
            palabra[i] = tolower(palabra[i]);
        palabra = quitarCaracteres(palabra, "!?,;""':.1234567890/()-*¨{}[]<>|$%&_^°¬¿¡ÔÇ£ÔÇØ#&=~+’“”"); //Se eliminan los caracteres señalados entre las comillas

        nuevoLibro->cantPalabras++; //Se aumenta la cantidad de palabras del libro en una unidad
        for (int i = 0; i < strlen(palabra); i++)
            nuevoLibro->cantCaracteres++; //Por cada carácter de cada palabra, se aumenta en una unidad la cantidad de caracteres del libro
        
        if (!esComun(palabra) && palabra[0] != '\0') //Se entra al if si la palabra no está en la lista de palabras comunes y no es una palabra vacía, es decir, que comienze con un espacio
        {
            if (searchMap(nuevoLibro->mapaPalabras, palabra) == NULL) //pasa por aquí si no se encuentra la palabra en el mapa
            {
                tipoPalabra* nuevaPalabra = (tipoPalabra*) malloc (sizeof(tipoPalabra)); //Nueva variable que sirve para guardar los datos de la nueva palabra
                nuevaPalabra->posicionPalabra = createList(); //Se crea la lista de posiciones
                long *posicion = malloc(sizeof(long));
                *posicion = ftell(texto); //corresponde a la posición de la palabra en el texto, que es un valor entero
                pushBack(nuevaPalabra->posicionPalabra, posicion); //se agrega la posición a la lista
                //se agrega la posición a la lista
                nuevaPalabra->apariciones = 1;
                strcpy(nuevaPalabra->palabra, palabra);
                nuevaPalabra->relevancia = 0;
                insertMap(nuevoLibro->mapaPalabras, palabra, nuevaPalabra); //Se inserta la nueva palabra en el mapa
            }
            else //si no, se pasa por aquí
            {
                palabraAuxiliar = searchMap(nuevoLibro->mapaPalabras, palabra); //Variable auxiliar para ingresar al dato especificado
                palabraAuxiliar->apariciones++;
                long *posicion = malloc(sizeof(long));
                *posicion = ftell(texto);
                pushBack(palabraAuxiliar->posicionPalabra, posicion);
            }
        }
        palabra = next_word(texto); //Se avanza a la siguiente palabra
    }
}

void cargarDocumentos (char* idLibros, Map* mapaLibrosPorID, TreeMap* mapaLibrosPorTitulo)
{
    char limit[2] = " "; //Se define el límite para ir palabra por palabra
    char path[100];
    char linea[1024];
    char* titulo = (char*) malloc (100*sizeof(char));
    char* token = strtok(idLibros, limit); //Con el uso de la función strtok, avanzamos por las palabras separadas por espacio
    if (token != NULL)
    {
        while (token != NULL) //el while sigue hasta que no hayan más palabras en el string original
        {
            snprintf(path, sizeof(path), "books/%s.txt", token); //Esta función nos ayuda a definir una variable para abrir el texto con más facilidad,
                                                                 //en donde se inserta la variable en un string. En este caso, para ingresar directamente a una carpeta
            FILE* texto = fopen(path, "rt"); //Así, se abre el archivo con la dirección (path) del archivo
            if (texto == NULL) //Si el texto no se encuentra, se le avisa al usuario y se avanza al siguiente token
            {
                printf("Texto %s no encontrado!\n", path);
                token = strtok(NULL, limit);
                continue;
            }
            else 
                printf("Texto %s abierto con exito!\n", path);
            
            tipoLibro* nuevoTexto = (tipoLibro*) malloc (sizeof(tipoLibro));
            strcpy(nuevoTexto->id, token);
            
            fgets(linea, 1023, texto); //Se entra a la primera línea del texto
            titulo[0] = '\0';
            const char *start = strchr(linea, 'f') + 2; //Se define el inicio para copiar el string
            strncat(titulo, start, strcspn(start, "\n")); //Se copia la variable de start dentro del titulo
            titulo = quitarCaracteres(titulo, ";""'/()-*¨{}[]<>|$%_^°¬¿¡ÔÇ£ÔÇØ#&=~+’“”"); //Se eliminan del titulo todos los caracteres que "estorben"
            strcpy(nuevoTexto->titulo, titulo); //Finalmente se copia el titulo dentro del nuevo libro

            //Se inician las variables restantes
            nuevoTexto->cantCaracteres = 0;
            nuevoTexto->cantPalabras = 0;
            nuevoTexto->mapaPalabras = createMap(is_equal_string);
            contarPalabrasYCaracteres(texto, nuevoTexto);

            insertMap(mapaLibrosPorID, nuevoTexto->id, nuevoTexto); //Se inserta al mapa cuya key es la ID
            insertTreeMap(mapaLibrosPorTitulo, nuevoTexto->titulo, nuevoTexto); //Se inserta al árbol cuya key es el título del libro

            fclose(texto); //Se cierra el archivo
            token = strtok(NULL, limit); //Se avanza al siguiente token
        }
    }
}

double calcularRelevancia(tipoLibro *elLibro, double cont_apariciones, double total_libros,tipoPalabra * buscador)
{
    double a,b; 
    a= buscador->apariciones/elLibro->cantPalabras ; /* se divide la cantidad de apariciones de la palabra en el libro
                                                        por la cantidad total de palabras del libro*/
    
    b= logf(total_libros/cont_apariciones); /* se divide todos los libros cargador por la cantidad de libros en que
                                                aparece la palabra buscada*/
    double relevancia = a* b ; 
    return relevancia;
}

void BuscarPorPalabra(char* palabra, Map* MapaLibros, List* listaPrioridad)
{
    tipoLibro* libro = firstMap(MapaLibros);  //Ingreso al primer libro del mapa 
    Mheap* monticuloRelevancia = createMheap(); //Se utiliza para ordenar los libros segun la relevancia de la palabra ingresada
    double cont_apariciones = 0;
    double total_libros = 0;
    //En el while se cuentan todos los libros y los libros en los que aparece la palabra
    while(libro != NULL)
    {
        tipoPalabra* buscador_palabra = searchMap(libro->mapaPalabras, palabra);
        if(buscador_palabra != NULL)
        {
            cont_apariciones++;
        }
        total_libros++;
        libro = nextMap(MapaLibros);
    }
    libro = firstMap(MapaLibros); //vuelvo al primero de todos los libros
    while(libro != NULL)
    {
        tipoPalabra* buscador_palabra = searchMap(libro->mapaPalabras, palabra); //busco la palabra en el libro
        if(buscador_palabra != NULL )
        {
            //si la palabra está, le calculo la relevancia y se agrega al Heap
            buscador_palabra->relevancia = calcularRelevancia(libro, cont_apariciones, total_libros, buscador_palabra);
            heap_push(monticuloRelevancia, libro, buscador_palabra->relevancia );
        }
        libro = nextMap(MapaLibros); //Se mueve al siguiente libro
    }
    if(cont_apariciones == 0)
    {
        printf("La palabra ingresada no se encuentra en ningún libro\n");
        return;
    }
    tipoPalabra* buscador;
    tipoLibro* elLibro;
    while (monticuloRelevancia != NULL) 
        {
            elLibro = heap_top(monticuloRelevancia); //Ingreso al top del monticulo
            if(elLibro == NULL) break;
            heap_pop(monticuloRelevancia); //Elimino el top y se reordena el monticulo
            printf("Nombre del libro: %s\nID del libro: %s\n", elLibro->titulo, elLibro->id) ;
            buscador = searchMap(elLibro->mapaPalabras, palabra);//Busca la palabra en el libro
            printf("Relevancia de la palabra: %lf\n", buscador->relevancia);     
        }  
        return;         
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
        printf("ID: %s\nTitulo: %s\nCantidad de palabras: %.0lf\nCantidad de caracteres: %ld\n\n", libroAuxiliar->id, libroAuxiliar->titulo, libroAuxiliar->cantPalabras, libroAuxiliar->cantCaracteres);
        IDActual = nextTreeMap(LibrosPorTitulo);
    } 

    //Al finalizar de recorrer todos los documentos, se avisa al usuario y se retorna al menú.
    printf("Todos los documentos han sido procesados correctamente!\n");

}

void buscarLibroPorTitulo(TreeMap* LibrosPorTitulo, char* palabrasTitulo){
    Pair* TituloActual = firstTreeMap(LibrosPorTitulo); //Se comienza a analizar el primer documento.
    char limit[2] = " "; //Variable que ayudará a separar cada palabra insertada.
    char* token = strtok(palabrasTitulo, limit); //Comienza tomando la primera palabra insertada.
    int contPalabras = 0; //Contador de palabras ingresadas presentes en un cierto documento.
    int largoPalabra = strlen(token); 
    tipoLibro* datosLibro; 
    int cantPalabrasIngresadas = 0; 
    char* listaPalabrasIng[100]; //Aquí se almacena una palabra por posición.
    int i=0; //Para recorrer el arreglo listaPalabrasIng y convertir palabras en minuscula.
    int contTitulos=0; //Titulos que poseen todas las palabras ingresadas.
    
    if(!TituloActual){ //Si no hay un primer documento, es porque no hay cargados en el programa.
        printf("No existen documentados cargados.\n\n");
        return;
    }

    printf("\n");

    if (token != NULL) //Se procede con las palabras.
    {
        while(token != NULL){ //Se reconoce la cantidad de palabras ingresadas.
            largoPalabra = strlen(token);
            for(i=0;i<largoPalabra;i++){ //Conversión de palabra a minusculas estricto.
                token[i]=tolower(token[i]);
            }
            listaPalabrasIng[cantPalabrasIngresadas] = token; //Se inserta la palabra en minusculas al arreglo.
            token = strtok(NULL, limit); 
            cantPalabrasIngresadas++; //Se lleva el conteo de las palabras ingresadas para una comparación más facil.
        }

        while(TituloActual != NULL){ //Mientras hayan documentos, se van analizando uno tras uno.

            contPalabras = 0; //Se reinicia el contador de palabras al pasar a un nuevo documento.
            datosLibro = TituloActual->value; //Los datos del nuevo documento.

            for(i=0; i<cantPalabrasIngresadas;i+=1)
            {

              if(searchMap(datosLibro->mapaPalabras,listaPalabrasIng[i]) != NULL){ 
                contPalabras++;
              } //Si una palabra está en el libro, aumenta la cantidad de palabras presentes.

            }

            if(contPalabras == cantPalabrasIngresadas){ //Si las palabras presentes coinciden con las ingresadas, se muestra el libro que las contiene.
                printf("%s\n", datosLibro->titulo);
                contTitulos++;
            }

            TituloActual = nextTreeMap(LibrosPorTitulo); //Una vez terminado con el libro, se avanza al siguiente.
        }

        if(contTitulos == 0) printf("Lo sentimos, pero ningun titulo posee todas las palabras.\n\n");
        //Si no hay titulos que contengan todas las palabras, se avisa al usuario.

        for(i=0;i<cantPalabrasIngresadas;i++){
            free(listaPalabrasIng[i]);
        } //Se procede a eliminar todas las palabras insertadas para un nuevo uso de la función.

    }

}

int comparadorApariciones(const void* a, const void* b)
{
    tipoPalabra* palabraA = *(tipoPalabra**) a; //obtener elemento al que apunta el puntero
    tipoPalabra* palabraB = *(tipoPalabra**) b;
    if (palabraA->apariciones < palabraB->apariciones) return -1;
    if (palabraA->apariciones == palabraB->apariciones) return 0;
    if (palabraA->apariciones > palabraB->apariciones) return 1;
}

void palabrasMasFrecuentes(Map* mapaLibrosPorID)
{
    char id[50];
    getchar();
    printf("Ingrese el id del libro ");
    scanf("%50[^\n]s", id);
    tipoLibro* libro = searchMap(mapaLibrosPorID, id);
    if (libro == NULL) 
    {
        printf("Libro no encontrado");
        return;
    }
    int sizeMapaPalabras = 0;
    tipoPalabra* palabra = firstMap(libro->mapaPalabras);
    while (palabra != NULL) //contar palabras del libro
    {
        //ya encontramos una palabra
        sizeMapaPalabras++;
        palabra = nextMap(libro->mapaPalabras);
    }
    if (sizeMapaPalabras == 0)
    {
        printf("Libro sin palabras");
        return;
    }
    tipoPalabra** arrayPalabras = (tipoPalabra**) malloc (sizeof(tipoPalabra*) * sizeMapaPalabras); //ocupar array para poder hacer qsort
    palabra = firstMap(libro->mapaPalabras);
    for (int i = 0; i < sizeMapaPalabras; i++)//copiar las palabras de MapaPalabras a arrayPalabras
    {
        arrayPalabras[i] = palabra;
        palabra = nextMap(libro->mapaPalabras);
    }
    qsort(arrayPalabras, sizeMapaPalabras, sizeof(tipoPalabra*), comparadorApariciones); //ordenar el arreglo de palabras de menor a mayor
    int contadorPalabras = 0;
    for(int i = sizeMapaPalabras-1; i >= 0; i--) //va retrocediendo desde la última posición hasta el comienzo del arreglo
    {
        double frecuencia = arrayPalabras[i]->apariciones/libro->cantPalabras; //cálculo de frecuencia
        printf("La palabra %s tiene una frecuencia de %f\n", arrayPalabras[i]->palabra, frecuencia);
        contadorPalabras++;
        if (contadorPalabras == 10) break; //solo necesitamos las 10 palabras más frecuentes, si es que el arreglo contiene más de 10 palabras
    }
    free(arrayPalabras);
}

void palabrasMasRelevantes(Map* MapaLibros)
{
    char nombre[100];
    getchar();
    printf("Ingrese el nombre del libro ");
    scanf("%100[^\n]s", nombre);
    tipoLibro* libro = firstMap(MapaLibros); //ingresa al primer libro de los cargados
    tipoLibro* libroBuscado = NULL ; 
    //El while busca el libro ingresado por el usuario
    while(libro != NULL)
    {
        if(strcmp(libro->titulo, nombre) == 0)
        {
            libroBuscado = libro ;
            break;
        }
        libro = nextMap(MapaLibros);
    }
    libro = firstMap(MapaLibros);
    if(libroBuscado == NULL) 
    {
        printf("El libro ingresado no se encuentra en nuestro catálogo");
        return;
    }
    Mheap* monticuloRelevancia = createMheap(); //El heap guarda las palabras del libro buscado según su relevancia
    double totalLibros = 0;
    double aparicionEnLibros = 0;
    //Se calcula el total de los libros
     while(libro != NULL)
    {
        totalLibros++;
        libro = nextMap(MapaLibros);
    }
    
    tipoPalabra* buscador_palabra = firstMap(libroBuscado->mapaPalabras); //Se ingresa a la primera palabra del libro buscado
    while(buscador_palabra != NULL)
    {
        aparicionEnLibros = 0;
        libro = firstMap(MapaLibros);
        while(libro != NULL) //Se calcula la cantidad de veces que aparece la palabra en los libros
        {
            if(searchMap(libro->mapaPalabras, buscador_palabra->palabra) != NULL) 
            {
                aparicionEnLibros++;
            }
            libro = nextMap(MapaLibros);
        }
        if(buscador_palabra->relevancia == 0) //Se calcula la relevancia
        {
            double a;
            a= buscador_palabra->apariciones/libroBuscado->cantPalabras;
            double b;
            b= logf(totalLibros/aparicionEnLibros);
            buscador_palabra->relevancia =a * b;
        } 
        heap_push(monticuloRelevancia, buscador_palabra, buscador_palabra->relevancia );//Se agrega al montículo la palabra con su relevancia
        buscador_palabra = nextMap(libroBuscado->mapaPalabras);
    }  
    for(int i = 0; i < 10; i++)  //Imprime las 10 palabras más relevantes del libro
        {
            tipoPalabra* palabra = heap_top(monticuloRelevancia); //Se ingresa al top del monticulo
            heap_pop(monticuloRelevancia); //Se elimina el top y se reordena el monticulo
            printf("%s: %lf\n", palabra->palabra, palabra->relevancia); //Se imprime la palabra
        }
}

void mostrarContexto(char* titulo, char* palabra, TreeMap* mapaLibrosPorTitulo)
{
    Pair* searchData = searchTreeMap(mapaLibrosPorTitulo, titulo); //Se busca el libro
    if(searchData == NULL)
    {
        printf("No existe el texto que busca!\n"); 
        return;
    }

    tipoLibro* textoCargado = searchData->value; //Al ser un árbol binario, se crea una nueva variable tipoLibro que almacenará el value del pair en el treeMap
    char path[1024];
    snprintf(path, sizeof(path), "books/%s.txt", textoCargado->id); //Al igual que en la función cargar, creamos una variable path para abrir el archivo con más facilidad
    FILE* f = fopen(path, "r"); //Se abre el archivo

    tipoPalabra* word = searchMap(textoCargado->mapaPalabras, palabra); //Se busca la palabra indicada por el usuario
    if (word == NULL)
    {
        printf("La palabra indicada no esta en este texto\n");
        return;
    }

    //Inicialización de variables
    char* palabraAntes = (char*) malloc (sizeof(char));
    long* posicion = (firstList(word->posicionPalabra)); //Ingresamos a la primera posición en la lista
    while (posicion != NULL)
    {
        printf("--");
        word->palabra[0] = toupper(word->palabra[0]); //Hacemos mayúscula la primera letra de la palabra para diferenciar entre oraciones
        printf("%s ", word->palabra); //Se imprime la palabra en sí
        for (int i = 0; i < 100; i+=strlen(palabraAntes)) //Dentro de este ciclo if, iremos palabra por palabra que vienen después de la indicada
        {
            fseek(f, *posicion+i, SEEK_SET); //Se busca la posición de la siguiente palabra
            fscanf(f, "%1023s", palabraAntes); //Se escribe en palabraAntes la palabra encontrada en la posición de fseek
            if (strlen(palabraAntes) > 1)
                printf("%s ", palabraAntes); //Si la palabra contiene más de un caracter, se imprime
        }
        printf("--");
        printf("\n\n");
        posicion = nextList(word->posicionPalabra); //Se avanza a la siguiente posición de la lista adjunta
    }
}

int main()
{
    //Inicialización de variables
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
                    getchar();
                    cargarDocumentos(idLibros, mapaLibrosPorID, mapaLibrosPorTitulo);
                    break;
            case 2: mostrarDocsOrdenados(mapaLibrosPorTitulo);
                    break;
            case 3: getchar();
                    printf("Ingrese palabras separadas por espacios, el algoritmo buscara titulos que las contengan: \n");
                    scanf("%100[^\n]s", titulo);
                    getchar();
                    buscarLibroPorTitulo(mapaLibrosPorTitulo, titulo);
                    break;
            case 4: palabrasMasFrecuentes(mapaLibrosPorID);
                    break;
            case 5: palabrasMasRelevantes(mapaLibrosPorID);
                    break;
            case 6: getchar();
                    printf("Ingrese la palabra que desea buscar\n");
                    scanf("%100[^\n]s", palabra);
                    BuscarPorPalabra(palabra, mapaLibrosPorID, listaPrioridadPorPalabra);
                    break;
            case 7: getchar();
                    printf("Ingrese el titulo del libro: \n");
                    scanf("%100[^\n]s", titulo);
                    getchar();
                    printf("Ingrese la palabra que desea buscar: \n");
                    scanf("%100[^\n]s", palabra);
                    getchar();
                    mostrarContexto(titulo, palabra, mapaLibrosPorTitulo);
                    break;
            case 0: break;
        }
        printf("\n");
    }
    return 0;
}