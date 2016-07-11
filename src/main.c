#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <syslog.h>

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

/**
* terminar()
* ---------------------------------------------------------------------------
* Termina el programa de forma segura. Cierra conexion de base de datos.
*/
static void terminar(int);

/**
* manejar_interrupciones()
* ---------------------------------------------------------------------------
* Registra señales que serán enviadas por el sistema operativo para el correcto
* cierre de base de datos.
*/
void manejar_interrupciones();

int main() {
    struct clase *clases = NULL;
    int cant_clases = 0;
    /* Inicializo logs */
    openlog(PROGRAM, LOG_CONS | LOG_PID, LOG_LOCAL0);
    /* Muestro informacion del build */
    syslog(LOG_DEBUG, "Revision: %s (%s)", REVISION, BUILD_MODE);
    /* Conecto base de datos */
    int sqlret = bd_conectar();
    if (sqlret != 0) {
        fprintf(stderr, "Error al conectar a la base de datos");
        exit(sqlret);
    }
    cant_clases = get_clases(&clases, NULL);
    imprimir(clases, cant_clases);
    /* Registro señales necesarias para cerrar correctamente el programa y
     * para liberar recursos. */
    manejar_interrupciones();
    terminar(EXIT_SUCCESS);
    return EXIT_SUCCESS;
}

/**
* terminar()
* ---------------------------------------------------------------------------
* Cierra conexion de base de datos
*/
static void terminar(int signum) {
    syslog(LOG_WARNING, "Interrupción recibida %d\n", signum);
    bd_desconectar();
    closelog();
    exit(EXIT_SUCCESS);
}

/**
* manejar_interrupciones()
* ---------------------------------------------------------------------------
* Registra señales que serán enviadas por el sistema operativo para el correcto
* cierre de base de datos
*/
void manejar_interrupciones() {
    signal(SIGINT, terminar);
    signal(SIGTERM, terminar);
    signal(SIGQUIT, terminar);
}
