#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include "Semaforos.h"


Semaforos::Semaforos( int cantidad, int valorInicial ) {
   int status;

   this->nsem    = cantidad;
   this->creator = getpid();
   this->semId   = semget( IPC_PRIVATE, cantidad, 0600 | IPC_CREAT );

   if(-1 == this->semId) {
      perror( "Semaforos::Semaforos(int, int) semget" );
      exit( 23 );
   }
   // semget para solicitar el arreglo de semaforos
   // semctp para colocar el valor inicial a cada semaforo
   status = 0;
   union semun arg;
   for ( int i = 0; i < cantidad && status != -1; i++ ) {
        arg.val = valorInicial;
        status = semctl( this->semId, i, SETVAL, arg );
    }
   if ( -1 == status ) {
      perror( "Semaforos::Semaforos(int, int) initializer" );
      exit( 23 );
   }

}


Semaforos::~Semaforos() {

   if ( this->creator == getpid() ) {
      semctl( this->semId, IPC_RMID, 0 );
   }

}

int Semaforos::Signal( int cual ) {
   struct sembuf op;
    op.sem_num = cual;
    op.sem_op = 1;
    op.sem_flg = 0;
 
    int status;
    do {
        status = semop( this->semId, &op, 1 );
    } while ( -1 == status && EINTR == errno );
   
   if ( -1 == status ) {
      perror( "Semaforos::Signal" );
      exit( 23 );
   }
   return status;

}


int Semaforos::Wait( int cual ) {
   struct sembuf op;
    op.sem_num = cual;
    op.sem_op = -1;
    op.sem_flg = 0;
 
    int status;
    do {
        status = semop( this->semId, &op, 1 );
    } while ( -1 == status && EINTR == errno );

   if ( -1 == status ) {
      perror( "Semaforos::Wait" );
      exit( 23 );
   }

   return status;

}

//Wait atómico sobre 2 semáforos el kernel puede aplicar
void Semaforos::SP( int primero, int segundo ) {
   struct sembuf ops[2];
    ops[0].sem_num = primero;
    ops[0].sem_op = -1;
    ops[0].sem_flg = 0;
    ops[1].sem_num = segundo;
    ops[1].sem_op = -1;
    ops[1].sem_flg = 0;
 
    int resultado;
    do {
        resultado = semop( this->semId, ops, 2 );
    } while ( -1 == resultado && EINTR == errno );
 
    if ( -1 == resultado ) {
        perror( "Semaforos::SP" );
        exit( 23 );
    }

}


//Signal atómico sobre 2 semáforos el kernel puede aplicar
void Semaforos::SV( int primero, int segundo ) {
     struct sembuf ops[2];
    ops[0].sem_num = primero;
    ops[0].sem_op = 1;
    ops[0].sem_flg = 0;
    ops[1].sem_num = segundo;
    ops[1].sem_op = 1;
    ops[1].sem_flg = 0;
 
    int resultado;
    do {
        resultado = semop( this->semId, ops, 2 );
    } while ( -1 == resultado && EINTR == errno );
 
    if ( -1 == resultado ) {
        perror( "Semaforos::SV" );
        exit( 23 );
    }
}


//POne el mismo valor a todos los semáforos del arreglo
void Semaforos::setValorInicial( int valorInicial ) {
   int resultado = 0;
    union semun arg;
 
    for ( int i = 0; i < this->nsem && resultado != -1; i++ ) {
        arg.val = valorInicial;
        resultado = semctl( this->semId, i, SETVAL, arg );
    }

   if ( -1 == resultado ) {
      perror( "Semaforos::setValorInicial" );
      exit( 23 );
   }

}

