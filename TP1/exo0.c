#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include "boucler.c"

int main(int argc, char *argv[]){
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <NS> <NB>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int NS = atoi(argv[1]);
    int NB = atoi(argv[2]);
    boucler(NS, NB);
    exit(EXIT_SUCCESS);
}