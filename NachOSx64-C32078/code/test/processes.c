#include "syscall.h"

int longer(char *s)
{
    int n = 0;
    while (s[n] != '\0')
        n++;
    return n;
}

void writer(char *s)
{
    Write(s, longer(s), ConsoleOutput);
}

int main()
{
    SpaceId son;
    int estatus;

    writer("padre: lanzando files\n");
    son = Exec("../test/files");
    estatus = Join(son);
    writer("padre: el hijo termino\n");
    Exit(estatus);
}