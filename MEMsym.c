/*	MEGATRONIX
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

int main(int argc, char **argv){

	int globaltime = 0, numaccesos = 0, numfallos = 0;

	T_CACHE_LINE tbl[NUM_FILAS];
	LimpiarCACHE(tbl);
	
	unsigned char Simul_RAM[4096];
	LeerRAM(Simul_RAM);

	unsigned int addr;
	int ETQ, linea, palabra, bloque;

	FILE *CPU = NULL;

	CPU = fopen("accesos_memoria.txt", "r");
	if (!CPU){
		printf("Error al abrir accesos_memoria.txt");
		exit (-1);
	}

	char *lineaLeida = (CPU);
	unsigned int linea_bi = 0;
	if (lineaLeida != NULL){
		linea_bi = (unsigned int)strtoul(lineaLeida, NULL, 16);
	}

	

	ParsearDireccion(linea_bi, &ETQ, &palabra, &linea, &bloque);

	if(tbl[linea].ETQ != (unsigned char) ETQ){
		numfallos++;
		printf("T: %d, Fallo de CACHE %d, ADDR %04X Label %X linea %02X palabra %02X bloque %02X", globaltime, numfallos, addr, ETQ, linea, palabra, bloque);
		
		TratarFallo(tbl, Simul_RAM, ETQ, linea, bloque);
	} else {
		unsigned char dato = tbl[linea].Data[palabra];
		printf("T: %d, Acierto de CACHE, ADDR %04X Label %X linea %02X palabra %02X DATO %02X", globaltime, addr, ETQ, linea, palabra, dato);
	}

	fclose(CPU);

	float tiempomedio = (float) globaltime / numaccesos;
	printf("Accesos totales = %d, fallos = %d, tiempo medio = %d\n", numaccesos, numfallos, tiempomedio);

	// texto leído

	return 0;
}

void LeerRAM(unsigned char *Simul_RAM){
	FILE *RAM = NULL;

	RAM = fopen("CONTENTS_RAM.bin", "rb");
	if (!RAM){
		printf("Error al abrir CONTENTS_RAM.bin");
		exit (-1);
	}

	fread(Simul_RAM, sizeof(unsigned char), 4096, RAM);
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
	for (int i = 0; i < NUM_FILAS; i++){
		// imprimir etiqueta
		printf("%02X", tbl[i].ETQ);
		
		// imprimir datos, 15 a 0
		printf("Datos: ");
		for (int j = TAM_LINEA - 1; j >= 0 ; j--){
			printf("%02X ", tbl[i].Data[j]);
		}
		printf("\n");
	}
	printf("\n");
}

void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque){
	// 11 10 9 8 7 | 6 5 4 | 3 2 1 0
	// etiqueta      línea   palabra 	

	// &, AND bit a bit
	*palabra = addr & 0xF;

	// >>, mover posiciones derecha
	*linea = (addr >> 4) & 0x7;
	*ETQ = (addr >> 7) & 0x1F;
	*bloque = addr / 4;
}

void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque){
	printf("Cargando el bloque %02X en la linea %02X", bloque, linea);
	
	// actualizar etiqueta
	tbl[linea].ETQ = (unsigned char) ETQ;

	// inicio bloque en RAM
	int inicio = bloque * TAM_LINEA;

	// copiar 16 B bloque desde RAM a CACHÉ
	for (int i = 0; i < TAM_LINEA; i++){
		tbl[linea].Data[i] = (unsigned char) MRAM[inicio + i];
	}
}
