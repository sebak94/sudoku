#include "server.h"
#include "socket_t.h"
#include "sudoku_t.h"
#include "protocol_to_client.h"
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>

#define SUCCESS 0
#define ERROR 1
#define INVALID_FD -1
#define BUF_LEN 1
#define PUT_BUF_LEN 3
#define OK_STATUS_LEN 3
#define ERROR_STATUS_LEN 6
#define BOARD_REPRESENTATION_SIZE 722
#define INVALID_CELL_TEXT_LEN 36

static int listen_to_client(
    socket_t *skt,
    const char *service,
    sudoku_t *sudoku
);
static int process_client_request(socket_t *skt, sudoku_t *sudoku, char* buf);
static int put_request(socket_t *skt, sudoku_t *sudoku);
static int reset_request(socket_t *skt, sudoku_t *sudoku);
static int get_request(socket_t *skt, sudoku_t *sudoku);
static int verify_request(socket_t *skt, sudoku_t *sudoku);
static int send_board(socket_t *skt, sudoku_t *sudoku);
static void free_resources(socket_t *skt, sudoku_t *sudoku);

int server_run(const char *service) {
    sudoku_t sudoku;
    socket_t skt;

    if (sudoku_init(&sudoku) != SUCCESS) {
        fprintf(stderr, "Error: %s\n", strerror(errno));
        return ERROR;
    }
    
    listen_to_client(&skt, service, &sudoku);
    free_resources(&skt, &sudoku);

    return SUCCESS;
}

static int listen_to_client(
    socket_t *skt,
    const char *service,
    sudoku_t *sudoku
) {
    int status;
    char buf[BUF_LEN];
    socket_t peerskt;
    bool continue_reading = true;

	socket_init(skt);
    socket_init(&peerskt);

	if (socket_bind_and_listen(skt, service) != SUCCESS) {
		return ERROR;
	}

    socket_accept(skt, &peerskt);
    if (peerskt.fd == INVALID_FD) {
        socket_release(&peerskt);
        return ERROR;
    }

    while (continue_reading) {
        status = socket_receive(&peerskt, buf, BUF_LEN);
        if (status == 0) {
            continue_reading = false;
        } else {
            if (process_client_request(&peerskt, sudoku, buf) == ERROR) {
                continue_reading = false;
            }
        }
    }

	return SUCCESS;
}

static int process_client_request(socket_t *skt, sudoku_t *sudoku, char* buf) {
    switch (buf[0]) {
        case 'P': return put_request(skt, sudoku);
        case 'R': reset_request(skt, sudoku); break;
        case 'G': get_request(skt, sudoku); break;
        case 'V': verify_request(skt, sudoku); break;
        default: return SUCCESS;
    }

    return SUCCESS;
}

static int put_request(socket_t *skt, sudoku_t *sudoku) {
    int status;
    char buf[PUT_BUF_LEN];

    status = socket_receive(skt, buf, PUT_BUF_LEN);

    if (status == 0) {
        return ERROR;
    }

    size_t row = (size_t)buf[0];
    size_t column = (size_t)buf[1];
    char value = buf[2] + '0';

    if (sudoku_put(sudoku, value, row, column) == ERROR) {
        server_protocol_t data;
        data.size = INVALID_CELL_TEXT_LEN;
        data.value = "La celda indicada no es modificable\n";
        if (send_to_client(skt, &data) < 1) {
            return ERROR;
        }
        return SUCCESS;
    }
    
    if (send_board(skt, sudoku) < 1) {
        return ERROR;
    }

    return SUCCESS;
}

static int reset_request(socket_t *skt, sudoku_t *sudoku) {
    sudoku_reset(sudoku);

    if (send_board(skt, sudoku) < 1) {
        return ERROR;
    }

    return SUCCESS;
}

static int get_request(socket_t *skt, sudoku_t *sudoku) {
    if (send_board(skt, sudoku) < 1) {
        return ERROR;
    }
    
    return SUCCESS;
}

static int verify_request(socket_t *skt, sudoku_t *sudoku) {
    server_protocol_t data;
    bool is_ok = sudoku_verify(sudoku) == SUCCESS;

    data.size = is_ok ? OK_STATUS_LEN : ERROR_STATUS_LEN;
    data.value = is_ok ? "OK\n" : "ERROR\n";

    return send_to_client(skt, &data);
}

static int send_board(socket_t *skt, sudoku_t *sudoku) {
    char board[BOARD_REPRESENTATION_SIZE];
    server_protocol_t data;

    sudoku_get(sudoku, board);

    data.size = BOARD_REPRESENTATION_SIZE;
    data.value = board;

    return send_to_client(skt, &data);
}

static void free_resources(socket_t *skt, sudoku_t *sudoku) {
    socket_release(skt);
    sudoku_release(sudoku);
}
