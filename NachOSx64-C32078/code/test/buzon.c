#include "syscall.h"


struct mensaje {
    long mtype;
    char mtext[16];
};

int main()
{
    struct mensaje env, rec;
    char *texto = "hola buzon";
    int id, i;

    id = MsgGet(4321, IPC_CREAT | 0666);

    env.mtype = 1;
    for (i = 0; texto[i] != '\0'; i++)
        env.mtext[i] = texto[i];
    env.mtext[i] = '\0';

    MsgSend(id, (struct msgbuf *)&env, 16, 0);
    MsgRecv(id, (struct msgbuf *)&rec, 16, 1, 0);
    Write(rec.mtext, 10, ConsoleOutput);
    MsgCtl(id, IPC_RMID, 0);
    Exit(0);
}