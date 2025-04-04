#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "boucler.c"


void SigUSR1_handler(int const sig) {
    printf(">> SIGUSR1 reçu par %d\n", getpid());
}

int main(int argc, char *argv[]){
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <NS> <NB>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int NS = atoi(argv[1]);
    int NB = atoi(argv[2]);
    struct sigaction act;
    act.sa_handler = SigUSR1_handler;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    if (sigaction(SIGUSR1, &act, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    boucler(NS, NB);
    exit(EXIT_SUCCESS);
}