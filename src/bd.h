#ifndef DB_H
#define DB_H

#include "paquete.h"
#include "analizador.h"

/**
 * bd_conectar()
 * -------------------------------------------------------------------------
 * Conecta con la base de datos
 */
int bd_conectar();

/**
 * bd_desconectar()
 * -------------------------------------------------------------------------
 * Desconecta con la base de datos
 */
void bd_desconectar();

/**
 * bd_commit()
 * -------------------------------------------------------------------------
 * Hace un commit de la transaccion en la base de datos
 */
void bd_commit();

/**
 * obtener_paquetes
 * -------------------------------------------------------------------------
 * Obtiene los paquetes capturados segun configuracion pasada por parametro y
 * llama a la funcion callback pasada por parametro. Devuelve la cantidad de
 * paquetes analizados
 */
int obtener_paquetes(struct s_analizador* analizador,
                     int (*callback)(const struct s_analizador*,
                                     const struct paquete*));

/**
 * obtener_clases(**clases, *cfg)
 * ---------------------------------------------------------------------------
 *  Obtiene el array de clases de trafico que se utilizara en el analisis.
 *  Devuelve la cantidad de clases que contiene el array
 *
 *  ### Parametros:
 *    * clases: Puntero a un array donde se almacenaran las clases
 *    * cfg: Puntero a la configuracion del analizador que contiene los
 *           parametros por los cuales se seleccionaran las clases.
 */
int obtener_clases(struct s_analizador*);

/**
 * init_clase
 * --------------------------------------------------------------------------
 *  Inicializa una estructura de clase de trafico a valores por defecto
 */
#define init_clase(x) memset(x, 0, sizeof(struct clase));

/**
 * free_clase
 * --------------------------------------------------------------------------
 *  Libera memoria ocupada por una clase de trafico
 */
void free_clase(struct clase *clase);

/**
 * obtener_subredes(*clase)
 * ---------------------------------------------------------------------------
 *  Obtiene el array de subredes que componen la clase de trafico.
 *  Devuelve la cantidad de subredes que contiene el array
 *
 *  El grupo puede ser 'a' o 'b'
 */
int obtener_subredes(struct clase*, char grupo);

/**
 * obtener_puertos(*clase)
 * ---------------------------------------------------------------------------
 *  Obtiene el array de puertos que componen la clase de trafico.
 *  Devuelve la cantidad de puertos que contiene el array
 *
 *  El grupo puede ser 'a' o 'b'
 */
int obtener_puertos(struct clase* clase, char grupo);

#endif /* DB_H */
