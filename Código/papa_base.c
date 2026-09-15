/**
  *  Ejemplo base para el problema de la ronda o papa caliente
  *
  *  CI-0122 Sistemas Operativos
  *  Fecha: 2026/Ago/12
  *
 **/

#include "papa_base.h"
#include "Buzon.h"

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
   return papa;
}

int siguienteParticipante(int actual, int n, int direccion) {
   if (direccion == 1) {
      return (actual + 1) % n;
   }
   return (actual - 1 + n) % n;
}

/**
  *   Código para cada participante
  *   Debe cambiar el valor de la papa y determinar si explotó
 **/
int participante( int id, struct papa papa ) {

   //Crear mensaje aleatorio
   int mensaje = randomNum( 1, 100 );
   printf("El participante %d ha creado el mensaje %d\n", id, mensaje);

   //Enviar mensaje al buzon
   enviar( id, 1, mensaje, "mensaje" );

   //Siguiente proceso recibe mensaje del buzon
   recibir( id, 1, NULL );

   //Verificar si cumple reglas de Collatz: cambiarPapa( mensaje );
   if(cambiarPapa( mensaje ) == 1){
         //Si cumple, pierde, se vuelve proceso pasivo, solo continua sin enviar nada
         printf("El participante %d ha perdido\n", id);
         // si si, pierde, se vuelve proceso pasivo, solo continua sin enviar nada
         papa.activo = false;
   }
      
   // 5. Ultimo en pie, gana
   if (papa.activo == false) {
   }
}

/**
  *   Código para el invasor
  *   Manda mensajes al azar a los participantes de la ronda
  *
 **/
int invasor( int id ) {

   _exit( 0 );	// Everything OK

}

/**
  *   El último en pie gana, 
  * pone mensaje negativo para finalizar el resto de procesos 
  **/
void ganar (int id){
   printf("El participante %d ha ganado la ronda\n", id);
}

/*
* Código para marcar cada ronda
* Recorrido de la papa entre los participantes
*/
void ronda ( int id, struct papa papa, int inicio, int final, int direccion ) {
   //Falta poner en orden de los participantes
   for (int i = inicio; i != final; i = (direccion == 0) ? (i + 1) % papa.id : (i - 1 + papa.id) % papa.id) {
      participante(i, papa);
   }
}

void jugar( int activos, struct papa papa, int inicio, int final, int direccion ) {
   int ronda_num = 0;

   for (int i =0; i < papa.id; i++) {
      if(papa.activo == true){
         ronda( id, papa, inicio, final, direccion );
         ronda_num++;
      }
      else{
         printf("El participante %d ha perdido\n", id);
      }
      //if ultimo en pie, ganar()
   }

}

void iniciarParametros( struct param *parametros, struct papa *papa, pid_t *pid ) {
   parametros->n = randomNum( 2, 10);
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
   int id = inicializar_buzon();
   
   if (id == -1) {
      perror("inicializar_buzon");
      exit(EXIT_FAILURE);
   }

   srand(time(NULL)); // Inicializa la semilla para números aleatorios
   //int buzon, id, i, j, resultado;
   struct param parametros;
   struct papa papa;

   pid_t *pid = (pid_t *)malloc(parametros.n * sizeof(pid_t));

   if (pid == NULL) {
      perror("malloc");
      exit(EXIT_FAILURE);
   }

   // El programa principal decidirá cual es el primer participante en arrancar y el valor inicial de la papa
   iniciarParametros( &parametros, &papa, pid );
 
   for (int i = 0; i < parametros.n; i++) {
      waitpid(pid[i], NULL, 0);
   }

   // Ya nadie utiliza el buzon - Eliminar
   if (msgctl(id, IPC_RMID, NULL) == -1) {
      perror("msgctl");
   }

   free(pid);

}

int main( int argc, char ** argv ) {
   iniciaJuego();
   return 0;
}
