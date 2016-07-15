#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <syslog.h>
#include <time.h>
#include <string.h>

#include "bd.h"
#include "analizador.h"

#ifndef REVISION
#define REVISION "DESCONOCIDA"
#endif /* REVISION */

#ifndef PROGRAM
#define PROGRAM "analizador"
#endif /* PROGRAM */

#ifdef DEBUG
#define BUILD_MODE "desarrollo"
#else
#define BUILD_MODE "produccion"
#endif /* BUILD_MODE */

#ifndef COPYLEFT
#define COPYLEFT "(c) Netcop 2016 - Universidad Nacional de la Matanza"
#endif /* PROGRAM */

#define ARGV_LENGHT 16 /* tamaño maximo de cadenas pasadas por parametro */

#define DEFAULT_SEGUNDOS 60 /* cantidad de segundos a analizar en caso de que
                             * no se hayan definido parametros.
                             */

/*
 * terminar()
 * ---------------------------------------------------------------------------
 *  Termina el programa de forma segura. Cierra conexion de base de datos.
 */
static void terminar();

/*
 * manejar_interrupciones()
 * ---------------------------------------------------------------------------
 *  Registra señales que serán enviadas por el sistema operativo para el
 *  correcto cierre de base de datos.
 */
void manejar_interrupciones();

/*
 * argumentos()
 * ---------------------------------------------------------------------------
 *  Maneja los argumentos pasados por parametro al programa. Devuelve cero en
 *  caso de exito, cualquier otro numero en caso de error.
 */
static void argumentos(int argc, const char* argv[], struct s_analizador *cfg);

/*
 * Configuracion del analizador. Contiene el array de clases de trafico
 * instaladas y la configuracion para la seleccion de paquetes.
 *
 * Lo declaro como variable global a este archivo porque lo necesito para
 * liberar la memoria de las clases de trafico en la funcion terminar()
 */
static struct s_analizador analizador;

int main(int argc, const char *argv[])
{
    int cantidad_paquetes;
    /* Inicializo logs */
    openlog(PROGRAM, LOG_CONS | LOG_PID, LOG_LOCAL0);
    /* Muestro informacion del build */
    syslog(LOG_DEBUG, "%s Revision: %s (%s)", PROGRAM, REVISION, BUILD_MODE);
    /* Parseo argumentos */
    argumentos(argc, argv, &analizador);
    /* Registro señales necesarias para cerrar correctamente el programa y para
     * liberar recursos. */
    manejar_interrupciones();
    /* Conecto base de datos */
    bd_conectar();
    /* obtengo clases */
    if (obtener_clases(&analizador) < 0) {
        fprintf(stderr, "Error al obtener las clases de trafico\n");
        exit(EXIT_FAILURE);
    }
    clock_t start = clock();
    /* analizo paquetes */
    cantidad_paquetes = obtener_paquetes(&analizador, analizar_paquete);
    /* imprimo resultado */
    imprimir(&analizador);
    clock_t end = clock();
    /* calculo tiempo de analisis. */
    double tiempo = (end - start) / (double) CLOCKS_PER_SEC;

#ifdef DEBUG
    printf("Se analizaron %d paquetes con %d clases en %.2f segundos\n",
           cantidad_paquetes,
           analizador.cant_clases,
           tiempo);
#endif

    syslog(LOG_DEBUG,
           "Se analizaron %d paquetes con %d clases en %.2f segundos",
           cantidad_paquetes,
           analizador.cant_clases,
           tiempo);
    terminar();
    return EXIT_SUCCESS;
}

/*
 * terminar()
 * ---------------------------------------------------------------------------
 * Cierra conexion de base de datos
 */
static void terminar()
{
    bd_desconectar();
    closelog();
    for(int i = 0; i < analizador.cant_clases; i++)
        free_clase(analizador.clases + i);
    free(analizador.clases);
    exit(EXIT_SUCCESS);
}

/*
 * handle
 * --------------------------------------------------------------------------
 * Maneja interrupciones
 */
static void handle (int signum)
{
    syslog(LOG_WARNING, "Interrupción recibida %d\n", signum);
    terminar();
}

/*
 * manejar_interrupciones()
 * ---------------------------------------------------------------------------
 *  Registra señales que serán enviadas por el sistema operativo para el
 *  correcto cierre de base de datos
 */
void manejar_interrupciones()
{
    signal(SIGINT, handle);
    signal(SIGTERM, handle);
    signal(SIGQUIT, handle);
}

/*
 * ayuda()
 * --------------------------------------------------------------------------
 *  Muestra mensaje de ayuda
 */
static void ayuda() {
    printf("Uso: %s [-h] | [-v] | [segundos] | [inicio fin]\n\n"
           "Este programa compara las clases de trafico intaladas con "
           "los paquetes capturados en un intervalo de tiempo especifico. "
           "Si no se especifica ningun parametro, se analizaran los paquetes "
           "recibidos desde los ultimos %u segundos."
           "\n\n"
           "Parametros:\n"
           "  -h, --help             Muestra esta ayuda.\n"
           "  -v, --version          Muestra numero de version.\n"
           "  segundos               Cantidad de segundos desde que se "
                                     "analizarán los paquetes\n"
           "  inicio fin             Intervalo de tiempo en los que se "
                                     "analizaran los paquetes en formato "
                                     "unixtime."
           "\n%s\n"
           , PROGRAM, DEFAULT_SEGUNDOS, COPYLEFT);
}

/*
 * argumentos()
 * ---------------------------------------------------------------------------
 *  Maneja los argumentos pasados por parametro al programa. Devuelve cero en
 *  caso de exito, cualquier otro numero en caso de error.
 *
 *  ### Posibles parametros
 *   * -h --help
 *   * -v --version
 *   * sin parametros: analiza los paquetes recibidos luego de DEFAULT_SEGUNDOS
 *   * un parametro numerico: se crea intervalo entre la cantidad segundos
 *                            pasada por parametro y el tiempo actual
 *   * dos parametros numericos: intervalo en formato unixtime (cantidad de
 *                               segundos desde el 1 de enero de 1970)
 */
static void argumentos(int argc, const char* argv[], struct s_analizador *cfg)
{
    unsigned int aux;
    /* inicio los valores por defecto */
    cfg->tiempo_inicio = time(NULL) - DEFAULT_SEGUNDOS;
    cfg->tiempo_fin = time(NULL);
    if (argc == 2) {
        /* -h --help */
        if(strncmp(argv[1], "-h", ARGV_LENGHT) == 0 ||
                strncmp(argv[1], "--help", ARGV_LENGHT) == 0) {
            ayuda();
            exit(EXIT_SUCCESS);
        }

        /* -v --version */
        if(strncmp(argv[1], "-v", ARGV_LENGHT) == 0 ||
                strncmp(argv[1], "--version", ARGV_LENGHT) == 0) {
            printf("%s - %s - %s\n", PROGRAM, REVISION, BUILD_MODE);
            exit(EXIT_SUCCESS);
        }

        /* segundos */
        if(sscanf(argv[1], "%u", &(aux)) != 1) {
            fprintf(stderr, "%s: Parámetro desconocido\n", argv[1]);
            ayuda();
            exit(EXIT_FAILURE);
        }
        cfg->tiempo_inicio = time(NULL) - aux;
    } else if (argc == 3) {
        /* intervalo */
        /* segundos */
        if(sscanf(argv[1], "%u", &(aux)) != 1) {
            fprintf(stderr, "%s: Parámetro desconocido\n", argv[1]);
            ayuda();
            exit(EXIT_FAILURE);
        }
        cfg->tiempo_inicio = aux;
        if(sscanf(argv[2], "%u", &(aux)) != 1) {
            fprintf(stderr, "%s: Parámetro desconocido\n", argv[2]);
            ayuda();
            exit(EXIT_FAILURE);
        }
        cfg->tiempo_fin = aux;
    } else if (argc > 3) {
        ayuda();
    }
}
