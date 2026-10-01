#ifndef PAPA_BASE_H
#define PAPA_BASE_H

//Para parámetros
#define _POSIX_C_SOURCE 200809L


#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define MAXPARTICIPANTES 10
//Así se identifica como invasor
#define EMISOR_INVASOR   -2   
#define NO_REGISTRADO    -1    //aun no se registro un emisor

    //Parámetros para meterlo en el constructor o generarlo random
    struct param {
        int n; //#Miembros de la ronda
        int v; //Lugar inicial
        int d; //direccion del buzon
    // 1 = clock: i. i+1, ... n-1, 0
    // 0 = unclock: i. i-1, ... 0, n-1 
    };
 
    int randomNum( int min, int max );

    void iniciarParametros(struct param *parametros);
    void iniciaJuego(int argc, char **argv);

#endif // PAPA_BASE_H