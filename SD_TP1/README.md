# Trabalho Prático 1 - Sistemas Distribuídos (COS470)

## Professor: Daniel Ratton Figueiredo

### 2023/1

## Objetivos

O objetivo deste trabalho é se familiarizar com os principais mecanismos de IPC (Interprocess Communication) baseados em troca de mensagens. Para cada parte, você deve desenvolver um programa na linguagem de programação de sua preferência, mas que tenha suporte direto aos mecanismos de IPC. A sugestão é utilizar C ou C++, tendo em vista a proximidade das bibliotecas destas linguagens com o sistema operacional, oferecendo ao desenvolvedor maior controle sobre essas operações.

Além da implementação, você deve testar seu programa, rodando os estudos de casos. Você deve preparar um relatório, com no máximo 5 páginas, com as decisões de projeto e implementação das funcionalidades especificadas, assim como a avaliação dos estudos de caso. O relatório deve conter a URL para o código-fonte da sua implementação. O trabalho deve ser realizado em dupla.

## Tarefas

### 1. Sinais

Nesta tarefa, você deve escrever dois programas distintos.

- O primeiro programa deve ser capaz de enviar um sinal a qualquer outro processo. Este programa recebe como parâmetros o número do processo destino e o sinal que deve ser enviado. Seu programa deve verificar se o processo indicado pelo parâmetro existe, e retornar um erro em caso negativo. Caso positivo, seu programa deve enviar o sinal indicado.

- O segundo programa deve ser capaz de receber alguns sinais específicos. Para isso, você precisa definir signal handlers. Seu programa deve capturar e reagir a três sinais diferentes (de sua escolha), ou mais, imprimindo no terminal uma mensagem diferente para cada sinal. Além disso, um dos sinais sendo capturados deve terminar a execução do programa, ou seja, sua signal handler deve terminar o processo. Após estipular as signal handlers, seu programa fica aguardando a chegada de sinais. Você deve implementar duas formas de esperar: busy wait e blocking wait (passado como parâmetro para o programa). Descubra como implementar cada uma dessas formas de fazer um processo esperar!

- Teste seus programas fazendo com que um envie sinais para o outro. Use também o programa `kill` para enviar sinais para o seu segundo programa!

### 2. Pipes

Implemente o programa Produtor-Consumidor como vimos em aula com dois processos que utilizam pipes (anonymous pipes, para ser mais preciso) para fazer a comunicação. O programa produtor deve gerar números inteiros aleatórios e crescentes, da seguinte forma: Ni = Ni−1 + ∆, onde N0 = 1 e ∆ é um valor aleatório entre 1 e 100. O programa consumidor deve receber o número e verificar se o mesmo é primo, imprimindo o resultado no terminal. Seu programa deve primeiramente criar um pipe e depois fazer um fork() para duplicar o processo, de forma que os dois processos (pai e filho) tenham as duas respectivas pontas do pipe (write end e read end).

O processo consumidor deve terminar quando receber o número 0. O programa produtor tem como parâmetro o número de números a serem gerados (ex. 1000), depois do qual o número zero é enviado, e o produtor termina execução.

Cuidado com a representação numérica ao escrever no pipe! Dica: converta o número para uma string de tamanho fixo, ex. 20 bytes. Escreva e leia do pipe este mesmo número de bytes, para cada mensagem. Teste o seu programa mostrando seu funcionamento para alguns casos.

### 3. Sockets

Implemente um programa Produtor-Consumidor com resposta com dois processos e sockets para fazer a comunicação. Utilize sockets do tipo TCP. Dessa forma, o programa consumidor deve fazer o papel de servidor (aguarda conexão) e o programa produtor deve fazer o papel de cliente (inicia a conexão).

Novamente, o programa produtor deve gerar números inteiros aleatórios e crescentes (conforme acima) e o programa consumidor deve receber o número e verificar se o mesmo é primo. Diferentemente do exercício acima, o programa consumidor deve enviar uma mensagem ao produtor informando se o número recebido é ou não primo, e este último deve imprimir o resultado no terminal. Implemente um protocolo bem simples, onde o produtor gera um número, envia ao consumidor, e aguarda a resposta de forma bloqueante. O programa produtor deve se conectar ao programa consumidor, enviar o número gerado, e aguardar o resultado. O programa consumidor aguarda a conexão, recebe o número gerado, determina se o mesmo é primo, envia o resultado ao produtor, e volta a esperar a chegada do próximo número (mantendo a conexão aberta).

Por fim, os programas devem terminar como no exercício acima (enviando zero para terminar o consumidor, tendo como parâmetro o número de números a serem gerados).

Novamente, cuidado com a representação numérica! Dica: converta o número para uma string de tamanho fixo, ex. 20 bytes, e escreva e leia esta mesma quantidade de bytes.

Teste o seu programa mostrando seu funcionamento para alguns casos. Teste seu programa utilizando computadores diferentes, se possível.

## Relatório

Você deve preparar um relatório com as decisões de projeto e implementação das funcionalidades especificadas, assim como a avaliação dos estudos de caso. O relatório deve conter a URL para o código-fonte da sua implementação. O relatório deve ter no máximo 5 páginas.
