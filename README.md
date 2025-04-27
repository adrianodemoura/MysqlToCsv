# MySQL to CSV Exporter

Este projeto é um script em C que exporta dados de uma tabela MySQL para arquivos CSV, utilizando múltiplas threads para melhor performance.

## Características

- Exporta dados em páginas de 1 milhão de registros
- Utiliza múltiplas threads para processamento paralelo
- Gerencia o uso de memória para não exceder 70% da RAM disponível
- Suporta configuração via arquivo JSON
- Gera arquivos CSV organizados por página

## Requisitos

- GCC
- libmysqlclient-dev
- libjson-c-dev
- pthread

## Instalação

1. Instale as dependências:
```bash
sudo apt-get install gcc libmysqlclient-dev libjson-c-dev
```

2. Clone o repositório:
```bash
git clone [URL_DO_REPOSITÓRIO]
cd MysqlToCsv
```

3. Compile o projeto:
```bash
make
```

## Configuração

O arquivo de configuração está em `config/config.json` e deve conter:

```json
{
    "mysql_host": "seu_host",
    "mysql_user": "seu_usuario",
    "mysql_pass": "sua_senha",
    "mysql_db": "seu_banco",
    "mysql_tabela": "sua_tabela",
    "records_per_page": 10,
    "total_records": 100
}
```

Parâmetros:
- `mysql_host`: Endereço do servidor MySQL
- `mysql_user`: Nome do usuário do banco de dados
- `mysql_pass`: Senha do usuário
- `mysql_db`: Nome do banco de dados
- `mysql_tabela`: Nome da tabela a ser exportada
- `records_per_page`: Número de registros por página (padrão: 10)
- `total_records`: Total de registros a serem exportados (padrão: 100)

## Uso

Execute o programa:

```bash
./bin/mysql_to_csv
```

Os arquivos CSV serão gerados no diretório `files_csv/`.

## Estrutura do Projeto

- `src/` - Código fonte
  - `main.c` - Arquivo principal
  - `mysql_connector.c` - Funções de conexão MySQL
  - `csv_writer.c` - Funções de escrita CSV
  - `memory_manager.c` - Gerenciamento de memória
  - `config_reader.c` - Leitura de configuração
- `config/` - Arquivos de configuração
- `files_csv/` - Diretório de saída dos arquivos CSV

## Limitações

- O programa limita o uso de memória a 70% da RAM disponível
- Utiliza no máximo 16 threads simultâneas
- Processa os dados em chunks de 100.000 registros para melhor gerenciamento de memória 

## Glossário

- **Chunks**: São blocos de dados que o programa processa por vez. No contexto deste projeto, um chunk representa um conjunto de 100.000 registros que são lidos do banco de dados e processados em memória antes de serem escritos no arquivo CSV. O uso de chunks ajuda a gerenciar o consumo de memória e permite o processamento paralelo dos dados.

- **Threads**: São unidades de execução que permitem que o programa realize múltiplas tarefas simultaneamente. Neste projeto, as threads são utilizadas para processar diferentes chunks de dados em paralelo, aumentando a performance da exportação.

- **Páginas**: São divisões dos dados exportados em arquivos CSV separados. Cada página contém 1 milhão de registros, facilitando o gerenciamento de arquivos grandes e permitindo o processamento em partes.

- **RAM (Random Access Memory)**: Memória de acesso aleatório, é o tipo de memória que o computador usa para armazenar dados temporariamente enquanto o programa está em execução. O projeto limita o uso de RAM a 70% para evitar sobrecarga do sistema.
