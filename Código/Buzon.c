#include "../Headers/Buzon.h"
#include "../Headers/papa_base.h"

/* Crea (o abre, si ya existe) el buzon asociado a KEY.
 * Devuelve el id del buzon, o termina el programa si falla. */
int inicializar_buzon(void) {
    int id = msgget(KEY, 0600 | IPC_CREAT);
    if (id == -1) {
        perror("inicializar_buzon: msgget");
    }
    return id;
}
 
/* Envia un mensaje de tipo mtype con "times" y "label" al buzon id.
 * Devuelve 0 si tuvo exito, -1 si fallo (sin terminar el programa). */
int enviar(int buzonId, struct RondaPapa *msg) { 
    int st = msgsnd(buzonId, msg, sizeof(*msg) - sizeof(long), 0);
    if (st == -1) {
        perror("enviar: msgsnd");
    }
    return st;
}
 
/* 
 * Recibe (bloqueante) un mensaje de tipo mtype desde el buzon id.
 * Guarda el resultado en *msg. Devuelve la cantidad de bytes leidos,
 */
int recibir(int buzonId, long mtype, struct RondaPapa *msg) {
    ssize_t st = msgrcv(buzonId, msg, sizeof(*msg) - sizeof(long), mtype, 0);
    if (st == -1) {
        perror("recibir: msgrcv");
    }
    return (int)st;
}
