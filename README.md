# Megatronix

Sistemas operativos.

Simulador de memoria caché en C. 

Lee la secuencia de accesos de memoria y registra qué accesos son aciertos y cuáles fallos.

## Cómo probarlo

```bash
gcc MEMsym.c -o MEMsym
./MEMsym
```

Entrada. `accesos_memoria.txt` y `CONTENTS_RAM.bin`

Salida. `logcache.txt` y `CONTENTS_CACHE.bin`

## Autores

Iván García-Romero (@IvanGRP) y Valeria Arcila (@valarcila)
