
#ifndef F_MENSAGENS
#define F_MENSAGENS

/**
 * @brief Representa um endereço de rede (IP + porta).
 *
 * Esta estrutura contém o endereço IP (como string) e a porta associada, usada para
 * identificar a origem ou destino de mensagens na rede.
 */
typedef struct endereco {
    char ip[16];     /**< Endereço IP no formato string (IPv4, ex: "192.168.0.1") */
    int porta;       /**< Número da porta associada ao IP */
} Endereco;


// Tipos de mensagem
#define CONTROLE 0   /**< Mensagem de controle (usada para gerenciar conexões ou estados). */
#define DADOS 1      /**< Mensagem de dados (conteúdo enviado entre roteadores). */

/**
 * @brief Estrutura que representa uma mensagem trocada entre roteadores.
 *
 * Contém o tipo da mensagem (controle ou dados), o conteúdo da mensagem (texto),
 * e os endereços de origem e destino.
 */
typedef struct mensagem {
    char tipoMensagem; /**< Tipo da mensagem (CONTROLE ou DADOS). */
    char data[200];    /**< Conteúdo da mensagem (texto). */
    int destinoId;     /**< Identificador único do roteador destino. */ 
    Endereco destino;  /**< Endereço de destino da mensagem. */
    int fonteId;       /**< Identificador único do roteador fonte. */ 
    Endereco fonte;    /**< Endereço de origem da mensagem. */
} Mensagem;

/**
 * @brief Representa um roteador vizinho conhecido pelo núcleo.
 *
 * Contém o endereço do roteador, seu identificador único (`id`) e a qualidade
 * do enlace (ligação) com este roteador.
 */
typedef struct roteador {
    Endereco endereco;  /**< Endereço de rede do roteador (IP e porta). */
    int id;             /**< Identificador único do roteador. */
    int enlace;         /**< Peso ou qualidade do enlace com o roteador (ex: custo). */
} Roteador;

/**
 * @brief Representa o próprio roteador do núcleo no sistema.
 *
 * Guarda o endereço e id do roteador atual, a quantidade de vizinhos conhecidos
 * e um ponteiro para a lista de roteadores vizinhos.
 */
typedef struct roteadorNucleo {
    Endereco endereco; /**< Endereço de rede do núcleo (IP e porta). */
    int id;             /**< Identificador único do núcleo. */
    int qtdVizinhos;    /**< Quantidade de roteadores vizinhos conhecidos. */
    Roteador *prox;     /**< Ponteiro para a lista de vizinhos. */
} RoteadorNucleo;


// Variáveis globais
extern Roteador *roteadores; /**< Ponteiro para o vetor de roteadores vizinhos conhecidos. */
extern RoteadorNucleo nucleo; /**< Estrutura que representa o próprio roteador núcleo. */

/**
 * @brief Representa uma entrada da tabela de vetor de distância.
 *
 * Contém o ID do roteador de destino e o custo para alcançá-lo.
 */
typedef struct {
    int idDestino;
    int custo;
} EntradaVetor;


/**
 * @brief Representa um roteador vizinho e seu vetor de distâncias.
 *
 * Contém o ID do roteador vizinho e um vetor com os custos estimados
 * para alcançar todos os outros roteadores da rede.
 */
typedef struct {
    int idVizinho;
    EntradaVetor *vetor;  // tamanho = totalRoteadores
} RegistroVizinho;

/**
 * @brief Representa as configurações do .ini
 *
 * Guarda o total de roteadores, o valor de INF e o tempo de envio do vetor.
 */
typedef struct {
    int totalRoteadores;
    int INF;
    int tempoVetor;
} ConfigInfo;

#endif // F_MENSAGENS
