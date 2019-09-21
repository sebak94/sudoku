#include "protocol_to_server.h"
#include "socket_t.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>

static int send_client_request(socket_t *skt, const char* buf, size_t length);

int send_put(socket_t *skt, put_protocol_t *put) {
    const size_t BUF_LEN = 4;
    char buf[BUF_LEN];

    memcpy(buf, &put->command, 1);
    memcpy(buf + 1, &put->row, 1);
    memcpy(buf + 2, &put->column, 1);
    memcpy(buf + 3, &put->value, 1);

    return send_client_request(skt, buf, BUF_LEN);
}

int send_get(socket_t *skt, get_protocol_t *get) {
    const size_t BUF_LEN = 1;
    char buf = get->command;

    return send_client_request(skt, &buf, BUF_LEN);
}

int send_reset(socket_t *skt, reset_protocol_t *reset) {
    const size_t BUF_LEN = 1;
    char buf = reset->command;

    return send_client_request(skt, &buf, BUF_LEN);
}

int send_verify(socket_t *skt, verify_protocol_t *verify) {
    const size_t BUF_LEN = 1;
    char buf = verify->command;

    return send_client_request(skt, &buf, BUF_LEN);
}

static int send_client_request(socket_t *skt, const char *buf, size_t length) {
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
