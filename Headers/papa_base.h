#ifndef PAPA_BASE_H
#define PAPA_BASE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <fcntl.h>
#include <signal.h>

#define MAXPARTICIPANTES 10
#define EMISOR_INVASOR   -2   /* nunca coincide con un id real (0..n-1) ni con -1 (fin de juego) */
#define NO_REGISTRADO    -1    /* aun no se registro el emisor valido para este participante */

/**
  *  Estructura para el paso de mensajes entre procesos
 **/
    struct RondaPapa {
      long mtype;
      int mensaje;
      int emisor;
      int receptor;
       // otros elementos a definir por el estudiante
    };

    struct procesos{
        int id;
        bool activo;
    };

    //Tentativo, idea meterlo en el constructor o generarlo random
    struct param {
        int n; //#Miembros de la ronda
        int v; //Lugar inicial
        int d; //direccion del buzon
    /*
    clock: i. i+1, ... n-1, 0
    unclock: i. i-1, ... 0, n-1 
    */
    };

    struct estado{
        bool activo[MAXPARTICIPANTES];
        int  ganador;   // -1 cuando el juego está en curso 
    };
 
    int randomNum( int min, int max );
    int cambiarPapa( int papa );
    int siguienteParticipante(int actual, int n, int direccion);

    int ronda(int buzonId, int procesoId, int siguiente,int *emisorValido, struct estado *estado, struct param *parametros);
    void jugar(int buzonId, int procesoId, int siguiente, struct estado *estado, struct param *parametros );

    void participante( int buzonId, int procesoId, struct estado *estado, struct param *parametros);
    void invasor(int buzonId, int n, struct estado *estado);

    void ganar(int procesoId);

    void iniciarParametros(struct param *parametros);
    void iniciaJuego();

#endif // PAPA_BASE_H