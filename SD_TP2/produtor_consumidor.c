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
} ThreadData;

bool primo(int num) {
    if (num <= 1) {
        return false;
    }

    for (int i = 2; i <= num/2; i++) {
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
        }
    }
}

void read_value(int size, int*vector) {
    for (int i = 0; i < size; i++) {
        if (vector[i] != 0) {
            int element = vector[i];
            if (primo(element)) {
                printf("Número %d consumido da posicao %d. PRIMO\n", element, i);
            }
            else {
                printf("Número %d consumido da posicao %d. NAO-PRIMO\n", element, i);
            }
            vector[i] = 0;
            break;
        }
    }
}

void update_status(int* status_vector, int m, int inc_or_dec) {
    for (int i = 0; i < m * 2; i++) {
        if (status_vector[i] == 0) {
            if (i == 0) {
                status_vector[i] = inc_or_dec;
            }
            else {
                status_vector[i] = status_vector[i-1] + inc_or_dec;
            }
        }
    }
}

bool check_consumed(int* consumed, int m) {
    if ((*consumed) != m) {
        return true;
    }
    exit(1);
}

void update_consumed(int* consumed) {
    (*consumed)++;
    printf("%d \n", *consumed);
}

void* produtor(void* thread_data) {
    ThreadData* data = (ThreadData*)thread_data;
    int n = data->n;
    int m = data->m;
    int* vector = data->vector;
    int* status_vector = data->status_vector;

    while(1) {
        sem_wait(&empty);
        sem_wait(&mutex);
        check_consumed(&consumed, m);
        write_value(n, vector);
        update_status(status_vector, m, 1);
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

    while(1) {
        sem_wait(&full);
        sem_wait(&mutex);
        check_consumed(&consumed, m);
        read_value(n, vector);
        update_status(status_vector, m, -1);
        update_consumed(&consumed);
        sem_post(&mutex);
        sem_post(&empty);
    }
}

int main() {

    int m = 100;
    int n = 5;
    int nc = 1;
    int np = 4;

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

    pthread_t threads_consumidoras[nc];
    ThreadData threads_consumidoras_data[nc];
    // start_time = clock();
    for (int i = 0; i < nc; ++i) {

        threads_consumidoras_data[i].n = n;
        threads_consumidoras_data[i].m = m;
        threads_consumidoras_data[i].vector = vector;
        threads_consumidoras_data[i].status_vector = status_vector;

        int result = pthread_create(&(threads_consumidoras[i]), NULL, consumidor, &(threads_consumidoras_data[i]));
        if (result != 0) {
            printf("Falha ao Criar Thread COnsumidora.\n");
            return 1;
        }
    }

    pthread_t threads_produtoras[np];
    ThreadData threads_produtoras_data[np];
    // start_time = clock();
    for (int i = 0; i < np; ++i) {

        threads_produtoras_data[i].n = n;
        threads_produtoras_data[i].m = m;
        threads_produtoras_data[i].vector = vector;
        threads_produtoras_data[i].status_vector = status_vector;

        int result = pthread_create(&(threads_produtoras[i]), NULL, produtor, &(threads_produtoras_data[i]));
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