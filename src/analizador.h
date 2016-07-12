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
#include "paquete.h"
#include "clase_trafico.h"

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
    int cant_clases; /* cantidad de clases de clases de trafico */
    struct clase* clases; /* array de clases de trafico */
};

/*
 * struct cidr_clase
 * ---------------------------------------------------------------------------
 * Relaciona una subred con un conjunto de clases que poseen ese rango en
 * alguna de sus subredes.
 *
 * Estructura auxiliar que sirve para las búsquedas binarias de clases de
 * tráfico ya que puede ordenarse por número de subred.
 */
struct cidr_clase {
    struct subred cidr; /* subred que agrupa clases */
    int cantidad; /* cantidad de clases que contiene el array */
    struct clase *clases; /* array de clases coincidentes */
};

/*
 * struct puerto_clase
 * ---------------------------------------------------------------------------
 * Relaciona un puerto con un conjunto de clases que poseen ese numero de puerto
 * en alguna de sus puertos.
 *
 * Estructura auxiliar que sirve para las búsquedas binarias de clases de
 * tráfico ya que puede ordenarse por número de puerto.
 */
struct puerto_clase {
    int numero;  /* numero de puerto */
    int cantidad; /* cantidad de clases que contienen el puerto */
    struct clase *clases; /* array de clases coincidentes */
};

/*
 * struct coincidencia
 * ---------------------------------------------------------------------------
 * Agrupa las coincidencias de clases de tráfico que coinciden con el paquete
 */
struct coincidencia {
    int cantidad; /* cantidad de clases que coinciden*/
    struct paquete *paquete; /* puntero al paquete en cuestion */
    struct clase *clases; /* array de punteros coincidentes */
};

/*
 * FUNCIONES
 * ===========================================================================
 */

/**
 * coincide(clase, paquete)
 * ---------------------------------------------------------------------------
 * Compara un paquete con una clase de trafico. Si el paquete coincide con la
 * clase devuelve 1, caso contrario devuelve cero
 */
int coincide(const struct clase *clase, const struct paquete *paquete);

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
int puerto_buscar_coincidencia(const struct cidr_clase *array,
        const struct paquete *paquete,
        int cantidad_clases,
        struct coincidencia *coincidencia);
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
 * subred_comparar(a, b)
 * --------------------------------------------------------------------------
 * Compara 2 subredes y retorna:
 * * -1: *a* es menor que *b*
 * *  0: *a* es igual o contiene a *b*
 * *  1: *a* es mayor que *b*
 */
int subred_comparar(const void *a, const void *b);

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
    B_CONTIENE_A, /* El segundo CIDR contiene al primero */
    IGUALES /* Ambas redes sin iguales */
};

/**
 * contiene_cidr(a, b)
 * --------------------------------------------------------------------------
 * Compara 2 cidr y retorna:
 * * A_CONTIENE_B: *a* es contiene a *b*
 * * B_CONTIENE_A: *b* es contiene a *a*
 * * SIN_COINCIDENCIA: no hay coincidencia entre a y b
 * * IGUALES: Ambas redes son iguales
 */
enum contiene cidr_contiene(const struct cidr_clase *a ,
        const struct cidr_clase *b);

/*
 * cidr_insertar(array, clase, cantidad_clases)
 * ---------------------------------------------------------------------------
 * Inserta una clase en el array ordenado. Siempre se respetara el orden de las
 * direcciones de red declaradas en cidr_clase.
 *
 * Devuelve 0 si pudo insertar el elemento en el array, cualquier otro valor
 * en caso de error
 */
int cidr_insertar(struct cidr_clase *array,
        const struct clase *clase,
        int cantidad_clases);

/**
 * puntaje(clase, paquete)
 * ---------------------------------------------------------------------------
 * Compara un paquete con una clase de tráfico y obtiene un puntaje que
 * representa la cantidad y calidad de coincidencias. A mayor puntaje, mayor
 * cantidad de coincidencias.
 */
int puntaje(const struct clase*, const struct paquete*);

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
int clases_to_json(FILE* file, const struct s_analizador*);

/**
 * get_clases(**clases, *cfg)
 * ---------------------------------------------------------------------------
 *  Obtiene el array de clases de trafico que se utilizara en el analisis.
 *  Devuelve la cantidad de clases que contiene el array
 *
 *  ### Parametros:
 *    * clases: Puntero a un array donde se almacenaran las clases
 *    * cfg: Puntero a la configuracion del analizador que contiene los
 *           parametros por los cuales se seleccionaran las clases.
 */
int get_clases(struct s_analizador*);

/**
 * init_clase
 * --------------------------------------------------------------------------
 *  Inicializa una estructura de clase de trafico a valores por defecto
 */
void init_clase(struct clase *clase);

/**
 * free_clase
 * --------------------------------------------------------------------------
 *  Libera memoria ocupada por una clase de trafico
 */
void free_clase(struct clase *clase);

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
 * * ip: Debe ser del tipo in_addr_t (campo s_addr de la estructura in_addr).
 * * red: Debe ser del tipo in_addr_t (campo s_addr de la estructura in_addr).
 * * mascara: Debe estar en formato hexadecimal.
 *
 * Para comprobar que una direccion IP pertenece a una IP hay que hacer una
 * operación AND con la máscara de subred y el resultado debe ser igual a la
 * dirección de red.
 */
#define IN_NET(ip, red, mascara) ((ip & mascara) == red)

#endif /* ANALIZADOR_H */
