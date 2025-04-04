execlp("./exo0", "./exo0", argv[1], argv[2], NULL);
    perror("Erreur execlp");
    exit(EXIT_FAILURE);