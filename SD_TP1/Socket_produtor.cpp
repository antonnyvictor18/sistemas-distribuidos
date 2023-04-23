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
        perror("Erro ao criar socket");
        return 1;
    }

    // Configuração do endereço do servidor
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Endereço IP do servidor
    serverAddr.sin_port = htons(8080); // Porta do servidor

    // Conexão ao servidor
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Erro ao conectar ao servidor");
        close(clientSocket);
        return 1;
    }
    std::cout << "Conexão estabelecida com o consumidor!" << std::endl;

    // Loop para gerar e enviar números
    int num;
    int count = 0;
    while (true) {
        // Solicita ao usuário para digitar um número
        std::cout << "Produtor: Digite um número (0 para sair): ";
        std::cin >> num;

        // Envia número ao consumidor
        if (send(clientSocket, std::to_string(num).c_str(), BUFFER_SIZE, 0) == -1) {
            perror("Erro ao enviar número");
            close(clientSocket);
            return 1;
        }
        // Aguarda resultado do consumidor
        if (recv(clientSocket, buffer, BUFFER_SIZE, 0) == -1) {
            perror("Erro ao receber resultado");
            close(clientSocket);
            return 1;
        }
        std::string result(buffer);
        std::cout << "Produtor: Número gerado: " << num << ". Resultado recebido: " << result << std::endl;
        
        if (result == "Finalize") {
            std::cout << "Produtor: Recebi o pedido de encerramento.\nEncerrando..." << std::endl;
            break; // Encerra o loop quando receber "Nao primo" como resultado
        }
        count++;
    }

    // Envia 0 para terminar o consumidor
    if (send(clientSocket, "0", BUFFER_SIZE, 0) == -1) {
        perror("Erro ao enviar número de término");
        close(clientSocket);
        return 1;
    }

    // Fecha o socket e termina o programa
    close(clientSocket);
    std::cout << "Produtor: Encerrado ! \nTotal de números gerados: " << count << std::endl;
    return 0;
}
