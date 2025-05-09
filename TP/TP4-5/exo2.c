#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

int solde = 0; // Variable globale pour le solde
typedef struct {
        int NO;
        int qte;
        pthread_mutex_t *mutex;
    } args_t;

void* debit(void *arg){
    args_t *args = (args_t*)arg;
    int NO = args->NO;
    int qte = args->qte;
    pthread_mutex_t *mutex = args->mutex;
    int *retour = malloc(sizeof(int));
    long num = pthread_self();
    if (retour == NULL){
        fprintf(stderr, "Error: malloc failed\n");
        pthread_exit(NULL);
    }
    if (pthread_mutex_lock(mutex) != 0){ /////////////////// Lock mutex
        fprintf(stderr, "Error: pthread_mutex_lock failed\n");
        *retour = -1;
        pthread_exit(retour);
    }
    if (solde <= 0){
        fprintf(stderr, "\tDebit %lu : Si debit de %d le solde (%d) sera debiteur\n", num, qte, solde);
        *retour = -1;
        pthread_exit(retour);
    }
    for (int i = 0; i < NO; i++){
        int delai = rand() % 101;
        usleep(delai);
        solde -= qte;
        printf("Debit %lu : debit (%d) => solde = %d\n", num, qte, solde);
        *retour = solde;
    }
    if (pthread_mutex_unlock(mutex) != 0){ /////////////////// unLock mutex
        fprintf(stderr, "Error: pthread_mutex_unlock failed\n");
        *retour = -1;
        pthread_exit(retour);
    }
    pthread_exit(retour);
}

void* credit(void *arg){
    args_t *args = (args_t*)arg;
    int NO = args->NO;
    int qte = args->qte;
    pthread_mutex_t *mutex = args->mutex;
    int *retour = malloc(sizeof(int));
    long num = pthread_self();
    if (retour == NULL){
        fprintf(stderr, "Error: malloc failed\n");
        pthread_exit(NULL);
    }
    if (pthread_mutex_lock(mutex) != 0){ /////////////////// Lock mutex
        fprintf(stderr, "Error: pthread_mutex_lock failed\n");
        *retour = -1;
        pthread_exit(retour);
    }
    for (int i = 0; i < NO; i++){
        int delai = rand() % 101;
        usleep(delai);
        solde += qte;
        printf("Credit %lu : credit (%d) => solde = %d\n", num, qte, solde);
        *retour = solde;
    }
    if (pthread_mutex_unlock(mutex) != 0){ /////////////////// unLock mutex
        fprintf(stderr, "Error: pthread_mutex_unlock failed\n");
        *retour = -1;
        pthread_exit(retour);
    }
    pthread_exit(retour);
}

int main(int argc, char *argv[]){
    if (argc != 4){
        fprintf(stderr, "Usage: %s <NT> <solde> <NO>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int NT = atoi(argv[1]);
    solde = atoi(argv[2]);
    int NO = atoi(argv[3]);
    if (NT <= 0 || solde < 0 || NO <= 0){
        fprintf(stderr, "Error: NT, solde and NO must be positive integers.\n");
        exit(EXIT_FAILURE);
    } 
    int *retour;
    pthread_mutex_t mutex;
    if (pthread_mutex_init(&mutex, NULL) != 0){
        fprintf(stderr, "Error: pthread_mutex_init failed\n");
        exit(EXIT_FAILURE);
    }

    pthread_t fils[NT];
    args_t args[NT];
    for (int i = 0; i < NT/2; i++){
        args[i].NO = NO;
        srand(time(NULL) + solde);
        args[i].qte = rand() % 9 + 1; // Génère un nombre aléatoire entre 1 et 9
        args[i].mutex = &mutex;
    }
    for (int i = 0; i < NT/2; i++){
        if (pthread_create(&fils[i], NULL, credit, (void*)&args[i]) != 0){
            fprintf(stderr, "Error: pthread_create failed for thread %d\n", i);
            strerror(errno);
            exit(EXIT_FAILURE);
        }
        if (pthread_create(&fils[i], NULL, debit, (void*)&args[i]) != 0){
            fprintf(stderr, "Error: pthread_create failed for thread %d\n", i);
            strerror(errno);
            exit(EXIT_FAILURE);
        }
    }
    for (int i = 0; i < NT/2; i++){
        if (pthread_join(fils[i], (void**)&retour) != 0){
            fprintf(stderr, "Error: pthread_join failed for thread %d\n", i);
            strerror(errno);
            exit(EXIT_FAILURE);
        }
        free(retour);
    }
    pthread_mutex_destroy(&mutex);
    printf("Solde = %d\n", solde);
    return 0;
}