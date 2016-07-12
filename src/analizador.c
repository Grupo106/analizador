#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

/**
 * imprimir(clases, cantidad)
 * ---------------------------------------------------------------------------
 *  Imprime las clases de trafico en la salida estandar en formato JSON.
 */
int imprimir(const struct s_analizador *analizador)
{
    return clases_to_json(stdout, analizador);
}

/**
 * to_json(file, clases, cantidad)
 * ---------------------------------------------------------------------------
 *  Escribe las clases de trafico en el archivo pasado por parametro en formato
 *  JSON
 */
int clases_to_json(FILE* file, const struct s_analizador *analizador)
{
    int i;
    int cantidad = analizador->cant_clases;
    struct clase* clases = analizador->clases;
    /* inicio array */
    fprintf(file, "[\n");
    /* imprimo clases */
    for(i = 0; i < cantidad; i++) {
        fprintf(file, "  {\n"
                      "    \"id\": %d,\n"
                      "    \"nombre\": \"%s\",\n"
                      "    \"descripcion\": \"%s\",\n"
                      "    \"subida\": %d,\n"
                      "    \"bajada\": %d\n"
                      "  }%c\n",
                      (clases + i)->id,
                      (clases + i)->nombre,
                      (clases + i)->descripcion,
                      (clases + i)->bytes_subida,
                      (clases + i)->bytes_bajada,
                      i + 1 < cantidad ? ',' : ' ');
    }
    /* fin array */
    fprintf(file, "]\n");
    return 0;
}

/**
 * get_clases(**clases, *cfg)
 * ---------------------------------------------------------------------------
 *  Obtiene el array de clases de trafico que se utilizara en el analisis.
 *  Devuelve la cantidad de clases que contiene el array
 *
 *  ### Parametros:
 *    * clases: Puntero a un array donde se almacenaran las clases
 *    * cfg: Puntero a la configuracion del analizador que contiene los
 *           parametros por los cuales se seleccionaran las clases.
 */
int get_clases(struct s_analizador *analizador)
{
    struct clase *c;
    analizador->cant_clases = 4;
    c = analizador->clases = malloc(4 * sizeof(struct clase));
    /* La primer clase es la default */
    init_clase(c);
    strncpy(c->nombre, "Default", LONG_NOMBRE);
    strncpy(c->descripcion, "Clase por defecto", LONG_DESCRIPCION);
    /* XXX: Mock clases */
    /* SSH */
    c++;
    init_clase(c);
    strncpy(c->nombre, "SSH", LONG_NOMBRE);
    strncpy(c->descripcion, "Secure shell - Administracion remota",
            LONG_DESCRIPCION);
    c->cant_puertos_a = 1;
    c->puertos_a = malloc(sizeof(u_int16_t));
    *(c->puertos_a) = 22;
    /* HTTP */
    c++;
    init_clase(c);
    strncpy(c->nombre, "HTTP", LONG_NOMBRE);
    strncpy(c->descripcion, "Navegacion web", LONG_DESCRIPCION);
    c->cant_puertos_a = 1;
    c->puertos_a = malloc(sizeof(u_int16_t));
    *(c->puertos_a) = 80;
    /* HTTPS */
    c++;
    init_clase(c);
    strncpy(c->nombre, "HTTPS", LONG_NOMBRE);
    strncpy(c->descripcion, "Navegacion web segura", LONG_DESCRIPCION);
    c->cant_puertos_a = 1;
    c->puertos_a = malloc(sizeof(u_int16_t));
    *(c->puertos_a) = 443;
    return 0;
}

/**
 * init_clase
 * --------------------------------------------------------------------------
 *  Inicializa una estructura de clase de trafico a valores por defecto
 */
void init_clase(struct clase *clase) {
    clase->id = 0;
    *(clase->nombre) = '\0';
    *(clase->descripcion) = '\0';
    clase->cant_subredes_a = 0;
    clase->cant_subredes_b = 0;
    clase->cant_puertos_a = 0;
    clase->cant_puertos_b = 0;
    clase->protocolo = 0;
    clase->bytes_subida = 0;
    clase->bytes_bajada = 0;
    clase->subredes_a = NULL;
    clase->subredes_b = NULL;
    clase->puertos_a = NULL;
    clase->puertos_b = NULL;
}

/**
 * free_clase
 * --------------------------------------------------------------------------
 *  Libera memoria ocupada por una clase de trafico
 */
void free_clase(struct clase *clase) {
    free(clase->subredes_a);
    free(clase->subredes_b);
    free(clase->puertos_a);
    free(clase->puertos_b);
}

int analizar_paquete(struct s_analizador* analizador, struct paquete* paquete)
{
    int coincidencias = 0,
        c = 0,
        i = 0;
    struct clase *clase = (analizador->clases + i);
    for (i = 1; i < analizador->cant_clases; i++) {
        c = coincide(clase, paquete);
        coincidencias += c;
        if (c) {
            if(paquete->direccion == ENTRANTE)
                clase->bytes_bajada += paquete->bytes;
            else if(paquete->direccion == SALIENTE)
                clase->bytes_subida += paquete->bytes;
        }
    }
    /* lo agrego a la clase default */
    if(!coincidencias) {
        if(paquete->direccion == ENTRANTE)
            analizador->clases->bytes_bajada += paquete->bytes;
        else if(paquete->direccion == SALIENTE)
            analizador->clases->bytes_subida += paquete->bytes;
    }
    return 0;
}
