#ifndef __SOCKET_T_H__
#define __SOCKET_T_H__

#include <stdio.h>

// TDA que se encarga de wrappear a la biblioteca sys/socket

// socket_t guarda el file descriptor
typedef struct {
    int fd;
} socket_t;

// Inicializa el fd en un valor inválido
int socket_init(socket_t *self);

// Crea un socket y se conecta a un servidor con los parámetros pasados
int socket_connect(socket_t *self, const char *host, const char* service);

// Crea un socket para escuchar conexiones
int socket_bind_and_listen(socket_t *self, const char* service);

// Acepta una conexión y guarda el fd nuevo en el socket_t pasado
void socket_accept(socket_t *self, socket_t *peerskt);

// Envia length bytes leídos de buf
int socket_send(socket_t *self, const char *buf, const size_t length);

// Recibe length bytes y los guarda en buf
int socket_receive(socket_t *self, char *buf, const size_t length);

// Libera todos los recursos tomados
void socket_release(socket_t *self);

#endif
