CC=gcc
CFLAGS=-std=c11 -Wpedantic -Wall -Wextra
LDLIBS=-lm
RM=rm -f
OBJ=file.o exo3.o
DIR=../TP3/file/
EXE=prog

# Pour produire le fichier exécutable
$(EXE):$(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(EXE) $(LDLIBS)

# Pour produire les fichiers objets
file.o:$(DIR)file.c $(DIR)file.h
	$(CC) $(CFLAGS) -c $(DIR)file.c -o file.o
exo3.o:exo3.c
	$(CC) $(CFLAGS) -c exo3.c -o exo3.o

# Pour effacer les objets et les exécutables
clean:
	$(RM) $(OBJ) $(EXE)