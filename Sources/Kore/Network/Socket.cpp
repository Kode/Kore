#include "Socket.h"

#include <Kore/Log.h>

#include <stdio.h>

using namespace Kore;

Socket::Socket() {
	kinc_socket_init(&sock);
}

Socket::~Socket() {
	kinc_socket_destroy(&sock);
}

void Socket::set(const char *host, int port, SocketFamily family, SocketProtocol protocol) {
	kinc_socket_set(&sock, host, port, (kinc_socket_family_t)family, (kinc_socket_protocol_t)protocol);
}

bool Socket::open(SocketOptions *options) {
	kinc_socket_options_t kinc_options;
	if (options == nullptr) {
		kinc_socket_options_set_defaults(&kinc_options);
	}
	else {
		kinc_options.broadcast = options->broadcast;
		kinc_options.non_blocking = options->nonBlocking;
		kinc_options.tcp_no_delay = options->tcpNoDelay;
	}
	return kinc_socket_open(&sock, &kinc_options);
}

bool Socket::select(uint32_t waittime, bool read, bool write) {
	return kinc_socket_select(&sock, waittime, read, write);
}

bool Socket::bind() {
	return kinc_socket_bind(&sock);
}

bool Socket::listen(int backlog) {
	return kinc_socket_listen(&sock, backlog);
}

bool Socket::accept(kinc_socket_t *newSocket, unsigned *remoteAddress, unsigned *remotePort) {
	return kinc_socket_accept(&sock, newSocket, remoteAddress, remotePort);
}

bool Socket::connect() {
	return kinc_socket_connect(&sock);
}

int Socket::send(const uint8_t *data, int size) {
	return kinc_socket_send(&sock, data, size);
}

int Socket::send(unsigned address, int port, const uint8_t *data, int size) {
	return kinc_socket_send_address(&sock, address, port, data, size);
}

int Socket::send(const char *url, int port, const uint8_t *data, int size) {
	return kinc_socket_send_url(&sock, url, port, data, size);
}

int Socket::receive(uint8_t *data, int maxSize, unsigned *fromAddress, unsigned *fromPort) {
	return kinc_socket_receive(&sock, data, maxSize, fromAddress, fromPort);
}

unsigned Socket::urlToInt(const char *url, int port) {
	return kinc_url_to_int(url, port);
}
