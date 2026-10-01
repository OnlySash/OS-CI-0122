/**
  *  Ejemplo base para el problema de la ronda o papa caliente
  *
  *  CI-0122 Sistemas Operativos
  *  Fecha: 2026/Ago/12
  *
 **/

#include "../Headers/papa_base.h"
#include "../Headers/Buzon.h"
#include "../Headers/juego.h"
#include "../Headers/invasor.h"

/*
 * Genera números aleatorios para distintos parametros
*/
int randomNum( int min, int max ) {
   return ( rand() % ( max - min + 1 ) ) + min;
}

/*
 * Revisa si usuario creo parametros, sino generarlos aleatoriamente
*/
void iniciarParametros( struct param *parametros) {
   // Cantidad de miembros de la ronda
   if ( parametros->n == -1 ) {
      parametros->n = randomNum( 2, MAXPARTICIPANTES );
   } else if ( parametros->n < 2 || parametros->n > MAXPARTICIPANTES ) {
      fprintf( stderr, "Error: -n debe estar entre 2 y %d\n", MAXPARTICIPANTES );
      exit( EXIT_FAILURE );
   }
   // Proceso que inicia
   if ( parametros->v == -1 ) {
      parametros->v = randomNum( 0, parametros->n - 1 );
   } else if ( parametros->v < 0 || parametros->v >= parametros->n ) {
      fprintf( stderr, "Error: -v debe estar entre 0 y %d (n-1)\n", parametros->n - 1 );
      exit( EXIT_FAILURE );
   }
   //Dirección
   if ( parametros->d == -1 ) {
      parametros->d = randomNum( 0, 1 );
   } else if ( parametros->d != 0 && parametros->d != 1 ) {
      fprintf( stderr, "Error: -d debe ser 0 (counter-clockwise) o 1 (clockwise)\n" );
      exit( EXIT_FAILURE );
   }

   printf( "Creando una ronda de %d participantes\n", parametros->n );
   printf( "El participante inicial es %d\n", parametros->v );
   printf( "La direccion del buzon es %s\n", parametros->d ? "clockwise" : "counter-clockwise" );
}


/**
  *   Inicia el juego: Dejar el main sin nada
  * Asigna # miembros, miembro inicial y dirección
  *  1. Crea un buzón
  *  2. Generar parámetros
  *  3. Generar un proceso para cada participante (fork)
  *  4. Termina: libera 
  **/
void iniciaJuego(int argc, char **argv){
   //Inicia buzon
   int buzonId = inicializar_buzon();
   
   if (buzonId == -1) {
      perror("inicializar_buzon");
      exit(EXIT_FAILURE);
   }
   //Semilla para números aleatorios
   srand(time(NULL)); 
   
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

   //Asignar parámetros
   struct param parametros;
   parametros.n = -1;
   parametros.v = -1;
   parametros.d = -1;
 
   int opt;
   while ( (opt = getopt( argc, argv, "n:v:d:" )) != -1 ) {
      switch ( opt ) {
         case 'n': parametros.n = atoi( optarg ); break;
         case 'v': parametros.v = atoi( optarg ); break;
         case 'd': parametros.d = atoi( optarg ); break;
         default:
            fprintf( stderr, "Uso: %s [-n participantes] [-v inicial] [-d direccion(0|1)]\n", argv[0] );
            exit( EXIT_FAILURE );
      }
   }
   iniciarParametros(&parametros);
   // Todos los participantes activos
   for ( int i = 0; i < parametros.n; i++ ) {
      estado->activo[i] = true;
   }
   estado->ganador = -1;

   //Para procesos
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
   //Liberar procesos
   free(pid);
}

int main(int argc, char **argv) {
   iniciaJuego(argc, argv);
   return 0;
}
