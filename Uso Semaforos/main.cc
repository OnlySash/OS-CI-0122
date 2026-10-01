/* Prueba 1: Semaforo 0: turno del padre, semaforo 1: turno del hijo
 *
 * Prueba 2: SP/SV exclusión mutua de dos semaforos tomados atomicamente.
 * Varios hijos escriben en pantalla dentro de la seccion critica.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "Semaforos.h"


void pruebaPingPong() {
    printf( " Prueba 1: Wait / Signal (ping-pong) \n" );
    fflush( stdout );

    Semaforos s( 2, 0 );
    const int RONDAS = 3;

    pid_t pid = fork();
    if ( 0 == pid ) {
        for ( int i = 0; i < RONDAS; i++ ) {
            s.Wait( 1 );                 // espera su turno
            printf( "  Hijo : pong %d\n", i + 1 );
            fflush( stdout );
            s.Signal( 0 );               // devuelve turno al padre
        }
        _exit( 0 );
    }

    for ( int i = 0; i < RONDAS; i++ ) {
        printf( "  Padre: ping %d\n", i + 1 );
        fflush( stdout );
        s.Signal( 1 );                   // da turno al hijo
        s.Wait( 0 );                     // espera respuesta
    }
    wait( NULL );
}

void pruebaSPSV() {
    printf( "\n Prueba 2: SP / SV (dos semaforos atomicos) \n" );
    fflush( stdout );

    Semaforos s( 2, 1 );                 // ambos semaforos "libres"
    const int HIJOS = 3;

    for ( int h = 0; h < HIJOS; h++ ) {
        if ( 0 == fork() ) {
            for ( int i = 0; i < 2; i++ ) {
                s.SP( 0, 1 );            // toma ambos recursos o ninguno
                printf( "  Hijo %d entra a la seccion critica (iter %d)\n", h, i + 1 );
                fflush( stdout );
                usleep( 100000 );
                printf( "  Hijo %d sale de la seccion critica\n", h );
                fflush( stdout );
                s.SV( 0, 1 );            // libera ambos
            }
            _exit( 0 );
        }
    }
    for ( int h = 0; h < HIJOS; h++ ) {
        wait( NULL );
    }
}

void pruebaSetValor() {
    printf( "\n=== Prueba 3: setValorInicial ===\n" );
    Semaforos s( 2, 0 );
    s.setValorInicial( 2 );              // ahora cada semaforo permite 2 Wait sin bloquear
    s.Wait( 0 ); s.Wait( 0 );
    s.Wait( 1 ); s.Wait( 1 );
    printf( "  4 Wait consecutivos sin bloqueo: OK\n" );
}

int main() {
    pruebaPingPong();
    pruebaSPSV();
    pruebaSetValor();
    printf( "\nTodas las pruebas terminaron.\n" );
    return 0;
}