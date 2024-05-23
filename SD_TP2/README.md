# Trabalho Prático 2 - Sistemas Distribuídos (COS470)

### 2023/1

## Objetivos

O objetivo deste trabalho é se familiarizar com threads e mecanismos de sincronização entre threads que são fundamentais para acesso à memória compartilhada e coordenação das threads. Para cada parte, você deve desenvolver um programa na linguagem de programação de sua preferência, mas que tenha suporte a threads e mecanismos de sincronização, como instruções atômicas e semáforos. A sugestão é utilizar C ou C++, tendo em vista a proximidade das bibliotecas destas linguagens com o sistema operacional, oferecendo ao desenvolvedor maior controle.

Além da implementação, você deve testar seu programa rodando os estudos de casos. Você deve preparar um relatório, com no máximo 6 páginas, com as decisões de projeto e implementação das funcionalidades especificadas, assim como a avaliação dos estudos de caso. O relatório deve conter a URL para o código-fonte da sua implementação. O trabalho deve ser realizado em dupla.

## Tarefas

### 1. Somador com Spinlocks

Considere o problema de calcular a soma de um grande número de números. Você deve projetar e implementar um programa multithreaded para resolver este problema, visando reduzir o tempo de execução. Assuma que os números a serem somados estão armazenados em um vetor de tamanho N e seus valores são números aleatórios no intervalo [-100, 100]. Seu programa deve ter como parâmetro K, que representa o número de threads que irão trabalhar simultaneamente. Uma forma de resolver o problema é dividir a soma em K parcelas com N/K valores e deixar cada thread realizar a soma de cada parcela, somando o resultado em um acumulador compartilhado. Como várias threads vão escrever no acumulador, o acesso ao mesmo deve ser sincronizado.

Para serializar o acesso ao acumulador compartilhado, implemente um spinlock como vimos em aula. Você deve implementar o spinlock utilizando instruções atômicas, como a test-and-set, utilizando busy wait para bloquear a thread. Em particular, implemente as funções `acquire()` e `release()` para entrar e sair da região crítica. Defina sua região crítica adequadamente.

Confira que a soma está correta. Use uma thread para calcular a soma e depois compare com o valor obtido pelas K threads. Retorne um erro caso estejam diferentes.

Para o estudo de caso, considere N = 10^7, N = 10^8 e N = 10^9. Você deve alocar memória utilizando apenas um byte para cada número, e preencher o vetor de forma aleatória (com números no intervalo [-100, 100]). Utilize os valores de K = 1, 2, 4, 8, 16, 32, 64, 128, 256. Para cada combinação de valores N, K, obtenha o tempo de execução do seu programa, rodando o programa 10 vezes para calcular o tempo médio de execução (para cada combinação de valores, teremos um tempo médio de execução). Não considere o tempo necessário para gerar e preencher o vetor com N posições, e nem calcular a soma com uma thread para verificação, mas apenas o tempo para calcular a soma. Apresente um gráfico mostrando o tempo médio de execução em função do número de threads para cada valor de N (cada N deve ser uma curva no gráfico).

### 2. Produtor-Consumidor com Semáforos

Implemente um programa Produtor-Consumidor multithreaded com memória compartilhada. Assuma que a memória compartilhada é um vetor de números inteiros de tamanho N. O número de threads do tipo produtor e consumidor são parâmetros do programa dados por Np e Nc, respectivamente. A thread produtor deve gerar números inteiros aleatórios entre 1 e 10^7 e colocar o número em uma posição livre da memória compartilhada. A thread consumidor deve retirar um número produzido por um produtor da memória compartilhada, liberar a posição do vetor, e verificar se o mesmo é primo, imprimindo o resultado no terminal.

Repare que a memória compartilhada será escrita e lida por várias threads, então o acesso deve ser serializado, evitando efeitos indesejáveis da condição de corrida. Utilize semáforos para serializar o acesso à memória compartilhada. Repare ainda que quando a memória compartilhada estiver cheia ou vazia (sem posições livres ou sem posições com números), as threads produtor ou consumidor devem aguardar bloqueadas, respectivamente. Ou seja, uma thread produtor aguarda até que haja uma posição de memória livre, e uma thread consumidor aguarda até que haja uma posição de memória ocupada. Utilize semáforos contadores para esta coordenação, como visto em aula.

Para o estudo de caso, considere que o programa termina sua execução após o consumidor processar M = 10^5 números. Considere ainda os valores N = 1, 10, 100, 1000, com as seguintes combinações de número de threads produtor/consumidor: (Np, Nc) = {(1, 1), (1, 2), (1, 4), (1, 8), (2, 1), (4, 1), (8, 1)}.

Para cada combinação de parâmetros, obtenha o tempo de execução do seu programa, rodando o programa 10 vezes para calcular o tempo médio de execução (para cada combinação de valores, teremos um tempo médio de execução). Apresente um gráfico mostrando o tempo médio de execução em função do número de threads produtor/consumidor para cada valor de N (cada N deve ser uma curva no gráfico). Analise o comportamento observado.

Para cada cenário, trace um gráfico representativo com a ocupação do buffer compartilhado ao longo do tempo. Para gerar o gráfico, utilize um vetor que armazena a ocupação do buffer após cada operação de produção ou consumo. Ao final, use o vetor para gerar o gráfico (ou arquivo).

O que você pode concluir em cada um dos casos?

## Relatório

Você deve preparar um relatório com as decisões de projeto e implementação das funcionalidades especificadas, assim como a avaliação dos estudos de caso. O relatório deve conter a URL para o código-fonte da sua implementação. O relatório deve ter no máximo 6 páginas.
