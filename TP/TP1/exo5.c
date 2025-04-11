#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include "boucler.c" // Pour la date

volatile sig_atomic_t cpt[2] = {0, 0};
volatile sig_atomic_t NS = 0;

void timeOut(int sig) {
    pid_t pid = getpid();
    for (int fils = 0; fils < 2; fils++) {
        printf("Pere (%d) - Fils %d : nombre de caracteres = %d\n", pid, fils, cpt[fils]);
    }
    alarm(NS); // On relance le timer
}

int main(int argc, char *argv[]){
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <NB> <NBL> <NS>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int NB = atoi(argv[1]);
    int NBL = atoi(argv[2]);
    NS = atoi(argv[3]);
    pid_t pid;
    int fils = -1;
    char c;
    int j = 0;
    int cpt2 = 0;
    int tube[2];
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

    if (pipe(tube) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < 2; i++){
        switch(fork()){
            case -1:
                perror("fork");
                exit(EXIT_FAILURE);
            case 0: // Process fils
                if (close(tube[0]) == -1){
                    perror("close");
                    exit(EXIT_FAILURE);
                } // Fermeture lecture
                pid = getpid();
                while (j < NBL){
                    c = getchar(); // Fonction bloquante 
                    if (c != '\n'){
                        cpt2++;
                        j++;
                        if (cpt2 == NB) {
                            printf("\tFils %d (%d) : %d caracteres de plus => %d\n", i, pid, NB, j);
                            if (write(tube[1], &i, sizeof(int)) == -1) {
                                perror("write");
                                exit(EXIT_FAILURE);
                            }
                            cpt2 = 0;
                        }
                    }
                }
                if (close(tube[1]) == -1){
                    perror("close");
                    exit(EXIT_FAILURE);
                } // Fermeture écriture
                printf("\tFils %d (%d) : Termine\n", i, pid);
                exit(EXIT_SUCCESS);
            default: // Process Pere
                // Code apres les forks
                break;
        }
    }
    if (close(tube[1]) == -1){
        perror("close");
        exit(EXIT_FAILURE);
    } // Fermeture écriture
    pid = getpid();
    alarm(NS); // On lance le timer
    while (read(tube[0], &fils, sizeof(int)) > 0) {
        sigsuspend(&oldmask);
        cpt[fils] += 1*NB;
    }
    for (int i = 0; i < 2; i++) { // attendre tous les fils
        wait(NULL); 
        printf("Pere (%d) - Fils %d : nombre de caracteres = %d\n", pid, fils, cpt[i]); // Affichage final
    }
    if (close(tube[0]) == -1){
        perror("close");
        exit(EXIT_FAILURE);
    } // Fermeture lecture
    printf("Pere (%d) - Je me termine (en dernier)\n", pid);
    exit(EXIT_SUCCESS);
    return 0;
}