#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "analizador.h"


/*
 * prefijo
 * ---------------------------------------------------------------------------
 *  Obtiene la cantidad de bits de la mascara de subred pasada por parametro.
 */
int prefijo(u_int32_t mascara)
{
    int i = 0;
    if(mascara == MASCARA_HOST) {
        return 32;
    }
    while (i < 32) {
        if(mascara == GET_MASCARA(i))
            return i;
        i++;
    }
    return -1;
}

/**
 * coincide_subred
 * ---------------------------------------------------------------------------
 *  Compara las ips del paquete con un array de subredes de la clase de
 *  trafico.
 *
 *  Devuelve puntaje de coincidencia. A mayor puntaje, mejor coincidencia
 */
int coincide_subred(const struct paquete *paquete,
                    const struct subred *subredes, int cantidad, int grupo)
{
    int i = 0;
    int puntos = cantidad == 0;
    struct in_addr ip = paquete->ip_destino;

    /* determino si voy a usar la ip de origen o de destino del paquete para
     * la comparacion
     */
    if (grupo == GRUPO_OUTSIDE && paquete->direccion == ENTRANTE)
        ip = paquete->ip_origen;
    else if (grupo == GRUPO_OUTSIDE && paquete->direccion == SALIENTE)
        ip = paquete->ip_destino;
    else if (grupo == GRUPO_INSIDE && paquete->direccion == ENTRANTE)
        ip = paquete->ip_destino;
    else if (grupo == GRUPO_INSIDE && paquete->direccion == SALIENTE)
        ip = paquete->ip_origen;

    while (!puntos && i < cantidad) {
        if (en_subred(ip, (subredes + i)))
            puntos = prefijo((subredes + i)->mascara);
        i++;
    }
    return puntos;
}

/**
 * coincide_puerto
 * ---------------------------------------------------------------------------
 *  Compara los puertos del paquete con un array de puertos de la clase de
 *  trafico.
 *
 *  Devuelve puntaje de coincidencia. A mayor puntaje, mejor coincidencia
 */
int coincide_puerto(const struct paquete *paquete,
                    const struct puerto *puertos, int cantidad, int grupo)
{
    int i = 0;
    int coincide = cantidad == 0;
    int puerto;

    /* determino si voy a usar el puerto de origen o de destino del paquete
     * para la comparacion
     */
    if (grupo == GRUPO_OUTSIDE && paquete->direccion == ENTRANTE)
        puerto = paquete->puerto_origen;
    else if (grupo == GRUPO_OUTSIDE && paquete->direccion == SALIENTE)
        puerto = paquete->puerto_destino;
    else if (grupo == GRUPO_INSIDE && paquete->direccion == ENTRANTE)
        puerto = paquete->puerto_destino;
    else if (grupo == GRUPO_INSIDE && paquete->direccion == SALIENTE)
        puerto = paquete->puerto_origen;

    while (!coincide && i < cantidad) {
        coincide = puerto == (puertos + i)->numero;
        /* comparo por protocolo. El protocolo es cero es comodin. */
        coincide &= (puertos + i)->protocolo == 0 ||
                    paquete->protocolo == (puertos + i)->protocolo;
        if (coincide)
            return coincide + PUNTOS_COINCIDENCIA_PUERTO;
        i++;
    }
    return coincide;
}

/**
 * coincide(clase, paquete)
 * ---------------------------------------------------------------------------
 *  Compara un paquete con una clase de trafico. Si el paquete coincide con la
 *  clase devuelve 1, caso contrario devuelve cero
 *
 *  Devuelve puntaje de coincidencia. A mayor puntaje, mejor coincidencia
 */
int coincide(const struct clase *clase, const struct paquete *paquete)
{
    /*
     * Estas variables son banderas que se indican si un parametro del paquete
     * coincide con la clase
     *
     * Si esta en 1 significa que se encontro ese parametro en el paquete. Por
     * defecto, si la clase no especifica ese parametro, se asume que se
     * encontro ese parametro, ya que al final se hace una operacion AND entre
     * todos los flags.
     */
    int redes_O = coincide_subred(paquete,
                                  clase->subredes_outside,
                                  clase->cant_subredes_outside,
                                  GRUPO_OUTSIDE);
    int redes_I = coincide_subred(paquete,
                                  clase->subredes_inside,
                                  clase->cant_subredes_inside,
                                  GRUPO_INSIDE);
    int puerto_O = coincide_puerto(paquete,
                                   clase->puertos_outside,
                                   clase->cant_puertos_outside,
                                   GRUPO_OUTSIDE);
    int puerto_I = coincide_puerto(paquete,
                                   clase->puertos_inside,
                                   clase->cant_puertos_inside,
                                   GRUPO_INSIDE);

    /* solamente devuelve el puntaje si todos los parametros que se compararon
     * son verdaderos. */
    if (redes_O && redes_I && puerto_O && puerto_I)
        return redes_O + redes_I + puerto_O + puerto_I;
    else
        return 0;
}

/**
 * imprimir(clases, cantidad)
 * ---------------------------------------------------------------------------
 *  Imprime las clases de trafico en la salida estandar en formato JSON.
 */
int imprimir(const struct s_analizador *analizador)
{
    return clases_to_file(stdout, analizador);
}

/**
 * to_json(file, clases, cantidad)
 * ---------------------------------------------------------------------------
 *  Escribe las clases de trafico en el archivo pasado por parametro en formato
 *  JSON
 */
int clases_to_file(FILE* file, const struct s_analizador *analizador)
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


/*
 * sumar_bytes
 * ---------------------------------------------------------------------------
 *  Suma los bytes del paquete a la clase de trafico.
 */
void sumar_bytes(struct clase *clase, const struct paquete *paquete) {
    if (paquete->direccion == ENTRANTE) {
        #pragma omp atomic
        clase->bytes_bajada += paquete->bytes;
    }
    else if (paquete->direccion == SALIENTE) {
        #pragma omp atomic
        clase->bytes_subida += paquete->bytes;
    }
}


/**
 * analizar_paquete(s_analizador, paquete)
 * --------------------------------------------------------------------------
 *  Compara un paquete con las clases de trafico instaladas. En caso que no
 *  coincida con ninuna, se agrega a la clase por defecto.
 *
 *  Se agregan los bytes a la clase con la mejor coincidencia.
 *
 *  Devuelve 1 en caso que haya coincidencia con alguna clase de trafico, 0 en
 *  caso de que se haya agregado el paquete a la clase por defecto.
 */
int analizar_paquete(const struct s_analizador* analizador,
                     const struct paquete* paquete)
{
    int mayor_puntaje = 0;
    struct clase *mejor_coincidencia = NULL;
    int puntaje = 0; /* almacena el resultado de la comparacion con la clase */
    int i = 0; /* iterador de clases */

    /* Por defecto, la primer clase es la clase por default, por lo tanto
     * empiezo a comparar con la clase 1.
     */
    for (i = 1; i < analizador->cant_clases; i++) {
        puntaje = coincide(analizador->clases + i, paquete);
        if (puntaje > mayor_puntaje) {
            mayor_puntaje = puntaje;
            mejor_coincidencia = analizador->clases + i;
        }
    }

    /* con coincidencia */
    if(mayor_puntaje > 0)
        sumar_bytes(mejor_coincidencia, paquete);

    /* sin coincidencia */
    else
        sumar_bytes((analizador->clases), paquete);

    return mayor_puntaje > 0;
}
