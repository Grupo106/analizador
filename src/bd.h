#ifndef DB_H
#define DB_H

#include "paquete.h"

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

#endif /* DB_H */
