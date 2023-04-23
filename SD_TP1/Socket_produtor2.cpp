#include <iostream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

const int BUFFER_SIZE = 20;

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    char buffer[BUFFER_SIZE];

    // Criação do socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Erro ao criar socket");
        return 1;
    }

    // Configuração do endereço do servidor
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(8081); // Porta a ser utilizada

    // Associação do endereço ao socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Erro ao fazer bind");
        close(serverSocket);
        return 1;
    }

    // Aguarda conexão do consumidor
    if (listen(serverSocket, 1) == -1) {
        perror("Erro ao escutar por conexões");
        close(serverSocket);
        return 1;
    }

    std::cout << "Produtor: Aguardando conexão do Consumidor..." << std::endl;

    // Aceita a conexão do consumidor
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSocket == -1) {
        perror("Erro ao aceitar conexão");
        close(serverSocket);
        return 1;
    }

    std::cout << "Produtor: Conexão do Consumidor aceita!" << std::endl;

    // Loop para envio de números ao consumidor
    int num;
    while (true) {
        std::cout << "Produtor: Digite um número (ou 0 para encerrar): ";
        std::cin >> num;

        // Converte o número para string
        std::string numStr = std::to_string(num);

        // Envia o número ao consumidor
        if (send(clientSocket, numStr.c_str(), numStr.length(), 0) == -1) {
            perror("Erro ao enviar número");
            close(clientSocket);
            close(serverSocket);
            return 1;
        }

        if (num == 0) {
            std::cout << "Produtor: Produção encerrada." << std::endl;
            break; // Encerra o loop quando receber o número 0
        }

        // Aguarda o resultado do consumidor
        if (recv(clientSocket, buffer, BUFFER_SIZE, 0) == -1) {
            perror("Erro ao receber resultado");
            close(clientSocket);
            close(serverSocket);
            return 1;
        }

        std::cout << "Produtor: Resultado recebido: " << buffer << std::endl;
    }

    // Fecha o socket do consumidor e do servidor
    close(clientSocket);
    close(serverSocket);
    sleep(1);

    return 0;
}
