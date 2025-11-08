/*	MEGATRONIX
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_FILAS 8
#define TAM_LINEA 16
#define MAX_LINEA 100

typedef struct {
	unsigned char ETQ;
	unsigned char Data[TAM_LINEA];
} T_CACHE_LINE;

void LeerRAM(unsigned char *Simul_RAM);
void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]);
char *LeerLineaArchivo(FILE *fd);
void VolcarCACHE(T_CACHE_LINE *tbl);
void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque);
void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque);

int main(int argc, char **argv){

	char *texto[100]={0};

	int globaltime = 0, numaccesos = 0, numfallos = 0;

	T_CACHE_LINE tbl[NUM_FILAS];
	LimpiarCACHE(tbl);
	
	unsigned char Simul_RAM[4096];
	LeerRAM(Simul_RAM);

	int ETQ, linea, palabra, bloque;
	char *lineaLeida = {0};

	FILE *CPU = NULL;

	CPU = fopen("accesos_memoria.txt", "r");
	if (!CPU){
		printf("\nError al abrir accesos_memoria.txt");
		return -1;
	}

	while ((lineaLeida = LeerLineaArchivo(CPU))!= NULL){
		numaccesos++;
		int validez = 0;
		globaltime += 1;

		
		unsigned int addr = 0;
		if (lineaLeida != NULL){
			addr = (unsigned int)strtoul(lineaLeida, NULL, 16);
		}
		
		ParsearDireccion(addr, &ETQ, &palabra, &linea, &bloque);
			
		while(validez!= 1){
			if(tbl[linea].ETQ != (unsigned char) ETQ){
				numfallos++;
				printf("\nT: %d, Fallo de CACHE %d, ADDR %04X Label %X linea %02X palabra %02X bloque %02X", globaltime, numfallos, addr, ETQ, linea, palabra, bloque);
				globaltime += 20;
				TratarFallo(tbl, Simul_RAM, ETQ, linea, bloque);
			} else {
				validez = 1;
				unsigned char dato = tbl[linea].Data[palabra];
				*texto+= dato;
				printf("\nT: %d, Acierto de CACHE, ADDR %04X Label %X linea %02X palabra %02X DATO %02X", globaltime, addr, ETQ, linea, palabra, dato);
				printf("\n");
				VolcarCACHE(tbl);
				
			}
		}
		
		
	}
	fclose(CPU);

	float tiempomedio = (float) globaltime / (float)numaccesos;
	printf("\nAccesos totales = %d, fallos = %d, tiempo medio = %.2f\n", numaccesos, numfallos, tiempomedio);

	FILE *bin = fopen("CONTENTS_CACHE.bin", "wb");
	if (bin == NULL) {
		printf("\nError al abrir CONTENTS_CACHE.bin\n");
		return -1;
	}

	for (int linea= 0; linea < 8; linea++) {
		for (int palabra = 0; palabra < 16; palabra++) {
			unsigned char dato = tbl[linea].Data[palabra];
			fwrite(&dato, sizeof(unsigned char), 1, bin);
		}
		unsigned char salto = '\n';
    	fwrite(&salto, sizeof(unsigned char), 1, bin);
	}
	fclose(bin);

	return 0;
}

void LeerRAM(unsigned char *Simul_RAM){
	FILE *RAM = NULL;

	RAM = fopen("CONTENTS_RAM.bin", "rb");
	if (!RAM){
		printf("\nError al abrir CONTENTS_RAM.bin");
		return;
	}

	fread(Simul_RAM, sizeof(unsigned char), 4096, RAM);
	fclose(RAM);
}

char *LeerLineaArchivo(FILE *fd){
	
	static char linea[MAX_LINEA] = {0};
    int c = '\0';
	int tam = 0;

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
		printf("%02X ", tbl[i].ETQ);
		
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
	*bloque = addr >> 4;
}

void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque){
    printf("\nCargando el bloque %02X en la linea %02X\n", bloque, linea);
    
	// actualizar etiqueta
    tbl[linea].ETQ = (unsigned char) ETQ;

	// inicio bloque en RAM
    int inicio = bloque * TAM_LINEA;

	// copiar 16 B bloque desde RAM a CACHÉ
    for (int i = 0; i < TAM_LINEA; i++){
            tbl[linea].Data[i] = MRAM[inicio + i];
    }
}
