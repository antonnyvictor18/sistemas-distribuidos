#include <iostream>
#include <csignal>
#include <signal.h>
#include <unistd.h>
#include <chrono>
using namespace std;

bool isRunning = true; // Variável de controle para saber se o programa está em execução2
int count = 0; // variavel contadora para mostrar processo em execução ou não
auto time_start = std::chrono::high_resolution_clock::now();

void signalHandler(int sinal) {

    if (sinal == 1){
        cout << "Sinal " << sinal << " recebido. \nMensagem: Processo finalizado." << endl;
        exit(0); // Termina a execução do programa
    }
    else if (sinal  == 2){
        cout << "Olá, recebi o sinal " << sinal << "! O pid é " << getpid() << endl;
    }
    else if (sinal == 3) {
        auto time_now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(time_now - time_start);
        cout << "Olá, recebi o sinal " << sinal << "! O processo começou a executar há " << duration.count() << " segundos" << endl;
    }
}


int main(int argc, char *argv[]) {
    
    if (argc != 2) {
        cerr << "Uso: " << argv[0] << " <modo_espera>" << endl;
        return 1;
    }

    sigset_t set;
    int sig;

    // isRunning = true;
    string modoEspera = argv[1]; // Modo de espera: "busy" ou "blocking"

    if ((modoEspera != "busy") && (modoEspera != "blocking")) {
        cerr << "Erro: Modo de espera inválido. Use 'busy' ou 'blocking'." << endl;
        return 1;  
    }

    cout << "PID do processo: " << getpid() << endl;
    cout << "Modo de espera: " << modoEspera << endl;
    cout << "Sinais Possíveis:\n1 (encerra o processo)\n2 (informa o pid)\n3 (informa o tempo de execução)\n" << endl;

    // Registra as funções de manipulação de sinais
    signal(1, signalHandler);
    signal(2, signalHandler);
    signal(3, signalHandler);

    if (modoEspera == "blocking") {
        sigwait(&set, &sig);
    }

    while(true) {
        sleep(2);
        cout << count << endl;
        count ++;            
    }
    return 0;
}
