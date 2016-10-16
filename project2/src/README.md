# Projeto 2 - Diretório *src*

Este diretório possui todos os arquivos e programas utilizados para as tarefas designadas para o Projeto 2.

## Execução da *pintool* nos *benchmarks*

Para esta tarefa, utilizamos os seguintes arquivos:

- `page_simulator_tool.cpp` : implementação da *tool* com 2 pares de TLBs e 3 níveis de cache (L1, L2, L3).
- `makefile.rules` : extende makefile para inserir dependências para executação do `pinplay`. Modificar variável `PINPLAY_HOME` dentro deste arquivo.
- `run-pintool.sh`: este *script* foi implementado para automatizar o processo de execução da *pintool* sobre os *benchmarks*. Ele define algumas variáveis de ambiente importantes:
  - `PIN_DIR`: endereço onde `pin` foi instalado.
  - `TOOL_NAME` : nome da *tool*. No caso, `page_simulator_tool`.
  - `PINBALL_DIR`: diretório onde os pinballs foram salvos.
  - `PROJECT_DIR`: diretório do projeto.
  - `SUFFIX` : sufixo a ser adicionado ao fim dos arquivos de resultados gerados. Apenas para facilitar processamento.
- `summarize.py`: lê todos os arquivos gerados e produz os arquivos `result`. Compatível com *python 2.7* somente. Define também 4 variáveis:
  - `project_dir`: diretório do projeto.
  - `benchmarks`: lista de *benchmarks* escolhidos.
  - `suffix`: sufixo especificado no arquivo `run-pintool.sh`.
  - `results_4KB`: local onde `results_4KB.csv` deve ser salvo.
  - `results_4MB`: local onde `results_4MB.csv` deve ser salvo.

O *script* `run-pintool.sh` necessita receber algum parâmetro de entrada, dentre eles:

- `int` : executa somente os *benchmarks* do conjunto inteiro.
- `fp` : executa somente os *benchmarks* do conjunto de ponto-flutuante.
- `set`: 1, 2 ou 3 = executa aquele 1/3 dos *benchmarks*
- `all` : executa todos os 10 *benchmarks*
- benchmark : executes aquele *benchmark*. Por exemplo, `./run-pintool.sh 403.gcc` executa somente o *benchmark* gcc.
- `toy` : executa somente o *benchmark* toy.

Para executar todos os *benchmarks*, basta rodas `./run-pintool.sh all`. O *script* também realiza a compilação de qualquer arquivo necessário.
