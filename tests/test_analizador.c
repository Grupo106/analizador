#include <assert.h>
#include <stdio.h>
#include <netinet/in.h>
#include "../src/analizador.h"

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
    ret = cidr_contiene(&a, &b);
    assert(ret == SIN_COINCIDENCIA);
    ret = cidr_contiene(&b, &a);
    assert(ret == SIN_COINCIDENCIA);
}

int main() {
    test_cidr_b_contiene_a();
    test_cidr_a_contiene_b();
    test_cidr_sin_coincidencia();
    return 0;
}
