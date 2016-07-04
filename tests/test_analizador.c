#include <assert.h>
#include <stdio.h>
#include <netinet/in.h>
#include "../src/analizador.h"

#define MASCARA_0  htonl(0x0)
#define MASCARA_8  htonl(0xff000000)
#define MASCARA_16 htonl(0xffff0000)
#define MASCARA_21 htonl(0xfffff800)
#define MASCARA_24 htonl(0xffffff00)
#define MASCARA_27 htonl(0xffffffe0)
#define MASCARA_31 htonl(0xfffffffe)

/**
 * test_mascara
 * --------------------------------------------------------------------------
 * Prueba que la macro MASCARA devuelva correctamente la mascara de subred
 * en formato hexadecimal a partir de la cantidad de bits que contiene la
 * mascara de subred.
 */
void test_mascara(void) {
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

/*
 * test_cidr_b_contiene_a
 * ---------------------------------------------------------------------------
 * Se prueba que la subred pasada como segundo parametro contenga a la red
 * pasada por primer parametro.
 *
 * En este caso de prueba se establecen los siguiente valores
 *
 * parametro       | direccion de red | mascara de subred | prefijo
 * --------------- + ---------------- + ----------------- + ------------
 * a               | 10.1.0.0         | 255.255.0.0       | 16
 * b               | 10.0.0.0         | 255.0.0.0         | 8
 */
void test_cidr_b_contiene_a() {
    enum contiene ret;
    struct cidr_clase a;
    struct cidr_clase b;
    /* configuro la red a */
    inet_aton("10.1.0.0", &(a.cidr.red)); /* red 10.1.0.0 */
    a.cidr.mascara = GET_MASCARA(16); /* mascara 255.255.0.0 */
    a.cantidad = 0; /* no importa las clases en este test */
    a.clases = NULL; /* no importa las clases en este test */
    /* configuro la red b */
    inet_aton("10.0.0.0", &(b.cidr.red)); /* red 10.1.0.0 */
    b.cidr.mascara = GET_MASCARA(8);/* mascara 255.0.0.0 */
    b.cantidad = 0; /* no importa las clases en este test */
    b.clases = NULL; /* no importa las clases en este test */
    /* Ejecuto tests */
    ret = cidr_contiene(&a, &b);
    assert(ret == B_CONTIENE_A);
}

/*
 * test_cidr_iguales
 * ---------------------------------------------------------------------------
 * Se prueba que la subred pasada como primer parametro sea igual a la red
 * pasada por segundo parametro.
 *
 * En este caso de prueba se establecen los siguiente valores
 *
 * parametro       | direccion de red | mascara de subred | prefijo
 * --------------- + ---------------- + ----------------- + ------------
 * a               | 10.0.0.0         | 255.0.0.0         | 16
 * b               | 10.0.0.0         | 255.0.0.0         | 16
 */
void test_cidr_iguales() {
    enum contiene ret;
    struct cidr_clase a;
    struct cidr_clase b;
    /* configuro la red a */
    inet_aton("10.0.0.0", &(a.cidr.red)); /* red 10.0.0.0 */
    a.cidr.mascara = GET_MASCARA(16); /* mascara 255.0.0.0 */
    a.cantidad = 0; /* no importa las clases en este test */
    a.clases = NULL; /* no importa las clases en este test */
    /* configuro la red b */
    inet_aton("10.0.0.0", &(b.cidr.red)); /* red 10.1.0.0 */
    b.cidr.mascara = GET_MASCARA(16);/* mascara 255.0.0.0 */
    b.cantidad = 0; /* no importa las clases en este test */
    b.clases = NULL; /* no importa las clases en este test */
    /* Ejecuto tests */
    ret = cidr_contiene(&a, &b);
    assert(ret == IGUALES);
    ret = cidr_contiene(&b, &a);
    assert(ret == IGUALES);
}

/*
 * test_cidr_a_contiene_b
 * ---------------------------------------------------------------------------
 * Se prueba que la subred pasada como primer parametro contenga a la red
 * pasada por segundo parametro.
 *
 * En este caso de prueba se establecen los siguiente valores
 *
 * parametro       | direccion de red | mascara de subred | prefijo
 * --------------- + ---------------- + ----------------- + ------------
 * a               | 10.0.0.0         | 255.0.0.0         | 8
 * b               | 10.1.0.0         | 255.255.0.0       | 16
 */
void test_cidr_a_contiene_b() {
    enum contiene ret;
    struct cidr_clase a;
    struct cidr_clase b;
    /* configuro la red a */
    inet_aton("10.0.0.0", &(a.cidr.red)); /* red 10.1.0.0 */
    a.cidr.mascara = GET_MASCARA(8);/* mascara 255.0.0.0 */
    a.cantidad = 0; /* no importa las clases en este test */
    a.clases = NULL; /* no importa las clases en este test */
    /* configuro la red b */
    inet_aton("10.1.0.0", &(b.cidr.red)); /* red 10.1.0.0 */
    b.cidr.mascara = GET_MASCARA(16); /* mascara 255.255.0.0 */
    b.cantidad = 0; /* no importa las clases en este test */
    b.clases = NULL; /* no importa las clases en este test */
    /* Ejecuto tests */
    ret = cidr_contiene(&a, &b);
    assert(ret == A_CONTIENE_B);
}

/*
 * test_cidr_sin_coincidencia
 * ---------------------------------------------------------------------------
 * Se prueba que no existan coincidencias entre las redes pasadas por parametro
 *
 * En este caso de prueba se establecen los siguiente valores
 *
 * parametro       | direccion de red | mascara de subred | prefijo
 * --------------- + ---------------- + ----------------- + ------------
 * a               | 10.1.0.0         | 255.255.0.0       | 16
 * b               | 10.2.0.0         | 255.255.0.0       | 16
 */
void test_cidr_sin_coincidencia() {
    enum contiene ret;
    struct cidr_clase a;
    struct cidr_clase b;
    /* configuro la red a */
    inet_aton("10.1.0.0", &(a.cidr.red)); /* red 10.1.0.0 */
    a.cidr.mascara = GET_MASCARA(16);/* mascara 255.255.0.0 */
    a.cantidad = 0; /* no importa las clases en este test */
    a.clases = NULL; /* no importa las clases en este test */
    /* configuro la red b */
    inet_aton("10.2.0.0", &(b.cidr.red)); /* red 10.2.0.0 */
    b.cidr.mascara = GET_MASCARA(16); /* mascara 255.255.0.0 */
    b.cantidad = 0; /* no importa las clases en este test */
    b.clases = NULL; /* no importa las clases en este test */
    /* Ejecuto tests */
    ret = cidr_contiene(&a, &b);
    assert(ret == SIN_COINCIDENCIA);
    ret = cidr_contiene(&b, &a);
    assert(ret == SIN_COINCIDENCIA);
}

/*
 * test_cidr_comparar_menor
 * --------------------------------------------------------------------------
 *  Prueba la funcion cidr_comparar. Compara dos CIDR, siendo el primer
 *  parametro menor que el segundo. La funcion debe devolver -1
 *
 *  En este caso de prueba se establecen los siguiente valores
 *
 *  parametro       | direccion de red | mascara de subred | prefijo
 *  --------------- + ---------------- + ----------------- + ------------
 *  a               | 10.1.0.0         | 255.255.0.0       | 16
 *  b               | 10.2.0.0         | 255.255.0.0       | 16
 */
void test_cidr_comparar_menor() {
    int ret = 0;
    struct cidr_clase a;
    struct cidr_clase b;
    /* configuro la red a */
    inet_aton("10.1.0.0", &(a.cidr.red));
    a.cidr.mascara = GET_MASCARA(16);
    a.cantidad = 0;
    a.clases = NULL;
    /* configuro la red b */
    inet_aton("10.2.0.0", &(b.cidr.red));
    b.cidr.mascara = GET_MASCARA(16);
    b.cantidad = 0;
    b.clases = NULL;
    /* Ejecuto tests */
    ret = cidr_comparar(&a, &b);
    assert(ret == -1);
}

/*
 * test_cidr_comparar_mayor
 * --------------------------------------------------------------------------
 *  Prueba la funcion cidr_comparar. Compara dos CIDR, siendo el primer
 *  parametro mayor que el segundo. La funcion debe devolver 1
 *
 *  En este caso de prueba se establecen los siguiente valores
 *
 *  parametro       | direccion de red | mascara de subred | prefijo
 *  --------------- + ---------------- + ----------------- + ------------
 *  a               | 10.2.0.0         | 255.255.0.0       | 16
 *  b               | 10.1.0.0         | 255.255.0.0       | 16
 */
void test_cidr_comparar_mayor() {
    int ret = 0;
    struct cidr_clase a;
    struct cidr_clase b;
    /* configuro la red a */
    inet_aton("10.2.0.0", &(a.cidr.red));
    a.cidr.mascara = GET_MASCARA(16);
    a.cantidad = 0;
    a.clases = NULL;
    /* configuro la red b */
    inet_aton("10.1.0.0", &(b.cidr.red));
    b.cidr.mascara = GET_MASCARA(16);
    b.cantidad = 0;
    b.clases = NULL;
    /* Ejecuto tests */
    ret = cidr_comparar(&a, &b);
    assert(ret == 1);
}

/*
 * test_cidr_comparar_igual
 * --------------------------------------------------------------------------
 *  Prueba la funcion cidr_comparar. Compara dos CIDR, siendo el primer
 *  parametro igual que al segundo. La funcion debe devolver 0
 *
 *  En este caso de prueba se establecen los siguiente valores
 *
 *  parametro       | direccion de red | mascara de subred | prefijo
 *  --------------- + ---------------- + ----------------- + ------------
 *  a               | 10.1.0.0         | 255.255.0.0       | 16
 *  b               | 10.1.0.0         | 255.255.0.0       | 16
 */
void test_cidr_comparar_igual() {
    int ret = 0;
    struct cidr_clase a;
    struct cidr_clase b;
    /* configuro la red a */
    inet_aton("10.1.0.0", &(a.cidr.red));
    a.cidr.mascara = GET_MASCARA(16);
    a.cantidad = 0;
    a.clases = NULL;
    /* configuro la red b */
    inet_aton("10.1.0.0", &(b.cidr.red));
    b.cidr.mascara = GET_MASCARA(16);
    b.cantidad = 0;
    b.clases = NULL;
    /* Ejecuto tests */
    ret = cidr_comparar(&a, &b);
    assert(ret == 0);
    ret = cidr_comparar(&b, &a);
    assert(ret == 0);
}

/*
 * test_cidr_comparar_contenida
 * --------------------------------------------------------------------------
 *  Prueba la funcion cidr_comparar. Compara dos CIDR, siendo el primer
 *  parametro contenido dentro del segundo. Debe devolver cero
 *
 *  En este caso de prueba se establecen los siguiente valores
 *
 *  parametro       | direccion de red | mascara de subred | prefijo
 *  --------------- + ---------------- + ----------------- + ------------
 *  a               | 10.1.0.0         | 255.255.0.0       | 16
 *  b               | 10.0.0.0         | 255.0.0.0       | 8
 */
void test_cidr_comparar_contenida() {
    int ret = 0;
    struct cidr_clase a;
    struct cidr_clase b;
    /* configuro la red a */
    inet_aton("10.1.0.0", &(a.cidr.red));
    a.cidr.mascara = GET_MASCARA(16);
    a.cantidad = 0;
    a.clases = NULL;
    /* configuro la red b */
    inet_aton("10.0.0.0", &(b.cidr.red));
    b.cidr.mascara = GET_MASCARA(8);
    b.cantidad = 0;
    b.clases = NULL;
    /* Ejecuto tests */
    ret = cidr_comparar(&a, &b);
    assert(ret == 0);
    ret = cidr_comparar(&b, &a);
    assert(ret == 0);
}

int main() {
    test_mascara();
    test_in_net();
    test_cidr_b_contiene_a();
    test_cidr_a_contiene_b();
    test_cidr_sin_coincidencia();
    test_cidr_comparar_menor();
    test_cidr_comparar_mayor();
    test_cidr_comparar_igual();
    test_cidr_comparar_contenida();
    printf("SUCCESS\n");
    return 0;
}
