#include <iostream>
#include <cstdlib>
#include <cstring>
#include <ctime>

using namespace std;

#define MESSAGE_SIZE 28

// Estrutura para representar uma mensagem
struct Message {
    int id;
    int processId;
    char content[MESSAGE_SIZE];

    // Construtor
    Message(int id, int processId, const char* content) : id(id), processId(processId) {
        strncpy(this->content, content, MESSAGE_SIZE);
    }

    // Método para imprimir a mensagem
    void print() {
        cout << "Message " << id << " from process " << processId << ": " << content << endl;
    }
};

// Função para enviar uma mensagem para um processo específico
void sendMessage(int processId, const char* content) {
    // Simulando o envio da mensagem para o processo
    Message message(rand() % 1000, processId, content);
    message.print();
}

// Função para o processo solicitar acesso à região crítica
void requestCriticalSection(int processId) {
    char content[MESSAGE_SIZE];
    snprintf(content, MESSAGE_SIZE, "REQUEST|%d|000000", processId);
    sendMessage(processId, content);
}

// Função para o coordenador conceder acesso à região crítica
void grantCriticalSection(int processId) {
    char content[MESSAGE_SIZE];
    snprintf(content, MESSAGE_SIZE, "GRANT|%d|000000", processId);
    sendMessage(processId, content);
}

// Função para o processo liberar a região crítica
void releaseCriticalSection(int processId) {
    char content[MESSAGE_SIZE];
    snprintf(content, MESSAGE_SIZE, "RELEASE|%d|000000", processId);
    sendMessage(processId, content);
}

int main() {
    // Inicialização do gerador de números aleatórios
    srand(static_cast<unsigned>(time(0)));

    // Simulação de um processo que solicita acesso à região crítica
    int processId = 1;
    requestCriticalSection(processId);

    // Simulação do coordenador concedendo acesso à região crítica
    grantCriticalSection(processId);

    // Simulação do processo liberando a região crítica
    releaseCriticalSection(processId);

    return 0;
}
