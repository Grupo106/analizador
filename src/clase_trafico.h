/**
 * clase_trafico.h
 * ==========================================================================
 * Este modulo agrupa las estructuras de datos necesarias para representar
 * los patrones de tráfico a reconocer en los paquetes de red.
 *
 * Ademas brinda algunas macros para facilitar la manipulacion de direcciones
 * de red y mascaras de subred en hexadecimal.
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
    struct in_addr red; /* Direccion de red (la seccion de host debe estar 
                         * en cero) 
                         */
    in_addr_t mascara; /* Mascara de subred en formato hexadecimal */
};

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
    struct s_subnet *subredes; /* Array de subredes que definen la clase */
    int *puertos;       /* Array de puertos que definen la clase */
    struct s_clase *siguiente; /* Estructura siguiente en la lista */
} t_clase;

/**
 * MASCARA(n)
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
#define MASCARA(n) htonl(0xffffffff & ~(0xffffffff >> n))

/**
 * IN_NET(ip, red, mascara)
 * --------------------------------------------------------------------------
 * Devuelve 1 si la direccion *ip* pertenece a la red *red* con la mascara
 * de subred *mascara*. 
 *
 * ###Parametros
 * * ip: Debe ser del tipo in_addr_t (campo s_addr de la estructura in_addr).
 * * red: Debe ser del tipo in_addr_t (campo s_addr de la estructura in_addr).
 * * mascara: Debe estar en formato hexadecimal.
 *
 * Para comprobar que una direccion IP pertenece a una IP hay que hacer una
 * operación AND con la máscara de subred y el resultado debe ser igual a la
 * dirección de red.
 */
#define IN_NET(ip, red, mascara) ((ip & mascara) == red)


#endif /* CLASE_TRAFICO_H */
