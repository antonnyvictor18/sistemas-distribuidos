#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <fstream>

using namespace std;

// Constantes
const int F = 10; // Tamanho fixo das mensagens em bytes
const char separator = '|'; // Separador de campos nas mensagens
const string logFilename = "log.txt";
const string resultFilename = "resultado.txt";

// Tipos de mensagem
enum MessageType {
    REQUEST,
    GRANT,
    RELEASE
};

// Estrutura para representar uma mensagem
struct Message {
    MessageType type;
    int processId;
    string data;
};

// Fila de pedidos
queue<int> requestQueue;

// Contador de vezes atendido
vector<int> processStats;

// Mutex e variável de condição para sincronização da fila de pedidos
mutex requestMutex;
condition_variable requestCV;

// Mutex para acesso ao arquivo de log
mutex logMutex;

// Função para imprimir a fila de pedidos
void printRequestQueue() {
    unique_lock<mutex> lock(requestMutex);
    cout << "Fila de pedidos: ";
    while (!requestQueue.empty()) {
        cout << requestQueue.front() << " ";
        requestQueue.pop();
    }
    cout << endl;
}

// Função para imprimir as estatísticas de acesso
void printProcessStats() {
    cout << "Estatísticas de acesso: " << endl;
    for (int i = 0; i < processStats.size(); i++) {
        cout << "Processo " << i << ": " << processStats[i] << " vezes" << endl;
    }
}

// Função para escrever mensagem no arquivo de log
void writeLog(const string& message) {
    lock_guard<mutex> lock(logMutex);
    ofstream logFile(logFilename, ios_base::app);
    if (logFile.is_open()) {
        logFile << message << endl;
        logFile.close();
    }
}

// Função para enviar uma mensagem
void sendMessage(MessageType type, int processId, int destination) {
    // Construir mensagem
    string message = to_string(type) + separator + to_string(processId);
    message.resize(F - 1, '0');
    message += separator;

    // Simular envio da mensagem
    cout << "Enviando mensagem: " << message << " para o processo " << destination << endl;
    // Aqui você pode implementar a lógica de envio real da mensagem
    // usando sockets, por exemplo
    // ...

    // Escrever no arquivo de log
    string logMessage = "Enviada | " + message + " | Destino: " + to_string(destination);
    writeLog(logMessage);
}

// Função para processar uma mensagem recebida
void processMessage(string& message) {
    // Extrair informações da mensagem
    string messageTypeStr = message.substr(0, message.find(separator));
    MessageType messageType = static_cast<MessageType>(stoi(messageTypeStr));
    message.erase(0, message.find(separator) + 1);
    string processIdStr = message.substr(0, message.find(separator));
    int processId = stoi(processIdStr);

    // Realizar ação com base no tipo de mensagem recebida
    switch (messageType) {
        case REQUEST: {
            // Adicionar o processo à fila de pedidos
            unique_lock<mutex> lock(requestMutex);
            requestQueue.push(processId);
            cout << "Processo " << processId << " solicitou acesso à região crítica." << endl;
            // Escrever no arquivo de log
            string logMessage = "Recebida | " + messageTypeStr + separator + processIdStr +
                " | Origem: " + to_string(processId);
            writeLog(logMessage);
            break;
        }
        case GRANT: {
            // Processo recebeu acesso à região crítica
            cout << "Processo " << processId << " recebeu acesso à região crítica." << endl;
            // Atualizar estatísticas de acesso
            processStats[processId]++;
            // Escrever no arquivo de log
            string logMessage = "Recebida | " + messageTypeStr + separator + processIdStr +
                " | Origem: " + to_string(processId);
            writeLog(logMessage);
            break;
        }
        case RELEASE: {
            // Processo liberou acesso à região crítica
            cout << "Processo " << processId << " liberou acesso à região crítica." << endl;
            // Escrever no arquivo de log
            string logMessage = "Recebida | " + messageTypeStr + separator + processIdStr +
                " | Origem: " + to_string(processId);
            writeLog(logMessage);
            break;
        }
    }
}

// Função para processar pedidos na fila
void processRequests() {
    while (true) {
        unique_lock<mutex> lock(requestMutex);
        // Aguardar até que haja pedidos na fila
        requestCV.wait(lock, [] { return !requestQueue.empty(); });
        // Processar pedido
        int processId = requestQueue.front();
        requestQueue.pop();
        // Enviar mensagem GRANT para o processo
        sendMessage(GRANT, processId, processId);
        // Escrever no arquivo de log
        string logMessage = "Enviada | " + to_string(GRANT) + separator + to_string(processId) +
            " | Destino: " + to_string(processId);
        writeLog(logMessage);
        // Liberar o mutex antes de processar a mensagem
        lock.unlock();
        // Simular execução da região crítica
        ofstream resultFile(resultFilename, ios_base::app);
        if (resultFile.is_open()) {
            auto currentTime = chrono::system_clock::now();
            auto currentTimeMs = chrono::duration_cast<chrono::milliseconds>(currentTime.time_since_epoch()).count();
            resultFile << "Processo " << processId << " - " << currentTimeMs << endl;
            resultFile.close();
        }
        this_thread::sleep_for(chrono::seconds(2));
        // Enviar mensagem RELEASE para o coordenador
        sendMessage(RELEASE, processId, 0);
        // Escrever no arquivo de log
        logMessage = "Enviada | " + to_string(RELEASE) + separator + to_string(processId) +
            " | Destino: 0";
        writeLog(logMessage);
    }
}

// Função para processar comandos da interface do terminal
void processTerminalCommands() {
    while (true) {
        string command;
        cout << "Digite o comando (1 - Fila de pedidos, 2 - Estatísticas, 3 - Sair): ";
        cin >> command;

        if (command == "1") {
            printRequestQueue();
        } else if (command == "2") {
            printProcessStats();
        } else if (command == "3") {
            break;
        } else {
            cout << "Comando inválido!" << endl;
        }
    }
}

int main() {
    int n = 2; // Número de processos
    int r = 10; // Número de repetições
    int k = 2; // Tempo de espera em segundos

    // Inicializar estatísticas de acesso
    processStats.resize(n, 0);

    // Iniciar thread para processar pedidos na fila
    thread requestThread(processRequests);

    // Iniciar thread para processar comandos da interface do terminal
    thread terminalThread(processTerminalCommands);

    // Aguardar até que todas as repetições sejam concluídas para finalizar o programa
    for (int i = 0; i < r; i++) {
        // Enviar mensagem REQUEST para o coordenador
        sendMessage(REQUEST, i % n, 0);
        // Escrever no arquivo de log
        string logMessage = "Enviada | " + to_string(REQUEST) + separator + to_string(i % n) +
            " | Destino: 0";
        writeLog(logMessage);
        this_thread::sleep_for(chrono::seconds(k));
    }

    // Aguardar finalização das threads
    requestThread.join();
    terminalThread.join();

    return 0;
}
