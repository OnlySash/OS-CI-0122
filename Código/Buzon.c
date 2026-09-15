#include "Buzon.h"

const char *html_labels[] = {
    "a", "b", "c", "d", "e", "li", ""
};

/* Crea (o abre, si ya existe) el buzon asociado a KEY.
 * Devuelve el id del buzon, o termina el programa si falla. */
int inicializar_buzon(void) {
    int id = msgget(KEY, 0600 | IPC_CREAT);
    if (id == -1) {
        perror("inicializar_buzon: msgget");
        exit(EXIT_FAILURE);
    }
    return id;
}
 
/* Envia un mensaje de tipo mtype con "times" y "label" al buzon id.
 * Devuelve 0 si tuvo exito, -1 si fallo (sin terminar el programa). */
int enviar(int id, long mtype, int times, const char *label) {
    struct msgbuffer msg;
    msg.mtype = mtype;
    msg.times = times;
    strncpy(msg.label, label, LABEL_SIZE - 1);
    msg.label[LABEL_SIZE - 1] = '\0';
 
    int st = msgsnd(id, &msg, sizeof(msg) - sizeof(long), 0);
    if (st == -1) {
        perror("enviar: msgsnd");
    }
    return st;
}
 
/* Recibe (bloqueante) un mensaje de tipo mtype desde el buzon id.
 * Guarda el resultado en *msg. Devuelve la cantidad de bytes leidos,
 * o -1 si fallo. */
ssize_t recibir(int id, long mtype, struct msgbuffer *msg) {
    ssize_t st = msgrcv(id, msg, sizeof(*msg) - sizeof(long), mtype, 0);
    if (st == -1) {
        perror("recibir: msgrcv");
    }
    return st;
}
