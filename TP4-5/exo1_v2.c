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
        int rang;
        int NBM;
        int NBL;
        int NBT;
        sem_t *tab_sem;
    } args_t;


void* affichage(void *arg){
    args_t *args = (args_t*)arg;
    int rang = args->rang;
    int NBM = args->NBM;
    int NBL = args->NBL;
    int NBT = args->NBT;
    sem_t *tab_sem = args->tab_sem;
    long num = pthread_self();
    int *retour = malloc(sizeof(int));
    if (retour == NULL){
        fprintf(stderr, "Error: malloc failed\n");
        pthread_exit(NULL);
    }
    *retour = rang;              
    for (int i = 0; i < NBM; i++){
        if (sem_wait(&tab_sem[rang]) != 0){
            fprintf(stderr, "Error: sem_wait failed\n");
            *retour = EXIT_FAILURE;
            pthread_exit(retour);
        }
        for (int j = 0; j < NBL; j++){
            printf("Afficheur %d (%lu), j'affiche ligne %d/%d du message %d/%d\n", rang, num, j, NBL, i, NBM);
        }
        if (sem_post(&tab_sem[(rang+1)%NBT]) != 0){
            fprintf(stderr, "Error: sem_post failed\n");
            *retour = EXIT_FAILURE;
            pthread_exit(retour);
        }
    }
    printf("Afficheur %d (%lu), je me termine\n", rang, num);
    pthread_exit(retour);
}
int main(int argc, char *argv[]){
    if (argc != 4){
        fprintf(stderr, "Usage: %s <NBT> <NBM> <NBL>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int NBT = atoi(argv[1]);
    int NBM = atoi(argv[2]);
    int NBL = atoi(argv[3]);
    if (NBT <= 0 || NBM <= 0 || NBL <= 0){
        fprintf(stderr, "Error: NBT, NBM and NBL must be positive integers.\n");
        exit(EXIT_FAILURE);
    }
    sem_t tab_sem[NBT];
    if (sem_init(&tab_sem[0], 0, 1) != 0){
        fprintf(stderr, "Error: sem_init failed for semaphore 0\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 1; i < NBT; i++){
        if (sem_init(&tab_sem[i], 0, 0) != 0){
            fprintf(stderr, "Error: sem_init failed for semaphore %d\n", i);
            exit(EXIT_FAILURE);
        }
    }
    pthread_t fils[NBT];
    args_t args[NBT];
    for (int i = 0; i < NBT; i++){
        args[i].rang = i;
        args[i].NBM = NBM;
        args[i].NBL = NBL;
        args[i].NBT = NBT;
        args[i].tab_sem = tab_sem;
    }
    for (int i = 0; i < NBT; i++){
        if (pthread_create(&fils[i], NULL, affichage, (void*)&args[i]) != 0){
            fprintf(stderr, "Error: pthread_create failed for thread %d\n", i);
            strerror(errno);
            exit(EXIT_FAILURE);
        }
    }
    for (int i = 0; i < NBT; i++){
        if (pthread_join(fils[i], NULL) != 0){
            fprintf(stderr, "Error: pthread_join failed for thread %d\n", i);
            strerror(errno);
            exit(EXIT_FAILURE);
        }
        sem_destroy(&tab_sem[i]);
    }
    printf("Fin de l'execution du thread principal\n");
    return 0;
}