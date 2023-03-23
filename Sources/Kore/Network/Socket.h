#pragma once

#include <kinc/network/socket.h>

namespace Kore {
	enum SocketProtocol { UDP, TCP };

	enum SocketFamily { IP4, IP6 };

	struct SocketOptions {
		bool nonBlocking;
		bool broadcast;
		bool tcpNoDelay;

		SocketOptions() : nonBlocking(true), broadcast(false), tcpNoDelay(false) {}
	};

	class Socket {
	public:
		Socket();
		~Socket();

		void set(const char *host, int port, SocketFamily family, SocketProtocol protocol);

		bool open(SocketOptions *options = nullptr);

		bool select(uint32_t waittime, bool read, bool write);

		bool bind();
		bool listen(int backlog);
		bool accept(kinc_socket_t *newSocket, unsigned *remoteAddress, unsigned *remotePort);
		bool connect();

		int send(const uint8_t *data, int size);
		int send(unsigned address, int port, const uint8_t *data, int size);
		int send(const char *url, int port, const uint8_t *data, int size);
		int receive(uint8_t *data, int maxSize, unsigned *from_address, unsigned *from_port);

		static unsigned urlToInt(const char *url, int port);

	private:
		kinc_socket_t sock;
	};
}
