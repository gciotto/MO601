# Bônus 1
A fim de calcular o número de entradas disponíveis no RAS do processador Intel i3, o arquivo `bonus.c` foi implementado. Considerando que tal pilha apresenta tamanhos iguais a potências de 2, este programa faz com que 3 funções se chamem ciclicamente por uma quantidade `counter` de vezes. Como nenhuma potência de 2 é divisível por 3, haverá um valor de `counter` que produzirá um *overflow* em RAS. A tabela abaixo representa os valores de `counter` e o intervalo de tempo que o ciclo levou para ser completado.

`Counter` | Tempo (ns)
----------|-----------
8 | 676
16 | 694
32 | 1096
64 | 1627
128 | 2986
256 | 5401
512 | 14586
1024 | 31869
2048 | 59121
4096 | 107938

A figura `plot.png` mostra os dados acima em uma representação visual. As execuções de 8, 16, 32 e 64 apresentam intervalos com variação muito pequena, sendo que a diferença da execução de 128 em relação é 64 ultrapassa 1us. Sendo assim, conclui-se que o RAS possui **64 entradas**.

