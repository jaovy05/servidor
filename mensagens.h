
#include "utils.h"

#ifndef F_MENSAGENS
#define F_MENSAGENS

typedef struct endereco
{
    char ip[16];
    uint porta;
} Endereco;

#define CONTROLE 0
#define DADOS 1 

typedef struct mensagem
{
    char tipoMensagem;
    char data[200];
    Endereco destino;
    Endereco fonte;
} Mensagem;


typedef struct roteador
{
    Endereco endereco;
    uint id;
    uint enlace;
} Roteador;

typedef struct roteadorNucleo
{
    Endereco endereco;
    uint id;
    uint qtdVizinhos;
} RoteadorNucleo;

extern Roteador *roteadores;
extern RoteadorNucleo nucleo;

#endif // F_MENSAGENS