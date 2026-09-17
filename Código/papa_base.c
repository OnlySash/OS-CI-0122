/**
  *  Ejemplo base para el problema de la ronda o papa caliente
  *
  *  CI-0122 Sistemas Operativos
  *  Fecha: 2026/Ago/12
  *
 **/

#include "../Headers/papa_base.h"
#include "../Headers/Buzon.h"

int randomNum( int min, int max ) {
   return ( rand() % ( max - min + 1 ) ) + min;
}

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

int siguienteParticipante(int actual, int n, int direccion) {
   if (direccion == 1) {
      return (actual + 1) % n;
   }
   return (actual - 1 + n) % n;
}

/*
* Código para marcar una ronda
* Recorrido de la papa entre los participantes
*  
*/
int ronda(int buzonId, int procesoId, int siguiente,int *emisorValido, struct estado *estado, struct param *parametros ) {
   struct RondaPapa msg;
 
   if ( recibir( buzonId, procesoId + 1, &msg ) == -1 ) {
      return -1;
   }
 
   // Mensaje especial de fin de juego
   if ( msg.emisor == -1 ) {
      if ( procesoId == msg.mensaje ) {
         ganar( procesoId );
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
 
   if ( estado->activo[procesoId] ) {
      printf( "El participante %d ha recibido el mensaje %d\n", procesoId, msg.mensaje );
 
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
 
            // Avisar a TODOS los demas (menos a mi mismo) que el juego termino
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
   int siguiente = siguienteParticipante(procesoId, parametros->n, parametros->d);
   
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

/**
  *   El último en pie gana, 
  * pone mensaje negativo para finalizar el resto de procesos 
  **/
void ganar (int procesoId){
   //int activos = 0;
   printf("El participante %d ha ganado la ronda\n", procesoId);

   /*for(int i = 0; i < procesos->id; i++){
      if(procesos->activo == true){
         activos++;
      }
   }
   if(activos == 1){
      exit(0);
   }*/
}

void iniciarParametros( struct param *parametros) {
   parametros->n = randomNum( 2, MAXPARTICIPANTES);
   parametros->v = randomNum( 0, parametros->n - 1);
   parametros->d = randomNum( 0, 1);

   printf( "Creando una ronda de %d participantes\n", parametros->n );
   printf( "El participante inicial es %d\n", parametros->v );
   printf( "La direccion del buzon es %s\n", parametros->d ? "clockwise" : "counter-clockwise" );
}


/**
  *   Inicia el juego: Dejar el main sin nada
  * Asigna # miembros, miembro inicial y dirección
  *  1. Crea un buzón
  *  2. Generar un proceso para cada participante (fork)
  *  3. Intercambiar mensajes con buzon
  **/
void iniciaJuego(){
   //Inicia buzon
   int buzonId = inicializar_buzon();
   
   if (buzonId == -1) {
      perror("inicializar_buzon");
      exit(EXIT_FAILURE);
   }

   srand(time(NULL)); // Inicializa la semilla para números aleatorios
   
   // Crear y adjuntar la memoria compartida antes de hacer fork
   int shmId = shmget( IPC_PRIVATE, sizeof(struct estado), IPC_CREAT | 0600 );
   if ( shmId == -1 ) {
      perror( "shmget" );
      exit( EXIT_FAILURE );
   }
 
   struct estado *estado = (struct estado *) shmat( shmId, NULL, 0 );
   if ( estado == (void *) -1 ) {
      perror( "shmat" );
      exit( EXIT_FAILURE );
   }


   struct param parametros;
   // El programa principal decidirá cual es el primer participante en arrancar y el valor inicial de la papa
   iniciarParametros(&parametros);

   for ( int i = 0; i < parametros.n; i++ ) {
      estado->activo[i] = true;
   }
   estado->ganador = -1;


   pid_t *pid = (pid_t *)malloc(parametros.n * sizeof(pid_t));

   if (pid == NULL) {
      perror("malloc");
      exit(EXIT_FAILURE);
   }

   //Crear procesos para cada participante
   for (int i = 0; i < parametros.n; i++) {
      fflush( stdout );  // evita que el hijo herede texto sin imprimir del buffer del padre
      pid[i] = fork();
 
      if ( pid[i] == -1 ) {
         perror( "fork" );
         exit( EXIT_FAILURE );
      }  

      if ( pid[i] == 0 ) {
         // Codigo hijo (participante i
         participante( buzonId, i, estado, &parametros );
 
         printf( "Participante %d ha terminado\n", i );
         shmdt( estado );
         exit( EXIT_SUCCESS );
      }
   }
 
   //Crear el proceso invasor
   fflush( stdout );
   pid[parametros.n] = fork();
 
   if ( pid[parametros.n] == -1 ) {
      perror( "fork" );
      exit( EXIT_FAILURE );
   }
 
   if ( pid[parametros.n] == 0 ) {
      invasor( buzonId, parametros.n, estado );
 
      printf( "El invasor ha terminado\n" );
      shmdt( estado );
      exit( EXIT_SUCCESS );
   }


   // El padre espera a que todos los participantes terminen
   for ( int i = 0; i < parametros.n; i++ ) {
      waitpid( pid[i], NULL, 0 );
   }


   // Ya nadie utiliza el buzon - Eliminar
   if (msgctl(buzonId, IPC_RMID, NULL) == -1) {
      perror("msgctl");
   }
   shmdt( estado );
   if ( shmctl( shmId, IPC_RMID, NULL ) == -1 ) {
      perror( "shmctl" );
   }

   free(pid);
}

int main() {
   iniciaJuego();
   return 0;
}
