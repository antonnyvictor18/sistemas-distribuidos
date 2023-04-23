#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
using namespace std;

int pid;
int sinal = 1;

int main(int qtd_parametros, char *parametros[]) {
    // Verifica se o número correto de argumentos foi passado
    if (qtd_parametros != 2) {
        cerr << "Número incorreto de argumentos\n";
        return EXIT_FAILURE;
    }

    // Converte o número do processo destino para inteiro
    pid = stoi(parametros[1]);

    // Verifica se o processo destino existe
    if (kill(pid, sinal) != 0) {
        cerr << "Processo destino não existe\n";
        return EXIT_FAILURE;
    }   
    cout << "Sinal enviado\n";
    return EXIT_SUCCESS;
}
