#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

// Fonction qui retourne la date formatée sous forme de chaîne statique
char* date(void) {
    static char buffer[30];
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buffer, sizeof(buffer), "[%a %b %d %T %Y]", tm_info);
    return buffer;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <nb_processus> <nb_fils> <nb_petits_fils>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int NF = atoi(argv[1]), NB = atoi(argv[2]), NS = atoi(argv[3]);
    if (NF <= 0 || NB <= 0 || NS <= 0) {
        fprintf(stderr, "Les arguments doivent être des entiers positifs.\n");
        exit(EXIT_FAILURE);
    }

    int tube1[2], tube2[2];
    if (pipe(tube1) == -1 || pipe(tube2) == -1) {
        perror("Erreur de pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid;
    for (int i = 0; i < NF; i++) {
        switch (pid = fork()) {
            case -1:
                perror("Erreur de fork");
                exit(EXIT_FAILURE);

            case 0: { // Processus fils
                close(tube1[1]); // Ferme l'écriture sur le 1er pipe
                close(tube2[0]); // Ferme la lecture sur le 2ème pipe
                int nb_chocolat = 0; // Variable locale pour la lecture
                int boite_size = (i + 1) * 6;
                int nb_boites = 0;

                while (read(tube1[0], &nb_chocolat, sizeof(nb_chocolat)) > 0) {
                    if (nb_chocolat >= boite_size) {
                        nb_chocolat -= boite_size;
                        nb_boites++;
                        printf("Emballage %d : nouvelle boite de %d produite\n", i, boite_size);
                    }
                    if (write(tube2[1], &nb_chocolat, sizeof(nb_chocolat)) == -1) {
                        perror("Erreur d'écriture dans le tube");
                        exit(EXIT_FAILURE);
                    }
                }
                printf("Emballage %d : j'ai produit %d boites de %d chocolats\n", i, nb_boites, boite_size);
                close(tube1[0]); // Ferme le tube de lecture
                close(tube2[1]); // Ferme le tube d'écriture
                exit(EXIT_SUCCESS);
            }

            default:
                break; // Le père continue à créer les autres fils
        }
    }

    // Processus père
    close(tube1[0]); // Ferme la lecture sur le 1er pipe
    close(tube2[1]); // Ferme l'écriture sur le 2ème pipe
    int nb_chocolat = 0;

    for (int j = 0; j < NB; j++) {
        printf("Usine : je produis 60 chocolats %s\n", date());
        nb_chocolat += 60;

        if (write(tube1[1], &nb_chocolat, sizeof(nb_chocolat)) == -1) {
            perror("Erreur d'écriture dans le tube");
            exit(EXIT_FAILURE);
        }

        if (read(tube2[0], &nb_chocolat, sizeof(nb_chocolat)) == -1) {
            perror("Erreur de lecture dans le tube");
            exit(EXIT_FAILURE);
        }

        sleep(NS);
    }

    close(tube1[1]); // Ferme l'écriture du 1er pipe
    close(tube2[0]); // Ferme la lecture du 2ème pipe

    // Attente des fils
    for (int i = 0; i < NF; i++) {
        wait(NULL);
    }

    return EXIT_SUCCESS;
}
