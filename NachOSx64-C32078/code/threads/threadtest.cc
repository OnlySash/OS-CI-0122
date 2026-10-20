// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create several threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustrate the inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.
//
//
// Copyright (c) -2026 Universidad de Costa Rica
#define PRUEBA 1 // 2= h2o 1 = filósofos, 0 = SimpleThread original

#include <unistd.h>
#include "copyright.h"
#include "system.h"
#include "diningph.h"

DiningPh * dp;

void Philo( void * p ) {

    int eats, thinks;
    long who = (long) p;

    currentThread->Yield();

    for ( int i = 0; i < 10; i++ ) {

        printf(" Philosopher %ld will try to pickup sticks\n", who + 1);

        dp->pickup( who );
        dp->print();
        eats = Random() % 6;

        currentThread->Yield();
        sleep( eats );

        dp->putdown( who );

        thinks = Random() % 6;
        currentThread->Yield();
        sleep( thinks );
    }

}

int cO = 0;
int cH = 0;
Semaphore * sH;           // cola donde duermen los hidrógenos  
Semaphore * sO;           // cola donde duermen los oxígenos
Semaphore * sMutex;       // protege cO y cH (y arma una molécula a la vez)
Semaphore * sDone;        // los H avisan al O que ya se unieron


void H( void * p ) {
    long id = (long) p;

    sMutex->P();
    cH++;
    if ( cH >= 2 && cO >= 1 ) {     // ya hay una molécula completa
        sH->V();  
        sH->V(); 
        cH -= 2;   // despierta 2 H 
        sO->V();  
        cO -= 1;   // despierta 1 O
    } else {
        sMutex->V();                   // todavía falta gente: libera y espera
    }

    sH->P();                           // espera a que se forme la molécula
    printf("   H%ld se une a la molecula\n", id);
    sDone->V();                        // avisa al oxígeno
}

void O( void * p ) {
    long id = (long) p;

    sMutex->P();
    cO++;
    if ( cH >= 2 ) {     // ya hay una molécula completa
        sH->V(); 
        sH->V();  
        cH -= 2;   // despierta 2 H 
        sO->V();  
        cO -= 1;  // despierta 1 O
    } else {
        sMutex->V();                   // todavía falta gente: libera y espera
    }

    sO->P();                           // espera a que se forme la molécula
    printf("   O%ld se une a la molecula\n", id);
    sDone->P();  
    sDone->P();  
    sMutex->V();
}

Semaphore * sFin;         

void PhiloFin( void * p ) { Philo( p ); sFin->V(); }
void HFin( void * p )     { H( p );     sFin->V(); }
void OFin( void * p )     { O( p );     sFin->V(); }

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 10 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"name" points to a string with a thread name, just for
//      debugging purposes.
//----------------------------------------------------------------------

void
SimpleThread(void* name)
{
    // Reinterpret arg "name" as a string
    char* threadName = (char*)name;
    
    // If the lines dealing with interrupts are commented,
    // the code will behave incorrectly, because
    // printf execution may cause race conditions.
    for (int num = 0; num < 3; num++) {
        //IntStatus oldLevel = interrupt->SetLevel(IntOff);
	printf( "*** thread %s looped %d times\n", currentThread->getName(), num );
	//interrupt->SetLevel(oldLevel);
        //currentThread->Yield();
    }
    //IntStatus oldLevel = interrupt->SetLevel(IntOff);
    printf( ">>> Thread %s has finished\n", threadName );
    //interrupt->SetLevel(oldLevel);

    currentThread->Yield();		// Yielding CPU will delete threads asynchronously

    currentThread->Finish();

}
//----------------------------------------------------------------------
// ThreadTest
// 	Set up a ping-pong between several threads, by launching
//	ten threads which call SimpleThread, and finally calling 
//	SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest()
{
    Thread * Ph;

    DEBUG('t', "Entering SimpleTest");

    // Prueba de los filósofos
    if ( PRUEBA == 1 ) {
        dp = new DiningPh();
        sFin = new Semaphore( "fin", 0 );
        for ( long k = 0; k < 5; k++ ) {
            char * nombre = new char[20];       // el nombre debe estar en el heap
            sprintf( nombre, "Filosofo %ld", k );
            Ph = new Thread( nombre );
            Ph->Fork( PhiloFin, (void *) k );
        }
        for ( int k = 0; k < 5; k++ )
            sFin->P();                            
        currentThread->Finish(); 
    }

    if ( PRUEBA == 2 ) {                       
        sMutex = new Semaphore( "mutex", 1 );
        sH     = new Semaphore( "H", 0 );
        sO     = new Semaphore( "O", 0 );
        sDone  = new Semaphore( "done", 0 );
        sFin   = new Semaphore( "fin", 0 );

        // 6 hidrógenos y 3 oxígenos, mezclados: salen 3 moléculas
        const char tipo[] = { 'H','H','H','O','H','O','H','H','O' };
        for ( long k = 0; k < 9; k++ ) {
            char * nombre = new char[20];
            sprintf( nombre, "%c%ld", tipo[k], k );
            Ph = new Thread( nombre );
            Ph->Fork( tipo[k] == 'H' ? HFin : OFin, (void *) k );
    }
    for ( int k = 0; k < 9; k++ )
        sFin->P();                         // main espera a los 9
    currentThread->Finish();
    }
/*
    dp = new DiningPh();

    for ( long k = 0; k < 5; k++ ) {
        Ph = new Thread( "dp" );
        Ph->Fork( Philo, (void *) k );
    }

    return;
*/
    char * threadName;
    Thread * newThread;
    for ( int k=1; k<5; k++ ) {
      threadName = new char[100];
      sprintf( threadName, "Hilo %d", k );
      newThread = new Thread( threadName );
      newThread->Fork ( SimpleThread, (void*) threadName );
    }

    currentThread->Yield();
 
    SimpleThread( (void*) "Hilo 0" );

    currentThread->Finish();

}

