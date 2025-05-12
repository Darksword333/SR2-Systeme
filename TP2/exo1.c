#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "../TP1/boucler.c" // Pour la date

volatile sig_atomic_t ecrire = 0;
int NS = 0; 
#define TAILLE 100

void timeOut(int sig) {
    if (sig == SIGALRM){
        ecrire = 1; // On peut écrire
        alarm(NS); // On relance le timer
    }
}

int main(int argc, char *argv[]){
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <NE> <NS>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int NE = 0; // Init = atoi(argv[1]) mais plus tard pour que le fils ne connaisse pas le nombre
    NS = atoi(argv[2]);
    int tube[2];
    // Flemme de gerer des mallocs et free
    char message[TAILLE]; // Init message plus tard pour que le fils ne le connaisse pas non plus
    char buffer[TAILLE];
    int i = 0;

    struct sigaction act;
    act.sa_handler = timeOut;
    act.sa_flags = 0;
    sigfillset(&act.sa_mask);
    sigdelset(&act.sa_mask, SIGALRM);
    sigdelset(&act.sa_mask, SIGINT);
    sigaction(SIGALRM, &act, NULL);

    sigset_t block_sigalrm; // Pour le fils
    sigemptyset(&block_sigalrm);
    sigaddset(&block_sigalrm, SIGALRM);

    if (sigaction(SIGALRM, &act, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    if (pipe(tube) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    switch (fork()) {

    case -1: // Error
        perror("fork");
        exit(EXIT_FAILURE);
    case 0: // Fils
        close(tube[1]); // Ferme l'écriture
        sigprocmask(SIG_BLOCK, &block_sigalrm, NULL);
        while(read(tube[0], buffer, sizeof(message)) > 0) {
            printf("Fils - Reçu de mon pere :  %s\n", buffer);
        }
        close(tube[0]); // Ferme la lecture
        printf("Fils - Je me termine a %s\n", date());
        exit(EXIT_SUCCESS);
    default:
        close(tube[0]); // Ferme la lecture
        NE = atoi(argv[1]);
        alarm(NS); // On lance le timer
        while (i < NE) {
            if (ecrire == 1){
                sprintf(message, "Je suis ton pere... %d envoye a %s", i, date());
                printf("\tPere - Message %d envoye\n", i);
                if (write(tube[1], message, sizeof(message)) == -1) {
                    perror("write");
                    exit(EXIT_FAILURE);
                }
                i++;
                ecrire = 0; // On ne peut plus écrire
            }
            //printf("\tPere - Je travaille entre les envois\n"); // En commentaire parce que sinon envoie trop de message
        }
        close(tube[1]); // Ferme l'écriture
        wait(NULL); // Attend la fin du fils
        printf("\tPere - Je me termine en dernier a %s\n", date());
        exit(EXIT_SUCCESS);
    }
    return 0;
}