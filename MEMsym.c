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

void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]);
void VolcarCACHE(T_CACHE_LINE *tbl);
void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque);
void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque);

void main(int argc, char **argv){

	T_CACHE_LINE cache_lines[NUM_FILAS];
	unsigned char Simul_RAM[4096];
	int globaltime = 0;
	int numfallos = 0;
	LeerRAM(Simul_RAM);

	
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
		for (int j = 0; j < TAM_LINEA; i++){
			tbl[i].Data[j] = 0x23;
		}
	}
}

void VolcarCACHE(T_CACHE_LINE *tbl){

}

void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque){

}

void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque){

}
