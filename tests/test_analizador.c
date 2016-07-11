#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
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
    /* configuro la red b */
    inet_aton("10.0.0.0", &(b.cidr.red)); /* red 10.1.0.0 */
    b.cidr.mascara = GET_MASCARA(8);/* mascara 255.0.0.0 */
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
    /* configuro la red b */
    inet_aton("10.0.0.0", &(b.cidr.red)); /* red 10.1.0.0 */
    b.cidr.mascara = GET_MASCARA(16);/* mascara 255.0.0.0 */
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
    /* configuro la red b */
    inet_aton("10.1.0.0", &(b.cidr.red)); /* red 10.1.0.0 */
    b.cidr.mascara = GET_MASCARA(16); /* mascara 255.255.0.0 */
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
    /* configuro la red b */
    inet_aton("10.2.0.0", &(b.cidr.red)); /* red 10.2.0.0 */
    b.cidr.mascara = GET_MASCARA(16); /* mascara 255.255.0.0 */
    /* Ejecuto tests */
    ret = cidr_contiene(&a, &b);
    assert(ret == SIN_COINCIDENCIA);
    ret = cidr_contiene(&b, &a);
    assert(ret == SIN_COINCIDENCIA);
}

/*
 * test_subred_comparar_menor
 * --------------------------------------------------------------------------
 *  Prueba la funcion subred_comparar. Compara dos CIDR, siendo el primer
 *  parametro menor que el segundo. La funcion debe devolver -1
 *
 *  En este caso de prueba se establecen los siguiente valores
 *
 *  parametro       | direccion de red | mascara de subred | prefijo
 *  --------------- + ---------------- + ----------------- + ------------
 *  a               | 10.1.0.0         | 255.255.0.0       | 16
 *  b               | 10.2.0.0         | 255.255.0.0       | 16
 */
void test_subred_comparar_menor() {
    int ret = 0;
    struct subred a;
    struct subred b;
    /* configuro la red a */
    inet_aton("10.1.0.0", &(a.red));
    a.mascara = GET_MASCARA(16);
    /* configuro la red b */
    inet_aton("10.2.0.0", &(b.red));
    b.mascara = GET_MASCARA(16);
    /* Ejecuto tests */
    ret = subred_comparar(&a, &b);
    assert(ret == -1);
}

/*
 * test_subred_comparar_mayor
 * --------------------------------------------------------------------------
 *  Prueba la funcion subred_comparar. Compara dos CIDR, siendo el primer
 *  parametro mayor que el segundo. La funcion debe devolver 1
 *
 *  En este caso de prueba se establecen los siguiente valores
 *
 *  parametro       | direccion de red | mascara de subred | prefijo
 *  --------------- + ---------------- + ----------------- + ------------
 *  a               | 10.2.0.0         | 255.255.0.0       | 16
 *  b               | 10.1.0.0         | 255.255.0.0       | 16
 */
void test_subred_comparar_mayor() {
    int ret = 0;
    struct subred a;
    struct subred b;
    /* configuro la red a */
    inet_aton("10.2.0.0", &(a.red));
    a.mascara = GET_MASCARA(16);
    /* configuro la red b */
    inet_aton("10.1.0.0", &(b.red));
    b.mascara = GET_MASCARA(16);
    /* Ejecuto tests */
    ret = subred_comparar(&a, &b);
    assert(ret == 1);
}

/*
 * test_subred_comparar_igual
 * --------------------------------------------------------------------------
 *  Prueba la funcion subred_comparar. Compara dos CIDR, siendo el primer
 *  parametro igual que al segundo. La funcion debe devolver 0
 *
 *  En este caso de prueba se establecen los siguiente valores
 *
 *  parametro       | direccion de red | mascara de subred | prefijo
 *  --------------- + ---------------- + ----------------- + ------------
 *  a               | 10.1.0.0         | 255.255.0.0       | 16
 *  b               | 10.1.0.0         | 255.255.0.0       | 16
 */
void test_subred_comparar_igual() {
    int ret = 0;
    struct subred a;
    struct subred b;
    /* configuro la red a */
    inet_aton("10.1.0.0", &(a.red));
    a.mascara = GET_MASCARA(16);
    /* configuro la red b */
    inet_aton("10.1.0.0", &(b.red));
    b.mascara = GET_MASCARA(16);
    /* Ejecuto tests */
    ret = subred_comparar(&a, &b);
    assert(ret == 0);
    ret = subred_comparar(&b, &a);
    assert(ret == 0);
}

/*
 * test_subred_comparar_contenida
 * --------------------------------------------------------------------------
 *  Prueba la funcion subred_comparar. Compara dos CIDR, siendo el primer
 *  parametro contenido dentro del segundo. Debe devolver cero
 *
 *  En este caso de prueba se establecen los siguiente valores
 *
 *  parametro       | direccion de red | mascara de subred | prefijo
 *  --------------- + ---------------- + ----------------- + ------------
 *  a               | 10.1.0.0         | 255.255.0.0       | 16
 *  b               | 10.0.0.0         | 255.0.0.0       | 8
 */
void test_subred_comparar_contenida() {
    int ret = 0;
    struct subred a;
    struct subred b;
    /* configuro la red a */
    inet_aton("10.1.0.0", &(a.red));
    a.mascara = GET_MASCARA(16);
    /* configuro la red b */
    inet_aton("10.0.0.0", &(b.red));
    b.mascara = GET_MASCARA(8);
    /* Ejecuto tests */
    ret = subred_comparar(&a, &b);
    assert(ret == 0);
    ret = subred_comparar(&b, &a);
    assert(ret == 0);
}

/*
 * crear_clase
 * ------------------------------------------------------------------
 *  Funcion auxiliar que sirve para crear clases de trafico con una
 *  sola subred
 */
void crear_clase(struct clase* clase, const char *red, int prefijo) {
    clase->cant_subredes_a = 1;
    clase->subredes_a = (struct subred *) malloc(sizeof(struct subred));
    inet_aton("10.1.0.0", &(clase->subredes_a->red));
    clase->subredes_a->mascara = GET_MASCARA(16);
}

/*
 * test_cidr_insertar
 * --------------------------------------------------------------------------
 *  Prueba insertar en orden las estructuras cidr_clase
 *
 *  En este caso de prueba se ingresaran las estructuras en el siguiente orden
 *
 *  orden ingreso | direccion de red | mascara de subred | prefijo
 *  ------------- + ---------------- + ----------------- + ------------
 *  0             | 10.1.0.0         | 255.255.0.0       | 16
 *  1             | 10.0.0.0         | 255.255.0.0       | 16
 *  2             | 10.10.0.0        | 255.255.0.0       | 17
 *  3             | 10.3.0.0         | 255.255.128.0     | 16
 *  4             | 192.168.1.0      | 255.0.0.0         | 24
 *  5             | 172.16.77.0      | 255.255.255.0     | 24
 *
 *  Como resultado deben quedar ordenadas de la siguiente forma
 *
 *  orden ingreso | direccion de red | mascara de subred | prefijo
 *  ------------- + ---------------- + ----------------- + ------------
 *  1             | 10.0.0.0         | 255.255.0.0       | 16
 *  0             | 10.1.0.0         | 255.255.0.0       | 16
 *  2             | 10.3.0.0         | 255.255.0.0       | 16
 *  3             | 10.10.0.0        | 255.255.128.0     | 17
 *  5             | 172.16.77.0      | 255.255.255.0     | 24
 *  4             | 192.168.1.0      | 255.255.255.0     | 24
 */
void test_cidr_insertar() {
    int ret = 0,
        i = 0;
    struct clase origen[6];
    struct cidr_clase destino[6]; /* este es el array destino */

    /* configuro las clases */
    crear_clase(&(origen[0]), "10.1.0.0", 16);
    crear_clase(&(origen[1]), "10.0.0.0", 16);
    crear_clase(&(origen[2]), "10.10.0.0", 17);
    crear_clase(&(origen[3]), "10.3.0.0", 16);
    crear_clase(&(origen[4]), "192.168.1.0", 24);
    crear_clase(&(origen[5]), "172.16.77.0", 24);

    /* inserto las clases en el array */
    while(i < 6 && ret == 0) {
        ret = cidr_insertar(destino, &(origen[i]), i);
        assert(ret == 0);
        i++;
    }

    /* verifico que el array este ordenado */
    assert(destino[0].clases == &(origen[1]));
    assert(destino[1].clases == &(origen[0]));
    assert(destino[2].clases == &(origen[2]));
    assert(destino[3].clases == &(origen[3]));
    assert(destino[4].clases == &(origen[5]));
    assert(destino[5].clases == &(origen[4]));

    /* libero memoria */
    for (i=0; i < 6; i++) {
        free(origen[i].subredes_a);
    }
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
    a.cant_subredes_a = 1;
    a.cant_subredes_b = 0;
    a.cant_puertos_a = 0;
    a.cant_puertos_b = 0;
    a.protocolo = 0;
    a.subredes_a = malloc(sizeof(struct subred));
    inet_aton("192.168.0.0", &(a.subredes_a->red));
    a.subredes_a->mascara = GET_MASCARA(16);

    b.cant_subredes_a = 1;
    b.cant_subredes_b = 0;
    b.cant_puertos_a = 0;
    b.cant_puertos_b = 0;
    b.protocolo = 0;
    b.subredes_a = malloc(sizeof(struct subred));
    inet_aton("10.0.0.0", &(b.subredes_a->red));
    b.subredes_a->mascara = GET_MASCARA(16);

    /* creo paquete */
    inet_aton("192.168.122.177", &(x.origen));
    inet_aton("200.150.180.210", &(x.destino));
    x.puerto_origen = 12345;
    x.puerto_destino = 80;

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
    a.cant_subredes_a = 3;
    a.cant_subredes_b = 0;
    a.cant_puertos_a = 0;
    a.cant_puertos_b = 0;
    a.protocolo = 0;
    a.subredes_a = malloc(3 * sizeof(struct subred));
    inet_aton("192.168.0.0", &(a.subredes_a->red));
    inet_aton("172.17.0.0", &((a.subredes_a + 1)->red));
    inet_aton("172.17.1.0", &((a.subredes_a + 2)->red));
    a.subredes_a->mascara = GET_MASCARA(16);
    (a.subredes_a + 1)->mascara = GET_MASCARA(24);
    (a.subredes_a + 2)->mascara = GET_MASCARA(24);

    b.cant_subredes_a = 2;
    b.cant_subredes_b = 0;
    b.cant_puertos_a = 0;
    b.cant_puertos_b = 0;
    b.protocolo = 0;
    b.subredes_a = malloc(2 * sizeof(struct subred));
    inet_aton("10.0.0.0", &(b.subredes_a->red));
    inet_aton("177.200.1.0", &((b.subredes_a + 1)->red));
    b.subredes_a->mascara = GET_MASCARA(16);
    (b.subredes_a + 1)->mascara = GET_MASCARA(25);

    /* creo paquete */
    inet_aton("177.200.1.128", &(x.origen));
    inet_aton("172.17.0.255", &(x.destino));
    x.puerto_origen = 12345;
    x.puerto_destino = 80;

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
    a.cant_subredes_a = 1;
    a.cant_subredes_b = 1;
    a.cant_puertos_a = 0;
    a.cant_puertos_b = 0;
    a.protocolo = 0;
    a.subredes_a = malloc(sizeof(struct subred)); /* a */
    inet_aton("192.168.0.0", &(a.subredes_a->red));
    a.subredes_a->mascara = GET_MASCARA(24);
    a.subredes_b = malloc(sizeof(struct subred)); /* b */
    inet_aton("192.168.1.0", &(a.subredes_b->red));
    a.subredes_b->mascara = GET_MASCARA(24);

    b.cant_subredes_a = 1;
    b.cant_subredes_b = 1;
    b.cant_puertos_a = 0;
    b.cant_puertos_b = 0;
    b.protocolo = 0;
    b.subredes_a = malloc(sizeof(struct subred)); /* a */
    inet_aton("192.168.1.0", &(b.subredes_a->red));
    b.subredes_a->mascara = GET_MASCARA(24);
    b.subredes_b = malloc(sizeof(struct subred)); /* b */
    inet_aton("10.0.0.0", &(b.subredes_b->red));
    b.subredes_b->mascara = GET_MASCARA(8);

    /* creo paquete */
    inet_aton("192.168.1.1", &(x.origen));
    inet_aton("192.168.0.121", &(x.destino));
    x.puerto_origen = 12345;
    x.puerto_destino = 80;

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
    a.cant_subredes_a = 1;
    a.cant_subredes_b = 0;
    a.cant_puertos_a = 0;
    a.cant_puertos_b = 0;
    a.protocolo = 0;
    a.subredes_a = malloc(sizeof(struct subred)); /* a */
    inet_aton("192.168.0.0", &(a.subredes_a->red));
    a.subredes_a->mascara = GET_MASCARA(24);

    /* creo paquete */
    inet_aton("192.168.1.1", &(x.origen));
    inet_aton("192.168.2.121", &(x.destino));
    x.puerto_origen = 12345;
    x.puerto_destino = 80;

    for(i = 0; i < cantidad_clases; i++) {
        coincide(&a, &x);
    }
}

int main() {
    test_mascara();
    test_in_net();
    test_cidr_b_contiene_a();
    test_cidr_a_contiene_b();
    test_cidr_iguales();
    test_cidr_sin_coincidencia();
    test_subred_comparar_menor();
    test_subred_comparar_mayor();
    test_subred_comparar_igual();
    test_subred_comparar_contenida();
    test_coincide_subred();
    test_coincide_muchas_subredes();
    test_coincide_subred_origen_destino();
    test_coincide_stress(50000000);
    //test_coincide_puerto();
    //test_coincide_muchos_puertos();
    //test_coincide_puerto_ab();
    printf("SUCCESS\n");
    return 0;
}
