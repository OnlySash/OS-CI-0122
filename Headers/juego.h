#ifndef JUEGO_H
#define JUEGO_H

#include <stdbool.h>

#include "papa_base.h"

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

    struct estado{
        bool activo[MAXPARTICIPANTES];
        int  ganador;   // -1 cuando el juego está en curso 
    };
 
    int cambiarPapa( int papa );
    int siguienteParticipante(int actual, int n, int direccion);

    int ronda(int buzonId, int procesoId, int siguiente,int *emisorValido, struct estado *estado, struct param *parametros);
    void jugar(int buzonId, int procesoId, int siguiente, struct estado *estado, struct param *parametros );

    void participante( int buzonId, int procesoId, struct estado *estado, struct param *parametros);

    void ganar(int procesoId);

#endif //JUEGO_H


