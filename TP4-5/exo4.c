#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct {
        int num_thread;
        int num_thread_max;
        pthread_mutex_t *mutex;
        sem_t *barriere;
    } args_t;
int cpt = 0;

void traitement(pthread_t id){
    srand(id);
    int temps = rand();
    temps = temps % 1000000; 
    usleep(temps); // Simule un traitement
}

void *rendez_vous(void *arg){
    pthread_t id = pthread_self();
    args_t *args = (args_t*)arg;
    int num_thread = args->num_thread;
    int num_thread_max = args->num_thread_max;
    pthread_mutex_t *mutex = args->mutex;
    sem_t *barriere = args->barriere;
    printf("Thread %d (%lu) : J'effectue un traitement en parallele avec les autres\n", num_thread, id);
    traitement(id);
    printf("Thread %d (%lu) : J'arrive au RdV\n", num_thread, id);
    pthread_mutex_lock(mutex);
    cpt++;
    pthread_mutex_unlock(mutex);
    if (cpt == num_thread_max){
        printf("\tThread %d (%lu) : Je suis le dernier au RdV\n", num_thread, id);
        for (int i = 0; i < num_thread_max; i++){
            sem_post(barriere);
        }
    }
    else
        printf("Thread %d (%lu) : Je ne suis pas le dernier au RdV\n", num_thread, id);
    sem_wait(barriere);
    printf("\tThread %d (%lu) : Je passe le point de RdV\n", num_thread, id);
    printf("Thread %d (%lu) : Je continue un traitement en parallele avec les autres\n", num_thread, id);
    traitement(id);
    printf("Thread %d (%lu) : Je termine mon traitement\n", num_thread, id);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]){
    if (argc != 2){
        fprintf(stderr, "Usage: %s <N>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int N = atoi(argv[1]);
    if (N < 0){
        fprintf(stderr, "Error: N must be a positive integer not equal to zero.\n");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_t mutex;
    if (pthread_mutex_init(&mutex, NULL) != 0){
        fprintf(stderr, "Error: pthread_mutex_init failed\n");
        exit(EXIT_FAILURE);
    }
    sem_t barriere;
    if (sem_init(&barriere, 0, 0) != 0){
        fprintf(stderr, "Error: sem_init failed\n");
        exit(EXIT_FAILURE);
    }
    args_t args[N];
    pthread_t threads[N];
    for (int i = 0; i < N; i++){
        args[i].num_thread = i;
        args[i].num_thread_max = N;
        args[i].mutex = &mutex;
        args[i].barriere = &barriere;
    }
    for (int i = 0; i < N; i++){
        if (pthread_create(&threads[i], NULL, rendez_vous, &args[i]) != 0){
            fprintf(stderr, "Error: pthread_create failed\n");
            exit(EXIT_FAILURE);
        }
    }
    for (int i = 0; i < N; i++){
        if (pthread_join(threads[i], NULL) != 0){
            fprintf(stderr, "Error: pthread_join failed\n");
            exit(EXIT_FAILURE);
        }
    }
    if (pthread_mutex_destroy(&mutex) != 0){
        fprintf(stderr, "Error: pthread_mutex_destroy failed\n");
        exit(EXIT_FAILURE);
    }
    if (sem_destroy(&barriere) != 0){
        fprintf(stderr, "Error: sem_destroy failed\n");
        exit(EXIT_FAILURE);
    }
    printf("Fin de l'execution du thread principal\n");
    exit(EXIT_SUCCESS);
}