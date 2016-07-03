#include "analizador.h"


/**
 * contiene_cidr(a, b)
 * --------------------------------------------------------------------------
 * Compara 2 cidr y retorna:
 * * A_CONTIENE_B: *a* es contiene a *b*
 * * B_CONTIENE_A: *b* es contiene a *a*
 * * SIN_COINCIDENCIA: no hay coincidencia entre a y b
 * * IGUALES: a y b son iguales
 */
enum contiene cidr_contiene(const struct cidr_clase *a , 
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
