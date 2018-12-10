#ifndef __SA__
#define __SA__
#include "inttypes.h"

#pragma pack(1)

#define TAM_MEMORIA 8192
#define QTD_ENTRADAS 12
#define TAM_BLOCO_PONTEIRO 2

/*
Estrutura de dados para arquivos, blocos de dados e blocos de indices
*/

typedef struct 
{
	uint16_t id;
	uint16_t posicao; 
} MEU_FILE;

typedef uint8_t  bloco_dados[32];

typedef uint16_t  bloco_indice[16];

struct inodo {
	char status;
	char nome[8];
	bloco_dados dados_diretos;
	uint16_t indireto;
	uint16_t tam;
};

#define TAM_ENTRADA (sizeof(struct inodo))
#define INICIO_PONTEIROS ((TAM_BLOCO_PONTEIRO)+QTD_ENTRADAS*TAM_ENTRADA)
#define INICIO_DADOS (INICIO_PONTEIROS + 225*TAM_BLOCO_PONTEIRO)

MEU_FILE * meu_fopen (const char *st, const char *modo);
void init(void);
void formata (void);
void meu_fclose(MEU_FILE *A);
void meu_fwrite(const char  *valor , MEU_FILE *A);
void meu_fputc(uint8_t valor , MEU_FILE *A);
void meu_fseek(MEU_FILE *A, uint16_t offset);
void abre_arquivo_imprime(MEU_FILE * A);
int meu_fgetc(MEU_FILE *A);
int meu_feof(MEU_FILE *A);
uint16_t meu_fread(MEU_FILE *A, uint8_t *buffer, int tamanho);
uint16_t meu_ftell(MEU_FILE *A);
uint16_t tamanho_arquivo(MEU_FILE *A);

#endif
