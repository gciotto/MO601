# Projeto 3 - Diretório *src*

Este diretório possui todos os arquivos e programas utilizados para as tarefas designadas para o Projeto 3.

## Instalação do *branch predictor Gshare* no diretório do *sniper*

A fim de utilizarmos o *branch predictor* do tipo *gshare*, é necessário copiar alguns arquivos para o diretório do *sniper* e, em seguida, compilar tais arquivos.

- Copie `branch_predictor.cc` na pasta `sniper/common/performance_model`
- Copie os arquivos `GShareBranchPredictor.h` e `GShareBranchPredictor.cc` para `sniper/common/performance_model/branch_predictors`
- Em `sniper/`, execute `make` para compilar esses novos arquivos.

## Rodando o *sniper*

Para rodar o *sniper*, utilizaremos o *script* `run-pintool.sh`. Neste arquivo, é necessário alterar os seguintes parâmetros:

- `PROJECT_DIR`: diretório onde o projeto se encontra;
- `PINBALL_DIR`: local onde os *pinballs* estão salvos;
- `SNIPER_DIR`: diretório onde o *sniper* foi instalado;
- `SNIPER_CFG`: nome do arquivo de configuração a ser utilizado;
- `SUFFIX_*`: sufixos a serem inseridos no caminho em que os arquivos contendo os resultados serão gerados.

O arquivo `run-pintool.sh` aceita alguns parâmetros, sendo eles:

- `int`: executa todos os *benchmarks* do conjunto inteiro (`SPEC CPUint2006`)
- `fp`: executa todos os *benchmarks* do conjunto de ponto flutuante (`SPEC CPUfp2006`)
- `i1`: executa o primeiro subconjunto de *benchmarks* do conjunto inteiro (400.perlbench, 401.bzip2, 403.gcc, 429.mcf ,445.gobmk e 456.hmmer).
- `i2`: executa o segundo subconjunto de *benchmarks* do conjunto inteiro (458.sjeng, 462.libquantum, 464.h264ref, 471.omnetpp, 473.astar e 483.xalancbmk).
- `fp1`: executa o primeiro subconjunto de *benchmarks* do conjunto de ponto flutuante (4410.bwaves, 416.gamess, 433.milc, 434.zeusmp, 436.cactusADM, 437.leslie3d, 444.namd e 447.dealII).
- `fp2`: executa o segundo subconjunto de *benchmarks* do conjunto de ponto flutuante (450.soplex, 453.povray, 454.calculix, 459.GemsFDTD, 465.tonto, 470.lbm, 481.wrf e 482.sphinx3).
- `all`: executa todos os *benchmarks*.

Para executar, por exemplo, todos os *benchmarks*, roda-se `./run-pintool.sh all`.

## Obtendo os resultados através do *script* `parse.py`

Os arquivos `csv` podem ser gerados automaticamente através da execução de `python parse.py` Para isso, algumas variáveis devem ser modificadas:

- `_dir`: deve possuir o mesmo que um dos `SUFFIX_*` acima.
- `project_dir`: diretório onde os resultados do comando `run-pintool.sh` forama gerados.
- `sniper_log` : nome do arquivo de *log* gerado pelo *sniper*.
- `results`: caminho completo do arquivo *csv* a ser gerado.
