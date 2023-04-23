#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
using namespace std;

const int BUFFER_SIZE = 20; // Tamanho fixo do buffer para representação numérica

int main() {
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
    int num;
    int count = 0;
    while (true) {
        // Solicita ao usuário para digitar um número
        cout << "Produtor: Digite um número (0 para sair): " << endl;
        cin >> num;

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
        
        if (result == "Finalize") {
            cout << "Produtor: Recebi o pedido de encerramento.\nEncerrando..." << endl;
            break; // Encerra o loop quando receber "Nao primo" como resultado
        }
        count++;
    }

    // Envia 0 para terminar o consumidor
    if (send(clientSocket, "0", BUFFER_SIZE, 0) == -1) {
        cerr << "Erro ao enviar número de término" << endl;
        close(clientSocket);
        return EXIT_FAILURE;
    }

    // Fecha o socket e termina o programa
    close(clientSocket);
    cout << "Produtor: Encerrado ! \nTotal de números gerados: " << count << endl;
    return EXIT_SUCCESS;
}
