#include "analizador.h"

/*
 * contiene_cidr(a, b)
 * --------------------------------------------------------------------------
 * Compara 2 cidr y retorna:
 * * A_CONTIENE_B: *a* es contiene a *b*
 * * B_CONTIENE_A: *b* es contiene a *a*
 * * SIN_COINCIDENCIA: no hay coincidencia entre a y b
 * * IGUALES: a y b son iguales
 */
enum contiene cidr_contiene(const struct cidr_clase *a,
                            const struct cidr_clase *b)
{
    /* verifico si son iguales */
    if (a->cidr.red.s_addr == b->cidr.red.s_addr &&
            a->cidr.mascara == b->cidr.mascara)
        return IGUALES;
    /* verifico si la ip de red de *b* esta en el rango de *a* */
    else if (IN_NET(b->cidr.red.s_addr, a->cidr.red.s_addr, a->cidr.mascara))
        return A_CONTIENE_B;
    /* verifico si la ip de red de *a* esta en el rango de *b* */
    else if (IN_NET(a->cidr.red.s_addr, b->cidr.red.s_addr, b->cidr.mascara))
        return B_CONTIENE_A;
    return SIN_COINCIDENCIA;
}

/**
 * subred_comparar(a, b)
 * --------------------------------------------------------------------------
 * Compara 2 cidr_clase y retorna:
 * * -1: *a* es menor que *b*
 * *  0: *a* es igual o contiene a *b*
 * *  1: *a* es mayor que *b*
 */
int subred_comparar(const void *x, const void *y)
{
    const struct subred *a, *b;
    a = (const struct subred*) x;
    b = (const struct subred*) y;
    /* verifico si son iguales */
    if (a->red.s_addr == b->red.s_addr &&
            a->mascara == b->mascara)
        return 0;
    /* si se contienen entre ellas, asumo que son iguales */
    else if (IN_NET(b->red.s_addr, a->red.s_addr, a->mascara))
        return 0;
    else if (IN_NET(a->red.s_addr, b->red.s_addr, b->mascara))
        return 0;
    /* solamente las comparo si las redes son disjuntas */
    return (a->red.s_addr < b->red.s_addr) ? -1 : 1;
}

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
