#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#ifndef NARGS
#define NARGS 4
#endif
#define ARGUMENTOS 2

void verificar_entrada(int argc, char *argv[]) {
    if (argc < ARGUMENTOS) {
        printf("Uso: %s <comando>\n", argv[0]);
        exit(1);
    }
}

void ejecutar_comando(char *args[]) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("Error en fork");
        exit(1);
    }

    if (pid == 0) {
        execvp(args[0], args);
        perror("Error en execvp");
        exit(1);
    } else {
        wait(NULL);
    }
}

int main(int argc, char *argv[]) {
    verificar_entrada(argc, argv);

    char *args[NARGS + 2];
    args[0] = argv[1]; // comando
    args[NARGS] = NULL;

    int index = 1;
    size_t len = 0;
    char *linea = NULL;

    while (getline(&linea, &len, stdin) != -1) {
        linea[strcspn(linea, "\n")] = '\0';
        args[index] = strdup(linea);
        index++;

        if (index == NARGS + 1) {
            args[index] = NULL;
            ejecutar_comando(args);
            index = 1;
        }
    }

    if (index > 1) {
        args[index] = NULL;
        ejecutar_comando(args);
    }
    
    free(linea);
    return 0;
}
