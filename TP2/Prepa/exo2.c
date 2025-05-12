#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

int main(int argc, char *argv[]){
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <NF> <NBV> <NBVM>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int NF = atoi(argv[1]);
    int NBV = atoi(argv[2]);
    int NBVM = atoi(argv[3]);
    int cpt = 0;
    int j = 0;
    int fils = 0;
    char c;
    pid_t pid;
    int tube[2];
    if (pipe(tube) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < NF; i++){
        switch(fork()){
            case -1:
                perror("fork");
                exit(EXIT_FAILURE);
            case 0: // Process fils
                close(tube[0]); // Fermeture lecture
                pid = getpid();
                while (j < NBVM){
                    c = getchar(); // Fonction bloquante 
                    if (c != '\n'){
                            cpt++;
                            j++;
                            printf("\tCapteur %d (%d) : 1 vehicule de plus => %d\n", i, pid, j);
                        if (cpt == NBV) {
                            if (write(tube[1], &i, sizeof(int)) == -1) {
                                perror("write");
                                exit(EXIT_FAILURE);
                            }
                            cpt = 0;
                        }
                    }
                }
                close(tube[1]); // Fermeture écriture
                printf("\tCapteur %d (%d) : Termine\n", i, pid);
                exit(EXIT_SUCCESS);
            default: // Process Pere
                // Apres la creation du fils
                break;
        }
    }
    close(tube[1]); // Fermeture écriture
    pid = getpid();
    while (read(tube[0], &fils, sizeof(int)) > 0) {
        cpt += 1*NBV;
        printf("Pere (%d) - Capteur (%d) : nombre de vehicules = %d\n", pid, fils, cpt);
    }
    close(tube[0]); // Fermeture lecture
    for (int i = 0; i < NF; i++) { // attendre tous les fils
        wait(NULL); 
    }
    printf("Pere (%d) - Je me termine en dernier\n", pid);
    exit(EXIT_SUCCESS);
    return 0;
}