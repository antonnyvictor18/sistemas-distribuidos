#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

const int F = 10;  // Tamanho fixo das mensagens
const int PORT = 5000;  // Porta para comunicação

// Enumeração para os tipos de mensagem
enum MessageType {
    ELECTION,
    COORDINATOR,
    REQUEST,
    GRANT,
    RELEASE
};

// Classe para representar uma mensagem
class Message {
public:
    MessageType type;
    int senderId;

    Message(MessageType t, int sender) : type(t), senderId(sender) {}

    string serialize() {
        string msg = to_string(type) + "|" + to_string(senderId);
        msg.resize(F, '0');  // Padding
        return msg;
    }

    static Message deserialize(const string& message) {
        MessageType type = static_cast<MessageType>(stoi(message.substr(0, 1)));
        int senderId = stoi(message.substr(2, 1));
        return Message(type, senderId);
    }
};

// Classe para o algoritmo de exclusão mútua
class DistributedMutex {
private:
    int processCount;
    int coordinatorId;
    int requestId;
    bool inCriticalSection;
    bool electionInProgress;
    mutex mutexLock;
    condition_variable cv;
    vector<thread> threads;
    vector<bool> replyReceived;
    vector<int> sockets;
    vector<struct sockaddr_in> addresses;

    void sendMessage(Message message, int receiverId) {
        string msg = message.serialize();

        // Envia a mensagem ao processo receiverId
        send(sockets[receiverId], msg.c_str(), msg.size(), 0);
        cout << "Sending message: " << msg << " to process " << receiverId << endl;
    }

    void receiveMessage(int senderId) {
        char buffer[F];
        int bytesRead = recv(sockets[senderId], buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            // Erro na recepção da mensagem
            return;
        }

        string message(buffer, bytesRead);
        Message receivedMessage = Message::deserialize(message);
        processMessage(receivedMessage);
    }

    void processMessage(Message message) {
        switch (message.type) {
            case ELECTION:
                processElection(message);
                break;
            case COORDINATOR:
                processCoordinator(message);
                break;
            case REQUEST:
                processRequest(message);
                break;
            case GRANT:
                processGrant(message);
                break;
            case RELEASE:
                processRelease(message);
                break;
            default:
                break;
        }
    }

    void processElection(Message election) {
        unique_lock<mutex> lock(mutexLock);
        if (electionInProgress) {
            // Ignora a mensagem de eleição se já houver uma eleição em andamento
            return;
        }

        electionInProgress = true;

        if (coordinatorId == election.senderId) {
            // O coordenador recebeu a mensagem de eleição
            // Ele responde com a mensagem de coordenador
            sendMessage(Message(COORDINATOR, coordinatorId), election.senderId);
        } else if (coordinatorId < election.senderId) {
            // O coordenador atual está desatualizado, inicia nova eleição
            electionInProgress = false;
            sendMessage(Message(ELECTION, election.senderId), election.senderId);
        }
    }

    void processCoordinator(Message coordinator) {
        unique_lock<mutex> lock(mutexLock);
        coordinatorId = coordinator.senderId;
        electionInProgress = false;
        cv.notify_all();
    }

    void processRequest(Message request) {
        unique_lock<mutex> lock(mutexLock);
        if (requestId == -1 || requestId > request.senderId) {
            replyReceived[request.senderId] = false;
            sendMessage(Message(GRANT, coordinatorId), request.senderId);
        } else {
            replyReceived[request.senderId] = true;
        }
    }

    void processGrant(Message grant) {
        unique_lock<mutex> lock(mutexLock);
        replyReceived[grant.senderId] = true;
        cv.notify_all();
    }

    void processRelease(Message release) {
        unique_lock<mutex> lock(mutexLock);
        inCriticalSection = false;
        requestId = -1;
        cv.notify_all();
    }

    void coordinatorThread() {
        while (true) {
            unique_lock<mutex> lock(mutexLock);
            cv.wait(lock, [this]() {
                for (bool reply : replyReceived) {
                    if (!reply) {
                        return false;
                    }
                }
                return true;
            });

            // Entra na região crítica
            inCriticalSection = true;

            // Simula o uso da região crítica por um tempo
            cout << "Process " << coordinatorId << " is in the critical section." << endl;
            this_thread::sleep_for(chrono::seconds(2));

            // Sai da região crítica
            inCriticalSection = false;
            requestId = -1;
            for (int i = 0; i < processCount; i++) {
                replyReceived[i] = false;
            }
            cout << "Process " << coordinatorId << " released the critical section." << endl;

            // Notifica os processos que a região crítica está livre
            for (int i = 0; i < processCount; i++) {
                if (i != coordinatorId) {
                    sendMessage(Message(RELEASE, coordinatorId), i);
                }
            }
        }
    }

    void initiateElection() {
        unique_lock<mutex> lock(mutexLock);
        electionInProgress = true;

        // Envia mensagens de eleição para os processos com IDs maiores
        for (int i = coordinatorId + 1; i < processCount; i++) {
            sendMessage(Message(ELECTION, coordinatorId), i);
        }

        // Aguarda resposta da eleição
        cv.wait(lock, [this]() {
            return !electionInProgress;
        });
    }

public:
    DistributedMutex(int processCount, int coordinatorId) : processCount(processCount), coordinatorId(coordinatorId),
                                                            requestId(-1), inCriticalSection(false),
                                                            electionInProgress(false),
                                                            replyReceived(processCount, false),
                                                            sockets(processCount), addresses(processCount) {
        threads.resize(processCount);
    }

    void initializeSockets() {
        // Cria um socket para cada processo
        for (int i = 0; i < processCount; i++) {
            sockets[i] = socket(AF_INET, SOCK_STREAM, 0);
            if (sockets[i] < 0) {
                cerr << "Error opening socket" << endl;
                exit(1);
            }

            // Configura o endereço para conexão
            memset(&addresses[i], 0, sizeof(addresses[i]));
            addresses[i].sin_family = AF_INET;
            addresses[i].sin_port = htons(PORT);
            if (inet_pton(AF_INET, "127.0.0.1", &(addresses[i].sin_addr)) <= 0) {
                cerr << "Invalid address" << endl;
                exit(1);
            }
        }

        // Estabelece conexões com os outros processos
        for (int i = 0; i < processCount; i++) {
            if (i != coordinatorId) {
                if (connect(sockets[i], (struct sockaddr*)&addresses[i], sizeof(addresses[i])) < 0) {
                    cerr << "Error connecting to process " << i << endl;
                    exit(1);
                }
            }
        }
    }

    void start() {
        initializeSockets();

        threads[coordinatorId] = thread(&DistributedMutex::coordinatorThread, this);
        // Inicializa outros processos
        for (int i = 0; i < processCount; i++) {
            if (i != coordinatorId) {
                threads[i] = thread([this, i]() {
                    while (true) {
                        this_thread::sleep_for(chrono::seconds(1));

                        // Envia uma requisição para entrar na região crítica
                        unique_lock<mutex> lock(mutexLock);
                        if (!inCriticalSection && requestId == -1) {
                            requestId = i;
                            sendMessage(Message(REQUEST, i), coordinatorId);
                        }

                        // Espera até receber o sinal para entrar na região crítica
                        cv.wait(lock, [this]() {
                            return inCriticalSection;
                        });

                        // Simula o uso da região crítica por um tempo
                        cout << "Process " << i << " is in the critical section." << endl;
                        this_thread::sleep_for(chrono::seconds(3));

                        // Sai da região crítica
                        unique_lock<mutex> lock2(mutexLock);
                        inCriticalSection = false;
                        requestId = -1;
                        for (int i = 0; i < processCount; i++) {
                            replyReceived[i] = false;
                        }
                        cout << "Process " << i << " released the critical section." << endl;

                        // Notifica o coordenador que a região crítica está livre
                        sendMessage(Message(RELEASE, i), coordinatorId);
                    }
                });
            }
        }

        // Processo de eleição
        if (coordinatorId != 0) {
            initiateElection();
        }

        for (int i = 0; i < processCount; i++) {
            if (i != coordinatorId) {
                threads[i].join();
            }
        }
        threads[coordinatorId].join();
    }
};

int main() {
    int processCount;

    cout << "Enter the number of processes: ";
    cin >> processCount;

    DistributedMutex mutex(processCount, 0);
    mutex.start();
    return 0;
}
