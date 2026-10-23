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

void hilo()
{
    writer("hilo: uno\n");
    Yield();
    writer("hilo: dos\n");
    Exit(0);
}

int main()
{
    Fork(hilo);
    writer("main: uno\n");
    Yield();
    writer("main: dos\n");
    Yield();
    Exit(0);
}