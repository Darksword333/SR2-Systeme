#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

void* producteur(void* arg){

}

void* consommateur(void* arg){

}

int main(int argc, char *argv[]){
    if (argc != 3){
        fprintf(stderr, "Usage: %s <NP> <NC> <nbreP> <nbreC> <N>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int NP = atoi(argv[0]);
    int NC = atoi(argv[1]);
    int nbreP = atoi(argv[2]);
    int nbreC = atoi(argv[3]);
    int N = atoi(argv[4]);
    if (NP <= 0 || NC <= 0 || nbreP <= 0 || nbreC <= 0 || N <= 0){
        fprintf(stderr, "Error: NP, NC, nbreP, nbreC and N must be positive integers.\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}