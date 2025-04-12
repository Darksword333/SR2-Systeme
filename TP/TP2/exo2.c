#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include "../TP1/boucler.c" // Pour la date

volatile sig_atomic_t ecrire = 0;
int NS = 0; 
bool stop = false; // Variable de controle pour arreter le fils

void sig_handler(int sig) {
    if (sig == SIGALRM){ // Time Out
        ecrire = 1; // On peut écrire
        alarm(NS); // On relance le timer
    }
    else if (sig == SIGTERM) { // On arrete le fils
        stop = true; // On arrete le fils
    }
}

int main(int argc, char *argv[]){
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <NF> <NBL> <NS>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int NF = atoi(argv[1]);
    int NBL = atoi(argv[2]);
    NS = atoi(argv[3]);
    int cpt[NF]; // Compteur de vehicules
    int j = 0;
    pid_t pid_fils;
    stop = false;
    int fils = 0;
    char c;
    pid_t pid;

    struct sigaction act;
    act.sa_handler = sig_handler;
    act.sa_flags = 0;
    sigfillset(&act.sa_mask);
    sigdelset(&act.sa_mask, SIGALRM);
    sigdelset(&act.sa_mask, SIGINT);
    sigaction(SIGALRM, &act, NULL);
    sigaction(SIGTERM, &act, NULL);

    sigset_t block_sigalrm; // Pour le père
    sigemptyset(&block_sigalrm);
    sigaddset(&block_sigalrm, SIGALRM);

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
                alarm(NS); // On lance le timer
                while (!stop){
                    c = getchar(); // Fonction bloquante 
                    if (c != '\n'){
                            cpt[i]++;
                            printf("\tCapteur %d (%d) : 1 vehicule de plus => %d\n", i, pid, j);
                        if (ecrire == 1) {
                            if (write(tube[1], &i, sizeof(int)) == -1 || write(tube[1], &pid, sizeof(pid_t)) == -1) {
                                perror("write");
                                exit(EXIT_FAILURE);
                            }
                            cpt[i] = 0;
                            ecrire = 0; // On ne peut plus écrire
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
    sigprocmask(SIG_BLOCK, &block_sigalrm, NULL);
    while (read(tube[0], &fils, sizeof(int)) > 0 || read(tube[0], &pid_fils, sizeof(pid_t)) > 0) {
        cpt[fils] += 1;
        printf("Pere (%d) - Capteur (%d) : nombre de vehicules = %d\n", pid, fils, cpt[fils]);
        if (cpt[fils] >= NBL){
            kill(pid_fils, SIGTERM); // On tue le fils
            printf("Pere (%d) - Signale au capteur %d (%d) sa fin\n", pid, fils, pid_fils);
        }
    }
    close(tube[0]); // Fermeture lecture
    for (int i = 0; i < NF; i++) { // attendre tous les fils
        wait(NULL); 
    }
    printf("Pere (%d) - Je me termine en dernier\n", pid);
    exit(EXIT_SUCCESS);
    return 0;
}