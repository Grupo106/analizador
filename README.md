[![GitHub tag](https://img.shields.io/github/tag/Grupo106/analizador.svg?maxAge=2592000?style=plastic)](https://github.com/Grupo106/analizador/releases)
[![Build Status](https://travis-ci.org/Grupo106/analizador.svg?branch=master)](https://travis-ci.org/Grupo106/analizador)

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

Ver logs
-------------------------------------------------------
Para ver logs generados por la aplicación se puede utilizar el journalctl
provisto por systemd
```
journalctl /usr/local/bin/analizador -f
```
