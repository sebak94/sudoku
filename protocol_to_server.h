#ifndef __PROTOCOL_TO_SERVER_H__
#define __PROTOCOL_TO_SERVER_H__

#include "socket_t.h"
#include <stdint.h>

typedef struct {
    char command;
    uint8_t row;
    uint8_t column;
    uint8_t value;
} put_protocol_t;

typedef struct {
    char command;
} get_protocol_t;

typedef struct {
    char command;
} reset_protocol_t;

typedef struct {
    char command;
} verify_protocol_t;

int send_put(socket_t *skt, put_protocol_t *put);
int send_get(socket_t *skt, get_protocol_t *get);
int send_reset(socket_t *skt, reset_protocol_t *reset);
int send_verify(socket_t *skt, verify_protocol_t *verify);

#endif
