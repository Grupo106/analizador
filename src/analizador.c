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
        int cantidad_clases)
{
    int i = 0;
    int cmp = 0;
    while (i < cantidad_clases) {
        cmp = subred_comparar(clase->subredes_a, array + i);
        i++;
    }
    return 1;
}
