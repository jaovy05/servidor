# Servidor

## Estrutura dos Arquivos de Configuração

O programa utiliza três arquivos de configuração:

### `enlaces.config`
Define as conexões (enlaces) entre os roteadores.

Formato esperado para cada linha:

<id do roteador1> <id do roteador2> <custo do enlace>


- **ID dos roteadores**: números inteiros de 1 a 99 (máximo 2 dígitos).
- **Custo do enlace**: número inteiro de 1 a 9999 (máximo 4 dígitos).

---

### `roteador.config`
Define os dados de cada roteador.

Formato esperado para cada linha:

<id> <porta> <ip>


- **ID**: número inteiro de 1 a 99 (máximo 2 dígitos).
- **Porta**: número inteiro de 1 a 65535 (máximo 5 dígitos).
- **IP**: string no formato de IP (ex: `192.168.0.1`).

### `config.ini`
Define dados geral do sistema

Formato esperado para cada linha:

<chave> '=' <valor>

- **roteadores**: número total de roteadores usados (máximo 100)
- **infinito**: valor do infinito a ser considerado (mínimo maior caminho da rede + 1)
- **tempoVetor**: valor em segundos para enviar o vetor distância aos seus vizinhos (mínimo 1)

---

## Como Compilar

Execute o seguinte comando no terminal:

```bash
gcc *.c -o main -lpthread
```

---

##Como Executar
Após compilar, execute o programa informando o ID do roteador desejado:

```bash
./main <id do roteador>
```

Exemplo:

```bash
./main 1
```