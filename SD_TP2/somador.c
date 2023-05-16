#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdatomic.h>

// Define strcut com variavies para serem passadas na função da thread 
typedef struct {
    int id;
    int pack_number;
    int *vector;
    int add_on;
} ThreadData;

// Define lock, acquire e release
typedef struct {
    int held;
} lock;

void acquire(lock* lock) {
    while (atomic_flag_test_and_set(&(lock->held)));
}

void release(lock* lock) {
    lock->held = 0;
}

// Inicializa as variavies globais total (soma do total) e lock
int total = 0;
lock lock_mutex;

// Função que será chamada pelas threads
void* sum_values(void* thread_data) {
    ThreadData* data = (ThreadData*)thread_data;
    int id = data->id;
    int pack_number = data->pack_number;
    int* vector = data->vector;
    int add_on = data->add_on;

    // Definindo o bloco de iteração de cada thread
    int start = id * pack_number;
    int end = start + pack_number + add_on;

    // Realiza a soma do bloco de cada thread
    int soma_thread = 0;
    for (int i = start; i < end; i++) {
        soma_thread += vector[i];
    }

    // Acessa a variável compartilhada soma_total e adiciona a soma da thread
    acquire(&lock_mutex);
    total += soma_thread;
    release(&lock_mutex);

    return NULL;
}

void fill_with_random_numbers(int size, int* vector) {
    for (int i = 0; i < size; i++) {
        vector[i] = (rand() % (200 + 1)) - 100;
    }
}

int main() {

    srand(time(0));
    int check_sum;
    clock_t start_time, end_time;
    double execution_time;
    int pack_number;

    // Open the file in write mode
    FILE* file = fopen("somador_output.txt", "w");

    // Check if the file was opened successfully
    if (file == NULL) {
        printf("Failed to open the file.\n");
        return 1;
    }

    fprintf(file, "exec n k time\n");

    for (int exec = 0; exec < 10; exec ++) {
        for (int n = 10000000; n <= 1000000000; n *= 10) {

            for (int k = 1; k <= 256; k*=2) {

                int* vector = (int*)malloc(n * sizeof(int));
                pack_number = round(n / k);

                // Inicializando as variáveis de controle das threads
                pthread_t threads[k];
                ThreadData thread_data[k];

                // Inicializando o vetor e preenchendo com números aleatórios
                fill_with_random_numbers(n, vector);
                // for (int i = 0; i < n; i++) {
                //     printf("%d ", vector[i]);
                // }         
                // printf("\n");

                // Cria as threads
                start_time = clock();
                for (int i = 0; i < k; ++i) {
                    if (i == k-1) {
                        thread_data[i].add_on = n - (k * pack_number);
                    }
                    else {
                        thread_data[i].add_on = 0;
                    }
                    thread_data[i].pack_number = pack_number;
                    thread_data[i].id = i;
                    thread_data[i].vector = vector;

                    int result = pthread_create(&(threads[i]), NULL, sum_values, &(thread_data[i]));
                    if (result != 0) {
                        printf("Failed to create thread %d.\n", i);
                        return 1;
                    }
                }

                // Aguarda o fim das Threads
                for (int i = 0; i < k; i++) {
                    pthread_join(threads[i], NULL);
                }
                end_time = clock();
                execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

                check_sum = 0;
                for (int i = 0; i < n; i++) {
                    check_sum += vector[i];
                }

                if (check_sum != total) {
                    printf("ERROR: The correct sum value was %d but the threads returned %d\n", check_sum, total);
                    return 1;
                }

                printf("Execution: %d || N: %d || K: %d || Time %f\n", exec, n, k, execution_time);
                fprintf(file, "%d %d %d %f\n", exec, n, k, execution_time);

                free(vector);
                total = 0;
            }
        }
    }
    fclose(file);
    return 0;
}
