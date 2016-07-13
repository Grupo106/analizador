#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <syslog.h>
#include <time.h>

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

/*
 * terminar()
 * ---------------------------------------------------------------------------
 *  Termina el programa de forma segura. Cierra conexion de base de datos.
 */
static void terminar(int);

/*
 * manejar_interrupciones()
 * ---------------------------------------------------------------------------
 *  Registra señales que serán enviadas por el sistema operativo para el
 *  correcto cierre de base de datos.
 */
void manejar_interrupciones();

/*
 * Configuracion del analizador. Contiene el array de clases de trafico
 * instaladas y la configuracion para la seleccion de paquetes.
 */
static struct s_analizador analizador;

int main() {
    int cantidad_paquetes = 0;
    analizador.clases = NULL;
    analizador.cant_clases = 0;
    /* Inicializo logs */
    openlog(PROGRAM, LOG_CONS | LOG_PID, LOG_LOCAL0);
    /* Muestro informacion del build */
    syslog(LOG_DEBUG, "%s Revision: %s (%s)", PROGRAM, REVISION, BUILD_MODE);
    /* Registro señales necesarias para cerrar correctamente el programa y
     * para liberar recursos. */
    manejar_interrupciones();
    /* Conecto base de datos */
    int sqlret = bd_conectar();
    if (sqlret != 0) {
        fprintf(stderr, "Error al conectar a la base de datos");
        exit(sqlret);
    }
    /* obtengo clases */
    if (get_clases(&analizador) != 0) {
        fprintf(stderr, "Error al obtener las clases de trafico\n");
        exit(EXIT_FAILURE);
    }
    clock_t start = clock();
    /* analizo paquetes */
    cantidad_paquetes = bd_paquetes(&analizador, analizar_paquete);
    bd_desconectar();
    /* imprimo resultado */
    imprimir(&analizador);
    clock_t end = clock();
    /* calculo tiempo de analisis. */
    double tiempo = (end - start) / (double) CLOCKS_PER_SEC;
#ifdef DEBUG
    printf("Se analizaron %d paquetes en %.2f segundos",
            cantidad_paquetes, tiempo);
#endif
    syslog(LOG_DEBUG, "Se analizaron %d paquetes en %.2f segundos",
           cantidad_paquetes, tiempo);
    terminar(EXIT_SUCCESS);
    return EXIT_SUCCESS;
}

/*
 * terminar()
 * ---------------------------------------------------------------------------
 * Cierra conexion de base de datos
 */
static void terminar(int signum) {
    if(signum > 0)
        syslog(LOG_WARNING, "Interrupción recibida %d\n", signum);
    bd_desconectar();
    closelog();
    for(int i = 0; i < analizador.cant_clases; i++)
        free_clase(analizador.clases + i);
    free(analizador.clases);
    exit(EXIT_SUCCESS);
}

/*
 * manejar_interrupciones()
 * ---------------------------------------------------------------------------
 *  Registra señales que serán enviadas por el sistema operativo para el
 *  correcto cierre de base de datos
 */
void manejar_interrupciones() {
    signal(SIGINT, terminar);
    signal(SIGTERM, terminar);
    signal(SIGQUIT, terminar);
}
