#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

const int PARAMETROS = 2;
const int MINIMO = 2;

void verificar_parametros(int argc, char *argv[]) {
    if (argc != PARAMETROS) {
        printf("Error: Cantidad de parámetros incorrecta.\n");
        exit(1);  
    }

    int num = atoi(argv[1]);
    if (num < MINIMO) {
        printf("Error: el número %d es inválido.\n", num);
        exit(1);
    }
}    

void generar_primos(int fds[], int n) {
    for (int i = MINIMO; i <= n; i++) {
        write(fds[1], &i, sizeof(i));
    }
    close(fds[1]);
}

void filtrar_numeros(int fds[]) {
    int primo;

    if (read(fds[0], &primo, sizeof(primo)) <= 0) {
        close(fds[0]);
        exit(0);
    }

    printf("primo %d\n", primo);

    int fds_der[2];
    pipe(fds_der);

    int pid = fork();
    
    if (pid == 0) {
        // Hijo
        close(fds[0]);
        close(fds_der[1]);
        filtrar_numeros(fds_der);
        exit(0);
    } else {
        int num;
        close(fds_der[0]);

        while (read(fds[0], &num, sizeof(num)) > 0) {
            if (num % primo != 0) {
                write(fds_der[1], &num, sizeof(num));
            }
        }

        close(fds[0]);
        close(fds_der[1]);
        wait(NULL);
    }
    
    exit(0);
}

int main(int argc, char *argv[]) {
    verificar_parametros(argc, argv);

    int fds[2];
    pipe(fds);
    int i = fork();
    int n = atoi(argv[1]);

    if (i == 0) {
        // Hijo
        close(fds[1]);
        filtrar_numeros(fds);
    } else {
        // Padre
        close(fds[0]);
        generar_primos(fds, n);
        close(fds[1]);
        wait(NULL);
    }

    return 0;
}
