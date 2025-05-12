#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define REFERENCE "AB"
#define QUANTITY 5


int main(int argc, char *argv[]){
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number_of_fournisseur>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int NF = atoi(argv[1]);
    if (NF <= 0) {
        fprintf(stderr, "Error: Number of fournisseurs must be a positive integer.\n");
        exit(EXIT_FAILURE);
    }
    int tube1[2], tube2[2], winner[2];
    char ref[2] = REFERENCE;
    int quantity = QUANTITY;
    int price = -1, old_price = -1;
    int best_fournisseur;
    for (int i=0; i < NF; i++){
        if (pipe(tube1) == -1 || pipe(tube2) == -1 || pipe(winner) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
        switch (fork()){
            case -1:
                fprintf(stderr, "Erreur fork\n");
                exit(EXIT_FAILURE);
            case 0: // Code Fournisseur
                close(tube1[1]); // Fermé en Ecriture
                close(winner[1]); // Fermé en Ecriture
                close(tube2[0]); // Fermé en Lecture
                srand(getpid());
                price = rand();
                if (read(tube1[0], &ref, sizeof(ref)) == -1){
                    fprintf(stderr, "Erreur read\n");
                    exit(EXIT_FAILURE);
                }
                if (read(tube1[0], &quantity, sizeof(quantity)) == -1){
                    fprintf(stderr, "Erreur read\n");
                    exit(EXIT_FAILURE);
                }
                close(tube1[0]); // Fermé en Lecture
                printf("Fournisseur %d : j'ai recu la demande (%s, %d)\n", i, ref, quantity);
                printf("Fournisseur %d : montant = %d\n", i, price);
                if (write(tube2[1], &price, sizeof(price)) == -1){
                    fprintf(stderr, "Erreur write\n");
                    exit(EXIT_FAILURE);
                }
                close(tube2[1]);
                if (read(winner[0], &best_fournisseur, sizeof(best_fournisseur)) == -1){
                    fprintf(stderr, "Erreur write\n");
                    exit(EXIT_FAILURE);
                }
                close(winner[0]);
                if (best_fournisseur == i)
                    printf("Fournisseur %d : j'ai obtenu le marche pour %d\n", i, price);
                else 
                    printf("Fournisseur %d : j'ai perdu le marche avec %d\n", i, price);
                exit(EXIT_SUCCESS);
            default: // Code Client
                close(tube1[0]); // Fermé en Lecture
                close(winner[0]); // Fermé en Lecture
                close(tube2[1]); // Fermé en Ecriture
                printf("Client : J'envoie demande au fournisseur %d\n", i);
                if (write(tube1[1], &ref, sizeof(ref)) == -1){
                    fprintf(stderr, "Erreur write\n");
                    exit(EXIT_FAILURE);
                }
                if (write(tube1[1], &quantity, sizeof(quantity)) == -1){
                    fprintf(stderr, "Erreur write\n");
                    exit(EXIT_FAILURE);
                }
                close(tube1[1]); // Fermé en Ecriture
                if (read(tube2[0], &price, sizeof(price)) == -1){
                    fprintf(stderr, "Erreur read\n");
                }
                close(tube2[0]); // Fermé en Lecture
                printf("Client : Je recois %d du fournisseur %d\n", price, i);
                if (old_price > price || old_price == -1){
                    best_fournisseur = i;
                    old_price = price;
                }
                break;
        }
    }
    printf("Client : Je choisis le fournisseur %d pour un montant de %d\n", best_fournisseur, old_price);
    if (write(winner[1], &best_fournisseur, sizeof(best_fournisseur)) == -1){
        fprintf(stderr, "Erreur write\n");
        exit(EXIT_FAILURE);
    }
    close(winner[1]);
    exit(EXIT_SUCCESS);
}