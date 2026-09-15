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
 
#define KEY 0xC54321
#define LABEL_SIZE 64
#define MTYPE_JUEGO 2026
 
/* La estructura del mensaje va a nivel de archivo, para que la puedan
 * usar tanto inicializar_buzon() como enviar() y recibir() */
struct msgbuffer {
    long mtype;              /* tipo de mensaje, debe ser > 0 */
    int  times;               /* dato de ejemplo (luego lo cambiamos por lo del juego) */
    char label[LABEL_SIZE];   /* dato de ejemplo (idem) */
};

extern const char *html_labels[];

int inicializar_buzon();
int enviar(int id, long mtype, int times, const char *label);
ssize_t recibir(int id, long mtype, struct msgbuffer *msg);


#endif // BUZON_H