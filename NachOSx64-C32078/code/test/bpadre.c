#include "syscall.h"

struct mensaje {
    long mtype;
    char mtext[16];
};

int main()
{
    struct mensaje env;
    char *texto = "hola hijo";
    int id, i, hijo;

    id = MsgGet(4321, IPC_CREAT | 0666);

    env.mtype = 1;
    for (i = 0; texto[i] != '\0'; i++)
        env.mtext[i] = texto[i];
    env.mtext[i] = '\0';

    hijo = Exec("../test/bhijo");
    MsgSend(id, (struct msgbuf *)&env, i + 1, 0);
    Join(hijo);

    MsgCtl(id, IPC_RMID, 0);
    Write("padre: listo\n", 13, ConsoleOutput);
    Exit(0);
}