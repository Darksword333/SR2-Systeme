#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "boucler.c"

#define NB_FILS 2
int compteur_fils[NB_FILS] = {0, 0};

void SigUSR_handler(int sig) {
    if (sig == SIGUSR1) {
        printf(">> SIGUSR1 reçu par %d\n", getpid());
        compteur_fils[0]++;
    }
    else if (sig == SIGUSR2) {
        printf(">> SIGUSR2 reçu par %d\n", getpid());
        compteur_fils[1]++;
    }
}

int main(int argc, char *argv[]){
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <NB> <NBMAX>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int NB = atoi(argv[1]);
    int nb_lu = 0;
    int NBMAX = atoi(argv[2]);
    char c;
    pid_t ppid;

    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGUSR2);

    struct sigaction act; // Pour masquer puis demasquer
    act.sa_handler = SigUSR_handler;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    if (sigaction(SIGUSR1, &act, NULL) == -1 || sigaction(SIGUSR2, &act, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < 2; i++) {
        switch (fork()) {
            case -1:
                perror("fork");
                exit(EXIT_FAILURE);
            case 0: // Processus Fils
                ppid = getppid();
                while (nb_lu < NBMAX) {
                    for (int j = 0; j < NB; j++) {
                        if (scanf("%c", &c) == EOF) {
                            exit(EXIT_SUCCESS);
                        }
                        
                    }
                    nb_lu+=NB;
                    printf("%d", nb_lu);
                    if (i == 0)
                        kill(ppid, SIGUSR1);
                    else 
                        kill(ppid, SIGUSR2);
                }
                printf(">> %d termine\n", getpid());
                exit(EXIT_SUCCESS);
            default: // Processus Père
                if (sigsuspend(&mask) == -1) {
                    perror("sigsuspend");
                    exit(EXIT_FAILURE);
                }
                break;
        }
    }
    exit(EXIT_SUCCESS);
}