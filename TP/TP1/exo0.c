#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

char* date(void) {
    static char buffer[30];
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buffer, sizeof(buffer), "%a %b %d %T %Y", tm_info);
    return buffer;
}

int main(int argc, char *argv[]){
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <NS> <NB>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    pid_t pid = getpid();
    int NS = atoi(argv[1]);
    int NB = atoi(argv[2]);
    while(1){
        for (int i = 0; i < NB; i++) {
            printf("Mon numero est %d, il est %s\n", pid, date());
        }
        sleep(NS);
    }
    exit(EXIT_SUCCESS);
}