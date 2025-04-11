#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "../TP1/boucler.c" // Pour la date

volatile sig_atomic_t NS = 0, ecrire = 0;

void timeOut(int sig) {
    ecrire = 1; // On peut écrire
    alarm(NS); // On relance le timer
}

int main(int argc, char *argv[]){
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <NS> <NE>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int NE = 0; // Init = atoi(argv[1]) mais plus tard pour que le fils ne connaisse pas le nombre
    int NS = atoi(argv[2]);
    int tube[2];
    char *message = malloc(100 * sizeof(char)); // Init message plus tard pour que le fils ne le connaisse pas non plus
    char *buffer = malloc(100 * sizeof(char));
    int i = 0;
    sigset_t mask, oldmask; // Pour changer de masque de signal
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    struct sigaction act;
    act.sa_handler = timeOut;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    if (sigaction(SIGALRM, &act, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    if (message == NULL || buffer == NULL) {
        perror("malloc");
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
        while(read(tube[0], &buffer, sizeof(message)) > 0) {
            printf("\tFils - Reçu de mon pere :  %s\n", buffer);
        }
        close(tube[0]); // Ferme la lecture
        printf("\tFils - Je me termine\n");
        exit(EXIT_SUCCESS);
    default:
        close(tube[0]); // Ferme la lecture
        NE = atoi(argv[1]);
        sigprocmask(SIG_UNBLOCK, &mask, NULL); // On débloque le signal
        alarm(NS); // On lance le timer
        while (i < NE) {
            if (ecrire == 1){
                sprintf(message, "Je suis ton pere... %d envoye a %s", i, date());
                printf("Pere - Message %d envoye\n", i);
                if (write(tube[1], &message, sizeof(message)) == -1) {
                    perror("write");
                    exit(EXIT_FAILURE);
                }
                ecrire = 0; // On ne peut plus écrire
            }
            //printf("Pere - Je travaille entre les envois\n");
        }
        close(tube[1]); // Ferme l'écriture
        wait(NULL); // Attend la fin du fils
        printf("\tPere - Je me termine en dernier\n");
        exit(EXIT_SUCCESS);
    }
    free(message);
    free(buffer);
    return 0;
}