#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

int *tab; // Tableau global

void* producteur(void* arg){
    int num = ((int*)arg)[0];
    int nbre = ((int*)arg)[1];
    pthread_t id = pthread_self();
    for (int i = 0; i < nbre; i++){
        tab[i] = (num*10) + i+1;
        printf("Prod %d (%ld) : Message depose = Bonjour %d de prod %d\n", num, id, i+1, num);
    }
    pthread_exit(EXIT_SUCCESS); 
}

void* consommateur(void* arg){
    int num = ((int*)arg)[0];
    int nbre = ((int*)arg)[1];
    int msg = 0, num_prod = 0;
    pthread_t id = pthread_self();
    for (int i = 0; i < nbre; i++){
        msg = tab[i]%10;
        num_prod = tab[i]/10;
        tab[i] = 0; // Simule la consommation du message
        if (msg == 0){
            printf("\tConso %d (%ld) : Message retire = xxxxx\n", num, id);
        }
        else {
            printf("\tConso %d (%ld) : Message retire = Bonjour %d de prod %d\n", num, id, msg, num_prod);
        }
    }
    pthread_exit(EXIT_SUCCESS); 
}

int main(int argc, char *argv[]){
    if (argc != 6){
        fprintf(stderr, "Usage: %s <NP> <NC> <nbreP> <nbreC> <N>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int NP = atoi(argv[1]);
    int NC = atoi(argv[2]);
    int nbreP = atoi(argv[3]);
    int nbreC = atoi(argv[4]);
    int N = atoi(argv[5]);
    tab = malloc(N * sizeof(int));
    if (NP <= 0 || NC <= 0 || nbreP <= 0 || nbreC <= 0 || N <= 0){
        fprintf(stderr, "Error: NP, NC, nbreP, nbreC and N must be positive integers.\n");
        exit(EXIT_FAILURE);
    }
    pthread_t prods[NP];
    pthread_t consos[NC];
    typedef struct {
        int num;
        int nbre;
    } args_t;
    args_t args_np[NP], args_nc[NC];
    for (int i = 0; i < NP; i++){ // Initialisation des arguments pour les threads producteurs
        args_np[i].num = i;
        args_np[i].nbre = nbreP;
    }
    for (int i = 0; i < NC; i++){ // Initialisation des arguments pour les threads consommateurs
        args_nc[i].num = i;
        args_nc[i].nbre = nbreC;
    }
    for (int i = 0; i < NP; i++){ // Création des threads producteurs
        if (pthread_create(&prods[i], NULL, producteur, (void*)&args_np[i]) != 0){
            fprintf(stderr, "Error: pthread_create failed for producer %d\n", i);
            exit(EXIT_FAILURE);
        }
    }
    for (int i = 0; i < NC; i++){ // Création des threads consommateurs
        if (pthread_create(&consos[i], NULL, consommateur, (void*)&args_nc[i]) != 0){
            fprintf(stderr, "Error: pthread_create failed for consumer %d\n", i);
            exit(EXIT_FAILURE);
        }
    }
    for (int i = 0; i < NP; i++){ // Attente de la fin des threads producteurs
        if (pthread_join(prods[i], NULL) != 0){
            fprintf(stderr, "Error: pthread_join failed for producer %d\n", i);
            exit(EXIT_FAILURE);
        }
    }
    for (int i = 0; i < NC; i++){ // Attente de la fin des threads consommateurs
        if (pthread_join(consos[i], NULL) != 0){
            fprintf(stderr, "Error: pthread_join failed for consumer %d\n", i);
            exit(EXIT_FAILURE);
        }
    }
    printf("Fin de l'exécution du main\n");
    free(tab);
    return EXIT_SUCCESS;
}