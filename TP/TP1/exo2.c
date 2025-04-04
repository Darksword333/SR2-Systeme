#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "boucler.c"

void SigINT_handler(int sig) {
    printf(">> SIGINT reçu par %d\n", getpid());
}

int main(int argc, char *argv[]){
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <NS> <NB>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int NS = atoi(argv[1]);
    int NB = atoi(argv[2]);
    struct sigaction act;
    act.sa_handler = SigINT_handler;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    if (sigaction(SIGINT, &act, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    switch (fork()) {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        case 0: // Processus Fils
            boucler(NS, NB);
            exit(EXIT_SUCCESS);
        default: // Processus Père
            wait(NULL); // Attendre la fin du fils
            break;
    }
    exit(EXIT_SUCCESS);
}