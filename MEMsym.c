/*	MEGATRONIX
*/

#include <stdio.h>
#include <stdlib.h>

#define NUM_FILAS 8
#define TAM_LINEA 16
#define MAX_LINEA 1024

typedef struct {
	unsigned char ETQ;
	unsigned char Data[TAM_LINEA];
} T_CACHE_LINE;

void LeerRAM(unsigned char *Simul_RAM);
void ComparacionCache(T_CACHE_LINE *tbl, int ETQ, int linea, int palabra, int bloque, unsigned char *Simul_RAM);
char* leeLineaArchivo(FILE *fd);
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
		return;
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

void ComparacionCache(T_CACHE_LINE *tbl, int ETQ, int linea, int palabra, int bloque, unsigned char *Simul_RAM){
	FILE *CPU=NULL;
	CPU = fopen("accesos_memoria", "r");
	if (!CPU){
		printf("error al abrir accesos_memoria.txt");
	}

	char *lineaLeida = leeLineaArchivo(CPU);
	unsigned int linea_bi = 0;
	if (lineaLeida != NULL){
		linea_bi = (unsigned int)strtoul(lineaLeida, NULL, 16);
	}

	fclose(CPU);

	ParsearDireccion(linea_bi, &ETQ, &palabra, &linea, &bloque);

	if(tbl[linea].ETQ == ETQ){
		//acierto
		printf("ACIERTO\n");
	}
	else{
		//fallo
		printf("FALLO\n");
		TratarFallo(tbl, Simul_RAM, ETQ, linea, bloque);
	}
}

char* leeLineaArchivo(FILE *fd){
	static char linea[MAX_LINEA];
    int tam = 0;
    int c;

    if (feof(fd)) {
		return NULL;
	}

    while ((c = fgetc(fd)) != EOF && c != '\n') {
        if (tam < MAX_LINEA - 1) {
            linea[tam++] = (char)c;
        }
    }

    if (tam == 0 && c == EOF) {
		return NULL;
	}

    linea[tam] = '\0';
    return linea;
}

void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque){

}
