#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>


void* affichage(void *arg){
    int rang = ((int*)arg)[0];
    int NF = ((int*)arg)[1];
    int delai = 0;
    long num = pthread_self();
    int *retour = malloc(sizeof(int));
    if (retour == NULL){
        fprintf(stderr, "Error: malloc failed\n");
        pthread_exit(NULL);
    }
    *retour = rang;
    srand(num);
    delai = rand() % 101;                
    for (int j = 0; j < NF; j++){
        delai += 1;
        printf("Activite rang %d : identifiant = %lu (delai = %d)\n", rang, num, delai);
        usleep(delai);
    }
    pthread_exit(retour);
}

int main(int argc, char *argv[]){
    if (argc != 3){
        fprintf(stderr, "Usage: %s <NA> <NF>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int NA = atoi(argv[1]);
    int NF = atoi(argv[2]);
    if (NA <= 0 || NF <= 0){
        fprintf(stderr, "Error: NA and NF must be positive integers.\n");
        exit(EXIT_FAILURE);
    }
    int *retour;
    pthread_t fils[NA];
    typedef struct {
        int rang;
        int NF;
    } args_t;

    args_t args[NA];
    for (int i = 0; i < NA; i++){
        args[i].rang = i;
        args[i].NF = NF;
    }
    for (int i = 0; i < NA; i++){
        if (pthread_create(&fils[i], NULL, affichage, (void*)&args[i]) != 0){
            fprintf(stderr, "Error: pthread_create failed for thread %d\n", i);
            strerror(errno);
            exit(EXIT_FAILURE);
        }
    }
    for (int i = 0; i < NA; i++){
        if (pthread_join(fils[i], (void**)&retour) != 0){
            fprintf(stderr, "Error: pthread_join failed for thread %d\n", i);
            strerror(errno);
            exit(EXIT_FAILURE);
        }
        printf("Valeur retournee par le fils %lu = %d\n", fils[i], *retour);
        free(retour);
    }
    return 0;
}