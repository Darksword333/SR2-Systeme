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
        printf(">> SIGUSR1 envoyé par fils [%d]\n", getpid());
        compteur_fils[0]++;
    }
    else if (sig == SIGUSR2) {
        printf(">> SIGUSR2 envoyé par fils [%d]\n", getpid());
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
    char buffer[NB];
    pid_t ppid, pid;

    sigset_t mask, oldmask; // Pour changer de masque de signal
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGUSR2);
    sigprocmask(SIG_BLOCK, &mask, &oldmask);

    struct sigaction act;
    act.sa_handler = SigUSR_handler;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    if (sigaction(SIGUSR1, &act, NULL) == -1 || sigaction(SIGUSR2, &act, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < 2; i++) {
        switch (pid = fork()) {
            case -1:
                perror("fork");
                exit(EXIT_FAILURE);
            case 0: // Processus Fils
                ppid = getppid();
                pid = getpid();
                while (nb_lu != NBMAX) {
                    for (int j = 0; j < NB; j++)
                        scanf(" %1c", &buffer[j]); // Lis 1 char et ne lis pas les "\n", "\t" " "
                    nb_lu+=NB;
                    printf("===Fils [%d] : J'ai lu %s, j'ai lu au total %d\n", pid, buffer, nb_lu);
                    kill(ppid, (i == 0) ? SIGUSR1 : SIGUSR2); //If premier fils SIGUSR1, else SIGUSR2
                }
                printf(">> Fils [%d] termine\n", pid);
                exit(EXIT_SUCCESS);
            default: // Processus Père
                while (compteur_fils[i]*NB < NBMAX) { //Le handler rajoute que un par un
                    if (sigsuspend(&mask) != -1) {
                        perror("Erreur sigsuspend\n");
                        exit(EXIT_FAILURE);
                    }
                    printf("===Père : Mon fils [%d] a lu %d\n", pid, compteur_fils[i]*NB);
                }
                wait(NULL); // Attends ses fils
                break;
        }
    }
    exit(EXIT_SUCCESS);
}