#include <iostream>
#include <csignal>
#include <unistd.h>
using namespace std;

bool isRunning = true; // Variável de controle para saber se o programa está em execução

void signalHandler(int sinal) {
    if (sinal == 0){
        cout << "Sinal " << sinal << " recebido. \nMensagem: Processo finalizado." << endl;
        exit(0); // Termina a execução do programa
    }
    else if (sinal  == 1){
        cout << "Sinal " << sinal << " recebido.\nMensagem: Processo pausado." << endl;
        isRunning = false; // Define a variável de controle como false para parar o programa
    }
    else{
        cout << "Sinal " << sinal <<  "recebido. \nMensagem: Processo em execução..." << endl;
    }
    
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Uso: " << argv[0] << " <modo_espera>" << endl;
        return 1;
    }

    isRunning = true;
    string modoEspera = argv[1]; // Modo de espera: "busy" ou "blocking"

    // Registra as funções de manipulação de sinais
    signal(0, signalHandler);
    signal(1, signalHandler);
    signal(2, signalHandler);

    cout << "PID do processo: " << getpid() << endl;
    cout << "Modo de espera: " << modoEspera << endl;

    // Aguarda a chegada de sinais
    if (modoEspera == "busy") {
        while (isRunning) {
            sleep(1);
            cout << "Busy wait: esperando os sinais chegarem..."<< endl;
            sleep(1);
        }
    } else if (modoEspera == "blocking") {
        // Blocking wait: pausa o processo até que um sinal seja recebido
        while (isRunning) {
            cout << "Processo pausado. Esperando sinais"<<endl;
            pause();
            cout << "Processo em execução novamente"<<endl;
        }
    } else {
        cerr << "Erro: Modo de espera inválido. Use 'busy' ou 'blocking'." << endl;
        return 1;
    }

    main(argc, argv);
    return 0;
}
