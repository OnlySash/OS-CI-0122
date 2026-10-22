#include "syscall.h"

int largo(char *s)
{
    int n = 0;
    while (s[n] != '\0')
        n++;
    return n;
}

int main()
{
    char *mensaje = "hola desde nachos\n";
    char buffer[40];
    int fd, leidos;

    Write(mensaje, largo(mensaje), ConsoleOutput);

    Create("prueba.txt");
    fd = Open("prueba.txt");
    Write(mensaje, largo(mensaje), fd);
    Close(fd);

    fd = Open("prueba.txt");
    leidos = Read(buffer, 39, fd);
    Close(fd);
    Write(buffer, leidos, ConsoleOutput);

    return 0;
}