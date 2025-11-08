/*	MEGATRONIX
*/

#include <stdio.h>
#include <stdlib.h>

#define NUM_FILAS 8
#define TAM_LINEA 16

typedef struct {
	unsigned char ETQ;
	unsigned char Data[TAM_LINEA];
} T_CACHE_LINE;

void LeerRAM(unsigned char *Simul_RAM);
void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]);
void VolcarCACHE(T_CACHE_LINE *tbl);
void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque);
void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque);

void main(int argc, char **argv){

	int globaltime = 0;
	int numfallos = 0;

	T_CACHE_LINE tbl[NUM_FILAS];
	LimpiarCACHE(tbl);
	
	unsigned char Simul_RAM[4096];
	LeerRAM(Simul_RAM);

	unsigned int addr;
}

void LeerRAM(unsigned char *Simul_RAM){
	FILE *RAM = NULL;

	RAM = fopen("CONTENTS_RAM.bin", "rb");
	if (!RAM){
		printf("error al abrir CONTENTS_RAM.bin");
	}

	fseek(RAM, 0, SEEK_END);
	long size = ftell(RAM);
	rewind(RAM);
	fread(Simul_RAM, sizeof(unsigned char), size, RAM);
	fclose(RAM);
}

void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]){
	for (int i = 0; i < NUM_FILAS; i++){
		// 0x indicar hexadecimal
		tbl[i].ETQ = 0xFF;
		for (int j = 0; j < TAM_LINEA; j++){
			tbl[i].Data[j] = 0x23;
		}
	}
}

void VolcarCACHE(T_CACHE_LINE *tbl){

}

void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque){
	/* 11 10 9 8 7 | 6 5 4 | 3 2 1 0
	   etiqueta      línea   palabra 	
	*/

	// &, AND bit a bit
	*palabra = addr & 0xF;

	// >>, mover posiciones derecha
	*linea = (addr >> 4) & 0x7;
	*ETQ = (addr >> 7) & 0x1F;
	*bloque = (addr >> 4);
}

void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque){

}
