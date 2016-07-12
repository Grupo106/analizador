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
 * bd_paquetes(*clases, *cfg)
 * -------------------------------------------------------------------------
 * Obtiene los paquetes capturados segun configuracion pasada por parametro y
 * llama a la funcion callback pasada por parametro.
 */
void bd_paquetes(struct s_analizador*,
                 int (*callback)(struct s_analizador*, struct paquete*));

#endif /* DB_H */
