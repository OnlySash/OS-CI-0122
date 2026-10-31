# Programas de prueba de NachOS (`code/test`)

Esta carpeta contiene los programas de usuario de NachOS: programas en C que se compilan con el compilador cruzado MIPS de `../bin/` y que NachOS carga en su memoria simulada con la opción `-x`.

Ningún programa de esta carpeta se ejecuta directamente en Linux: se ejecutan dentro del simulador (`userprog/nachos`).

## Requisitos

- El compilador cruzado de `../bin/` (`mips-gcc`, `as`, `mips-ld`, `coff2noff`) es de 32 bits. En Fedora hace falta instalar:
  `sudo dnf install glibc.i686 libgcc.i686 libstdc++.i686 zlib-ng-compat.i686`
- El kernel debe estar compilado: `cd ../userprog && gmake`
- Con la memoria original de 32 marcos no caben el shell, `todos` y `copy` a la vez.
Las pruebas se hicieron con `NumPhysPages = 64` en `machine/machine.h`.

## Compilar y ejecutar un programa por separado

Desde `code/test`:

```bash
make <programa>            # genera el ejecutable en formato NOFF
cd ../userprog
gmake                      # solo si cambió el kernel
./nachos -x ../test/<programa>
```

Ejemplo con `halt`:

```bash
cd code/test
make halt
cd ../userprog
./nachos -x ../test/halt
```

Notas:
- La opción `-x` es obligatoria. Sin ella, NachOS arranca, no ejecuta nada y se apaga.
- La ruta del programa se resuelve desde el directorio donde se ejecuta `nachos`
  (`userprog/`), por eso se escribe `../test/<programa>`.


## Programas que se usaron

### Propios (mostrando los cambios del proyecto)

| Programa | Qué demuestra | Cómo se ejecuta | Resultado esperado |
| `files` | Create, Open, Read, Write y Close | `./nachos -x ../test/files` | `hola desde nachos` dos veces (una escrita directo y otra leída desde el archivo). Crea `prueba.txt` en `userprog/` |
| `threads` | Fork y Yield: hilos que comparten el espacio de direcciones | `./nachos -x ../test/threads` | `main: uno`, `hilo: uno`, `main: dos`, `hilo: dos` |
| `processes` | Exec, Join y el código de salida | `./nachos -x ../test/processes` | `padre: lanzando files`, luego el texto de `files` dos veces, y por último `padre: el hijo termino` |
| `buzon` | MsgGet, MsgSend, MsgRecv y MsgCtl (colas de mensajes de Linux) | `./nachos -x ../test/buzon` | `hola buzon`|

### Del curso (sin modificar)

| Programa | Qué demuestra | Cómo se ejecuta | Resultado esperado |
| `halt` | Que se pueda cargar y ejecutar un programa de usuario | `./nachos -x ../test/halt` | NachOS imprime las estadísticas y se apaga |
| `copy` | Archivos: copia `nachos.1` en `nachos.2` | Ver «Copia del ejecutable» | `nachos.2` idéntico a `nachos.1` |
| `shell` | Exec, Join y lectura de la consola | `./nachos -x ../test/shell` | Prompt `--`; escribir un comando con su ruta, por ejemplo `../test/threads` |
| `todos` | Exec de `copy`, Join, Fork y Open | `./nachos -x ../test/todos` | `Bien!` si todo funciona, `:(` si falla |

### Copia del ejecutable (`copy` y `nachos.2`)

`copy` abre `nachos.1` y crea `nachos.2`, por eso `nachos.1` debe existir en el
directorio desde el que se ejecuta `nachos`:

```bash
cd code/userprog
cp nachos nachos.1
./nachos -x ../test/copy
cmp nachos.1 nachos.2 && echo "iguales"
chmod u+x nachos.2
./nachos.2 -x ../test/shell
```

### `todos` dentro del `shell`

```bash
./nachos -x ../test/shell
--../test/todos
--../test/halt        # salida limpia
```

## Dificultades detectadas

- `../bin/mips-gcc: No existe el fichero`: Faltan las librerías de 32 bits (ver Requisitos)
- NachOS arranca y se apaga con `user 0` en las estadísticas: Falta la opción `-x` 
- `Unable to open file ../test/<programa>`: El programa no se compiló o la ruta es incorrecta |
- `Assertion failed` en `addrspace.cc`: No hay marcos libres suficientes |
- `Assertion failed ... sysdep.cc`: Linux rechazó un `open`: revisar procesos vivos con `pgrep -a nachos`
- El shell no responde: Escribir el comando con su ruta completa y pulsar Enter. Salir con `../test/halt`, no con `Ctrl+Z`
- Es necesario matar los procesos con `kill % <num proceso>` para manejar el marco de 64 bits