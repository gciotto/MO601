# Projeto 1 - Diretório *src*

Este diretório possui todos os arquivos e programas utilizados para as tarefas designadas para o Projeto 1. A maioria dos arquivos corresponde a *scripts* `bash` que automatizam o processo de execução dos *benchmarks* ou definem algumas variáveis de ambiente que tornam sua manutenção mais fácil. A seguir, será apresentada uma breve descrição de como tais programas são executados e suas principais características.

## Contagem das intruções dos *benchmarks* 

Para esta tarefa, utilizamos os seguintes arquivos:

- `run-spec-command.sh`: este *script* foi implementado para ser utilizado dentro do arquivo de configuração do SPEC. Sendo assim ele define algumas variáveis contendo os caminhos completos das ferramentas, tais como a localização da instalação do `pin` e da *tool* a ser utilizada. São definidas, portanto, 4 variáveis de ambiente, sendo elas :
  - `PIN_ROOT`: endereço onde `pin` foi instalado.
  - `PIN_TOOL`: nome da `pintool` a ser utilizado. Neste caso, `inscount_tls`.
  - `PROJECT_DIR`: diretório do projeto.
  - `TOOL_ROOT`: localização da `pintool` a ser utilizada.
- `project1c.cfg`: arquivo de configuração utilizado pelo SPEC para executar os *benchmarks*. Conforme orientado em aula, utilizou-se a *flag* `use_submit_for_speed` e o comando `submit` foi modificado para `submit = $BIND $PROJECT_DIR/run-spec-command.sh "$command"`. Além disso, modificamos o comando `output_root` para `$PROJECT_DIR`.
- `run-pintool-all-benchmarks.sh`: executa a `pintool`  em todos os *benchmarks*. Define também 4 variáveis de ambiente:
  - `SPEC_DIR`: local onde o SPEC foi instalado.
  - `PIN_DIR`: diretório onde `pin` foi instalado.
  - `PROJECT_DIR`: diretório do projeto.
  - `PIN_TOOL`: nome da ferramenta.

## Implementação de uma `pintool` 
Duas novas `pintools` foram criadas. Os seguintes arquivos foram implementados para esta etapa:
- `most-used-ins.cpp`: arquivo *cpp* da `pintool` que conta instruções por *thread* e por rotina. 
- `bp-pintool.cpp`: arquivo *cpp* da `pintool` que simula *branch predictors* de 1 e 2 *bits*.
- `makefile`: arquivo necessário para a compilação das `pintools`. Para a sua utilização, é necessário executar o comando `export PIN_ROOT=/path/to/pin` e, em seguida, `make obj-intel64/most-used-ins.so`.
- `run-spec-command-mytool.sh`: equivalente ao *script* `run-spec-command.sh` da seção anterior.
- `project1d.cfg`: arquivo de configuração do SPEC. Necessário modificar os comandos `output_root` e `submit` para as localidades corretas do projeto e do *script* `run-spec-command-mytool.sh`, respectivamente.
- `run-custom-pintool-5-benchmarks.sh`: equivalente ao *script* `run-pintool-all-benchmarks.sh` da seção anterior.

