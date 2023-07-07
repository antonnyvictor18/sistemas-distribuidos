#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdatomic.h>
#include <semaphore.h>
#include <unistd.h>

sem_t mutex;
sem_t full;
sem_t empty;

int consumed = 0;

typedef struct {
    int n;
    int m;
    int *vector;
    int *status_vector;
    FILE *file;
} ThreadData;

bool primo(int num) {
    if (num <= 1) {
        return false;
    }
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) {
            return false;
        }
    }
    return true;
}

void write_value(int size, int* vector) {
    for (int i = 0; i < size; i++) {
        if (vector[i] == 0) {
            int num = (rand() % 10000000) + 1;
            vector[i] = num;
            printf("Número %d produzido na posicao %d.\n", num, i);
            break;
        }
    }
}

int read_value(int size, int*vector, int*consumed) {
    for (int i = 0; i < size; i++) {
        if (vector[i] != 0) {
            int x = vector[i];
            vector[i] = 0;
            (*consumed)++;
            return x;
        }
    }
}

int update_status(int* status_vector, int m, int inc_or_dec, FILE* file) {
    for (int i = 0; i < m * 2; i++) {
        if (status_vector[i] == 0) {
            if (i == 0) {
                status_vector[i] = inc_or_dec;
            }
            else if (inc_or_dec == -1) {
                status_vector[i] = status_vector[i-1] - 1;
            }
            else {
                status_vector[i] = status_vector[i-1] + 1;
            }
            fprintf(file, "%d %d\n", i, status_vector[i]);
            return 0;
        }
    }
    return 0;
}

bool check_consumed(int* consumed, int m, FILE* file) {
    if ((*consumed) < m) {
        return true;
    }
    fclose(file);
    exit(0);
}

void update_consumed(int* consumed) {
    (*consumed)++;
}

void* produtor(void* thread_data) {
    ThreadData* data = (ThreadData*)thread_data;
    int n = data->n;
    int m = data->m;
    int* vector = data->vector;
    int* status_vector = data->status_vector;
    FILE* file = data->file;
    int inc_or_dec = 1;

    while(1) {
        sem_wait(&empty);
        sem_wait(&mutex);
        check_consumed(&consumed, m, file);
        write_value(n, vector);
        update_status(status_vector, m, inc_or_dec, file);
        sem_post(&mutex);
        sem_post(&full);
    }
}

void* consumidor(void* thread_data) {
    ThreadData* data = (ThreadData*)thread_data;
    int n = data->n;
    int m = data->m;
    int* vector = data->vector;
    int* status_vector = data->status_vector;
    FILE* file = data->file;
    int inc_or_dec = -1;
    int result;

    while(1) {

        sem_wait(&full);
        sem_wait(&mutex);
        check_consumed(&consumed, m, file);
        // update_consumed(&consumed);
        result = read_value(n, vector, &consumed);
        update_status(status_vector, m, inc_or_dec, file);
        sem_post(&mutex);
        sem_post(&empty);

        // update_consumed(&consumed);
        if (primo(result)) {
            printf("Número %d consumido é PRIMO\n", result);
        }
        else {
            printf("Número %d consumido é NAO-PRIMO\n", result);
        }
    }
}

int main(int argc, char *argv[]) {
    // Verificar se o número de argumentos está correto
    if (argc != 4) {
        printf("O programa deve receber 3 argumentos: n, np, nc");
        exit(1);
    }


    int n =  atoi(argv[1]);
    int np =  atoi(argv[2]);
    int nc =  atoi(argv[3]);
    int m = 5000;

    FILE* file = fopen("consumidor_produtor_output.txt", "w");
    // Check if the file was opened successfully
    if (file == NULL) {
        printf("Failed to open the file.\n");
        return 1;
    }
    fprintf(file, "stage fullness\n");


    // Inicializando o buffer e o buffer que controla o preenchimento deste
    int* vector = (int*)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        vector[i] = 0;
    }
    int* status_vector = (int*)malloc(m * 2 * sizeof(int));
    for (int i = 0; i < m * 2; i++) {
        status_vector[i] = 0;
    }

    // Inicializando semáforos
    if (sem_init(&mutex, 0, 1) != 0) {
        printf("Não foi possivel inicializar o mutex.\n");
        return 1;
    }
    if (sem_init(&full, 0, 0) != 0) {
        printf("Não foi possivel inicializar o semaforo full.\n");
        return 1;
    }
    if (sem_init(&empty, 0, n) != 0) {
        printf("Não foi possivel inicializar o semaforo empty.\n");
        return 1;
    }

    pthread_t threads_produtoras[np];
    ThreadData threads_produtoras_data[np];

    for (int i = 0; i < np; ++i) {

        threads_produtoras_data[i].n = n;
        threads_produtoras_data[i].m = m;
        threads_produtoras_data[i].vector = vector;
        threads_produtoras_data[i].status_vector = status_vector;
        threads_produtoras_data[i].file = file; 

        int result = pthread_create(&(threads_produtoras[i]), NULL, produtor, &(threads_produtoras_data[i]));
        if (result != 0) {
            printf("Falha ao Criar Thread Consumidora.\n");
            return 1;
        }
    }

    pthread_t threads_consumidoras[nc];
    ThreadData threads_consumidoras_data[nc];

    for (int i = 0; i < nc; ++i) {

        threads_consumidoras_data[i].n = n;
        threads_consumidoras_data[i].m = m;
        threads_consumidoras_data[i].vector = vector;
        threads_consumidoras_data[i].status_vector = status_vector;
        threads_consumidoras_data[i].file = file;

        int result = pthread_create(&(threads_consumidoras[i]), NULL, consumidor, &(threads_consumidoras_data[i]));
        if (result != 0) {
            printf("Falha ao Criar Thread Consumidora.\n");
            return 1;
        }
    }

    // Aguarda o fim das Threads
    for (int i = 0; i < np; i++) {
        pthread_join(threads_produtoras[i], NULL);
    }
    for (int i = 0; i < nc; i++) {
        pthread_join(threads_consumidoras[i], NULL);
    }

    return 0;

}