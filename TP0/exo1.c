#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <number> <number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int NF = atoi(argv[1]);
    int NM = atoi(argv[2]);
    pid_t pid = 0;
    int status = -1;
    if (NF <= 0 || NM <= 0) {
        fprintf(stderr, "NF et NM doivent être des entiers positifs\n");
        exit(EXIT_FAILURE);
    }
    for (int i=0; i < NF; i++){
        switch(pid = fork()){
            case -1:
                perror("fork");
                exit(EXIT_FAILURE);
            case 0:
            // Processus fils
                pid = getpid(); // Un unique appel système
                for (int j=0; j < NM; j++)
                    printf("Activite rang %d : identifiant = %d\n", i, pid);
                exit(i);
            default:
            // Processus pere
                wait(&status); // Retourne le pid du fils pid = wait(NULL);
                printf("Valeur retournee par le fils %d = %d\n", pid, WEXITSTATUS(status));
                break;
        }
    }
    exit(EXIT_SUCCESS);
}
