// synch.cc 
//	Routines for synchronizing threads.  Three kinds of
//	synchronization routines are defined here: semaphores, locks 
//   	and condition variables (the implementation of the last two
//	are left to the reader).
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation.  We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts.  While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.
//
// Copyright (c) -2026 Universidad de Costa Rica

#include "copyright.h"
#include "synch.h"
#include "system.h"

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	Initialize a semaphore, so that it can be used for synchronization.
//
//	"debugName" is an arbitrary name, useful for debugging.
//	"initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

Semaphore::Semaphore(const char* debugName, int initialValue)
{
    name = (char *)debugName;
    value = initialValue;
    queue = new List<Thread*>;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	De-allocate semaphore, when no longer needed.  Assume no one
//	is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore()
{
    delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
// 	Wait until semaphore value > 0, then decrement.  Checking the
//	value and decrementing must be done atomically, so we
//	need to disable interrupts before checking the value.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
//----------------------------------------------------------------------

void
Semaphore::P()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
    
    while (value == 0) { 			// semaphore not available
	queue->Append(currentThread);		// so go to sleep
	currentThread->Sleep();
    } 
    value--; 					// semaphore available, 
						// consume its value
    
    interrupt->SetLevel(oldLevel);		// re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
// 	Increment semaphore value, waking up a waiter if necessary.
//	As with P(), this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that threads
//	are disabled when it is called.
//----------------------------------------------------------------------

void
Semaphore::V()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    thread = queue->Remove();
    if (thread != NULL)	   // make thread ready, consuming the V immediately
	scheduler->ReadyToRun(thread);
    value++;
    interrupt->SetLevel(oldLevel);
}

#ifdef USER_PROGRAM
//----------------------------------------------------------------------
// Semaphore::Destroy
// 	Destroy the semaphore, freeing the waiting threads
//	This is used to destroy a user semaphore
//----------------------------------------------------------------------

void
Semaphore::Destroy()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    while ( (thread = queue->Remove() ) != NULL )	// make thread ready
	scheduler->ReadyToRun(thread);

    interrupt->SetLevel(oldLevel);
}

#endif


// Dummy functions -- so we can compile our later assignments 
// Note -- without a correct implementation of Condition::Wait(), 
// the test case in the network assignment won't work!
Lock::Lock(const char* debugName) {
    name = debugName;
    owner = NULL;
    sem = new Semaphore( "lock", 1 );
}


Lock::~Lock() {
    delete sem;
}


void Lock::Acquire() {
    ASSERT( !isHeldByCurrentThread() );   // no re-adquirir un lock propio
    sem->P();
    owner = currentThread;
}


void Lock::Release() {
    ASSERT( isHeldByCurrentThread() );
    owner = NULL;
    sem->V();
}


bool Lock::isHeldByCurrentThread() {
   return owner == currentThread;
}


Condition::Condition(const char* debugName) {
    name = debugName;
    waiters = 0;
    sem = new Semaphore( "cond", 0 );
}


Condition::~Condition() {
    delete sem;
}


void Condition::Wait( Lock * conditionLock ) {
    ASSERT( conditionLock->isHeldByCurrentThread() );
    waiters++;
    conditionLock->Release();   // suelta el lock antes de dormir
    sem->P();                   // duerme hasta un Signal
    conditionLock->Acquire();
}


void Condition::Signal( Lock * conditionLock ) {
    ASSERT( conditionLock->isHeldByCurrentThread() );
    if ( waiters > 0 ) {        // si nadie espera, no hace nada
        waiters--;
        sem->V();
    }
}


void Condition::Broadcast( Lock * conditionLock ) {
    ASSERT( conditionLock->isHeldByCurrentThread() );
    while ( waiters > 0 ) {
        waiters--;
        sem->V();
    }
}


// Mutex class
Mutex::Mutex( const char * debugName ) {
    name = debugName;
    sem = new Semaphore( "mutex", 1 ); 
}

Mutex::~Mutex() {
    delete sem;
}

void Mutex::Lock() {
    sem->P();
}

void Mutex::Unlock() {
    sem->V();
}


// Barrier class
Barrier::Barrier( const char * debugName, int count ) {
    name = debugName;
    n = count;
    arrived = 0;
    mutex = new Semaphore( "barrier_mutex", 1 );
    turn1 = new Semaphore( "barrier_turn1", 1 );
    turn2 = new Semaphore( "barrier_turn2", 1 );
}

Barrier::~Barrier() {
    delete mutex;
    delete turn1;
    delete turn2;
}

void Barrier::Wait() {
    //No pasan hasta que lleguen n hilos
    mutex->P();
    arrived++;
    if ( arrived == n )
        for ( int i = 0; i < n; i++ ) turn1->V();
    
    mutex->V();
    turn1->P();

    //No entran hasta que todos hayan salido
    mutex->P();
    arrived--;
    if ( arrived == 0 )
        for ( int i = 0; i < n; i++ ) turn2->V();
    
    mutex->V();
    turn2->P();
}

