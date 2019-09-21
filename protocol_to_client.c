#include "protocol_to_client.h"
#include "socket_t.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define SIZE_BUF_LEN 4
#define INVALID_CELL_TEXT_LEN 36

static int send_server_response(socket_t *skt, const char *buf, size_t length);

int send_to_client(socket_t *skt, server_protocol_t *data) {
    int status;
    char size_buf[SIZE_BUF_LEN];
    char *text_buf = malloc(data->size * sizeof(char));

    memcpy(size_buf, &data->size, SIZE_BUF_LEN);

    status = send_server_response(skt, size_buf, SIZE_BUF_LEN);
    if (status < 1) {
        free(text_buf);
        return status;
    }

    memcpy(text_buf, data->value, data->size);

    status = send_server_response(skt, text_buf, data->size);
    
    free(text_buf);
    return status;
}

static int send_server_response(socket_t *skt, const char *buf, size_t length) {
    int status;

    status = socket_send(skt, buf, length);

    if (status == 0) {
    	printf("The connection was closed.\n");
    }
    if (status == -1) {
    	printf("Error: %s\n", strerror(errno));
    }

    return status;
}
