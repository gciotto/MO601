# Bônus 1
A fim de calcular o número de entradas disponíveis no RAS do processador Intel i3, o arquivo `bonus.c` foi implementado. O programa é constituído por 2 laços: o mais externo é responsável por armazenar os diversos resultados para um posterior cálculo de média e o mais interno, por medir o tempo de execução de recursões com números variáveis de chamadas. A ideia desta lógica consiste no fato de que quando a capacidade do RAS for estourada, o programa levará mais tempo para ser completado. 

## Parâmetros

O laço mais interno produz valores do número de chamadas variando de 2 até 512 com intervalo de 2 entre cada um, enquanto que o mais externo executa este primeiro 10000 vezes. Estes parâmetros são configurados através das constantes `MAX_RECURSION` e `NUMBER_ITERATIONS`, respectivamente.

## Execução

Para a contagem do tempo, utiliza-se um relógio disponível do Linux acessado através da função `clock_gettime()` que recebe o respectivo clock. Por motivos de precisão, escolhemos o `CLOCK_REALTIME`, que segundo a documentação, necessita de privilégios especiais para ser setado. Portanto, este programa necessita ser executado como `root`.

O script `run.sh` executa o aplicativo.

## Resultado

Para o processador Intel i3, o valor calculado foi de 16 entradas. Para os valores de recursão entre 2 e 16, a diferença entre cada um deles variou de aproximamente 15 nanossegundos.

