#include <assert.h>
#include "clase_trafico.h"

#define MASCARA_0  0x0
#define MASCARA_8  0xff000000
#define MASCARA_16 0xffff0000
#define MASCARA_21 0xfffff800
#define MASCARA_24 0xffffff00
#define MASCARA_27 0xffffffe0
#define MASCARA_31 0xfffffffe

/**
 * test_MASCARA
 * --------------------------------------------------------------------------
 * Prueba que la macro MASCARA devuelva correctamente la mascara de subred
 * en formato hexadecimal a partir de la cantidad de bits que contiene la
 * mascara de subred.
 */
void test_MASCARA(void) {
    assert(MASCARA(0) == MASCARA_0);
    assert(MASCARA(8) == MASCARA_8);
    assert(MASCARA(16) == MASCARA_16);
    assert(MASCARA(21) == MASCARA_21);
    assert(MASCARA(24) == MASCARA_24);
    assert(MASCARA(27) == MASCARA_27);
    assert(MASCARA(31) == MASCARA_31);
}

int main() {
    test_MASCARA();
    return 0;
}
