#define _POSIX_C_SOURCE 200112L
#include "socket_t.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define SUCCESS 0
#define ERROR 1
#define INVALID_FD -1

int socket_init(socket_t *self) {
    self->fd = INVALID_FD;
	return SUCCESS;
}

int socket_bind_and_listen(socket_t *self, const char *service) {
	int status;

	struct addrinfo hints;
	struct addrinfo *result;

	int val = 1;

    memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	status = getaddrinfo(0, service, &hints, &result);
	if (status != 0) { 
		fprintf(stderr, "Error in getaddrinfo: %s\n", gai_strerror(status));
		return ERROR;
	}

	self->fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (self->fd == INVALID_FD) {
		freeaddrinfo(result);
		return ERROR;
	}

	status = setsockopt(self->fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
	if (status == -1) {
		close(self->fd);
		freeaddrinfo(result);
		return ERROR;
	}

	status = bind(self->fd, result->ai_addr, result->ai_addrlen);
	if (status == -1) {
		close(self->fd);
		freeaddrinfo(result);
		return ERROR;
	}

	freeaddrinfo(result);

	status = listen(self->fd, 1);
	if (status == -1) {
		close(self->fd);
		return ERROR;
	}

	return SUCCESS;
}

int socket_connect(socket_t *self, const char *hostname, const char *service) {
    int status;
	bool are_we_connected = false;

	struct addrinfo hints;
	struct addrinfo *result, *ptr;

    memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;

	status = getaddrinfo(hostname, service, &hints, &result);

	if (status != 0) { 
		fprintf(stderr, "Error in getaddrinfo: %s\n", gai_strerror(status));
		return ERROR;
	}

	for (ptr = result; ptr != NULL && !are_we_connected; ptr = ptr->ai_next) {
		self->fd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (self->fd != INVALID_FD) {
			status = connect(self->fd, ptr->ai_addr, ptr->ai_addrlen);
			if (status == -1) {
				close(self->fd);
			}
			are_we_connected = status != -1;
		}
	}

	freeaddrinfo(result);

	if (are_we_connected == false) {
		return ERROR;
	}

    return SUCCESS;
}

void socket_accept(socket_t *self, socket_t *peerskt) {
	peerskt->fd = accept(self->fd, NULL, NULL);
}

int socket_send(socket_t *self, const char *buf, const size_t length) {
    int status;
	int bytes_sent = 0;

    while (bytes_sent < length) {
		status = send(self->fd, &buf[bytes_sent], length - bytes_sent, MSG_NOSIGNAL);

		if (status == -1) {
        	return status;
		}

		if (status == 0) {
			return status;
		}
        
		bytes_sent += status;
	}

    return bytes_sent;
}

int socket_receive(socket_t *self, char *buf, const size_t length) {
	int status;
	int bytes_recv = 0;

    while (bytes_recv < length) {
		status = recv(self->fd, &buf[bytes_recv], length - bytes_recv, 0);

		if (status == -1) {
        	return status;
		}

		if (status == 0) {
			return status;
		} else {
         	bytes_recv += status;
      	}
	}

    return bytes_recv;
}

void socket_release(socket_t *self) {
    shutdown(self->fd, 2);
    close(self->fd);
}
