#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "boucler.c"

void SigUSR_handler(int sig) {
    if (sig == SIGUSR1) 
        printf(">> SIGUSR1 reçu par %d\n", getpid());
    else if (sig == SIGUSR2) 
        printf(">> SIGUSR2 reçu par %d\n", getpid());
}

int main(int argc, char *argv[]){
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <NB> <NBMAX>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int NB = atoi(argv[1]);
    int nb_lu = 0;
    int nb = 0;
    int NBMAX = atoi(argv[2]);
    char c;
    pid_t pid, ppid;
    struct sigaction act, oldact; // Pour masquer puis demasquer
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
                pid = getpid();
                ppid = getppid();
                while (nb_lu != NBMAX) {
                    while (nb != NB) {
                        nb = scanf("%1c", &c);
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
                sigsuspend(&act.sa_mask);
                break;
        }
    }
    exit(EXIT_SUCCESS);
}