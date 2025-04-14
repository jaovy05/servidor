
#ifndef F_MENSAGENS
#define F_MENSAGENS

typedef struct endereco
{
    char ip[16];
    int porta;
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
    int id;
    int enlace;
} Roteador;

typedef struct roteadorNucleo
{
    Endereco endereco;
    int id;
    int qtdVizinhos;
    Roteador *prox; 
} RoteadorNucleo;

extern Roteador *roteadores;
extern RoteadorNucleo nucleo;

#endif // F_MENSAGENS