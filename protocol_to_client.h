#ifndef __PROTOCOL_TO_CLIENT_H__
#define __PROTOCOL_TO_CLIENT_H__

#include "socket_t.h"
#include <stdint.h>
#include <stdbool.h>

#define BOARD_REPRESENTATION_SIZE 722
#define INVALID_CELL_TEXT_LEN 36

typedef struct {
    uint32_t size;
    char *value;
} server_protocol_t;

int send_to_client(socket_t *skt, server_protocol_t *data);

#endif
