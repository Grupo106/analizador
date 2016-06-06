/**
 * clase_trafico.h
 * ==========================================================================
 * Este modulo agrupa las estructuras de datos necesarias para representar
 * los patrones de tráfico a reconocer en los paquetes de red.
 */
#ifndef CLASE_TRAFICO_H
#define CLASE_TRAFICO_H

#include <arpa/inet.h>

/**
 * s_subnet
 * ---------------------------------------------------------------------------
 * Estructura que representa una subred. Basicamente tiene una dirección de red
 * y la máscara de subred en formato hexadecimal.
 *
 * Para comprobar que una direccion IP pertenece a una IP hay que hacer una
 * operación AND con la máscara de subred y el resultado debe ser igual a la
 * dirección de red.
 */
struct s_subnet {
    in_addr red; /* Direccion de red (la seccion de host debe estar en cero) */
    u_int32_t mascara; /* Mascara de subred en formato hexadecimal */
}

/**
 * t_clase
 * ---------------------------------------------------------------------------
 * Estructura de datos que representa los patrones a reconocer en los paquetes
 * de red para identificarlos como miembros de una clase de tráfico.
 *
 * Posee 2 campos que son la sumatoria de bytes separados en trafico de subida
 * y bajada. Entiendase como paquete de subida todo aquel paquete con dirección
 * INBOUND (ver paquete.h), es decir que tiene origen en la LAN y como destino
 * una dirección en Internet. El paquete de bajada por el contrario tiene
 * origen en una dirección de Internet y como destino un equipo en la LAN.
 */
typedef struct s_clase {
    int id;             /* Identificado de la clase. Obligatorio */
    int bytes_subida;   /* Sumatoria de bytes de paquetes que aplican a esta
                         * clase con direccion OUTBOUND
                         */
    int bytes_bajada;   /* Sumatoria de bytes de paquetes que aplican a esta
                         * clase con direccion INBOUND
                         */
    int cant_subredes;  /* Cantidad de subredes que tiene la clase */
    int cant_puertos;   /* Cantidad de puertos que tiene la clase */
    int protocolo;      /* Protocolo que aplica esta clase */
    s_subnet *subredes; /* Array de subredes que definen la clase */
    int *puertos;       /* Array de puertos que definen la clase */
    s_clase *siguiente; /* Estructura siguiente en la lista */
} t_clase;

/**
 * IN_NET(a, n, m)
 * --------------------------------------------------------------------------
 * Devuelve verdadero si la direccion *a* pertenece a la red *n* con la mascara
 * de subred *m*
 *
 * Para comprobar que una direccion IP pertenece a una IP hay que hacer una
 * operación AND con la máscara de subred y el resultado debe ser igual a la
 * dirección de red.
 */
#define IN_NET(a, n, m) ((in_addr) (a & m) == n)

#endif /* CLASE_TRAFICO_H */
