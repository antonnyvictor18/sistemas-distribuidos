#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

const int BUFFER_SIZE = 20; // Tamanho fixo do buffer para representação numérica

// função para verificar se o número é primo ou não
bool primo(int num) {
    if (num <= 1) return false;
    for (int i = 2; i * i <= num; ++i) {
        if (num % i == 0) return false;
    }
    return true;
}

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
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(8080); // Porta a ser utilizada

    // Associação do endereço ao socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Erro ao fazer bind");
        close(serverSocket);
        return 1;
    }

    // Aguarda conexão do produtor
    if (listen(serverSocket, 1) == -1) {
        perror("Erro ao escutar por conexões");
        close(serverSocket);
        return 1;
    }
    std::cout << "Aguardando conexão do produtor..." << std::endl;
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSocket == -1) {
        perror("Erro ao aceitar conexão");
        close(serverSocket);
        return 1;
    }
    std::cout << "Conexão estabelecida com o produtor!" << std::endl;

    // Loop para receber números e enviar resultados
    int num;
    int count = 0;
    while (true) {
        // Recebe número do produtor
        if (recv(clientSocket, buffer, BUFFER_SIZE, 0) == -1) {
            perror("Erro ao receber número");
            close(clientSocket);
            close(serverSocket);
            return 1;
        }
        num = std::stoi(buffer); // Converte a string recebida para inteiro
        if (num == 0) {
            std::cout << "Consumidor: Produtor terminou a produção." << std::endl;
            if(send(clientSocket, "Finalize", BUFFER_SIZE, 0)== -1){
                perror("Erro ao enviar resultado");
                close(clientSocket);
                close(serverSocket);
                return 1;
            }
            break; // Encerra o loop quando receber o número 0
        }
        // Verifica se o número é primo
        bool prime = primo(num);
        // Envia o resultado ao produtor
        if (prime) {
            if (send(clientSocket, "Primo", BUFFER_SIZE, 0) == -1) {
                perror("Erro ao enviar resultado");
                close(clientSocket);
                close(serverSocket);
                return 1;
            }
        } 
	else {
	       if (send(clientSocket, "Nao primo", BUFFER_SIZE, 0) == -1) {
	                perror("Erro ao enviar resultado");
			close(clientSocket);
			close(serverSocket);
			return 1;
		}
	}
	std::cout << "Consumidor: Número recebido: " << num << ". Resultado enviado: " << (prime ? "Primo" : "Não primo") << std::endl;
	count++;
	}

	// Fecha os sockets e termina o programa
    sleep(1);
	close(clientSocket);
	close(serverSocket);
    
	std::cout << "Consumidor: Encerrado ! \nTotal de números recebidos: " << count << std::endl;
	return 0;
}