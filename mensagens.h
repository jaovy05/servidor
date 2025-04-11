

#ifndef S_ENDERECO
#define S_ENDERECO

typedef struct endereco
{
    char ip[16];
    int porta;
} Endereco;

#endif // S_ENDERECO

#ifndef S_MENSAGEM
#define S_MENSAGEM

#define CONTROLE 0
#define DADOS 1 

typedef struct mensagem
{
    char tipoMensagem;
    char data[200];
    Endereco destino;
    Endereco fonte;
} Mensagem;

#endif // S_MENSAGEM