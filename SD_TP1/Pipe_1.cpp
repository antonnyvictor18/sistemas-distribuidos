#include <sys/wait.h>
#include <stdbool.h>
#include <time.h>
#include <iostream>
#include <csignal>
#include <unistd.h>
using namespace std;

bool primo(int num) {
    if (num <= 1) {
        return false;
    }

    for (int i = 2; i <= num/2; i++) {
        if (num % i == 0) {
            return false;
        }
    }
    return true;
}

int main(int argc, char *argv[]) {

    int num_numeros = stoi(argv[1]);
    int pipefd[2];
    pid_t pid = fork();
    int num = 42;
    // Verificar se o número de argumentos está correto
    if (argc != 2) {
        cout << "O programa: " << argv[0] << " não tem 2 argumentos"<< endl;
        exit(1);
    }

    
    if (pipe(pipefd) == -1) {
        cerr << "Erro ao criar o pipe" << endl;
        exit(1);
    }

 
    if (pid == -1) {
        cerr << "Erro ao criar o processo filho"<< endl;
        exit(1);
    } 
    
    else if (pid == 0) {
        // Processo filho - Consumidor
        close(pipefd[1]); // Fechar a ponta de escrita do pipe no processo filho
        cout << "entrou" <<endl;
        while (read(pipefd[0], &num, sizeof(int)) > 0) {
            if (num == 0) {
                break;
            }

            cout << "Consumidor: Número Recebido: " << num << endl;
            if (primo(num)) {
                cout << "primo"<< endl;
            } 
            else {
                cout << "Nao primo"<< endl;
            }
        }

        close(pipefd[0]); // Fechar a ponta de leitura do pipe no processo filho
        exit(0);
    } 
    else {
        // Processo pai - Produtor
        close(pipefd[0]); // Fechar a ponta de leitura do pipe no processo pai


        srand(time(0)); // Seed para geração de números aleatórios
        for (int i = 0; i <= num_numeros; i++) {
            cout << num_numeros << endl;
            int delta = rand() % 100 + 1; // Gerar delta aleatório entre 1 e 100
            num += delta; // Calcular o próximo número

            cout << "Produtor: Número Gerado: " << num << endl;
            if (write(pipefd[1], &num, sizeof(int)) == -1) {
                cerr << "Erro ao escrever no pipe"<<endl;
                exit(1);
            }

        }

        num = 0; // Enviar número 0 para indicar o fim da produção
        if (write(pipefd[1], &num, sizeof(int)) == -1) {
            cerr << "Erro ao escrever no pipe" << endl;
            exit(1);
        }

        close(pipefd[1]); // Fechar a ponta de escrita do pipe no processo pai

        // Aguardar o processo filho terminar
        wait(NULL);
    }

    return 0;
}
