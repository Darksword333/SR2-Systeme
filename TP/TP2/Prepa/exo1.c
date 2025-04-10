#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

int main(int argc, char *argv[]){
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <NE>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int NE = 0; // Init = atoi(argv[1]) mais plus tard pour que le fils ne connaisse pas le nombre
    int tube[2];
    char *message = malloc(100 * sizeof(char));
    char *buffer = malloc(100 * sizeof(char));
    if (message == NULL || buffer == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    if (pipe(tube) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    switch (fork()) {

    case -1: // Error
        perror("fork");
        exit(EXIT_FAILURE);
    case 0: // Fils
        close(tube[1]); // Ferme l'écriture
        while(read(tube[0], &buffer, sizeof(message)) > 0) {
            printf("\tFils - Reçu de mon pere :  %s\n", buffer);
        }
        close(tube[0]); // Ferme la lecture
        printf("\tFils - Je me termine\n");
        exit(EXIT_SUCCESS);
    default:
        close(tube[0]); // Ferme la lecture
        NE = atoi(argv[1]);
        message = "Je suis ton pere...";
        for (int i = 0; i < NE; i++) {
            printf("Pere - Message %d envoye\n", i);
            if (write(tube[1], &message, sizeof(message)) == -1) {
                perror("write");
                exit(EXIT_FAILURE);
            }
        }
        close(tube[1]); // Ferme l'écriture
        wait(NULL); // Attend la fin du fils
        printf("\tPere - Je me termine en dernier\n");
        exit(EXIT_SUCCESS);
    }
    free(message);
    free(buffer);
    return 0;
}