#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../src/analizador.h"
#include "../src/bd.h"

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
    assert(IN_NET(ip, net, mascara) == 1);

    /* 192.168.1.1 -> 192.168.2.0/24 FALSO */
    ip.s_addr = inet_addr("192.168.1.1");
    net.s_addr = inet_addr("192.168.2.0");
    mascara = MASCARA_24;
    assert(IN_NET(ip, net, mascara) == 0);

    /* 192.168.1.33 -> 192.168.1.0/27 FALSO */
    ip.s_addr = inet_addr("192.168.1.33");
    net.s_addr = inet_addr("192.168.1.0");
    mascara = MASCARA_27;
    assert(IN_NET(ip, net, mascara) == 0);

    /* 172.18.1.1 -> 172.16.0.0/12 VERDADERO */
    ip.s_addr = inet_addr("172.18.1.1");
    net.s_addr = inet_addr("172.16.0.0");
    mascara = GET_MASCARA(12);
    assert(IN_NET(ip, net, mascara) == 1);

    /* 10.200.0.10 -> 10.200.0.0/16 VERDADERO */
    ip.s_addr = inet_addr("10.200.0.10");
    net.s_addr = inet_addr("10.200.0.0");
    mascara = MASCARA_16;
    assert(IN_NET(ip, net, mascara) == 1);
}

/*
 * crear_clase
 * ------------------------------------------------------------------
 *  Funcion auxiliar que sirve para crear clases de trafico con una
 *  sola subred
 */
void crear_clase(struct clase* clase, const char *red, int prefijo) {
    clase->cant_subredes_outside = 1;
    clase->subredes_outside = (struct subred *) malloc(sizeof(struct subred));
    inet_aton("10.1.0.0", &(clase->subredes_outside->red));
    clase->subredes_outside->mascara = GET_MASCARA(16);
}

/*
 * test_coincide_subred
 * --------------------------------------------------------------------------
 *  Prueba la funcion coincide. Compara un paquete con una clase de trafico por
 *  la direccion ip del paquete. Se prueba un caso por verdadero y un caso por
 *  falso
 *
 *  En este caso de prueba se establecen los siguiente valores:
 *
 *  clase trafico   | direccion de red | prefijo
 *  --------------- + ---------------- + --------
 *  a               | 192.168.0.0      | 16
 *  b               | 10.0.0.0         | 16
 *
 *  paquete | ip origen       | ip dest        | puerto origen | puerto dest
 *  ------- + --------------- + -------------- + ------------- + -----------
 *  x       | 192.168.122.177 | 200.150.180.210| 12345         | 80
 */
void test_coincide_subred() {
    struct clase a, b;
    struct paquete x;
    /* creo clases de trafico */
    init_clase(&a);
    a.cant_subredes_outside = 1;
    a.subredes_outside = malloc(sizeof(struct subred));
    inet_aton("192.168.0.0", &(a.subredes_outside->red));
    a.subredes_outside->mascara = GET_MASCARA(16);

    init_clase(&b);
    b.cant_subredes_outside = 1;
    b.subredes_outside = malloc(sizeof(struct subred));
    inet_aton("10.0.0.0", &(b.subredes_outside->red));
    b.subredes_outside->mascara = GET_MASCARA(16);

    /* creo paquete */
    inet_aton("192.168.122.177", &(x.ip_origen));
    inet_aton("200.150.180.210", &(x.ip_destino));
    x.puerto_origen = 12345;
    x.puerto_destino = 80;
    x.direccion = ENTRANTE;

    assert(coincide(&a, &x) == 1);
    assert(coincide(&b, &x) == 0);
}

/*
 * test_coincide_muchas_subredes
 * --------------------------------------------------------------------------
 *  Prueba la funcion coincide. Compara un paquete con una clase de trafico por
 *  la direccion ip del paquete. Se prueba un caso por verdadero y un caso por
 *  falso. Las clases definen muchas subredes
 *
 *  En este caso de prueba se establecen los siguiente valores:
 *
 *  clase trafico   | direccion de red | prefijo
 *  --------------- + ---------------- + --------
 *  a               | 192.168.0.0      | 16
 *                  | 172.17.0.0       | 24
 *                  | 172.17.1.0       | 24
 *  b               | 10.0.0.0         | 16
 *                  | 177.200.1.0      | 25
 *
 *  paquete | ip origen       | ip dest        | puerto origen | puerto dest
 *  ------- + --------------- + -------------- + ------------- + -----------
 *  x       | 177.200.1.128   | 172.17.0.255   | 12345         | 80
 */
void test_coincide_muchas_subredes() {
    struct clase a, b;
    struct paquete x;
    /* creo clases de trafico */
    init_clase(&a);
    init_clase(&b);

    a.cant_subredes_outside = 3;
    a.subredes_outside = malloc(3 * sizeof(struct subred));
    inet_aton("192.168.0.0", &(a.subredes_outside->red));
    inet_aton("172.17.0.0", &((a.subredes_outside + 1)->red));
    inet_aton("172.17.1.0", &((a.subredes_outside + 2)->red));
    a.subredes_outside->mascara = GET_MASCARA(16);
    (a.subredes_outside + 1)->mascara = GET_MASCARA(24);
    (a.subredes_outside + 2)->mascara = GET_MASCARA(24);

    b.cant_subredes_outside = 2;
    b.subredes_outside = malloc(2 * sizeof(struct subred));
    inet_aton("10.0.0.0", &(b.subredes_outside->red));
    inet_aton("177.200.1.0", &((b.subredes_outside + 1)->red));
    b.subredes_outside->mascara = GET_MASCARA(16);
    (b.subredes_outside + 1)->mascara = GET_MASCARA(25);

    /* creo paquete */
    inet_aton("177.200.1.128", &(x.ip_origen));
    inet_aton("172.17.0.255", &(x.ip_destino));
    x.puerto_origen = 12345;
    x.puerto_destino = 80;
    x.direccion = SALIENTE;

    assert(coincide(&a, &x) == 1);
    assert(coincide(&b, &x) == 0);
}

/*
 * test_coincide_subred_origen_destino
 * --------------------------------------------------------------------------
 *  Prueba la funcion coincide. Compara un paquete con una clase de trafico por
 *  la direccion ip del paquete. Se prueba un caso por verdadero y un caso por
 *  falso. La el paquete debe coincidir en ip origen y destino
 *
 *  En este caso de prueba se establecen los siguiente valores:
 *
 *  clase trafico   | direccion de red a | direccion de red b
 *  --------------- + ------------------ + ------------------
 *  a               | 192.168.0.0/24     | 192.168.1.0/24
 *  b               | 192.168.1.0/24     | 10.0.0.0/8
 *
 *  paquete | ip origen       | ip dest        | puerto origen | puerto dest
 *  ------- + --------------- + -------------- + ------------- + -----------
 *  x       | 192.168.1.1     | 192.168.0.121  | 12345         | 80
 */
void test_coincide_subred_origen_destino() {
    struct clase a, b;
    struct paquete x;
    /* creo clases de trafico */
    init_clase(&a);
    init_clase(&b);
    a.cant_subredes_outside = 1;
    a.cant_subredes_inside = 1;
    a.subredes_outside = malloc(sizeof(struct subred)); /* outside */
    inet_aton("192.168.0.0", &(a.subredes_outside->red));
    a.subredes_outside->mascara = GET_MASCARA(24);
    a.subredes_inside = malloc(sizeof(struct subred)); /* inside */
    inet_aton("192.168.1.0", &(a.subredes_inside->red));
    a.subredes_inside->mascara = GET_MASCARA(24);

    b.cant_subredes_outside = 1;
    b.cant_subredes_inside = 1;
    b.subredes_outside = malloc(sizeof(struct subred)); /* outside */
    inet_aton("192.168.0.0", &(b.subredes_outside->red));
    b.subredes_outside->mascara = GET_MASCARA(24);
    b.subredes_inside = malloc(sizeof(struct subred)); /* inside */
    inet_aton("10.0.0.0", &(b.subredes_inside->red));
    b.subredes_inside->mascara = GET_MASCARA(8);

    /* creo paquete */
    inet_aton("192.168.1.1", &(x.ip_origen));
    inet_aton("192.168.0.121", &(x.ip_destino));
    x.puerto_origen = 12345;
    x.puerto_destino = 80;
    x.direccion = SALIENTE;

    assert(coincide(&a, &x) == 1);
    assert(coincide(&b, &x) == 0);
}

/*
 * test_coincide_stress()
 * --------------------------------------------------------------------------
 *  Prueba de stress de la funcion coincide para evaluar el rendimiento del
 *  algoritmo.
 *  ### Parametros:
 *    * cantidad_clases: cantidad de supuestas clases de trafico.
 */
void test_coincide_stress(int cantidad_clases) {
    struct clase a;
    struct paquete x;
    int i;
    /* creo clases de trafico */
    init_clase(&a);
    a.cant_subredes_outside = 1;
    a.subredes_outside = malloc(sizeof(struct subred)); /* a */
    inet_aton("192.168.0.0", &(a.subredes_outside->red));
    a.subredes_outside->mascara = GET_MASCARA(24);

    /* creo paquete */
    inet_aton("192.168.1.1", &(x.ip_origen));
    inet_aton("192.168.2.121", &(x.ip_destino));
    x.puerto_origen = 12345;
    x.puerto_destino = 80;
    x.direccion = ENTRANTE;

    for(i = 0; i < cantidad_clases; i++) {
        coincide(&a, &x);
    }
}

/*
 * test_coincide_puerto
 * --------------------------------------------------------------------------
 *  Prueba la funcion coincide. Compara un paquete con una clase de trafico por
 *  el puerto de origen o destino del paquete. Se prueba un caso por verdadero
 *  y un caso por falso
 *
 *  En este caso de prueba se establecen los siguiente valores:
 *
 *  clase trafico   | direccion de red | puerto  | coincide
 *  --------------- + ---------------- + ------- + --------
 *  a               |                  | 22      | si
 * ---------------- + ---------------- + ------- + --------
 *  b               | 10.0.0.0/8       | 80      | no
 * ---------------- + ---------------- + ------- + --------
 *  c               | 10.0.0.0/8       | 22      | no
 *
 *  paquete | ip origen       | ip dest        | puerto origen | puerto dest
 *  ------- + --------------- + -------------- + ------------- + -----------
 *  x       | 192.168.122.177 | 200.150.180.210| 12345         | 22
 */
void test_coincide_puerto() {
    struct clase a, b, c;
    struct paquete x;
    /* creo clases de trafico */
    init_clase(&a);
    init_clase(&b);
    init_clase(&c);
    a.cant_puertos_outside = 1;
    a.puertos_outside = malloc(sizeof(struct puerto));
    a.puertos_outside->numero = 22;
    a.puertos_outside->protocolo = IPPROTO_TCP;

    b.cant_subredes_outside = 1;
    b.cant_puertos_outside = 1;
    b.subredes_outside = malloc(sizeof(struct subred));
    inet_aton("10.0.0.0", &(b.subredes_outside->red));
    b.subredes_outside->mascara = GET_MASCARA(8);
    b.puertos_outside = malloc(sizeof(struct puerto));
    b.puertos_outside->numero = 80;
    b.puertos_outside->protocolo = IPPROTO_TCP;

    c.cant_subredes_outside = 1;
    c.cant_puertos_outside = 1;
    c.subredes_outside = malloc(sizeof(struct subred));
    inet_aton("10.0.0.0", &(c.subredes_outside->red));
    c.subredes_outside->mascara = GET_MASCARA(8);
    c.puertos_outside = malloc(sizeof(struct puerto));
    c.puertos_outside->numero = 22;
    c.puertos_outside->protocolo = IPPROTO_TCP;

    /* creo paquete */
    inet_aton("192.168.122.177", &(x.ip_origen));
    inet_aton("200.150.180.210", &(x.ip_destino));
    x.puerto_origen = 12345;
    x.puerto_destino = 22;
    x.protocolo = IPPROTO_TCP;
    x.direccion = SALIENTE;

    assert(coincide(&a, &x) == 1);
    assert(coincide(&b, &x) == 0);
    assert(coincide(&c, &x) == 0);
}

/*
 * test_coincide_muchos_puertos
 * --------------------------------------------------------------------------
 *  Prueba la funcion coincide. Compara un paquete con una clase de trafico por
 *  el puerto de origen o destino del paquete. Se prueba un caso por verdadero
 *  y un caso por falso. Las clases tienen muchos puertos
 *
 *  En este caso de prueba se establecen los siguiente valores:
 *
 *  clase trafico   | direccion de red | puerto  | coincide
 *  --------------- + ---------------- + ------- + --------
 *  a               |                  | 22      | si
 *                  |                  | 80      |
 *                  |                  | 443     |
 * ---------------- + ---------------- + ------- + --------
 *  b               | 10.0.0.0/8       | 80      | no
 *                  |                  | 443     |
 * ---------------- + ---------------- + ------- + --------
 *  c               |                  | 80      | no
 *                  |                  | 22      |
 *
 *  paquete | ip origen       | ip dest        | puerto origen | puerto dest
 *  ------- + --------------- + -------------- + ------------- + -----------
 *  x       | 192.168.122.177 | 200.150.180.210| 12345         | 443
 */
void test_coincide_muchos_puertos() {
    struct clase a, b, c;
    struct paquete x;
    /* creo clases de trafico */
    init_clase(&a);
    init_clase(&b);
    init_clase(&c);

    a.cant_puertos_outside = 3;
    a.puertos_outside = malloc(3 * sizeof(struct puerto));
    (a.puertos_outside)->numero = 22;
    (a.puertos_outside)->protocolo = 0;
    (a.puertos_outside + 1)->numero = 80;
    (a.puertos_outside + 1)->protocolo = 0;
    (a.puertos_outside + 2)->numero = 443;
    (a.puertos_outside + 2)->protocolo = 0;

    b.cant_subredes_outside = 1;
    b.cant_puertos_outside = 2;
    b.subredes_outside = malloc(sizeof(struct subred));
    inet_aton("10.0.0.0", &(b.subredes_outside->red));
    b.subredes_outside->mascara = GET_MASCARA(8);
    b.puertos_outside = malloc(2 * sizeof(struct puerto));
    (b.puertos_outside)->numero = 80;
    (b.puertos_outside)->protocolo = IPPROTO_TCP;
    (b.puertos_outside + 1)->numero = 443;
    (b.puertos_outside + 1)->protocolo = IPPROTO_TCP;

    c.cant_puertos_outside = 2;
    c.puertos_outside = malloc(2 * sizeof(struct puerto));
    (c.puertos_outside)->numero = 22;
    (c.puertos_outside)->protocolo = IPPROTO_TCP;
    (c.puertos_outside + 1)->numero = 80;
    (c.puertos_outside + 1)->protocolo = IPPROTO_TCP;

    /* creo paquete */
    inet_aton("192.168.122.177", &(x.ip_origen));
    inet_aton("200.150.180.210", &(x.ip_destino));
    x.puerto_origen = 12345;
    x.puerto_destino = 443;
    x.protocolo = IPPROTO_TCP;
    x.direccion = SALIENTE;

    assert(coincide(&a, &x) == 1);
    assert(coincide(&b, &x) == 0);
    assert(coincide(&c, &x) == 0);
}

/*
 * test_coincide_puerto_origen_destino
 * --------------------------------------------------------------------------
 *  Prueba la funcion coincide. Compara un paquete con una clase de trafico por
 *  el puerto de origen o destino del paquete. Se prueba un caso por verdadero
 *  y un caso por falso. Debe coincidir el puerto de origen y el de destino
 *
 *  En este caso de prueba se establecen los siguiente valores:
 *
 *  clase trafico   | direccion de red   | puerto o  | puerto d | coincide
 *  =============== + ================== + ========= + ======== + ========
 *  a               |                    | 22        | 1025     | si
 *                  |                    | 80        | 12345    |
 *                  |                    | 443       |          |
 * ---------------- + ------------------ + --------- + -------- + -----------
 *  b               | 10.0.0.0/8         | 80        | 80       | no
 *                  |                    | 443       | 12345    |
 * ---------------- + ------------------ + --------- + -------- + -----------
 *  c               | o: 192.168.122.177 | 443       | 12345    | no
 *                  | i: 200.150.0.0/16  |           |          |
 *
 *  paquete | ip origen       | ip dest        | puerto origen | puerto dest
 *  ======= + =============== + ============== + ============= + ===========
 *  x       | 192.168.122.177 | 200.150.180.210| 12345         | 443
 */
void test_coincide_puerto_origen_destino() {
    struct clase a, b, c;
    struct paquete x;
    /* creo clases de trafico */
    init_clase(&a);
    init_clase(&b);
    init_clase(&c);

    a.cant_puertos_outside = 3;
    a.cant_puertos_inside = 2;
    a.puertos_outside = malloc(3 * sizeof(struct puerto));
    (a.puertos_outside)->numero = 22;
    (a.puertos_outside)->protocolo = 0;
    (a.puertos_outside + 1)->numero = 80;
    (a.puertos_outside + 1)->protocolo = 0;
    (a.puertos_outside + 2)->numero = 443;
    (a.puertos_outside + 2)->protocolo = 0;
    a.puertos_inside = malloc(2 * sizeof(struct puerto));
    (a.puertos_inside)->numero = 1025;
    (a.puertos_inside)->protocolo = 0;
    (a.puertos_inside + 1)->numero = 12345;
    (a.puertos_inside + 1)->protocolo = 0;

    b.cant_subredes_outside = 1;
    b.cant_puertos_outside = 2;
    b.cant_puertos_inside = 2;
    b.subredes_outside = malloc(sizeof(struct subred));
    inet_aton("10.0.0.0", &(b.subredes_outside->red));
    b.subredes_outside->mascara = GET_MASCARA(8);
    b.puertos_outside = malloc(2 * sizeof(struct puerto));
    (b.puertos_outside)->numero = 80;
    (b.puertos_outside)->protocolo = 0;
    (b.puertos_outside + 1)->numero = 443;
    (b.puertos_outside + 1)->protocolo = 0;
    b.puertos_inside = malloc(2 * sizeof(struct puerto));
    (b.puertos_inside)->numero = 80;
    (b.puertos_inside)->protocolo = 0;
    (b.puertos_inside + 1)->numero = 12345;
    (b.puertos_inside + 1)->protocolo = 0;

    c.cant_subredes_outside = 1;
    c.cant_subredes_inside = 1;
    c.cant_puertos_outside = 1;
    c.cant_puertos_inside = 1;
    c.subredes_outside = malloc(sizeof(struct subred));
    inet_aton("192.168.122.177", &(c.subredes_outside->red));
    c.subredes_outside->mascara = MASCARA_HOST;
    c.subredes_inside = malloc(sizeof(struct subred));
    inet_aton("200.150.0.0", &(c.subredes_inside->red));
    c.subredes_inside->mascara = GET_MASCARA(16);
    c.puertos_outside = malloc(sizeof(struct puerto));
    c.puertos_outside->numero = 443;
    c.puertos_outside->protocolo = 0;
    c.puertos_inside = malloc(sizeof(struct puerto));
    c.puertos_inside->numero = 12345;
    c.puertos_inside->protocolo = 0;

    /* creo paquete */
    inet_aton("192.168.122.177", &(x.ip_origen));
    inet_aton("200.150.180.210", &(x.ip_destino));
    x.puerto_origen = 12345;
    x.puerto_destino = 443;
    x.protocolo = IPPROTO_TCP;
    x.direccion = SALIENTE;

    assert(coincide(&a, &x) == 1);
    assert(coincide(&b, &x) == 0);
    assert(coincide(&c, &x) == 0);
}

/*
 * test_coincide_protocolo
 * --------------------------------------------------------------------------
 *  Prueba la funcion coincide. Compara un paquete con una clase de trafico por
 *  el protocolo del paquete. Se prueba un caso por verdadero y un caso por
 *  falso
 *
 *  En este caso de prueba se establecen los siguiente valores:
 *
 *  clase trafico   | direccion de red | puerto  | protocolo  | coincide
 *  =============== + ================ + ======= + ========== + ========
 *  a               |                  | 22      | tcp        | si
 * ---------------- + ---------------- + ------- + ---------- + --------
 *  b               | 10.0.0.0/8       | 22      | tcp        | no
 * ---------------- + ---------------- + ------- + ---------- + --------
 *  c               |                  | 22      | udp        | no
 *
 *  paquete | ip origen       | ip dest        | puerto origen | puerto dest
 *  ------- + --------------- + -------------- + ------------- + -----------
 *  x       | 192.168.122.177 | 200.150.180.210| 12345         | 22
 */
void test_coincide_protocolo() {
    struct clase a, b, c;
    struct paquete x;
    /* creo clases de trafico */
    init_clase(&a);
    init_clase(&b);
    init_clase(&c);

    a.cant_puertos_outside = 1;
    a.puertos_outside = malloc(sizeof(struct puerto));
    a.puertos_outside->numero = 22;
    a.puertos_outside->protocolo = IPPROTO_TCP;


    b.cant_subredes_outside = 1;
    b.cant_puertos_outside = 1;
    b.subredes_outside = malloc(sizeof(struct subred));
    inet_aton("10.0.0.0", &(b.subredes_outside->red));
    b.subredes_outside->mascara = GET_MASCARA(8);
    b.puertos_outside = malloc(sizeof(struct puerto));
    b.puertos_outside->numero = 22;
    b.puertos_outside->protocolo = IPPROTO_TCP;

    c.cant_puertos_outside = 1;
    c.puertos_outside = malloc(sizeof(struct puerto));
    c.puertos_outside->numero = 22;
    c.puertos_outside->protocolo = IPPROTO_UDP;

    /* creo paquete */
    inet_aton("192.168.122.177", &(x.ip_origen));
    inet_aton("200.150.180.210", &(x.ip_destino));
    x.puerto_origen = 12345;
    x.puerto_destino = 22;
    x.direccion = SALIENTE;
    x.protocolo = IPPROTO_TCP;

    assert(coincide(&a, &x) == 1);
    assert(coincide(&b, &x) == 0);
    assert(coincide(&c, &x) == 0);
}

/*
 * test_imprimir
 * --------------------------------------------------------------------------
 *  Prueba la funcion imprimir. Imprime una lista de clases de trafico a la
 *  salida estandar en formato JSON.
 *
 *  En este caso de prueba se establecen los siguiente valores:
 *
 *  clase trafico   | nombre | descripcion   | subida | bajada
 *  =============== + ====== + ============= + ====== + =======
 *  0               | SSH    | Proto ssh     | 25108  | 2105
 *  --------------- + ------ + ------------- + ------ + ------
 *  1               | HTTP   | Nav. web      | 15     | 11020
 *  --------------- + ------ + ------------- + ------ + ------
 *  2               | DNS    | Serv. nombres | 22111  | 53
 */
void test_imprimir() {
    struct s_analizador analizador;
    struct clase clases[4];
    /* creo clases de trafico */
    clases[0].id = 0;
    strncpy(clases[0].nombre, "SSH", LONG_NOMBRE);
    strncpy(clases[0].descripcion, "Proto ssh", LONG_DESCRIPCION);
    clases[0].bytes_subida = 25108;
    clases[0].bytes_bajada = 2105;

    clases[1].id = 1;
    strncpy(clases[1].nombre, "HTTP", LONG_NOMBRE);
    strncpy(clases[1].descripcion, "Nav. Web", LONG_DESCRIPCION);
    clases[1].bytes_subida = 15;
    clases[1].bytes_bajada = 11020;

    clases[2].id = 2;
    strncpy(clases[2].nombre, "DNS", LONG_NOMBRE);
    strncpy(clases[2].descripcion, "Serv. nombres", LONG_DESCRIPCION);
    clases[2].bytes_subida = 22111;
    clases[2].bytes_bajada = 53;

    clases[3].id = 3;
    strncpy(clases[3].nombre, "No se debe mostrar", LONG_NOMBRE);
    strncpy(clases[3].descripcion, "Clases con 0 bytes no se muestran",
            LONG_DESCRIPCION);
    clases[3].bytes_subida = 0;
    clases[3].bytes_bajada = 0;

    analizador.clases = clases;
    analizador.cant_clases = 4;

    imprimir(&analizador);
}

/*
 * test_analizar_paquete
 * --------------------------------------------------------------------------
 *  Prueba la funcion analizar_paquete. Compara un paquete con las clases de
 *  trafico instaladas. Debe aumentar el contador de bytes de las clases.
 *
 *  En este caso de prueba se establecen los siguiente valores:
 *
 *  clase trafico   | direccion de red | puerto
 *  =============== + ================ + =======
 *   default        |                  |
 * ---------------- + ---------------- + -------
 *   c1             | i: 1.0.0.0/8     |
 * ---------------- + ---------------- + -------
 *   c2             |                  | o: 12
 *
 *  paquete | origen  | destino | p_origen | p_dest | bytes | direccion
 *  ======= + ======= +======== + ======== + ====== + ===== | ==========
 *   p0     | 1.1.1.1 | 2.2.2.2 |  1       |  11    |  10   | SALIENTE
 *  ------- + ------- +-------- + -------- + ------ + ----- + ----------
 *   p1     | 1.1.1.1 | 3.3.3.3 |  2       |  12    |  10   | SALIENTE
 *  ------- + ------- +-------- + -------- + ------ + ----- + ----------
 *   p2     | 2.2.2.2 | 1.1.1.1 |  11      |  1     |  10   | ENTRANTE
 *  ------- + ------- +-------- + -------- + ------ + ----- + ----------
 *   p3     | 8.8.8.8 | 2.2.2.2 |  53      |  1     |  10   | ENTRANTE
 *  ------- + ------- +-------- + -------- + ------ + ----- + ----------
 *
 *
 *  ### Resultado esperado
 *
 *  clase trafico   | subida | bajada
 *  =============== + ====== + =======
 *   default        | 0      | 10
 *  --------------- + ------ + ------
 *   c1             | 20     | 10
 *  --------------- + ------ + ------
 *   c2             | 10     | 0
 */
void test_analizar_paquete() {
    struct s_analizador analizador;
    struct clase clases[3];
    struct paquete paquetes[4];

    /* creo clases de trafico */
    init_clase(clases);
    strncpy(clases[0].nombre, "Default", LONG_NOMBRE);

    init_clase(clases + 1);
    strncpy(clases[1].nombre, "c1", LONG_NOMBRE);
    clases[1].cant_subredes_inside = 1;
    clases[1].subredes_inside = malloc(sizeof(struct subred));
    inet_aton("1.0.0.0", &(clases[1].subredes_inside->red));
    clases[1].subredes_inside->mascara = GET_MASCARA(8);

    init_clase(clases + 2);
    strncpy(clases[2].nombre, "c2", LONG_NOMBRE);
    clases[2].cant_puertos_outside = 1;
    clases[2].puertos_outside = malloc(sizeof(struct puerto));
    clases[2].puertos_outside->numero = 12;
    clases[2].puertos_outside->protocolo = 0;

    analizador.clases = clases;
    analizador.cant_clases = 3;

    /* creo paquetes */
    inet_aton("1.1.1.1", &(paquetes[0].ip_origen));
    inet_aton("2.2.2.2", &(paquetes[0].ip_destino));
    paquetes[0].puerto_origen = 1;
    paquetes[0].puerto_destino = 11;
    paquetes[0].bytes = 10;
    paquetes[0].direccion = SALIENTE;
    paquetes[0].protocolo = IPPROTO_TCP;

    inet_aton("1.1.1.1", &(paquetes[1].ip_origen));
    inet_aton("3.3.3.3", &(paquetes[1].ip_destino));
    paquetes[1].puerto_origen = 2;
    paquetes[1].puerto_destino = 12;
    paquetes[1].bytes = 10;
    paquetes[1].direccion = SALIENTE;
    paquetes[1].protocolo = IPPROTO_TCP;

    inet_aton("2.2.2.2", &(paquetes[2].ip_origen));
    inet_aton("1.1.1.1", &(paquetes[2].ip_destino));
    paquetes[2].puerto_origen = 11;
    paquetes[2].puerto_destino = 1;
    paquetes[2].bytes = 10;
    paquetes[2].direccion = ENTRANTE;
    paquetes[2].protocolo = IPPROTO_TCP;

    inet_aton("8.8.8.8", &(paquetes[3].ip_origen));
    inet_aton("2.2.2.2", &(paquetes[3].ip_destino));
    paquetes[3].puerto_origen = 53;
    paquetes[3].puerto_destino = 1;
    paquetes[3].bytes = 10;
    paquetes[3].direccion = ENTRANTE;
    paquetes[3].protocolo = IPPROTO_TCP;

    /* comparo paquetes con las clases de trafico */
    for (int i=0; i < 4; i++)
        analizar_paquete(&analizador, paquetes + i);

    /* Verifico que el resultado del analisis sea correcto */
    assert(clases[0].bytes_subida == 0);
    assert(clases[0].bytes_bajada == 10);
    assert(clases[1].bytes_subida == 20);
    assert(clases[1].bytes_bajada == 10);
    assert(clases[2].bytes_subida == 10);
    assert(clases[2].bytes_bajada == 0);
}

int main() {
    test_mascara();
    test_in_net();
    test_coincide_subred();
    test_coincide_muchas_subredes();
    test_coincide_subred_origen_destino();
    test_coincide_stress(50000000);
    test_coincide_puerto();
    test_coincide_muchos_puertos();
    test_coincide_puerto_origen_destino();
    test_coincide_protocolo();
    test_imprimir();
    test_analizar_paquete();
    printf("SUCCESS\n");
    return 0;
}
