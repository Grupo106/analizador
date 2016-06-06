#include <assert.h>
#include <arpa/inet.h>
#include "clase_trafico.h"

#define MASCARA_0  htonl(0x0)
#define MASCARA_8  htonl(0xff000000)
#define MASCARA_16 htonl(0xffff0000)
#define MASCARA_21 htonl(0xfffff800)
#define MASCARA_24 htonl(0xffffff00)
#define MASCARA_27 htonl(0xffffffe0)
#define MASCARA_31 htonl(0xfffffffe)

/**
 * test_MASCARA
 * --------------------------------------------------------------------------
 * Prueba que la macro MASCARA devuelva correctamente la mascara de subred
 * en formato hexadecimal a partir de la cantidad de bits que contiene la
 * mascara de subred.
 */
void test_MASCARA(void) {
    assert(GET_MASCARA(0) == MASCARA_0);
    assert(GET_MASCARA(8) == MASCARA_8);
    assert(GET_MASCARA(16) == MASCARA_16);
    assert(GET_MASCARA(21) == MASCARA_21);
    assert(GET_MASCARA(24) == MASCARA_24);
    assert(GET_MASCARA(27) == MASCARA_27);
    assert(GET_MASCARA(31) == MASCARA_31);
}

/**
 * test_in_net
 * --------------------------------------------------------------------------
 * Prueba que la funcion in_net devuelva 1 cuando una ip pertenece a una subred
 * y 0 cuando no.
 */
void test_in_net(void) {
    struct in_addr ip, net;
    u_int32_t mascara;

    /* 192.168.1.1 -> 192.168.1.0/24 VERDADERO */
    ip.s_addr = inet_addr("192.168.1.1");
    net.s_addr = inet_addr("192.168.1.0");
    mascara = MASCARA_24;
    assert(IN_NET(ip.s_addr, net.s_addr, mascara) == 1);

    /* 192.168.1.1 -> 192.168.2.0/24 FALSO */
    ip.s_addr = inet_addr("192.168.1.1");
    net.s_addr = inet_addr("192.168.2.0");
    mascara = MASCARA_24;
    assert(IN_NET(ip.s_addr, net.s_addr, mascara) == 0);

    /* 192.168.1.33 -> 192.168.1.0/27 FALSO */
    ip.s_addr = inet_addr("192.168.1.33");
    net.s_addr = inet_addr("192.168.1.0");
    mascara = MASCARA_27;
    assert(IN_NET(ip.s_addr, net.s_addr, mascara) == 0);

    /* 172.18.1.1 -> 172.16.0.0/12 VERDADERO */
    ip.s_addr = inet_addr("172.18.1.1");
    net.s_addr = inet_addr("172.16.0.0");
    mascara = GET_MASCARA(12);
    assert(IN_NET(ip.s_addr, net.s_addr, mascara) == 1);

    /* 10.200.0.10 -> 10.200.0.0/16 VERDADERO */
    ip.s_addr = inet_addr("10.200.0.10");
    net.s_addr = inet_addr("10.200.0.0");
    mascara = MASCARA_16;
    assert(IN_NET(ip.s_addr, net.s_addr, mascara) == 1);
}

int main() {
    test_MASCARA();
    test_in_net();
    return 0;
}
