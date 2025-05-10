#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include "../TP3/file/file.h"

buffer_t tab[MAX_CHAR]; // Initialisation globale du tableau
typedef struct {
        int num;
        int nbre;
        sem_t *mutex_conso;
        sem_t *mutex_prod;
    } args_t;

void* producteur(void* arg){
    args_t *args = (args_t*)arg;
    int num = args->num;
    int nbre = args->nbre;
    sem_t *tab_mutex_conso = args->mutex_conso;
    sem_t *tab_mutex_prod = args->mutex_prod;
    char *message = malloc(MAX_CHAR);
    pthread_t id = pthread_self();
    for (int i = 1; i <= nbre; i++){
        sem_wait(&tab_mutex_prod[num]);
        sprintf(message, "Bonjour %d de prod %d\n", i, num); // Création d'un message
        enfiler_message(tab, message); // Production d'un message
        printf("Prod %d (%ld) : Message depose = %s\n", num, id, message);
        sem_post(&tab_mutex_conso[num]);
    }
    pthread_exit(message); 
}

void* consommateur(void* arg){
    args_t *args = (args_t*)arg;
    int num = args->num;
    int nbre = args->nbre;
    sem_t *tab_mutex_conso = args->mutex_conso;
    sem_t *tab_mutex_prod = args->mutex_prod;
    char * message = malloc(MAX_CHAR);
    pthread_t id = pthread_self();
    for (int i = 0; i < nbre; i++){
        sem_wait(&tab_mutex_conso[num]);
        message = defiler_message(tab); // Consommation d'un message
        printf("\tConso %d (%ld) : Message retire = %s\n", num, id, message);
        sem_post(&tab_mutex_prod[num]);
    }
    sem_post(&tab_mutex_prod[num+1]);
    pthread_exit(message); 
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
    tab->nbre_message = 0;
    tab->tete = 0;
    tab->queue = 0;
    tab->taille_totale = 0;
    if (NP <= 0 || NC <= 0 || nbreP <= 0 || nbreC <= 0 || N <= 0){
        fprintf(stderr, "Error: NP, NC, nbreP, nbreC and N must be positive integers.\n");
        exit(EXIT_FAILURE);
    }
    void *message;
    pthread_t prods[NP];
    pthread_t consos[NC];
    sem_t tab_mutex_prod[NP+1], tab_mutex_conso[NC];
    sem_init(&tab_mutex_prod[0], 0, 1); // Initialisation du sémaphore
    for (int i = 1; i < NP+1; i++){
        sem_init(&tab_mutex_prod[i], 0, 0); // Initialisation des sémaphores
    }
    for (int i = 0; i < NC; i++){
        sem_init(&tab_mutex_conso[i], 0, 0); // Initialisation des sémaphores
    }
    args_t args_np[NP], args_nc[NC];
    for (int i = 0; i < NP; i++){ // Initialisation des arguments pour les threads producteurs
        args_np[i].num = i;
        args_np[i].nbre = nbreP;
        args_np[i].mutex_conso = tab_mutex_conso;
        args_np[i].mutex_prod = tab_mutex_prod;
    }
    for (int i = 0; i < NC; i++){ // Initialisation des arguments pour les threads consommateurs
        args_nc[i].num = i;
        args_nc[i].nbre = nbreC; 
        args_nc[i].mutex_conso = tab_mutex_conso;
        args_nc[i].mutex_prod = tab_mutex_prod;
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
        if (pthread_join(prods[i], &message) != 0){
            fprintf(stderr, "Error: pthread_join failed for producer %d\n", i);
            exit(EXIT_FAILURE);
        }
        sem_destroy(&tab_mutex_prod[i]); // Destruction des sémaphores
    }
    for (int i = 0; i < NC; i++){ // Attente de la fin des threads consommateurs
        if (pthread_join(consos[i], &message) != 0){
            fprintf(stderr, "Error: pthread_join failed for consumer %d\n", i);
            exit(EXIT_FAILURE);
        }
        sem_destroy(&tab_mutex_conso[i]); // Destruction des sémaphores
    }
    etat_buffer(tab); // Affiche l'état du buffer
    afficher_buffer(tab); // Affichage des messages restants dans le buffer si il en reste
    free(message); // Libération de la mémoire allouée pour le message
    printf("Fin de l'exécution du main\n");
    return EXIT_SUCCESS;
}