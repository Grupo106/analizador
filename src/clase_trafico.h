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

/*
 * ESTRUCTURAS
 * ===========================================================================
 */

/**
 * struct subnet
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
 * ### Grupos A y B
 * Las subredes y puertos se dividen en A y B. Generalmente se usará el grupo
 * A para las coincidencias por hosts. Solo se usará el subgrupo B cuando sea
 * necesario reconocer paquetes por el par A y B, es decir que el origen sea
 * el host X y el destino el host Y. En ese caso el host X estarà en el
 * grupo A y el host Y en el grupo B
 */
struct clase {
    int id; /* Identificado de la clase.*/
    int bytes_subida; /* Sumatoria de bytes de paquetes que aplican a esta
                       * clase con direccion OUTBOUND
                       */
    int bytes_bajada; /* Sumatoria de bytes de paquetes que aplican a esta
                       * clase con direccion INBOUND
                       */
    int cant_puertos_a; /* Cantidad de puertos que tiene el grupo A */
    int cant_puertos_b; /* Cantidad de puertos que tiene el grupo B */
    int cant_subredes_a; /* Cantidad de subredes que tiene el grupo A */
    int cant_subredes_b; /* Cantidad de subredes que tiene el grupo B */
    int protocolo; /* Protocolo que aplica esta clase */
    int *puertos_a; /* Array de puertos que definen el grupo A */
    int *puertos_b; /* Array de puertos que definen el grupo B */
    struct subred *subredes_a; /* Array de subredes que definen el grupo A */
    struct subred *subredes_b; /* Array de subredes que definen el grupo B */
    char nombre[LONG_NOMBRE]; /* Nombre que identifica clase de trafico */
    char descripcion[LONG_DESCRIPCION]; /* Descripcion de clase de trafico */
};

/**
* struct cidr_clase
* -----------------------------------------------------------------------------
* Relaciona una subred con un conjunto de clases que poseen ese rango en alguna
* de sus subredes.
*
* Estructura auxiliar que sirve para las búsquedas binarias de clases de
* tráfico ya que puede ordenarse por número de subred.
* */
struct cidr_clase {
    struct subred red;
    struct clase *clases;
    int cantidad;
};

/**
* struct puerto_clase
* -----------------------------------------------------------------------------
* Relaciona un puerto con un conjunto de clases que poseen ese numero de puerto
* en alguna de sus puertos.
*
* Estructura auxiliar que sirve para las búsquedas binarias de clases de
* tráfico ya que puede ordenarse por número de puerto.
* */
struct puerto_clase {
    int numero; 
    int cantidad;
    struct clase *clases;
};

/*
 * FUNCIONES
 * ===========================================================================
 */

/**
 * cidr_buscar_coincidencia(array, paquete, cantidad_clases)
 * ---------------------------------------------------------------------------
 * Busca la clase de trafico que mejor coincida con el paquete según su CIDR. 
 * En caso de que ninguna clase de trafico coincida, devuelve NULL.
 */
struct clase* cidr_buscar_coincidencia(const struct cidr_clase *array,
                                       const struct paquete *paquete,
                                       int cantidad_clases);

/**
 * puerto_buscar_coincidencia(array, paquete, cantidad_clases)
 * ---------------------------------------------------------------------------
 * Busca la clase de trafico que mejor coincida con el paquete según su número
 * de puerto. En caso de que ninguna clase de trafico coincida, devuelve NULL.
 */
struct clase* puerto_buscar_coincidencia(const struct cidr_clase *array,
                                         const struct paquete *paquete,
                                         int cantidad_clases);

/**
 * deducir(clases, paquete, cantidad_clases)
 * ---------------------------------------------------------------------------
 * Intenta deducir a que clase de trafico pertenece un paquete. Debe usarse en
 * caso que no exista coincidencia con las clases de trafico existentes.
 *
 * Busca en los puertos conocidos declarados por la IANA, en caso de
 * coincidencia crea una nueva clase de trafico en el array de clases de
 * trafico
 *
 * En caso de no encontrar coincidencia, se asume que pertenece a la clase 
 * DEFAULT.
 *
 * Devuelve clase que se aplicó la coincidencia
 */
struct clase* deducir(const struct clase *clases,
                      const struct paquete *paquete,
                      int *cantidad_clases);

/**
 * cidr_comparar(a, b)
 * --------------------------------------------------------------------------
 * Compara 2 cidr_clase y retorna:
 * * -1: *a* es menor que *b*
 * *  0: *a* es igual o contiene a *b*
 * *  1: *a* es mayor que *b*
 */
int cidr_comparar(const void *a, const void *b);

/**
 * puerto_comparar(a, b)
 * --------------------------------------------------------------------------
 * Compara 2 puerto_clase y retorna:
 * * -1: *a* es menor que *b*
 * *  0: *a* es igual o contiene a *b*
 * *  1: *a* es mayor que *b*
 */
int puerto_comparar(const void *a, const void *b);

/**
 * enum contiene
 * ---------------------------------------------------------------------------
 * Declara posibles retornos de la funcion cidr_contiene
 */
enum contiene {
    SIN_COINCIDENCIA, /* No existe coincidencia entre CIDR */
    A_CONTIENE_B, /* El primer CIDR contiene al segundo */
    B_CONTIENE_A /* El segundo CIDR contiene al primero */
};

/**
 * contiene_cidr(a, b)
 * --------------------------------------------------------------------------
 * Compara 2 cidr y retorna:
 * * A_CONTIENE_B: *a* es contiene a *b*
 * * B_CONTIENE_A: *b* es contiene a *a*
 * * SIN_COINCIDENCIA: no hay coincidencia entre a y b
 */
enum contiene cidr_contiene(const struct cidr_clase *a , 
                            const struct cidr_clase *b);

/**
 * cidr_insertar(array, clase, cmp)
 * ---------------------------------------------------------------------------
 * Inserta una clase en el array ordenado. Siempre se respetara el orden de las
 * direcciones de red declaradas en cidr_clase.
 */
void cidr_insertar(struct cidr_clase *array, const struct clase *clase);

/**
 * puntaje(clase, paquete)
 * ---------------------------------------------------------------------------
 * Compara un paquete con una clase de tráfico y obtiene un puntaje que
 * representa la cantidad y calidad de coincidencias. A mayor puntaje, mayor
 * cantidad de coincidencias.
 */
int puntaje(const struct clase*, const struct paquete*);

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
