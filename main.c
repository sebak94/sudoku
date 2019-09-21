#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include "client.h"
#include "server.h"

#define SUCCESS 0
#define ERROR 1

int handle_user_input(int argc, char const *argv[]);
bool client_parameters_are_valid(int argc, char const *argv[]);
bool server_parameters_are_valid(int argc, char const *argv[]);

int main(int argc, char const *argv[]) {
	if (argc == 1) {
		fprintf(
			stderr,
			"Modo no soportado, el primer parámetro debe ser server o client\n");
		return ERROR;
	}

	return handle_user_input(argc, argv);
}

int handle_user_input(int argc, char const *argv[]) {
	if (strncmp(argv[1], "client", strlen(argv[1])) == 0) {
		if (client_parameters_are_valid(argc, argv)) {
			return client_run(argv[2], argv[3]);
		} else {
			return ERROR;
		}
	}
	if (strncmp(argv[1], "server", strlen(argv[1])) == 0) {
		if (server_parameters_are_valid(argc, argv)) {
			return server_run(argv[2]);
		} else {
			return ERROR;
		}
	}

	return ERROR;
}

bool client_parameters_are_valid(int argc, char const *argv[]) {
	if (argc != 4) {
		fprintf(stderr, "Uso: ./tp client <host> <puerto>\n");
		return false;
	}
	
	return true;
}

bool server_parameters_are_valid(int argc, char const *argv[]) {
	if (argc != 3) {
		fprintf(stderr, "Uso: ./tp server <puerto>\n​");
		return false;
	}
	
	return true;
}
