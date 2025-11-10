/*	megatronix
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// número bloques RAM = número líneas caché
#define NUM_FILAS 8
#define TAM_LINEA 16
#define MAX_LINEA 100
#define MAX_CARACTERES 100

typedef struct {
	unsigned char ETQ;
	unsigned char Data[TAM_LINEA];
} T_CACHE_LINE;

void LeerRAM(unsigned char *Simul_RAM);
char *LeerLineaFichero(FILE *fd);
void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]);
void VolcarCACHE(T_CACHE_LINE *tbl);
void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque);
void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque);

int main(int argc, char **argv){
	int globaltime = 0;
	int numaccesos = 0;
	int acierto = 0;
	int numfallos = 0;
	float tiempomedio = 0;

	T_CACHE_LINE tbl[NUM_FILAS];
	LimpiarCACHE(tbl);
	
	unsigned char Simul_RAM[4096];
	LeerRAM(Simul_RAM);

	unsigned int addr = 0;
	int ETQ, linea, palabra, bloque;

	char *lineaFichero = {0};

	char texto[MAX_CARACTERES] = {0};
	int posicion = 0;

	FILE *CPU = NULL;
	CPU = fopen("accesos_memoria.txt", "r");

	if (!CPU){
		printf("Error al abrir accesos_memoria.txt\n");
		return -1;
	}

	// leer accesos_memoria.txt
	while ((lineaFichero = LeerLineaFichero(CPU))!= NULL){
		globaltime += 1;
		numaccesos++;
		addr = 0;
		acierto = 0;
		
		if (lineaFichero != NULL){
			// convertir lineaFichero char * (hexadecimal) a unsigned int
			// strtoul(puntero a línea, dónde termina conversión, base numérica)
			addr = (unsigned int) strtoul(lineaFichero, NULL, 16);
		}
		
		ParsearDireccion(addr, &ETQ, &palabra, &linea, &bloque);
			
		while (acierto != 1){
			if(tbl[linea].ETQ != (unsigned char) ETQ){
				// fallo caché
				numfallos++;
				printf("T: %d, Fallo de CACHE %d, ADDR %04X Label %X linea %02X palabra %02X bloque %02X\n", globaltime, numfallos, addr, ETQ, linea, palabra, bloque);
				globaltime += 20;
				TratarFallo(tbl, Simul_RAM, ETQ, linea, bloque);
			} else {
				// acierto caché
				acierto = 1;
				unsigned char data = tbl[linea].Data[palabra];
				printf("T: %d, Acierto de CACHE, ADDR %04X Label %X linea %02X palabra %02X DATO %02X\n", globaltime, addr, ETQ, linea, palabra, data);
				
				// guardar carácter en texto
				if (posicion < MAX_CARACTERES - 1){
					texto[posicion++] = data;
				}
				VolcarCACHE(tbl);
			}
		}
		sleep(1);
	}
	fclose(CPU);

	tiempomedio = (float) globaltime / (float) numaccesos;

	printf("Accesos totales = %d, fallos = %d, tiempo medio = %.2f\n", numaccesos, numfallos, tiempomedio);

	texto[posicion] = '\0';

	printf("Texto leído: %s\n", texto);

	// caché > CONTENTS_CACHE.bin
	FILE *bin = NULL;
	bin = fopen("CONTENTS_CACHE.bin", "wb");

	if (!bin) {
		printf("Error al abrir CONTENTS_CACHE.bin\n");
		return -1;
	}
	for (int linea = 0; linea < NUM_FILAS; linea++) {
		for (int palabra = 0; palabra < TAM_LINEA; palabra++) {
			// leer B línea
			unsigned char data = tbl[linea].Data[palabra];

			// escribir B en .bin
			fwrite(&data, sizeof(unsigned char), 1, bin);
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
		printf("Error al abrir CONTENTS_RAM.bin\n");
		return;
	}
	// leer 4096 B CONTENTS_RAM.bin > Simul_RAM
	fread(Simul_RAM, sizeof(unsigned char), 4096, RAM);

	fclose(RAM);
}

char *LeerLineaFichero(FILE *fd){
	// memoria estática
	static char linea[MAX_LINEA] = {0};
    int c = '\0';
	int tamano = 0;

    if (feof(fd)) {
		return NULL;
	}
    while ((c = fgetc(fd)) != EOF && c != '\n') {
        if (tamano < MAX_LINEA - 1) {
            linea[tamano++] = (char) c;
        }
    }
    if (tamano == 0 && c == EOF) {
		return NULL;
	}
    linea[tamano] = '\0';

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
    printf("Cargando el bloque %02X en la linea %02X\n", bloque, linea);
    
	// actualizar etiqueta
    tbl[linea].ETQ = (unsigned char) ETQ;

	// inicio bloque en RAM
    int inicio = bloque * TAM_LINEA;

	// copiar 16 B bloque desde RAM a CACHÉ
    for (int i = 0; i < TAM_LINEA; i++){
		tbl[linea].Data[i] = (unsigned char) MRAM[inicio + i];
	}
}
