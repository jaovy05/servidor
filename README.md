# Servidor

## Estrutura dos Arquivos de Configuração

O programa utiliza dois arquivos de configuração:

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