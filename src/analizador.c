#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "analizador.h"

/**
 * coincide(clase, paquete)
 * ---------------------------------------------------------------------------
 *  Compara un paquete con una clase de trafico. Si el paquete coincide con la
 *  clase devuelve 1, caso contrario devuelve cero
 */
int coincide(const struct clase *clase, const struct paquete *paquete)
{
    int i = 0;
    /*
     * Estas variables son banderas que se indican si un parametro del paquete
     * coincide con la clase
     *
     * Si esta en 1 significa que se encontro ese parametro en el paquete Por
     * defecto, si la clase no especifica ese parametro, se asume que se
     * encontro ese parametro, ya que al final se hace una operacion AND entre
     * todos los flags.
     */
    int redes_A = !(clase->cant_subredes_a > 0);
    int redes_B = !(clase->cant_subredes_b > 0);
    int puerto_A = !(clase->cant_puertos_a > 0);
    int puerto_B = !(clase->cant_puertos_b > 0);
    int protocolo = (clase->protocolo != 0)
                        ? paquete->protocolo == clase->protocolo
                        : 1;

    /* busco coincidencia en subredes a */
    while (!redes_A && i < clase->cant_subredes_a) {
        /* si la ip de origen o la de destino del paquete estan en la subred
         * definida en la clase. */
        redes_A = IN_NET(paquete->origen.s_addr, /* origen */
                         (clase->subredes_a + i)->red.s_addr,
                         (clase->subredes_a + i)->mascara) ||
                  IN_NET(paquete->destino.s_addr, /* destino */
                         (clase->subredes_a + i)->red.s_addr,
                         (clase->subredes_a + i)->mascara);
        i++;
    }

    /* busco coincidencia en subredes b */
    i = 0;
    while (!redes_B && i < clase->cant_subredes_b) {
        /* si la ip de origen o la de destino del paquete estan en la subred
         * definida en la clase. */
        redes_B = IN_NET(paquete->origen.s_addr, /* origen */
                         (clase->subredes_b + i)->red.s_addr,
                         (clase->subredes_b + i)->mascara) ||
                  IN_NET(paquete->destino.s_addr, /* destino */
                         (clase->subredes_b + i)->red.s_addr,
                         (clase->subredes_b + i)->mascara);
        i++;
    }

    /* busco coincidencia en puertos a */
    i = 0;
    while (!puerto_A && i < clase->cant_puertos_a) {
        /* si el puerto de origen o de destino del paquete es igual al que
         * define la clase */
        puerto_A = paquete->puerto_origen == *(clase->puertos_a + i) ||
                   paquete->puerto_destino == *(clase->puertos_a + i);
        i++;
    }

    /* busco coincidencia en puertos b */
    i = 0;
    while (!puerto_B && i < clase->cant_puertos_b) {
        /* si el puerto de origen o de destino del paquete es igual al que
         * define la clase */
        puerto_B = paquete->puerto_origen == *(clase->puertos_b + i) ||
                   paquete->puerto_destino == *(clase->puertos_b + i);
        i++;
    }
    /* solamente devuelve verdadero si todos los parametros que se compararon
     * son verdaderos. */
    return redes_A && redes_B && puerto_A && puerto_B && protocolo;
}

/**
 * imprimir(clases, cantidad)
 * ---------------------------------------------------------------------------
 *  Imprime las clases de trafico en la salida estandar en formato JSON.
 */
int imprimir(const struct s_analizador *analizador)
{
    return clases_to_json(stdout, analizador);
}

/**
 * to_json(file, clases, cantidad)
 * ---------------------------------------------------------------------------
 *  Escribe las clases de trafico en el archivo pasado por parametro en formato
 *  JSON
 */
int clases_to_json(FILE* file, const struct s_analizador *analizador)
{
    int i;
    int cantidad = analizador->cant_clases;
    struct clase* clases = analizador->clases;
    /* inicio array */
    fprintf(file, "[\n");
    /* imprimo clases */
    for (i = 0; i < cantidad; i++) {
        /* evito imprimir clases sin bytes */
        if ((clases + i)->bytes_subida || (clases + i)->bytes_bajada) {
            fprintf(file,
                    "  {\n"
                    "    \"id\": %d,\n"
                    "    \"nombre\": \"%s\",\n"
                    "    \"descripcion\": \"%s\",\n"
                    "    \"subida\": %d,\n"
                    "    \"bajada\": %d\n"
                    "  }%c\n",
                    (clases + i)->id,
                    (clases + i)->nombre,
                    (clases + i)->descripcion,
                    (clases + i)->bytes_subida,
                    (clases + i)->bytes_bajada,
                    i + 1 < cantidad ? ',' : ' ');
        }
    }
    /* fin array */
    fprintf(file, "]\n");
    return 0;
}

/**
 * analizar_paquete(s_analizador, paquete)
 * --------------------------------------------------------------------------
 *  Compara un paquete con las clases de trafico instaladas. En caso que no
 *  coincida con ninuna, se agrega a la clase por defecto.
 *
 *  Devuelve 1 en caso que haya coincidencia con alguna clase de trafico, 0 en
 *  caso de que se haya agregado el paquete a la clase por defecto.
 */
int analizar_paquete(const struct s_analizador* analizador,
        const struct paquete* paquete)
{
    int coincidencias = 0; /* flag para saber si el paquete tuvo alguna
                            * coincidencia*/
    int cmp = 0; /* almacena el resultado de la comparacion con la clase */
    int i = 0; /* iterador de clases */

    /* Por defecto, la primer clase es la clase por default, por lo tanto
     * empiezo a comparar con la clase 1 */
    for (i = 1; i < analizador->cant_clases; i++) {
        cmp = coincide(analizador->clases + i, paquete);
        coincidencias |= cmp;
        if (cmp) {
            if (paquete->direccion == ENTRANTE) {
                #pragma omp atomic
                (analizador->clases + i)->bytes_bajada += paquete->bytes;
            } /* paquete entrante */
            else {
                #pragma omp atomic
                (analizador->clases + i)->bytes_subida += paquete->bytes;
            } /* paquete saliente*/
        }
    }
    /* si no hubo coincidencia con ninguna clase lo agrego a la clase default*/
    if(!coincidencias) {
        if(paquete->direccion == ENTRANTE)
            #pragma omp atomic
            analizador->clases->bytes_bajada += paquete->bytes;
        else
            #pragma omp atomic
            analizador->clases->bytes_subida += paquete->bytes;
    }
    /* devuelvo si existio coincidencias con alguna clase de trafico que no sea
     * la default. */
    return coincidencias;
}
