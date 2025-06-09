#include <stdio.h>
#include <arpa/inet.h>
#include "mensagens.h"

#ifndef F_UTILS

#define F_UTILS

/**
 * Encerra o programa exibindo uma mensagem
 *
 * @param s Ponteiro para a mensagem a ser exibida.
 */
void die(char *s);

/**
 * Cria e configura um socket UDP
 *
 * @param port Número da porta que o socket irá escutar.
 * @return Número do descritor de arquivo ou -1 em caso de erro.
 */
int makeSocket(int port);

/**
 * Lê os enlaces do roteador principal a partir de um arquivo,
 * alocando memória dinamicamente para os roteadores conectados
 * e salvando as informações de enlace na estrutura apropriada.
 *
 * @param i Profundidade da recursão (quantidade de roteadores encontrados).
 * @param file Ponteiro para o arquivo que contém os enlaces.
 */
void readEnlaces(int i, FILE *file);

/**
 * Lê os dados dos roteadores conectados a partir de um arquivo,
 * preenchendo o endereço IP e a porta.
 *
 * @param file Ponteiro para o arquivo contendo os dados dos roteadores.
 */
void readRoteadores(FILE *file);

/**
 * Compara dois roteadores pelo ID.
 * 
 * @param a Ponteiro para o primeiro roteador (Roteador *).
 * @param b Ponteiro para o segundo roteador (Roteador *).
 * @return difença entre a e b.
 */
int cmp(const void *a, const void *b);

/**
 * @brief Busca um roteador pelo ID.
 *
 * @param id O ID do roteador a ser buscado.
 * @return Ponteiro para o roteador correspondente ao ID, ou NULL se não encontrado.
 *
 * @note A busca é realizada através de uma busca binária nos roteadores registrados.
 */
Roteador *findById(int id);

ConfigInfo readIni();
#endif // F_UTILS
