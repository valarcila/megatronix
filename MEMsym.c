/*	MEGATRONIX
*/

#include <stdio.h>
#include <stdlib.h>

#define TAM_LINEA 8
#define NUM_FILAS 8

typedef struct {
	unsigned char ETQ;
	unsigned char Data[TAM_LINEA];
} T_CACHE_LINE;

void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]);
void VolcarCACHE(T_CACHE_LINE *tbl);
void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque);
void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque);

void main(int argc, char **argv){
	printf("prueba");
}

void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]){

}

void VolcarCACHE(T_CACHE_LINE *tbl){

}

void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque){

}

void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque){

}
