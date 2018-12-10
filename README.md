# Sistema_de_Arquivos

O trabalho é a construção de um sistema de arquivos para utilizar uma memória i2c EEPROM com um núcleo AVR. O sitema de arquivo se comportava da seguinte maneira:
- Um cabeçalho de 2 Bytes que indica qual é o número do primeiro bloco de dados livre. Após, existem 12 entradas de
arquivos (inodos). Cada entrada de arquivo é composta por campos (Status - Indica se a entrada está livre ou ocupado, Nome
- 8 letras para o nome do arquivo, tamanho - quantos bytes o arquivo ocupa, bloco direto (32 bytes de dados), bloco indireto (é
o endereco de um bloco cujo conteúdo são enderecos para blocos de dados) . 

- Um bloco ponteiros destes os blocos de dados. Os blocos de ponteiros possuem 2 Bytes cada e iniciamente foram organizados para criar uma lista
encadeada. Um bloco armazena nos seus 2 Bytes o endereço o próximo bloco livre. Existe uma relação entre cada bloco de
ponteiro e o bloco de dados. O bloco de ponteiro 0 está relacionado ao bloco de dados 0.
