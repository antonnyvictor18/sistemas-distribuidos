#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
using namespace std;

const int BUFFER_SIZE = 20; // Tamanho fixo do buffer para representação numérica
bool primo;


// função para verificar se o número é primo ou não
bool eh_primo(int num) {
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
        cerr << "Erro ao criar socket" << endl;
        return EXIT_FAILURE;
    }

    // Configuração do endereço do servidor
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(8080); // Porta a ser utilizada

    // Associação do endereço ao socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        cerr << "Erro ao fazer bind" << endl;
        close(serverSocket);
        return EXIT_FAILURE;
    }

    // Aguarda conexão do produtor
    if (listen(serverSocket, 1) == -1) {
        cerr << "Erro ao escutar por conexões" << endl;
        close(serverSocket);
        return EXIT_FAILURE;
    }
    cout << "Aguardando conexão do produtor..." << endl;
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSocket == -1) {
        cerr << "Erro ao aceitar conexão" << endl;
        close(serverSocket);
        return EXIT_FAILURE;
    }
    cout << "Conexão estabelecida com o produtor!" << endl;

    // Loop para receber números e enviar resultados
    int num;
    int count = 0;
    while (true) {
        // Recebe número do produtor
        if (recv(clientSocket, buffer, BUFFER_SIZE, 0) == -1) {
            cerr << "Erro ao receber número" << endl;
            close(clientSocket);
            close(serverSocket);
            return EXIT_FAILURE;
        }
        num = stoi(buffer); // Converte a string recebida para inteiro
        if (num == 0) {
            cout << "Consumidor: Produtor terminou a produção." << endl;
            if(send(clientSocket, "Finalize", BUFFER_SIZE, 0)== -1){
                cerr << "Erro ao enviar resultado" << endl;
                close(clientSocket);
                close(serverSocket);
                return EXIT_FAILURE;
            }
            break; // Encerra o loop quando receber o número 0
        }

        // Verifica se o número é primo e envia o resultado ao produtor
        primo = eh_primo(num);
        if (primo) {
            if (send(clientSocket, "Primo", BUFFER_SIZE, 0) == -1) {
                cerr << "Erro ao enviar resultado" << endl;
                close(clientSocket);
                close(serverSocket);
                return EXIT_FAILURE;
            }
        } 
	else {
	       if (send(clientSocket, "Nao primo", BUFFER_SIZE, 0) == -1) {
	            cerr << "Erro ao enviar resultado" << endl;
			    close(clientSocket);
			    close(serverSocket);
			    return EXIT_FAILURE ;
		}
	}
	cout << "Consumidor: Número recebido: " << num << ". Resultado enviado: " << (primo ? "Primo" : "Não primo") << endl;
	count++;
	}

	// Fecha os sockets e termina o programa
	close(clientSocket);
	close(serverSocket);
    
	cout << "Consumidor: Encerrado ! \nTotal de números recebidos: " << count << endl;
	return EXIT_SUCCESS;
}