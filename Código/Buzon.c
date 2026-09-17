#include "../Headers/Buzon.h"

/* Crea (o abre, si ya existe) el buzon asociado a id.
 * Devuelve el id del buzon
 */
int inicializar_buzon(void) {
    int id = msgget(KEY, 0600 | IPC_CREAT);
    if (id == -1) {
        perror("inicializar_buzon: msgget");
    }
    return id;
}
 
/* 
 * Envia un mensaje
 */
int enviar(int buzonId, struct RondaPapa *msg) { 
    int st = msgsnd(buzonId, msg, sizeof(*msg) - sizeof(long), 0);
    if (st == -1) {
        perror("enviar: msgsnd");
    }
    return st;
}
 
/* 
 * Recibe (bloqueante) un mensaje de tipo mtype desde el buzon id.
 * Guarda el resultado en *msg. 
 */
int recibir(int buzonId, long mtype, struct RondaPapa *msg) {
    ssize_t st = msgrcv(buzonId, msg, sizeof(*msg) - sizeof(long), mtype, 0);
    if (st == -1) {
        perror("recibir: msgrcv");
    }
    return (int)st;
}
