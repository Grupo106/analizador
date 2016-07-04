#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
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

int main() {
    test_cidr_b_contiene_a();
    test_cidr_a_contiene_b();
    test_cidr_sin_coincidencia();
    test_subred_comparar_menor();
    test_subred_comparar_mayor();
    test_subred_comparar_igual();
    test_subred_comparar_contenida();
    test_cidr_insertar();
    return 0;
}
