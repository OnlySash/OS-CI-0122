/**
  *  Recorrido del juego
  *
  *  CI-0122 Sistemas Operativos
  *  Fecha: 2026/Ago/12
  *
 **/

#include "../Headers/papa_base.h"
#include "../Headers/invasor.h"
#include "../Headers/juego.h"
#include "../Headers/Buzon.h"

/**
  *  Aplica las reglas de Collatz al valor de la papa
  * si el resultado es uno, la papa explotó, 
  * ese proceso “sale” del juego y se convierte en un comunicador pasivo.
  *
 **/
int cambiarPapa( int papa ) {
   if ( 1 == (papa & 0x1) ) {		// papa es impar
            papa = (papa << 1) + papa + 1;	// papa = papa * 2 + papa + 1
         } else {
            papa >>= 1;			// n = n / 2, utiliza corrimiento a la derecha, una posicion
         }
   printf("Collatz = %d \n", papa);
   return papa;
}

/*
 * Según la dirección, describe qué participa después
*/
int siguienteParticipante(int actual, int n, int direccion) {
   if (direccion == 1) {
      return (actual + 1) % n;
   }
   return (actual - 1 + n) % n;
}

/*
* Código para marcar una ronda
* Recorrido de la papa entre los participantes
*/
int ronda(int buzonId, int procesoId, int siguiente,int *emisorValido, struct estado *estado, struct param *parametros ) {
   struct RondaPapa msg;

   if ( recibir( buzonId, procesoId + 1, &msg ) == -1 ) {
      return -1;
   }
 
   // Mensaje especial de fin de juego al ganar
   if ( msg.emisor == -1 ) {
      if ( procesoId == msg.mensaje ) {
         printf("El participante %d ha ganado el juego\n", procesoId);
      }
      return 1;
   }

   // Validar el emisor
   if ( *emisorValido == NO_REGISTRADO ) {
      *emisorValido = msg.emisor;   // primera ronda: garantizado valido, se registra
   } else if ( msg.emisor != *emisorValido ) {
      printf( "El participante %d descarto un mensaje invalido (emisor %d)\n", procesoId, msg.emisor );
      return 0;   // se descarta: no se procesa ni se reenvia, se sigue jugando
   }
   // Valida que se recibió
   if ( estado->activo[procesoId] ) {
      printf( "El participante %d ha recibido el mensaje %d\n", procesoId, msg.mensaje );

      // Revisa si pierde
      int resultado = cambiarPapa( msg.mensaje );
      msg.mensaje = resultado;
      if ( resultado == 1 ) {
         printf( "El participante %d ha perdido\n", procesoId );
         estado->activo[procesoId] = false;
 
         // Contar cuantos quedan activos
         int activos = 0, ultimo = -1;
         for ( int i = 0; i < parametros->n; i++ ) {
            if ( estado->activo[i] ) {
               activos++;
               ultimo = i;
            }
         }
 
         if ( activos == 1 ) {
            estado->ganador = ultimo;
 
            // Avisar a todos los demas que el juego termino
            struct RondaPapa fin;
            fin.emisor  = -1;
            fin.mensaje = ultimo;
 
            for ( int j = 0; j < parametros->n; j++ ) {
               if ( j == procesoId ) continue;
               fin.mtype    = j + 1;
               fin.receptor = j;
               enviar( buzonId, &fin );
            }
            return 1; // este participante (recien perdido) tambien termina
         }
      }
   }
   // Si esta pasivo, msg.mensaje se retransmite tal cual, sin tocarlo
   msg.mtype    = siguiente + 1;
   msg.emisor   = procesoId;
   msg.receptor = siguiente;
   enviar( buzonId, &msg );
 
   return 0;
}

/*
* Proceso infinito de rondas
*/
void jugar( int buzonId, int procesoId, int siguiente, struct estado *estado, struct param *parametros ) {
   int resultado;
   int emisorValido = NO_REGISTRADO;

   do {
      resultado = ronda( buzonId, procesoId, siguiente, &emisorValido, estado, parametros );
   } while ( resultado == 0 );
}

/**
  *   Código para cada participante
  *   Debe cambiar el valor de la papa y determinar si explotó
 **/
void participante( int buzonId, int procesoId, struct estado *estado, struct param *parametros)
{
    //Guia el siguiente
   int siguiente = siguienteParticipante(procesoId, parametros->n, parametros->d);
   
   // Genera el mensaje y asigna los parámetros del mismo
   if ( procesoId == parametros->v ) {
      int mensaje = randomNum( 1, 100 );
      printf( "El participante %d ha creado el mensaje %d\n", procesoId, mensaje );
 
      struct RondaPapa msg;
      msg.mtype    = siguiente + 1;
      msg.mensaje  = mensaje;
      msg.emisor   = procesoId;
      msg.receptor = siguiente;
      enviar( buzonId, &msg );
   }
   jugar( buzonId, procesoId, siguiente, estado, parametros );
}

