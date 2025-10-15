# Mini-Shell
Desarrollo de una mini-shell para el curso de sistemas operativos
Este proyecto consiste en el desarrollo de un intérprete de comandos (mini-shell) para sistemas Linux, implementado en C++, que permite ejecutar comandos del sistema, manejar redirecciones, procesos, hilos y estadísticas de memoria.

Requerimientos
Sistema operativo: Linux (Ubuntu, Debian, Fedora, etc.)
Compilador: g++ (versión 9.0 o superior)
Librerías:
  <unistd.h> (manejo de procesos)
  <sys/wait.h> (sincronización de procesos)
  <pthread.h> (manejo de hilos)
  <fcntl.h> y <sys/types.h> (I/O y redirección)
