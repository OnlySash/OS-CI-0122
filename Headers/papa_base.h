#ifndef PAPA_BASE_H
#define PAPA_BASE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <fcntl.h>
#include <signal.h>

#include "Buzon.h"



    int randomNum( int min, int max );
    int cambiarPapa( int papa );

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

    struct papa{
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

    int randomNum(int min, int max);
    int cambiarPapa(int papa);

    int participante(int id, int buzon, struct papa *papa, struct param *parametros);
    int invasor(int id);

    void ganar(int id);
    void jugar(int id, int buzon, struct papa *papa, struct param *parametros);
    void ronda(int id, int buzon, struct papa *papa, struct param *parametros);

    void iniciarParametros(struct param *parametros);
    void iniciaJuego();

#endif // PAPA_BASE_H