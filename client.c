#include "client.h"
#include "socket_t.h"
#include "protocol_to_server.h"
#include "split.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>

#define SUCCESS 0
#define ERROR 1
#define MAX_INPUT_BUF_LEN 20
#define SIZE_BUF_LEN 4
#define NUMBERS "0123456789"

static void listen_user_input(socket_t *skt);
static void handle_input(
	socket_t *skt,
	char buf[MAX_INPUT_BUF_LEN],
	bool *continue_reading,
	bool *command_exists,
	bool *should_wait_server_response
);
static bool validate_put(char* buf_ptr, put_protocol_t *put);
static int get_command(socket_t *skt);
static int reset_command(socket_t *skt);
static int verify_command(socket_t *skt);
static int receive_response(socket_t *skt);

int client_run(const char *hostname, const char *service) {
	socket_t skt;

	socket_init(&skt);

	if (socket_connect(&skt, hostname, service) != 0) {
		return ERROR;
	}

	listen_user_input(&skt);

	socket_release(&skt);

	return SUCCESS;
}

static void listen_user_input(socket_t *skt) {
	char buf[MAX_INPUT_BUF_LEN];
	bool continue_reading = true;

	while (continue_reading && fgets(buf, MAX_INPUT_BUF_LEN, stdin)) {
		bool command_exists = false;
		bool should_wait_server_response = true;

		handle_input(
			skt,
			buf,
			&continue_reading,
			&command_exists,
			&should_wait_server_response);

		if (!command_exists) {
			should_wait_server_response = false;
			fprintf(stderr, "Comando no soportado\n");
		}

		if (should_wait_server_response) {
			receive_response(skt);
		}
	}
}

static void handle_input(
	socket_t *skt,
	char buf[MAX_INPUT_BUF_LEN],
	bool *continue_reading,
	bool *command_exists,
	bool *should_wait_server_response
) {
	int status = 1;
	char *buf_ptr = buf;
	char *command = split(buf_ptr, " ", &buf_ptr);
	if (strcmp(command, "exit\n") == 0) {
		*continue_reading = false;
		*command_exists = true;
		*should_wait_server_response = false;
	}
	if (strcmp(command, "put") == 0) {
		*command_exists = true;
		put_protocol_t put;
		if (validate_put(buf_ptr, &put)) {
			status = send_put(skt, &put);
		} else {
			*should_wait_server_response = false;
		}
	}
	if (strcmp(command, "get\n") == 0) {
		*command_exists = true;
		status = get_command(skt);
	}
	if (strcmp(command, "reset\n") == 0) {
		*command_exists = true;
		status = reset_command(skt);
	}
	if (strcmp(command, "verify\n") == 0) {
		*command_exists = true;
		status = verify_command(skt);
	}
	if (*command_exists && status < 1) {
		*continue_reading = false;
	}
}

static bool validate_put(char* buf_ptr, put_protocol_t *put) {
	int value = atoi(split(buf_ptr, " ", &buf_ptr));

	if (value < 1 || value > 9) {
		fprintf(stderr, "Error en el valor ingresado. Rango soportado: [1,9]\n");
		return false;
	}

	split(buf_ptr, " ", &buf_ptr);
	int row = atoi(split(buf_ptr, ",", &buf_ptr));
	int column = atoi(split(buf_ptr, ",", &buf_ptr));

	if (row < 1 || row > 9 || column < 1 || column > 9) {
		fprintf(stderr, "Error en los índices. Rango soportado: [1,9]\n");
		return false;
	}

	put->command = 'P';
	put->row = (uint8_t)row;
	put->column = (uint8_t)column;
	put->value = (uint8_t)value;

	return true;
}

static int get_command(socket_t *skt) {
	get_protocol_t get;
	get.command = 'G';
	return send_get(skt, &get);
}

static int reset_command(socket_t *skt) {
	reset_protocol_t reset;
	reset.command = 'R';
	return send_reset(skt, &reset);
}

static int verify_command(socket_t *skt) {
	verify_protocol_t verify;
	verify.command = 'V';
	return send_verify(skt, &verify);
}

static int receive_response(socket_t *skt) {
	int status;
	char size_buf[SIZE_BUF_LEN];
	int size;

	status = socket_receive(skt, size_buf, SIZE_BUF_LEN);
	if (status < 1) {
		return ERROR;
	}

	memcpy(&size, size_buf, SIZE_BUF_LEN);
	char *response_buf = malloc((size + 1) * sizeof(char));

	status = socket_receive(skt, response_buf, size);
	if (status < 1) {
		free(response_buf);
		return ERROR;
	}

	response_buf[size] = '\0';
	printf("%s", response_buf);

	free(response_buf);
	return status;
}
