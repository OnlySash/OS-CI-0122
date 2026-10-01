/**
  *  Ejemplo base para el problema de la ronda o papa caliente
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
  *   Código para el invasor
  *   Manda mensajes al azar a los participantes de la ronda
  *
 **/
void invasor( int buzonId, int n, struct estado *estado) {
   srand( time(NULL) ^ getpid() );   // semilla distinta a la de los participantes
 
   //sleep( randomNum(2, 4) );         // la "burocracia" antes de arrancar
 
   while ( estado->ganador == -1 ) {
      int objetivo = randomNum( 0, n - 1 );
 
      struct RondaPapa msg;
      msg.mtype    = objetivo + 1;
      msg.mensaje  = randomNum( 1, 100 );
      msg.emisor   = EMISOR_INVASOR;
      msg.receptor = objetivo;
 
      enviar( buzonId, &msg );
      printf( "El invasor envio un mensaje falso al participante %d\n", objetivo );
 
      sleep( randomNum(1, 3) );      // "cada cierto tiempo"
   }
}
