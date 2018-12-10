#include <stdio.h>
#include "SA.h"
#include "uart.h"
#include "LPC17xx.h"
int main (int argc, char * argv[])
{
	SystemInit();
	init();
	UART0_Init(115200);
	printf("Esta funcionando!\n");
	formata();
	
	MEU_FILE * C, * B, * A;

	A = meu_fopen ("Mateus","w");
	B = meu_fopen ("Mihael","w");
	C = meu_fopen ("Diogo","w");

	printf("Abriu os arquivo no modo w\n");

	meu_fwrite("Mateus trabalho de digitais embarcados", A);
	meu_fwrite("Mihael", B);
	meu_fwrite("Diogo", C);

	printf("Escreveu no arquivo\n");

	meu_fclose(A);
	meu_fclose(B);
	meu_fclose(C);

	printf("Fechou os arquivos\n");

	A = meu_fopen ("Mateus","r+");
	C = meu_fopen ("Diogo","r+");
	B = meu_fopen ("Mihael","r+");

	printf("Abriu os arquivos no modo r+\n");

	meu_fseek(B, tamanho_arquivo(B));
	meu_fwrite(" cade ele", B);

	printf("Escreveu no arquivo B\n");

	abre_arquivo_imprime(A);
	abre_arquivo_imprime(B);
	abre_arquivo_imprime(C);

	printf("Acabou!\n");
	
	meu_fclose(A);
	meu_fclose(B);
	meu_fclose(C);
	while(1)
	{

	}
}