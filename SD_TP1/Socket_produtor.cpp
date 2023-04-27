#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
using namespace std;



const int BUFFER_SIZE = 20; // Tamanho fixo do buffer para representação numérica

int main(int argc, char *argv[]) {

    // Verificar se o número de argumentos está correto
    if (argc != 2) {
        cout << "O programa: " << argv[0] << " não tem 2 argumentos"<< endl;
        exit(1);
    }

    int number_of_sendings = stoi(argv[1]);

    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];

    // Criação do socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        cerr << "Erro ao criar socket" << endl;
        return EXIT_FAILURE;
    }
    
    // Configuração do endereço do servidor
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Endereço IP do servidor
    serverAddr.sin_port = htons(8080); // Porta do servidor

    // Conexão ao servidor
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        cerr << "Erro ao conectar ao servidor" << endl;
        close(clientSocket);
        return EXIT_FAILURE;
    }
    cout << "Conexão estabelecida com o consumidor!" << endl;

    // Loop para gerar e enviar números
    int num = 1;
    int delta;
    srand(time(0));

     for (int i = 0; i <= number_of_sendings; i++) {

        if (i == number_of_sendings) {
            num = 0;
        }

        cout << "Produtor enviando numero: " << num << endl;
        
        // Envia número ao consumidor
        if (send(clientSocket, std::to_string(num).c_str(), BUFFER_SIZE, 0) == -1) {
            cerr << "Erro ao enviar número"<< endl;
            close(clientSocket);
            return EXIT_FAILURE;
        }
        // Aguarda resultado do consumidor
        if (recv(clientSocket, buffer, BUFFER_SIZE, 0) == -1) {
            cerr << "Erro ao receber resultado" << endl;
            close(clientSocket);
            return EXIT_FAILURE;
        }

        string result(buffer);
        cout << "Produtor: Número gerado: " << num << ". Resultado recebido: " << result << endl;
        sleep(2);

        int delta = rand() % 100 + 1; // Gerar delta aleatório entre 1 e 100
        num += delta; // Calcular o próximo número
    }

    // Fecha o socket e termina o programa
    close(clientSocket);
    return EXIT_SUCCESS;
}
