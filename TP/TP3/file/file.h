#ifndef __FILE_H__
#define __FILE_H__

#define MAX_CHAR 1000
#define MAX_MESSAGE 100
typedef struct {
    char *buffer[MAX_CHAR];
    int taille_message[MAX_MESSAGE];
    int nbre_message;
    int tete;
    int queue;
} buffer_t;

extern char* defiler_message(buffer_t* tab);
extern void enfiler_message(buffer_t* tab, char* message);
extern void afficher_buffer(buffer_t* tab);
extern void etat_buffer(buffer_t* tab);

#endif