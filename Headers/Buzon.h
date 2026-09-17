#ifndef BUZON_H
#define BUZON_H
 
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

struct RondaPapa; 
#define KEY 0xC54321


int inicializar_buzon();
int enviar(int id, struct RondaPapa *msg);
int recibir(int id, long mtype, struct RondaPapa *msg);


#endif // BUZON_H