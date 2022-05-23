#include <wchar.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "treemap.h"
#include "list.h"

typedef struct 
{
    char * string;
    int repeticiones;
    double frecuencia;     //puede que haya que cambiar el tipo de dato a un float o double
    double relevancia;
    char * contexto;    /*Guardar aqui la linea en donde se encuentra la palabra*/
}Palabra;

typedef struct
{
    char * ID;
    char * titulo;
    TreeMap * content;  //ignorar primera linea de cada libro
    unsigned long tamanyo;
    unsigned long cantidadPalabras;
    unsigned long long caracteres;
    /*Para estos dos creo que habría que pasarles la cantidad de repeticiones como key... creo????? y ponerle el hash a la funcion de */
    Palabra ** MasFrecuentes;     //palabras con más frecuencia, son 10 palabras, fre = cant Apariciones / total palabras en libro
    Palabra ** MasRelevantes;
}Libro;




int lower_than_string(void* key1, void* key2)
{
    char* k1=(char*) key1;
    char* k2=(char*) key2;
    if(strcmp(k1,k2)<0) return 1;
    return 0;
}



int higher_than_double(void * key1, void * key2)
{
    if(*(double*)key1 > *(double*)key2) return 1;
    return 0;
}


/*SubFuncion que utiliza la funcion "Mostrar documentos ordenados", esta indica la id, y los nombres ordenados alfabeticamente, además
indica la cantidad de palabras y caracteres el libro.*/
void Mostrar_Libro (Libro * libro)
{
    printf("EL ID DEL LIBRO ES: %s\n", libro->ID);
    /*
    printf("\nEl libro contiene:\n");
    Palabra * pal = firstTreeMap(libro->content)->value;
    printf("Palabra: %s\nRepeticiones: %i\nFrecuencia: %lf\n\n", pal->string , pal->repeticiones , pal->frecuencia);
    for (size_t cont = 0 ; cont < libro->tamanyo - 1 ; cont++)
    {
        pal = nextTreeMap(libro->content)->value;
        if (pal->string == NULL)
        {
            break;
        }
        printf("Palabra: %s\nRepeticiones: %i\nFrecuencia: %lf\n\n", pal->string , pal->repeticiones , pal->frecuencia);
    }*/
}


//Esta funcion se encarga de mostrar las las palabras mas frecuentes 
void Mostrar_Frecuentes (Palabra ** pal)
{
    printf("\nMostrando las mas frecuentes:\n");
    Palabra * palabra;
    for (size_t cont = 0 ; cont < 10 ; cont += 1)
    {
        palabra = pal[cont];
        printf("Palabra: %s\nRepeticiones: %i\nFrecuencia: %lf\n\n", palabra->string , palabra->repeticiones , palabra->frecuencia);
    }
}



char* next_word (FILE *f)
{
    char x[1024];
    if (fscanf(f, " %1023s", x) == 1)
        return strdup(x);
    else
        return NULL;
}


//Dentro de esta funcion se cuenta la cantidad de palabras que poseen los libros medienate una variable auxiliar y un ciclo for. 
void Calcular_Cantidad_Palabras (Libro * libro)
{
    //printf("entra a calcular cantidad palabras\n");
    libro->cantidadPalabras = 0;
    Palabra * palabra = firstTreeMap(libro->content)->value;
    libro->cantidadPalabras = palabra->repeticiones;
    for (size_t cont = 0 ; cont < libro->tamanyo - 1 ; cont++)
    {
        palabra = nextTreeMap(libro->content)->value;
        libro->cantidadPalabras += palabra->repeticiones;
        if (palabra->string == NULL)
        {
            break;
        }
    }

    //printf("el libro tiene %lu palabras\n", libro->cantidadPalabras);
}


/* Esta funcion es la encargada de calcular la frecuencia de las palabras dentro de los libros, se obtiene dividiendo las veces que la palabra 
 * se repite con el total de palabras que posee el libro.
 *
*/
void Calcular_Frecuencias (Libro * libro)
{
    //printf("entra a calcular frecuencias\n");
    Palabra * palabra = firstTreeMap(libro->content)->value;
    palabra->frecuencia = ((double)palabra->repeticiones)/((double)libro->cantidadPalabras);
    //printf("se obtiene la frecuencia dividiendo %i con %lu\nEl resultado es: %lf\n\n", palabra->repeticiones , libro->cantidadPalabras , palabra->frecuencia);
    for (size_t cont = 0 ; cont < libro->tamanyo - 1 ; cont++)
    {
        palabra = nextTreeMap(libro->content)->value;
        palabra->frecuencia = ((double)palabra->repeticiones)/((double)libro->cantidadPalabras);
        if (palabra->string == NULL)
        {
            break;
        }
    }
}


/* Dentro de esta funcion se guardan los datos de los libros tales como la cantidad de palabras distintas y 
 * la cantidad de caracteres que posee, ademas dentro de esta funcion se llama a las funciones encargadas de calcular la cantidad y frecuencia de las palabras.
 *
*/
void Llenar_Libro (Libro * libro , FILE * f)
{
    Palabra * PalAux;
    Pair * aux = (Pair *) malloc(sizeof(Pair));

    while (1)
    {
        Palabra * pal = (Palabra *) malloc(sizeof(Palabra));
        pal->string = next_word(f);
        if (pal->string == NULL)
        {
            break;
        }
        aux = searchTreeMap(libro->content , pal->string);
        libro->caracteres += strlen(pal->string);

        if (aux != NULL)
        {
            PalAux = aux->value;
            PalAux->repeticiones += 1;
            //printf("la palabra %s se encuentra repetida %i veces\n", PalAux->string , PalAux->repeticiones);
            insertTreeMap(libro->content , PalAux->string , PalAux);
            continue;
        }
        pal->repeticiones = 1;
        libro->tamanyo += 1;
        //printf("la palabra %s se ha encontrado por primera vez\n", pal->string);
        insertTreeMap(libro->content , pal->string , pal);
    }

    //printf("el libro tiene %i palabras distintas\n", libro->tamanyo);
    //printf("el libro tiene %lu caracteres en total sin contar los espacios\n", libro->caracteres);
    Calcular_Cantidad_Palabras(libro);      /*Puede que quede mejor estéticamente dejar esto en la funcion cargar libros, no debería cambiar nada en teoría*/
    Calcular_Frecuencias(libro);            /*Puede que quede mejor estéticamente dejar esto en la funcion cargar libros, no debería cambiar nada en teoría*/
}


/* La funcion bubble sort ordena las posiciones del arerglo con su contenido dentro al ser un arreglo de tipo palabra**
 * esta funcion se utiliza dentro de la funcion mayores frecuencias
 *
*/
void bubbleSort (Palabra ** array , int size)
{
    for (size_t step = 0 ; step < size - 1 ; step += 1)
    {
        for (size_t i = 0 ; i < size - step - 1 ; i += 1)
        {
            if (array[i]->frecuencia < array[i + 1]->frecuencia)
            {
                Palabra * temp = array[i];
                array[i] = array[i + 1];
                array[i + 1] = temp;
            }
        }
    }
}


/*
    la funcion mayores frecuencias crea un arreglo el cual va asignando las frecuencias de las palabras,
    para luego ser ordenadas de MAYOR a MENOR con la funcion bubble sort y asi asignarle el puntero a libro->mas frecuentes;
*/
void Mayores_Frecuencias (Libro * libro)
{
    Palabra ** array = (Palabra **) malloc (libro->tamanyo * sizeof(Palabra*));
    Palabra * palabra = firstTreeMap(libro->content)->value;

    for (size_t i = 0 ; i < libro->tamanyo ; i += 1)
    {
        array[i] = palabra;
        if (i == libro->tamanyo - 1)
        {
            break;
        }
        palabra = nextTreeMap(libro->content)->value;
    }

    bubbleSort(array , libro->tamanyo);
    libro->MasFrecuentes = array;
}


/* Esta función indica las 10 palabras que se repiten con mayor frecuencia del texto que indica el usuario*/
void Mayores_Relevancias(Libro * libro)
{
    Palabra ** array = (Palabra **) malloc (libro->tamanyo * sizeof(Palabra*));
    Palabra * palabra = firstTreeMap(libro->content)->value;

    for (size_t i = 0 ; i < libro->tamanyo ; i += 1)
    {
        //Calcular_Relevancia();
        array[i] = palabra;
        if (i == libro->tamanyo - 1)
        {
            break;
        }
        palabra = nextTreeMap(libro->content)->value;
    }

    bubbleSort(array , libro->tamanyo);
    libro->MasFrecuentes = array;
}

/* Función principal que lee los id de los textos que ingresa el usuario */
void Cargar_Libros(TreeMap * MapaLibros , int * cantidadDocumentos)
{
    char IDs[1024];
    char titulo[1024];
    char separador[2] = " ";
    char * IDActual;
    int largoTitulo = 0;

    printf("Ingrese los IDs de los libros separados por un espacio e incluyendo el .txt: ");
    fflush(stdin);
    gets(IDs);
    IDActual = strtok(IDs , separador);

    while (IDActual != NULL)
    {
        //printf("ID libro: %s\n", IDActual);
        FILE * arch = fopen(IDActual , "r");
        if(arch == NULL)
        {
            printf("El ID %s ingresado no existe\n", IDActual);
            //return; puede que solo queramos dejar el continue
            //pedir otro libro

            printf("Ingrese un ID valido: ");
            fflush(stdin);
            gets(IDs);
            IDActual = strtok(IDs , separador);
            continue;
        }

        Libro * libro = (Libro *) malloc (sizeof(Libro));
        libro->ID = (char *) malloc ((strlen(IDActual) + 1) * sizeof(char));
        strcpy(libro->ID , IDActual);
        //strcpy(libro->ID , IDActual);
        //printf("EL ID DEL LIBRO ES: %s\n", libro->ID);  //prueba id
        libro->tamanyo = 0;
        libro->caracteres = 0;
        libro->content = createTreeMap(lower_than_string);


        fgets(titulo , 1024 , arch);
        largoTitulo = strlen(titulo) + 1;
        libro->titulo = (char *) malloc (largoTitulo * sizeof(char));
        strncpy(libro->titulo , titulo, largoTitulo-2);
        //printf("el titulo es: %s\n", libro->titulo);

        Llenar_Libro(libro , arch);     /*Hay que pulir para ignorar signos de exclamación, puntos y mayúsculas*/
        Mayores_Frecuencias(libro);     /*Recorrer árbol entero y sacar top 10 de las más frecuentes usando el repeticiones de Palabra*/
        //Mayores_Relevancias(libro);     /*Recorrer árbol entero y sacar top 10 de las más relevantes usando la fórmula: ((ocurrencias de la palabra escogida en el doc)/(cantidad de palabras en el doc)) * log(nro de docs/docs con la palabra escogida)*/
        //Mostrar_Libro(libro);
        //Mostrar_Frecuentes(libro->MasFrecuentes);
        insertTreeMap(MapaLibros , libro->ID , libro);
        //printf("EL ID DEL LIBRO ES: %s\n", libro->ID);  //prueba id
        IDActual = strtok(NULL , separador);
        (*cantidadDocumentos) += 1;
    }
}


/* Esta funcion es la encargada de mostrar por pantalla los titulos de los libros cargados en orden alfabetico, 
 * ademas muestra su ID, cantidad de palabras y la cantidad de cararcteres que posee cada libro.
*/
void Mostrar_Docs_Ordenados (TreeMap * MapaLibros , int cantidadDocumentos)
{
    Pair * pairAux = firstTreeMap(MapaLibros);
    Libro * libro = pairAux->value;
    for (size_t cont = 0 ; cont < cantidadDocumentos ; cont++)
    {
        //Mostrar_Libro(libro);
        printf("ID: %s\n", libro->ID);
        printf("Titulo: %s\n", libro->titulo);
        Calcular_Cantidad_Palabras(libro);
        printf("Cantidad de caracteres sin contar espacios: %llu\n\n", libro->caracteres);
        pairAux = firstTreeMap(MapaLibros);
        if (pairAux == NULL)
        {
            break;
        }
        libro = pairAux->value;
    }
}

/* La funcion se encarga de mostrar las 10 palabras mas frecuentes de un libro ingresado por el usuario, 
 * muestra por pantalla la palabra, el numero de veces que se repite y su frecuencia.
 *
*/
void Ingresar_Libro_Buscar_Frecuentes (TreeMap * MapaLibros , int cantidadDocumentos)
{
    char ID[32];
    Pair * aux = firstTreeMap(MapaLibros);
    Libro * libro = aux->value;
    //printf("el id del primer libro es %s\n", libro->ID);

    printf("Ingrese el ID del libro que quiere buscar con el .txt(tienen que ser menos de 16 caracteres): ");
    fflush(stdin);
    scanf("%s", ID);

    for (size_t cont = 0 ; cont < cantidadDocumentos ; cont += 1)
    {
        //char * IDAux = (char *) malloc ((strlen(libro->ID)+1) * sizeof(char));
        //strcpy(IDAux , libro->ID);

        //printf("se compara %s y %s\n",ID , aux->key);
        if (strncmp(ID , aux->key , strlen(ID)) == 0)
        {
            //printf("id libro : %s\n", libro->ID);
            //printf("key libro : %s\n", aux->key);
            //printf("cantidad de palabras distintas del libro es : %lu\n", libro->tamanyo);
            printf("\nMostrando las mas frecuentes:\n");
            for (int cont = 0 ; cont < 10 ; cont += 1)
            {
                //printf("cont = %i\n", cont);
                if(cont == libro->tamanyo)
                {
                    return;
                }
                printf("Palabra: %s\nRepeticiones: %i\nFrecuencia: %lf\n\n", libro->MasFrecuentes[cont]->string , libro->MasFrecuentes[cont]->repeticiones , libro->MasFrecuentes[cont]->frecuencia);
            }
            
            //strcpy(libro->ID , aux->key);
            //printf("id libro final: %s\n", libro->ID);
            //printf("key libro final: %s\n", aux->key);
            return;
        }
        else
        {
            //printf("comparacion falla\n");
            aux = nextTreeMap(MapaLibros);
            if (aux == NULL)
            {
                break;
            }
            libro = aux->value;
            //printf("id libro anterior: %s\n", IDAux);
        }
    }

    printf("no se ha encontrado el ID ingresado");
}


//Dentro de esta funcion se revisa si la palabra se encuentra dentro de los textos y retorna la cantidad de textos que poseen la palabra
int BuscarEnLibro (TreeMap * MapaLibros , int cantidadDocumentos , char * palabra)
{
    int documentosContienenP = 0;
    Libro * libroTemp = firstTreeMap(MapaLibros)->value;
    Pair * pairBusqueda = searchTreeMap(libroTemp->content , palabra);
    if (pairBusqueda != NULL)
    {
        documentosContienenP += 1;
    }

    for (size_t cont = 1 ; cont < cantidadDocumentos ; cont += 1)
    {
        libroTemp = nextTreeMap(MapaLibros)->value;
        pairBusqueda = searchTreeMap(libroTemp->content , palabra);
        if (pairBusqueda != NULL)
        {
            documentosContienenP += 1;
        }
    }
    
    return documentosContienenP;
}


/*FrecuenciaDeP * log(nroDocumentos/documentosContienenP)   
Esta funcion calcula la relevancia de las palabras dentro de el texto. */
void Calcular_Relevancia(TreeMap * MapaLibros , int cantidadDocumentos, Libro * libro)
{
    int documentosContienenP;
    libro->MasRelevantes = (Palabra **) malloc (libro->tamanyo * sizeof(Palabra*));
    for (size_t cont = 0 ; cont < libro->tamanyo ; cont += 1)
    {
        documentosContienenP = BuscarEnLibro(MapaLibros , cantidadDocumentos , libro->MasFrecuentes[cont]->string);
        //printf("hay %i doc que tienen %s\n", documentosContienenP , libro->MasFrecuentes[cont]->string);
        libro->MasFrecuentes[cont]->relevancia = (double)libro->MasFrecuentes[cont]->frecuencia * log((double)cantidadDocumentos/(double)documentosContienenP);
        libro->MasRelevantes[cont] = libro->MasFrecuentes[cont];
        //printf("Palabra: %s\nRepeticiones: %i\nRelevancia: %lf\n\n", libro->MasRelevantes[cont]->string , libro->MasRelevantes[cont]->repeticiones , libro->MasRelevantes[cont]->relevancia);
    }
}


/*Dentro de esta funcion se llama la funcion encargada de calcular la relevancia, pero su objetivo es mostrar las 10 palabras mas relevantes de los textos, 
muestra por pantalla la palabra en cuestion, las veces que se repite y su relevancia dentro del texto.*/
void Mostrar_Relevantes (TreeMap * MapaLibros , int cantidadDocumentos)
{
    char tituloInput[128];
    char palabraInput[32];

    printf("Introduzca el titulo del libro: ");
    fflush(stdin);
    gets(tituloInput);

    char * titulo = (char *) malloc (strlen(tituloInput)+1 * sizeof(char));
    strcpy(titulo , tituloInput);
    printf("Se ingreso %s\n", titulo);

    Pair * pairTemp = firstTreeMap(MapaLibros);
    Libro * libro = pairTemp->value;
    for (size_t cont = 0 ; cont < cantidadDocumentos ; cont++)
    {
        if (strcmp(libro->titulo , titulo) == 0)
        {
            break;
        }
        pairTemp = nextTreeMap(MapaLibros);
        if (pairTemp == NULL)
        {
            printf("No se ha encontrado el titulo ingresado.\n");
            return;
        }
        libro = pairTemp->value;
    }

    Calcular_Relevancia(MapaLibros , cantidadDocumentos , libro);
    for (size_t cont = 0 ; cont < 10 ; cont++)
    {
        printf("Palabra: %s\nRepeticiones: %i\nRelevancia: %lf\n\n", libro->MasRelevantes[cont]->string , libro->MasRelevantes[cont]->repeticiones , libro->MasRelevantes[cont]->relevancia);
    }
}


/*La funcion buscar por titulo muestra los nombres de los libros que contienen las palabras ingresadas por el usuario
    Se guardan las palarbas ingresadas separadas por espacios con la funcion strtok guardandolas en un treemap, 
    luego se comparan los titulos de los libros que tambien son separados por palabras, y cuando encuentra las coincidencias utilizan una bandera para verificar
    las coincidencias 
*/
void Buscar_Titulo (TreeMap * MapaLibros , int cantidadDocumentos)
{
    TreeMap * listaPalabras = createTreeMap(lower_than_string);
    int elementosLista = 0;
    int aciertos = 0;
    char cadena[1024];
    char separador[2] = " ";
    char * palabraActual;
    int largoTitulo = 0;
    int flagValido = 0;

    printf("Ingrese las palabras con las que quiere buscar el libro: ");
    fflush(stdin);
    gets(cadena);
    
    palabraActual = strtok(cadena , separador);
    while (palabraActual != NULL)
    {
        insertTreeMap(listaPalabras , palabraActual , palabraActual);
        elementosLista += 1;
        palabraActual = strtok(NULL , separador);
    }

    Libro * libro = firstTreeMap(MapaLibros)->value;
    for (size_t cont = 0 ; cont < cantidadDocumentos ; cont++)
    {
        flagValido = 1;
        char * palabraAux = firstTreeMap(listaPalabras)->value;
        int largoPalabra = strlen(palabraAux);
        palabraActual = (char *) malloc (largoPalabra+1 * sizeof(char));
        strcpy(palabraActual , firstTreeMap(listaPalabras)->value);

        TreeMap * palabrasTitulo = createTreeMap(lower_than_string);
        char * tituloAux = (char *) malloc ((strlen(libro->titulo)+1) * sizeof(char));
        strcpy(tituloAux , libro->titulo);
        char * palabraAuxTitulo;
        palabraAuxTitulo = strtok(tituloAux , separador);
        while (palabraAuxTitulo != NULL)
        {
            insertTreeMap(palabrasTitulo , palabraAuxTitulo , palabraAuxTitulo);
            palabraAuxTitulo = strtok(NULL , separador);
        }

        for (size_t elem = 0 ; elem < elementosLista ; elem++)
        {
            Pair * pairAux = searchTreeMap(palabrasTitulo , palabraActual);
            if (pairAux == NULL)
            {
                flagValido = 0;
                break;
            }
            Pair * posibleNulo = nextTreeMap(listaPalabras);
            if (posibleNulo == NULL)
            {
                break;
            }
            palabraActual = posibleNulo->value;
        }

        if (flagValido == 1)
        {
            printf("Posible Titulo: %s\n", libro->titulo);
        }

        Pair * pairAux2 = nextTreeMap(MapaLibros);
        if (pairAux2 == NULL)
        {
            break;
        }
        libro = pairAux2->value;
    }
}

/* No muestra en orden de relevancia
 *
 *
 * 
*/

/*
    La funcion buscar palabra
    ingresa una palabra y muestra los titulos de los libros e IDs de lso libros que contienen la palabra ingresada
    recorre las palabras de los libros buscando coincidencias, si encuentra las coincidencias, guarda los libros en un treemap y luego los muestra
*/
void Buscar_Palabra (TreeMap * MapaLibros , int cantidadDocumentos)
{
    char palabraInput[30];
    printf("ingrese la palabra que desea buscar: ");
    fflush(stdin);
    gets(palabraInput);

    char * palabraBuscar = (char *) malloc (strlen(palabraInput)+1 * sizeof(char));
    strcpy(palabraBuscar , palabraInput);
    printf("se ingreso %s\n", palabraBuscar);

    TreeMap * mapaResultado = createTreeMap(lower_than_string);
    int tamanyoResultado = 0;
    Pair * pairTemp = firstTreeMap(MapaLibros);
    Libro * libro = pairTemp->value;
    for (size_t cont = 0 ; cont < cantidadDocumentos ; cont++)
    {
        Pair * resultadoBusqueda = searchTreeMap(libro->content , palabraBuscar);
        if(resultadoBusqueda != NULL)
        {
            Palabra * palAux = resultadoBusqueda->value;
            insertTreeMap(mapaResultado , libro->ID , libro);
            tamanyoResultado += 1;
            printf("relevancia: %lf\n", palAux->relevancia);
            printf("se inserto %s\n", libro->ID);
        }
        
        pairTemp = nextTreeMap(MapaLibros);
        if (pairTemp == NULL)
        {
            break;
        }
        libro = pairTemp->value;
    }

    if (tamanyoResultado == 0)
    {
        printf("no se ha encontrado ningun libro con esa palabra\n");
        return;
    }

    printf("tamanyo resultado : %i\n", tamanyoResultado);
    printf("Los libros que se encontraron son: \n");
    pairTemp = firstTreeMap(mapaResultado);
    libro = pairTemp->value;
    for (size_t contRes = 0 ; contRes < tamanyoResultado ; contRes++)
    {
        printf("Titulo: %s\n", libro->titulo);
        printf("ID: %s\n\n", libro->ID);
        pairTemp = nextTreeMap(mapaResultado);
        if (pairTemp == NULL)
        {
            break;
        }
        libro = pairTemp->value;
    }
}

/*
    Esta función muestra las diferentes apariciones de cada palabra en el contexto del documento
*/

void Mostrar_Contexto(TreeMap * MapaLibros , int cantidadDocumentos)
{
    char tituloInput[128];
    char palabraInput[32];
    long * pos;

    printf("Introduzca el titulo del libro: ");
    fflush(stdin);
    gets(tituloInput);

    char * titulo = (char *) malloc (strlen(tituloInput)+1 * sizeof(char));
    strcpy(titulo , tituloInput);
    printf("Se ingreso %s\n", titulo);

    Pair * pairTemp = firstTreeMap(MapaLibros);
    Libro * libro = pairTemp->value;
    for (size_t cont = 0 ; cont < cantidadDocumentos ; cont++)
    {
        if (strcmp(libro->titulo , titulo) == 0)
        {
            break;
        }
        pairTemp = nextTreeMap(MapaLibros);
        if (pairTemp == NULL)
        {
            printf("No se ha encontrado el titulo ingresado.\n");
            return;
        }
        libro = pairTemp->value;
    }

    printf("Introduzca la palabra que desea encontrar: ");
    fflush(stdin);
    gets(palabraInput);

    char * palabra = (char *) malloc (strlen(palabraInput)+1 * sizeof(char));
    strcpy(palabra , palabraInput);
    printf("Se ingreso %s\n", palabra);


}



int main()
{
    TreeMap * MapaLibros = createTreeMap(lower_than_string);     //reservar memoria
    int cantidadDocumentos = 0;
    bool importado = false;
    int opciones = 1;

    while(opciones != 0)
    {
        printf("\nIngrese la opcion que desea utilizar\n\n");
        printf("1)  Cargar documentos\n");              //check
        printf("2)  Mostrar documentos ordenados\n");   //check
        printf("3)  Buscar libro por título\n");        //check
        printf("4)  Palabras con mayor frecuencia en un libro\n");      //check
        printf("5)  Palabras mas relevantes en un libro\n");            //check
        printf("6)  Buscar por palabra\n");             //check
        printf("7)  Mostrar palabra en su contenido dentro de un libro en especifico\n");       //falta
        printf("0)  EXIT!!\n\n");
        printf("Ingrese opcion: ");
        scanf("%d", &opciones);

        switch (opciones)
        {
        case 1: ;
            Cargar_Libros(MapaLibros , &cantidadDocumentos);
            //printf("la cantidad de documentos es : %i\n", cantidadDocumentos);
            importado = true;
            break;
        
        case 2:
            if (importado == false) printf("\nPrimero debe importar un archivo\n");
            else Mostrar_Docs_Ordenados(MapaLibros , cantidadDocumentos);      /*mostrar los libros con sus IDs en orden alfabético, junto con la cantidad de palabras que tiene cada uno de ellos, deberíamos usar una funcion o algo para ver eso, de toda*/
            break;
        
        case 3:
            if (importado == false) printf("\nPrimero debe importar un archivo\n");
            else Buscar_Titulo(MapaLibros , cantidadDocumentos);
            break;
        
        case 4:
            if (importado == false) printf("\nPrimero debe importar un archivo\n");
            else
            {
                Ingresar_Libro_Buscar_Frecuentes(MapaLibros , cantidadDocumentos);
            } 
            break;
        
        case 5:
            if (importado == false) printf("\nPrimero debe importar un archivo\n");
            else Mostrar_Relevantes(MapaLibros , cantidadDocumentos);
            break;
        
        case 6:
            if (importado == false) printf("\nPrimero debe importar un archivo\n");
            else Buscar_Palabra(MapaLibros , cantidadDocumentos);
            break;
        
        case 7:// Mostrar todos los productos
            if (importado == false) printf("\nPrimero debe importar un archivo\n");
            else Mostrar_Contexto(MapaLibros , cantidadDocumentos);
            break;
        
        case 0:// en el caso que la opcion sea 0 el programa termina
            return 0; 
        
        default:
            printf("\nOpcion no disponible, por favor intentelo de nuevo\n");   //falta pedir opcion de nuevo... creo
            continue;
            break;
        }
    }

    return 0;
}