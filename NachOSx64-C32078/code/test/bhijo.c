#include "syscall.h"

struct mensaje {
    long mtype;
    char mtext[16];
};

int main()
{
    struct mensaje rec;
    int id, n, intentos = 0;

    id = MsgGet(4321, 0666);

    /* IPC_NOWAIT + Yield: un MsgRecv bloqueante congelaria todo NachOS */
    while ((n = MsgRecv(id, (struct msgbuf *)&rec, 16, 1, IPC_NOWAIT)) < 0) {
        if (++intentos > 1000)
            Exit(1);
        Yield();
    }

    Write("hijo recibio: ", 14, ConsoleOutput);
    Write(rec.mtext, n - 1, ConsoleOutput);
    Write("\n", 1, ConsoleOutput);
    Exit(0);
}