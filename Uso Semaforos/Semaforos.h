#ifndef SEMAFOROS_H
#define SEMAFOROS_H

class Semaforos {
   public:

   // En Linux el programa debe definir esta union para semctl()
    union semun {
       int val;
        struct semid_ds *buf;
        unsigned short  *array;
    };

      Semaforos( int, int = 0 );
      ~Semaforos();
      int Signal( int = 0 );
      int Wait( int = 0 );
      void SP( int, int );
      void SV( int, int );
      void setValorInicial( int );

   private:
      int semId;		// Id del semaforo
      int nsem;	// Cantidad de semaforos en el arreglo
      pid_t creator;	// Creador del semaforo
};

#endif