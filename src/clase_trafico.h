/**
 * clase_trafico.h
 * ==========================================================================
 * Este modulo agrupa las estructuras de datos necesarias para representar
 * los patrones de tráfico a reconocer en los paquetes de red.
 */

#ifndef CLASE_TRAFICO_H
#define CLASE_TRAFICO_H

#include <arpa/inet.h>
#include "paquete.h"

/*
 * CONSTANTES
 * ===========================================================================
 */
#define LONG_NOMBRE 32 /* Longitud maxima del nombre de clase de trafico */
#define LONG_DESCRIPCION 160 /* Longitud maxima de la descripcion de trafico.
                              * Se modifica esta cantidad para que el tamaño de
                              * la estructura de clase de trafico sea potencia
                              * de dos.
                              */
#define MASCARA_HOST htonl(0xffffffff); /* Mascara de subred para hosts con
                                         * todos los bits en uno.
                                         */
/* identificador para grupos de redes o puertos de host ubicados en la red 
 * local
 */
#define GRUPO_INSIDE 'i'
/* identificador para grupos de redes o puertos de host ubicados en INTERNET */
#define GRUPO_OUTSIDE 'o'

/*
 * ESTRUCTURAS
 * ===========================================================================
 */

/**
 * struct subret
 * ---------------------------------------------------------------------------
 * Estructura que representa una subred. Basicamente tiene una dirección de red
 * y la máscara de subred en formato hexadecimal.
 *
 * Para comprobar que una direccion IP pertenece a una IP hay que hacer una
 * operación AND con la máscara de subred y el resultado debe ser igual a la
 * dirección de red.
 */
struct subred {
    struct in_addr red; /* Direccion de red (la seccion de host debe estar en
                         * cero)
                         */
    in_addr_t mascara; /* Mascara de subred en formato hexadecimal */
};

/**
 * struct puerto
 * ---------------------------------------------------------------------------
 * Estructura que representa una puerto. Tiene un numero de puerto y numero
 * de protocolo (el 6 es TCP y el 17 es UDP)
 */
struct puerto {
    int numero; /* desde 1 a 65535 */
    int protocolo; /* 6 es TCP, 17 es UDP */
};

/**
 * struct clase
 * ---------------------------------------------------------------------------
 * Estructura de datos que representa los patrones a reconocer en los paquetes
 * de red para identificarlos como miembros de una clase de tráfico.
 *
 * ### Bytes de subida y de bajada
 * Posee 2 campos que son la sumatoria de bytes separados en trafico de subida
 * y bajada. Entiendase como paquete de subida todo aquel paquete con dirección
 * INBOUND (ver paquete.h), es decir que tiene origen en la LAN y como destino
 * una dirección en Internet. El paquete de bajada por el contrario tiene
 * origen en una dirección de Internet y como destino un equipo en la LAN.
 *
 * ### Grupos outside e inside
 * Las subredes y puertos se dividen en ´outside´ e ´inside.´ Estos la
 * pertenencia al grupo ´outside´ significa que la subred o puerto esta del
 * lado de Internet. Si la subred o puerto estan en el grupo ´inside´
 * significan que pertenecen a la red local. Generalmente se usará el grupo
 * ´outside´ para las coincidencias por hosts.
 */
struct clase {
    int id; /* Identificador de la clase.*/
    int bytes_subida; /* Sumatoria de bytes de paquetes que aplican a esta
                       * clase con direccion OUTBOUND
                       */
    int bytes_bajada; /* Sumatoria de bytes de paquetes que aplican a esta
                       * clase con direccion INBOUND
                       */
    int cant_puertos_outside; /* Cantidad de puertos que tiene el grupo
                               * outside.
                               */
    int cant_puertos_inside; /* Cantidad de puertos que tiene el grupo inside*/
    int cant_subredes_outside; /* Cantidad de subredes que tiene el grupo
                                * outside
                                */
    int cant_subredes_inside; /* Cantidad de subredes que tiene el grupo
                               * inside.
                               */
    struct subred *subredes_outside; /* Array de subredes que definen el grupo
                                      * outside.
                                      */
    struct subred *subredes_inside; /* Array de subredes que definen el grupo
                                     * inside.
                                     */
    struct puerto *puertos_outside; /* Array de puertos que definen el grupo
                                     * outside.
                                     */
    struct puerto *puertos_inside; /* Array de puertos que definen el grupo
                                    * inside.
                                    */
    char nombre[LONG_NOMBRE]; /* Nombre que identifica clase de trafico */
    char descripcion[LONG_DESCRIPCION]; /* Descripcion de clase de trafico */
};

#endif /* CLASE_TRAFICO_H */
