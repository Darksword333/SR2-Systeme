#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include "file.h"

char* defiler_message(buffer_t* tab) {
    /*Défile le message pour passer au message suivant. Retourne le message défilé*/
    if (tab->tete == tab->queue) {
        fprintf(stderr, "Buffer is empty\n");
        exit(EXIT_FAILURE);
    }
    char* message = tab->buffer[tab->tete];
    tab->tete += tab->taille_message[tab->tete] % MAX_CHAR;
    tab->nbre_message --;
    return message;
}

void enfiler_message(buffer_t* tab, char* message) {
    /*Enfile le message*/
    tab->buffer[tab->queue] = message;
    tab->taille_message[tab->queue] = strlen(message);
    tab->queue = (tab->queue + 1* strlen(message)) % MAX_CHAR;
    tab->nbre_message ++;
    if (tab->queue == tab->tete) {
        fprintf(stderr, "Buffer is full\n");
        exit(EXIT_FAILURE);
    }
}

void afficher_buffer(buffer_t* tab) {
    /*Affiche les messages*/
    if (tab->nbre_message == 0){
        printf("Buffer is empty\n");
    }
    else {
        for (int i = 0; i < tab->nbre_message; i++){
            printf("%s\n", tab->buffer[(tab->tete + i) % MAX_CHAR]);
        }
    }
}

void etat_buffer(buffer_t* tab) {
    /*Affiche l'état du buffer*/
    printf("Buffer: \n");
    printf("Tete: %d, Queue: %d, Nbre_message: %d\n", tab->tete, tab->queue, tab->nbre_message);
}