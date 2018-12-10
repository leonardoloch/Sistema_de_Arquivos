
#include "SA.h"
#include "memoria.h"
#include "inttypes.h"
#include "string.h"
#include <stdlib.h>
#include <stdio.h>


uint16_t aloca (void);
uint16_t verifica_arquivo (const char *st);
void remove_entrada (uint8_t id);
void escreva_bloco_dados(uint16_t numero ,uint8_t *valor);
void leia_bloco_dados(uint16_t numero, uint8_t *valor);
void leia_entrada (uint8_t numero, struct inodo *i);

uint16_t tamanho_arquivo(MEU_FILE *A){
	struct inodo arq;
	leia_entrada (A->id, &arq);
	return arq.tam;
}

void le_entrada_arquivo (uint16_t numero, struct inodo *tmp)
{
	uint16_t posicao = 2 + numero*sizeof(struct inodo);
	memoria.read(posicao, sizeof(struct inodo), (uint8_t *) tmp);
}
void escreve_entrada( uint16_t numero, struct inodo tmp)
{
	uint16_t posicao = 2 + numero*sizeof(struct inodo);
	memoria.write(posicao, sizeof(struct inodo), (uint8_t *)&tmp);
}
	
void meu_fclose(MEU_FILE *A)
{
    if(A != NULL)
		free(A);
	A = NULL;
}

void abre_arquivo_imprime(MEU_FILE * A)
{
	printf("Tentei imprimir\n");
	uint16_t j = tamanho_arquivo(A);
	uint8_t *b = (uint8_t *) malloc(j+1);
	meu_fread(A, b, j);
	b[j] = 0;
	printf("%s\r\n", b);
	free(b);
}

uint16_t meu_fread(MEU_FILE *A, uint8_t *buffer, int tamanho)
{
	struct inodo lida;
	bloco_dados  bloco;
	bloco_indice bloco_idx;
	uint16_t n = 0;
	uint16_t total = 0;
	uint16_t cont = 0;

	if (A == NULL) return 0;
	uint16_t id = A->id;
	uint16_t posicao = 0;
	
	leia_entrada(id, &lida);
	while(total < tamanho) 
	{		
		if (cont < 32)
		{
			buffer[cont] = lida.dados_diretos[posicao];
			total++;
		}
		if(tamanho > 32 && (cont + 32 < tamanho))  
		{	
			if (lida.indireto==0xFFFF) 
			{
				return total;
			}
			uint16_t numero_entrada = (posicao + 32) / 32;
			uint8_t deslocamento    = (posicao + 32) % 32;
			numero_entrada--;
			
			leia_bloco_dados (lida.indireto, (uint8_t *) &bloco_idx);
			if (bloco_idx[numero_entrada]==0xffff)
			{
				return total;
			}

			leia_bloco_dados (lida.indireto, (uint8_t *) &bloco_idx);
			n = bloco_idx[numero_entrada];

			leia_bloco_dados (n, (uint8_t *) &bloco);
			buffer[cont + 32] = bloco[deslocamento];
			total++;	
		}
		cont++;	
		posicao++;
	}
	return total;
}


MEU_FILE * meu_fopen (const char *st, const char *modo)
{
	MEU_FILE * tmp = NULL;
	struct inodo meu_inodo;
	if (strcmp(modo, "w")==0)
	{
		int livre = -1;
		for (int x=0;x<12;x++)
		{
			le_entrada_arquivo(x, &meu_inodo);
			if ((meu_inodo.status == 0) && (livre == -1)) livre = x;
			if (strcmp(meu_inodo.nome, st)==0)
			{
				remove_entrada(x);
				if(livre == -1) livre = x;
			}
		}
		meu_inodo.status = 1;
		meu_inodo.tam=0;
		strcpy(meu_inodo.nome,st);
		meu_inodo.indireto=0xffff;
		escreve_entrada(livre , meu_inodo );		
	
		tmp  = (MEU_FILE *) malloc (sizeof(MEU_FILE));
		tmp->id = livre;
		tmp->posicao = 0;
		
		
	}
	else if (strcmp(modo,"r+")==0)
    {
		uint16_t p = 0;
        p = verifica_arquivo(st);
        if (p == 0xffff) return NULL;
        tmp = (MEU_FILE *) malloc (sizeof(MEU_FILE));
		tmp->posicao = 0;
		tmp->id=p;
	}

	return tmp;
}

uint16_t le_cabecalho ()
{
	uint16_t numero;
	memoria.read(0, 2, (uint8_t *) &numero);
	return numero;
}

void escreve_cabecalho (uint16_t numero)
{
	memoria.write(0, 2, (uint8_t *) &numero);
}

/* Dada um indice de entrada e um ponteiro para um inodo, lemos a informação e colocamos
na estrutura do inodo */
void leia_entrada (uint8_t numero, struct inodo *i)
{
	uint16_t posicao = 2 + numero*sizeof(struct inodo);
	memoria.read(posicao, sizeof(struct inodo), (uint8_t *)i);
}

/*Escreve na entrada a estrutura inodo*/
void escreva_entrada (uint8_t numero, struct inodo *i)
{
	uint16_t posicao = 2 + numero*sizeof(struct inodo);
	memoria.write(posicao, sizeof(struct inodo), (uint8_t *)i);
}

/*Retorna a posição final do arquivo*/
uint16_t meu_ftell(MEU_FILE *A)
{
	return A->posicao;
}

/*Redefine a posição final do arquivo*/
void meu_fseek(MEU_FILE *A, uint16_t offset)
{
	A->posicao = offset;
}

/* Pega caracter a caracter do arquivo */
int meu_fgetc(MEU_FILE *A)
{
	struct inodo lida;
	bloco_indice bloco_idx;
	bloco_dados  bloco;
	uint16_t n;
	uint8_t valor;
	
	if (A == NULL) return 0;
	uint16_t id = A->id;
	uint16_t posicao = A->posicao;
	
		
	leia_entrada(id, &lida);

	if (posicao < 32)
	{
		valor = lida.dados_diretos[posicao];
	}
	else  
	{	
		if (lida.indireto==0xFFFF) 
		{
			return 0;
		}
		uint16_t numero_entrada = posicao / 32;
		uint8_t deslocamento    = posicao % 32;
		numero_entrada--;
		
		leia_bloco_dados (lida.indireto, (uint8_t *) &bloco_idx);
		if (bloco_idx[numero_entrada]==0xffff)
		{
			return 0;
		}

		leia_bloco_dados (lida.indireto, (uint8_t *) &bloco_idx);
		n = bloco_idx[numero_entrada];

		leia_bloco_dados (n, (uint8_t *) &bloco);
		valor = bloco[deslocamento];	
	}
	A->posicao++;
	return valor;
}

/* Verifica se o arquivo chegou ao fim, caso tenha chegado retorna 1, se não retorna 0 */
int meu_feof (MEU_FILE *A)
{
	struct inodo lida;
	if (A == NULL) return 1;
	uint16_t id = A->id;

	leia_entrada(id, &lida);
	
	if (lida.tam > A->posicao) return 0;
	return 1;

}

/* Dado um valor o escreve no arquivo */
void meu_fwrite(const char * valor , MEU_FILE *A )
{
	struct inodo lida;
	bloco_indice bloco_idx;
	bloco_dados  bloco;
	uint16_t n=0xffff;

	uint16_t cont = 0;
	uint16_t total = 0;
	if (A == NULL) return;
	uint16_t id = A->id;

	uint16_t tamanho_vetor = strlen(valor);
	char * n_str = (char *) malloc(sizeof(char *)*(A->posicao+tamanho_vetor));
	if(A->posicao != 0)
	{
		uint8_t *b = (uint8_t *) malloc(A->posicao);
		meu_fread(A, b, A->posicao);
		sprintf(n_str, "%s%s", b, valor);
		free(b);
	} 
	else 
	{
		sprintf(n_str, "%s", valor);
	}
	tamanho_vetor = tamanho_vetor + A->posicao;
	A->posicao = 0;
	uint16_t posicao = 0;

	leia_entrada(id, &lida);

	while(total < tamanho_vetor)
	{

		if (posicao < 32)
		{
			lida.dados_diretos[posicao] = (uint8_t)n_str[cont];
			if (A->posicao >= lida.tam) lida.tam++;
			A->posicao++;
			total++;
		}
		if(tamanho_vetor > 32 && (cont + 32) < tamanho_vetor)  
		{	
			if (lida.indireto==0xFFFF) 
			{
				lida.indireto = aloca ();
			}
			uint16_t numero_entrada = (posicao + 32) / 32;
			uint8_t deslocamento    = (posicao + 32) % 32;
			numero_entrada--;
			
			
			leia_bloco_dados (lida.indireto, (uint8_t *) &bloco_idx);
			if (bloco_idx[numero_entrada]==0xffff)
			{
				bloco_idx[numero_entrada] = aloca();
				escreva_bloco_dados(lida.indireto , (uint8_t *) bloco_idx);
			}
			leia_bloco_dados (lida.indireto, (uint8_t *) &bloco_idx);
			n = bloco_idx[numero_entrada];
			
			leia_bloco_dados (n, (uint8_t *) &bloco);
			bloco[deslocamento] = (uint8_t) n_str[cont+32];
			escreva_bloco_dados(n ,  (uint8_t *) &bloco);	
			if ((A->posicao+32) >= (lida.tam+32)) lida.tam++;
			A->posicao++;
			total++;
		}
		posicao++;
		cont++;	
	}
	if (n != 0xffff) escreva_bloco_dados(n, (uint8_t *) &bloco);
	escreva_entrada(id, &lida);
}

void meu_fputc ( uint8_t valor , MEU_FILE *A )
{
	struct inodo lida;
	bloco_indice bloco_idx;
	bloco_dados  bloco;
	uint16_t n;
	
	
	if (A == NULL) return;
	uint16_t id = A->id;
	uint16_t posicao = A->posicao;
	
		
	leia_entrada(id, &lida);

	if (posicao < 32)
	{
		lida.dados_diretos[posicao] = valor;
		
	}
	else  
	{	
		if (lida.indireto==0xFFFF) 
		{
			lida.indireto = aloca ();
		}
		uint16_t numero_entrada = posicao / 32;
		uint8_t deslocamento    = posicao % 32;
		numero_entrada--;
		
		
		leia_bloco_dados (lida.indireto, (uint8_t *) &bloco_idx);
		if (bloco_idx[numero_entrada]==0xffff)
		{
			bloco_idx[numero_entrada] = aloca();
			escreva_bloco_dados(lida.indireto , (uint8_t *) bloco_idx);
		}
		leia_bloco_dados (lida.indireto, (uint8_t *) &bloco_idx);
		n = bloco_idx[numero_entrada];
		
		leia_bloco_dados (n, (uint8_t *) &bloco);
		bloco[deslocamento] = valor;
		escreva_bloco_dados(n ,  (uint8_t *) &bloco);
		 
		
	}
	if (A->posicao >= lida.tam) lida.tam++;
	A->posicao++;
	escreva_entrada(id, &lida);
	
}

void cria_entrada (uint8_t numero)
{
	struct inodo tmp;
	tmp.status = 0;
	tmp.tam=0;
	strcpy(tmp.nome,"VAZIO");
	tmp.indireto=0xFFFF;
	for (int a=0;a<32;a++) tmp.dados_diretos[a]=0;
	
	uint16_t posicao = 2 + numero*sizeof(struct inodo);
	memoria.write(posicao, sizeof(struct inodo), (uint8_t *)&tmp);
	
}
void escreva_bloco_ponteiro(uint16_t numero, uint16_t valor)
{
	uint16_t endereco = numero * 2 + INICIO_PONTEIROS; 
	memoria.write(endereco, 2, (uint8_t *)&valor);
}
void leia_bloco_ponteiro(uint16_t numero, uint16_t *valor)
{
	uint16_t endereco = numero * 2 + INICIO_PONTEIROS; 
	memoria.read(endereco, 2, (uint8_t *)valor);
}
uint16_t aloca (void)
{
	bloco_indice  bloco_idx;
	 
	uint16_t lido;
	uint16_t  novo_bloco = le_cabecalho();
	
	leia_bloco_ponteiro(novo_bloco, &lido);
	
	escreve_cabecalho (lido);
	
	leia_bloco_dados (novo_bloco, (uint8_t *) &bloco_idx);
	for (int x=0;x<16;x++) bloco_idx[x]=0xffff;
	escreva_bloco_dados(novo_bloco, (uint8_t *) &bloco_idx);
	
	
	return novo_bloco;
}

uint16_t verifica_arquivo (const char *st)
{
	struct inodo arq;
 	uint8_t i;
	for (i = 0; i < QTD_ENTRADAS; i++)
	{
		leia_entrada (i, &arq);	
		if (arq.status==1)
		{
			if (strcmp(st, arq.nome)==0) return i;			
		}	
	}
    return 0xffff;
}

void escreva_bloco_dados(uint16_t numero ,uint8_t *valor)
{
		uint16_t endereco = numero * sizeof(bloco_dados) + INICIO_DADOS; 
		memoria.write(endereco,  sizeof(bloco_dados),  valor);
}

void leia_bloco_dados(uint16_t numero ,uint8_t *valor)
{
		uint16_t endereco = numero * sizeof(bloco_dados) + INICIO_DADOS; 
		memoria.read(endereco, sizeof(bloco_dados),  valor);
}

void remove_entrada(uint8_t id){
	
	struct inodo tmp;
	leia_entrada(id, &tmp);
	
	uint16_t bloco_indireto = tmp.indireto;
	if(bloco_indireto != 0xFFFF){
	
		bloco_indice  bloco_idx;
		leia_bloco_dados (bloco_indireto, (uint8_t *) &bloco_idx);
		
		for (int i = 0; i < 16 && bloco_idx[i] != 0xffff; i++)
		{
			escreva_bloco_ponteiro(bloco_idx[i], le_cabecalho());
			escreve_cabecalho(bloco_idx[i]); 
		}
		escreva_bloco_ponteiro(bloco_indireto, le_cabecalho());
		escreve_cabecalho(bloco_indireto);
	}
	cria_entrada(id);
}

void cria_blocos_livres()
{
	for (int x=0;x<224;x++)
		escreva_bloco_ponteiro(x, x+1);
	
	escreva_bloco_ponteiro(224, 0xFFFF);
}

void cria_blocos_dados()
{
	uint8_t buffer[32];
	for (int x = 0; x < 32; x++) buffer[x]=0;
	for (int x = 0; x < 225; x++)
		escreva_bloco_dados(x, buffer);
}

/*
Formata o arquivo da partição, caso não exista a partição ele cria uma.
Cria o cabeçalho, as entradas de arquivos, os blocos livres, e os blocos de dados;
*/

void init(void)
{
	memoria.init();
}

void formata (void)
{
	escreve_cabecalho(0);
	for (int x=0;x<12;x++) cria_entrada(x);
	cria_blocos_livres();
	cria_blocos_dados();
}


