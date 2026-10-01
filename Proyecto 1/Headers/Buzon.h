#ifndef BUZON_H
#define BUZON_H

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#include "juego.h"

#define KEY 0xC54321


int inicializar_buzon();
int enviar(int id, struct RondaPapa *msg);
int recibir(int id, long mtype, struct RondaPapa *msg);


#endif // BUZON_H