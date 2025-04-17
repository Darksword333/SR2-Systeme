#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
    if (argc != 3){
        fprintf(stderr, "Usage: %s <NA> <NF>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int NA = atoi(argv[1]);
    int NF = atoi(argv[2]);
    if (NA <= 0 || NF <= 0){
        fprintf(stderr, "Error: NA and NF must be positive integers.\n");
        exit(EXIT_FAILURE);
    }
    int delai = 0, retour = 0;
    pid_t pid;
    for (int i = 0; i < NA; i++){
        switch(pid = fork()){
            case -1:
                perror("fork");
                exit(EXIT_FAILURE);
            case 0: // Code Fils
                pid = getpid();
                srand(pid);
                delai = rand() % 101;                
                for (int j = 0; j < NF; j++){
                    delai += 1;
                    printf("Activite rang %d : identifiant = %d (delai = %d)\n", i, pid, delai);
                    usleep(delai);
                }
                exit(i);
            default: // Code Pere
                wait(&retour);
                printf("Valeur retournee par le fils %d = %d\n", pid, WEXITSTATUS(retour));
                break;
        }
    }
    return 0;
}