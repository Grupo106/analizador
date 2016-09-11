/**
 * analizador.h
 * ==========================================================================
 * Este modulo agrupa las estructuras de datos necesarias para comparar los
 * paquetes capturados con los patrones de tráfico.
 *
 * Ademas brinda algunas macros para facilitar la manipulacion de direcciones
 * de red y mascaras de subred en hexadecimal.
 */
#ifndef ANALIZADOR_H
#define ANALIZADOR_H

#include <stdio.h>
#include <time.h>
#include "paquete.h"
#include "clase_trafico.h"

#define PUNTOS_COINCIDENCIA_PUERTO 5
#define LEN_ISO8601 32

/*
 * ESTRUCTURAS
 * ===========================================================================
 */

/*
 * struct s_analizador
 * ---------------------------------------------------------------------------
 * Configuracion del analizador. Contiene el array de clases de trafico
 * instaladas y la configuracion para la seleccion de paquetes.
 */
struct s_analizador {
    /* valor menor del intervalo de tiempo de paquetes a analizar. */
    time_t tiempo_inicio;
    /* valor mayor del intervalo de tiempo de paquetes a analizar. */
    time_t tiempo_fin;
    /* inicio y fin del intervalo en formato ISO8601*/
    char inicio[LEN_ISO8601];
    char fin[LEN_ISO8601];
    /* cantidad de clases de clases de trafico. */
    int cant_clases;
    /* array de clases de trafico. */
    struct clase* clases;
};

/*
 * FUNCIONES
 * ===========================================================================
 */

/*
 * prefijo
 * ---------------------------------------------------------------------------
 *  Obtiene la cantidad de bits de la mascara de subred pasada por parametro.
 */
int prefijo(u_int32_t mascara);

/**
 * coincide(clase, paquete)
 * ---------------------------------------------------------------------------
 * Compara un paquete con una clase de trafico. Si el paquete coincide con la
 * clase devuelve 1, caso contrario devuelve cero
 */
int coincide(const struct clase *clase, const struct paquete *paquete);

/**
 * imprimir(clases, cantidad)
 * ---------------------------------------------------------------------------
 *  Imprime las clases de trafico en la salida estandar en formato JSON.
 */
int imprimir(const struct s_analizador *analizador);

/**
 * to_json(file, clases, cantidad)
 * ---------------------------------------------------------------------------
 *  Escribe las clases de trafico en el archivo pasado por parametro en formato
 *  JSON
 */
int clases_to_file(FILE* file, const struct s_analizador*);

/**
 * analizar_paquete(s_analizador, paquete)
 * --------------------------------------------------------------------------
 *  Compara un paquete con las clases de trafico instaladas. En caso que no
 *  coincida con ninuna, se agrega a la clase por defecto.
 *
 *  Devuelve 1 en caso que haya coincidencia con alguna clase de trafico, 0 en
 *  caso de que se haya agregado el paquete a la clase por defecto.
 */
int analizar_paquete(const struct s_analizador*, const struct paquete*);

/*
 * MACROS
 * ===========================================================================
 */

/**
 * GET_MASCARA(n)
 * --------------------------------------------------------------------------
 * Obtiene la mascara de subred en formato hexadecimal a partir de *n* que
 * representa la cantidad de bits que contiene la direccion de red.
 *
 * Es necesario llamar a htonl porque las direcciones de red tienen el bit mas
 * significativo a la derecha (es decir el numero se representa al reves en
 * hexadecimal).
 *
 * Por ejemplo, la direccion 10.0.32.0/21 tiene 21 bits de mascara de subred
 * por lo tanto la mascara de subred es 255.255.248.0 y en formato hexadecimal
 * es 0xfffff800
 */
#define GET_MASCARA(n) htonl(0xffffffff & ~(0xffffffff >> n))

/**
 * IN_NET(ip, red, mascara)
 * --------------------------------------------------------------------------
 * Devuelve 1 si la direccion *ip* pertenece a la red *red* con la mascara
 * de subred *mascara*.
 *
 * ### Parametros
 * * ip: Debe ser del tipo struct in_addr
 * * red: Debe ser del tipo struct in_addr
 * * mascara: Debe estar en formato hexadecimal.
 *
 * Para comprobar que una direccion IP pertenece a una IP hay que hacer una
 * operación AND con la máscara de subred y el resultado debe ser igual a la
 * dirección de red.
 */
#define IN_NET(ip, red, mascara) ((ip.s_addr & mascara) == red.s_addr)

/*
 * en_subred(ip, *subred)
 * --------------------------------------------------------------------------
 *  Devuelve 1 si la ip pertenece a la subred.
 *
 *  Se implemento como macro para evitar tanto codigo repetido en el analizador
 *
 *  ### Parametros
 *    * ip: Debe ser del tipo struct in_addr
 *    * subred: Debe ser un puntero a struct subred
 *
 */
#define en_subred(ip, subred) IN_NET(ip, \
                                     subred->red,\
                                     subred->mascara)

#endif /* ANALIZADOR_H */
