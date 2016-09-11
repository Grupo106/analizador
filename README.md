[![GitHub tag](https://img.shields.io/github/tag/Grupo106/analizador.svg?maxAge=2592000?style=plastic)](https://github.com/Grupo106/analizador/releases)
[![Build Status](https://travis-ci.org/Grupo106/analizador.svg?branch=master)](https://travis-ci.org/Grupo106/analizador)
[![codecov](https://codecov.io/gh/Grupo106/analizador/branch/master/graph/badge.svg)](https://codecov.io/gh/Grupo106/analizador)

Analizador
======================================================
> Obtiene información de los paquetes capturados en un intervalo de tiempo.

Funcionalidades
------------------------------------------------------
* Compara tráfico capturado con las clases de tráfico instaladas.
* Genera JSON como resultado de la captura en la salida estándar.

Dependencias
-------------------------------------------------------
### Debian

```sh
apt-get install posgresql-dev
```

Compilación e instalación
-------------------------------------------------------
### Desarrollo
```sh
make debug
```

### Produccion
```sh
make
sudo make install
```

Uso
-------------------------------------------------------
```
Uso: analizar [-h] | [-v] | [segundos] | [inicio fin]

Este programa compara las clases de trafico intaladas con los paquetes capturados
en un intervalo de tiempo especifico. Si no se especifica ningun parametro, se
analizaran los paquetes recibidos desde los ultimos 60 segundos.

Parametros:
  -h, --help             Muestra esta ayuda.
  -v, --version          Muestra numero de version.
  segundos               Cantidad de segundos desde que se analizarán los paquetes
  inicio fin             Intervalo de tiempo en los que se analizaran los paquetes en formato ISO8601.
(c) Netcop 2016 - Universidad Nacional de la Matanza
```

Ver logs
-------------------------------------------------------
Para ver logs generados por la aplicación se puede utilizar el journalctl
provisto por systemd
```
journalctl /usr/local/bin/analizador -f
```
